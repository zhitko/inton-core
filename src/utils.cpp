#include "../include/utils.h"

#include "define.h"

#include <algorithm>
#include <cmath>
#include<iterator>

std::vector<double> IntonCore::normalizeVector(std::vector<double> vector, double target_min, double target_max)
{
    std::vector<double> result;

    if (vector.empty()) return result;

    const auto [source_min, source_max] = std::minmax_element(vector.begin(), vector.end());

    const double source_scale = *source_max - *source_min;
    const double target_scale = target_max - target_min;

    double zsrc = 0.0;
    double scaled = 0.0;

    for (double &it : vector) {
        zsrc = it - *source_min;
        scaled = zsrc * target_scale / source_scale;
        result.push_back(scaled + target_min);
    }

    return result;
}

std::vector<double> IntonCore::normalizeVectorByOctaves(std::vector<double> vector, std::list<double> octaves)
{
    std::vector<double> result;

    if (vector.empty()) return result;

    for (double &vi : vector) {
        int i = 1;
        for (double &oi : octaves) {
            if (oi > vi) {
                result.push_back(i);
                break;
            } else {
                i++;
            }
        }
    }

    return result;
}

std::vector<double> IntonCore::resizeVectorByMinMax(std::vector<double> vector, uint32_t target_size)
{
    std::vector<double> result;

    if(vector.empty()) return result;

    target_size /= 2;

    auto step = static_cast<uint32_t>(std::ceil(vector.size() / target_size));

    auto from = vector.begin();
    auto to = vector.begin() + step/2;
    for (uint32_t i=0; i< target_size; i++)
    {
        const auto [source_min, source_max] = std::minmax_element(from, to);
        auto min_distance = std::distance(from, source_min);
        auto max_distance = std::distance(from, source_max);
        if (min_distance < max_distance)
        {
            result.push_back(*source_min);
            result.push_back(*source_max);
        }
        else
        {
            result.push_back(*source_max);
            result.push_back(*source_min);
        }
        to = to + step;
        from = from + step;
    }

    return result;

}

uint32_t IntonCore::normalizeValue(uint32_t value, uint32_t source_size, uint32_t target_size)
{
    return static_cast<int>(
        std::ceil(value * target_size / source_size)
    );
}

std::vector<double> IntonCore::linerSmoothVector(std::vector<double> vector, uint32_t frame)
{
    std::vector<double> result;

    if (vector.empty()) return result;

    uint32_t index = 0;
    uint32_t size = vector.size();
    uint32_t shift = static_cast<uint32_t>(std::ceil(frame / 2.0));
    frame = shift * 2;
    double middle;

    for(uint32_t i=0; i<size; i++)
    {
        middle = 0.0;
        for(uint32_t j=0; j<frame; j++)
        {
            index = j + i - shift;
            if (index > 0 && index < size)
            {
                middle += vector.at(index);
            }
        }
        result.push_back(middle/frame);
    }

    return result;
}

std::vector<uint32_t> IntonCore::segmentsToMask(std::vector<std::pair<uint32_t, uint32_t> > segments, uint32_t result_length)
{
    std::vector<uint32_t> mask(result_length, 2);

    uint32_t min = result_length;
    uint32_t max = 0;

    for (auto &it: segments)
    {
        if (min > it.first) min = it.first;
        if (max < (it.first + it.second)) max = it.first + it.second;

        for (uint32_t i=0; i<=it.second; i++)
        {
            mask[it.first + i] = 1;
        }
    }

    for (uint32_t i=min; i<=max; i++)
    {
        if (mask[i] == 2) mask[i] = 0;
    }

    return mask;
}

std::vector<std::pair<uint32_t, uint32_t> > IntonCore::invertSegments(std::vector<std::pair<uint32_t, uint32_t> > segments)
{
    std::vector<std::pair<uint32_t, uint32_t> > inverted;

    std::vector<std::pair<uint32_t, uint32_t> >::iterator it = segments.begin();

    uint32_t from = it.base()->first + it.base()->second;

    it++;

    for(; it != segments.end(); it++)
    {
        inverted.push_back(std::pair<uint32_t, uint32_t>(from, it.base()->first - from));
        from = it.base()->first + it.base()->second;
    }

    return inverted;
}

