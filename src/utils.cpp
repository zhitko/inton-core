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
        data.push_back(vector.at(i-1) - vector.at(i));
    }

    return data;
}
