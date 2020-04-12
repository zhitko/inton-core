#include "../include/utils.h"

#include "define.h"

#include <algorithm>
#include <cmath>


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

int IntonCore::normalizeValue(int value, int source_size, int target_size)
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

std::vector<int> IntonCore::segmentsToMask(std::vector<std::pair<int, int> > segments, uint32_t result_length)
{
    std::vector<int> mask(result_length, 0);

    for (auto &it: segments)
    {
        for (int i=0; i<=it.second; i++)
        {
            mask[it.first + i] = 1;
        }
    }

    return mask;
}

std::vector<std::pair<int, int> > IntonCore::invertSegments(std::vector<std::pair<int, int> > segments)
{
    std::vector<std::pair<int, int> > inverted;

    std::vector<std::pair<int, int> >::iterator it = segments.begin();

    int from = it.base()->first + it.base()->second;

    it++;

    for(; it != segments.end(); it++)
    {
        inverted.push_back(std::pair<int, int>(from, it.base()->first - from));
        from = it.base()->first + it.base()->second;
    }

    return inverted;
}