std::vector<double> IntonCore::cutVectorBySegments(std::vector<double> vector, std::vector<std::pair<uint32_t,uint32_t>> segments, uint32_t segments_data_size)
{
    std::vector<double> data;
    copy(vector.begin(), vector.end(), back_inserter(data));

    uint32_t pos = 0;
    for (auto &it: segments)
    {
        auto to = IntonCore::normalizeValue(it.first, segments_data_size, data.size());
        for (auto i = pos; i <= to && i < data.size(); i++)
        {
            data.at(i) = 0;
        }
        pos = to + IntonCore::normalizeValue(it.second, segments_data_size, data.size());
    }

    for (auto i = pos; i < data.size(); i++)
    {
        data.at(i) = 0;
    }

    return data;
}

std::vector<double> IntonCore::calculateVectorDerivative(std::vector<double> vector)
{
    std::vector<double> data;

    for (auto i = 1; i < vector.size(); i++)
    {
        data.push_back(vector.at(i) - vector.at(i-1));
    }

    return data;
}

std::vector<double> IntonCore::reinforcedVectorFrequency(std::vector<double> vector, double k)
{
    std::vector<double> data;

    //  S-n = Sn - S(n-1)*k
    for (auto i = 1; i < vector.size(); i++)
    {
        data.push_back(vector.at(i) - vector.at(i-1)*k);
    }

    return data;
}

std::vector<double> IntonCore::highFrequencyReinforcedVectorFrequency(std::vector<double> vector, double k)
{
    std::vector<double> data;

    //  S''n = S'n – S'(n-1)*k1
    for (auto i = 2; i < vector.size(); i++)
    {
        if (k > 0.0) {
            auto sn1 = vector.at(i-2) - vector.at(i-1);
            auto sn2 = vector.at(i-1) - vector.at(i);
            data.push_back(sn2 - sn1*k);
        } else {
            data.push_back(vector.at(i));
        }
    }

    return data;
}

std::vector<double> IntonCore::lowFrequencyReinforcedVectorFrequency(std::vector<double> vector, double k)
{
    std::vector<double> data;

    //  S''n = Sn + [S(n-1) + S(n-2) + S(n-3)]/2 * k2
    for (auto i = 4; i < vector.size(); i++)
    {
        auto sn1 = vector.at(i-3);
        auto sn2 = vector.at(i-2);
        auto sn3 = vector.at(i-1);
        auto sn4 = vector.at(i);
        data.push_back(sn4 + (sn3 + sn2 + sn1)*k);
    }

    return data;
}

std::vector<double> IntonCore::multiplyWaveVector(std::vector<double> vector, double k)
{
    std::vector<double> data;
    auto max = 32767 / k;
    auto min = max * -1;

    for (auto i = 0; i < vector.size(); i++)
    {
        auto val = vector.at(i);
        if(val > max) {
            data.push_back(max);
        } else if(val < min) {
            data.push_back(min);
        } else {
            data.push_back(val*k);
        }
    }

    return data;
}

std::vector<double> IntonCore::fullWaveVectorCorrection(std::vector<double> vector, double hfk, double lfk, double sk, bool twice)
{
    auto wave_hf = reinforcedVectorFrequency(vector, hfk);
    if (twice) wave_hf = reinforcedVectorFrequency(wave_hf, hfk);
    // auto wave_lf = reinforcedVectorFrequency(wave_hf, -1.0*lfk);
    // if (twice) wave_lf = reinforcedVectorFrequency(wave_lf, -1.0*lfk);
    auto wave_lf = lowFrequencyReinforcedVectorFrequency(wave_hf, lfk);
    if (twice) wave_lf = lowFrequencyReinforcedVectorFrequency(wave_lf, lfk);
    return multiplyWaveVector(wave_lf, sk);
}

const double FIND_ZERRO_CROSSING_LIMIT = 10.0;
const int FIND_ZERRO_CROSSING_MAX_ITERATIONS = 10;

std::tuple<double, double> IntonCore::findZeroCrossingK(
    std::vector<double> vector,
    uint32_t frame_rate,
    double zero_crossing_target,
    bool twice,
    double high_frequency_k,
    double low_frequency_k,
    int iteration,
    double delta
) {
    auto updatedVector = IntonCore::fullWaveVectorCorrection(vector, high_frequency_k, low_frequency_k, 1.0, twice);
    auto zero_crossing_current = IntonCore::calculateVectorZeroCrossing(updatedVector, frame_rate);
    auto zero_crossing_delta = std::abs(zero_crossing_target - zero_crossing_current);
    DEBUG("findZeroCrossingK: zero_crossing_current=%f", zero_crossing_current)
    DEBUG("findZeroCrossingK: zero_crossing_target=%f", zero_crossing_target)
    DEBUG("findZeroCrossingK: zero_crossing_delta=%f", zero_crossing_delta)
    DEBUG("findZeroCrossingK: high_frequency_k=%f", high_frequency_k)
    DEBUG("findZeroCrossingK: low_frequency_k=%f", low_frequency_k)
    DEBUG("findZeroCrossingK: ---------------")

    if (iteration > FIND_ZERRO_CROSSING_MAX_ITERATIONS) {
        DEBUG("findZeroCrossingK: Done limit iterations")
        if (twice) {
            return {high_frequency_k, low_frequency_k};
        } else {
            return {-1, -1};
        }
    }

    if (zero_crossing_delta < FIND_ZERRO_CROSSING_LIMIT) {
        DEBUG("findZeroCrossingK: Done")
        return {high_frequency_k, low_frequency_k};
    }

    bool is_up = zero_crossing_target > zero_crossing_current;
    bool using_hight = high_frequency_k != 0.0 && low_frequency_k == 0.0;
    bool using_low = low_frequency_k != 0.0 && high_frequency_k == 0.0;

    if (is_up && using_hight) high_frequency_k += delta;
    else if (is_up && using_low) low_frequency_k -= delta;
    else if (!is_up && using_hight) high_frequency_k -= delta;
    else if (!is_up && using_low) low_frequency_k += delta;
    else if (is_up) high_frequency_k += delta;
    else if (!is_up) low_frequency_k += delta;

    iteration++;

    return findZeroCrossingK(
        vector,
        frame_rate,
        zero_crossing_target,
        twice,
        high_frequency_k,
        low_frequency_k,
        iteration,
        delta / 2.0
    );
}

std::vector<double> IntonCore::waveVectorZeroCrossingCorrection(
    std::vector<double> vector,
    uint32_t frame_rate,
    double zero_crossing_target
) {
    double high_frequency_k, low_frequency_k;
    std::tie(high_frequency_k, low_frequency_k) = findZeroCrossingK(vector, frame_rate, zero_crossing_target);

    auto vec_high_frequency = reinforcedVectorFrequency(vector, high_frequency_k);
    auto vec_low_frequency = reinforcedVectorFrequency(vec_high_frequency, -1.0*low_frequency_k);

    return vec_low_frequency;
}

std::vector<double> IntonCore::dynamicWaveVectorCorrection(
    std::vector<double> vector,
    uint32_t frame_rate,
    double zero_crossing_target,
    double strength_target,
    int frame,
    int shift
) {
    std::vector<double> data;
    for (int i=0; i<vector.size(); i++) {
        data.push_back(0);
    }

    for (int i=0; i<vector.size(); i+=shift)
    {
        auto from = i;
        auto to = i + frame;
        if (to > vector.size()) to = vector.size();
        std::vector<double>::const_iterator first = vector.begin() + from;
        std::vector<double>::const_iterator last = vector.begin() + to;
        std::vector<double> new_vec(first, last);

        double high_frequency_k, low_frequency_k;
        std::tie(high_frequency_k, low_frequency_k) = findZeroCrossingK(new_vec, frame_rate, zero_crossing_target);

        auto new_vec_high_frequency = reinforcedVectorFrequency(new_vec, high_frequency_k);
        auto new_vec_low_frequency = reinforcedVectorFrequency(new_vec_high_frequency, -1.0*low_frequency_k);
        double strength_k = strength_target / calculateVectorRMS(new_vec_low_frequency);
        auto corrected = multiplyWaveVector(new_vec_low_frequency, strength_k);

        for (int j=0; j<corrected.size(); j++) {
            if (data.at(i+j) == 0) {
                data.at(i+j) = corrected.at(j);
            } else {
                data.at(i+j) = (data.at(i+j) + corrected.at(j)) / 2;
            }
        }
    }

    return data;
}

std::vector<double> IntonCore::dynamicWaveStrengthVectorCorrection(
    std::vector<double> vector,
    double strength_target,
    int frame,
    int shift
    ) {
    std::vector<double> data;
    for (int i=0; i<vector.size(); i++) {
        data.push_back(0);
    }

    for (int i=0; i<vector.size(); i+=shift)
    {
        auto from = i;
        auto to = i + frame;
        if (to > vector.size()) to = vector.size();
        std::vector<double>::const_iterator first = vector.begin() + from;
        std::vector<double>::const_iterator last = vector.begin() + to;
        std::vector<double> new_vec(first, last);
        double strength_k = strength_target / calculateVectorRMS(new_vec);
        auto corrected = multiplyWaveVector(new_vec, strength_k);

        for (int j=0; j<corrected.size(); j++) {
            if (data.at(i+j) == 0) {
                data.at(i+j) = corrected.at(j);
            } else {
                data.at(i+j) = (data.at(i+j) + corrected.at(j)) / 2;
            }
        }
    }

    return data;
}

double IntonCore::calculateVectorRMS(std::vector<double> data)
{
    double result = 0.0;
    for (int i=0; i<data.size(); i++)
    {
        auto val = data.at(i);
        result += pow(data.at(i), 2);
    }
    return sqrt(result / data.size());
}

double IntonCore::calculateVectorZeroCrossing(std::vector<double> data, uint32_t frame_rate)
{
    double result = 0.0;
    if (data.empty()) return result;

    for (int i=1; i<data.size(); i++)
    {
        auto value1 = data.at(i-1);
        auto value2 = data.at(i);
        if (value1*value2 < 0) result += 1.0;
    }
    result = result * frame_rate / data.size();
    return result;
}

std::vector<double> IntonCore::calculateDynamicVectorZeroCrossing(std::vector<double> vector, uint32_t frame_rate, uint32_t frame, uint32_t shift)
{
    std::vector<double> data;
    auto to = vector.size() - frame;
    DEBUG("calculateDynamicVectorZeroCrossing: vector.size=%lo", vector.size())
    DEBUG("calculateDynamicVectorZeroCrossing: frame=%i", frame)
    DEBUG("calculateDynamicVectorZeroCrossing: to=%lo", to)

    for (int i=0; i<to; i+=shift)
    {
        std::vector<double>::const_iterator first = vector.begin() + i;
        std::vector<double>::const_iterator last = vector.begin() + i + frame;
        std::vector<double> newVec(first, last);
        auto zc = IntonCore::calculateVectorZeroCrossing(newVec, frame_rate);
        data.push_back(zc);
    }

    return data;
}

std::vector<double> IntonCore::calculateDynamicVectorRMS(std::vector<double> vector, uint32_t frame, uint32_t shift)
{
    std::vector<double> data;
    auto to = vector.size() - frame;
    DEBUG("calculateDynamicVectorZeroCrossing: vector.size=%lo", vector.size())
    DEBUG("calculateDynamicVectorZeroCrossing: frame=%i", frame)
    DEBUG("calculateDynamicVectorZeroCrossing: to=%lo", to)

    for (int i=0; i<to; i+=shift)
    {
        std::vector<double>::const_iterator first = vector.begin() + i;
        std::vector<double>::const_iterator last = vector.begin() + i + frame;
        std::vector<double> newVec(first, last);
        auto zc = IntonCore::calculateVectorRMS(newVec);
        data.push_back(zc);
    }

    return data;
}

double IntonCore::calculateVectorGravityCenterSubvector(std::vector<double> data, int from, int to)
{
    double result = 0.0;

    if (from < 0) from = 0;
    if (to > data.size()) to = data.size();

    double px = 0.0;
    double p = 0.0;
    for (int i=from; i<to; i++)
    {
        px += data.at(i) * i;
        p += data.at(i);
    }
    result = px / p;

    return result;
}

double IntonCore::calculateVectorGravityCenter(std::vector<double> data)
{
    return IntonCore::calculateVectorGravityCenterSubvector(data, 0, data.size());
}
