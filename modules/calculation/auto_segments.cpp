#include "auto_segments.h"

#include <algorithm>
#include <cmath>

#include "define.h"


double getRelativeLimitForSement(uint32_t start_position, uint32_t stop_position, std::vector<double> data, double relative_limit)
{
    auto begin = data.begin() + start_position;
    auto end = data.begin() + stop_position;
    const auto [min_value, max_value] = std::minmax_element(begin, end);

    DEBUG("maxValue: %f", *max_value.base())
    DEBUG("minValue: %f", *min_value.base())

    // Calculate new absolute limit
    double limit = *min_value.base() + ((*max_value.base() - *min_value.base()) * relative_limit);

    return limit;
}

std::vector<std::pair<uint32_t, uint32_t> > IntonCore::intensityToSegments(std::vector<double> intensity, double threshold_absolute, double threshold_relative, uint32_t minimum_length)
{
    DEBUG("intensityToSegments %f, %f, %i", threshold_absolute, threshold_relative, minimum_length)

    std::vector<std::pair<uint32_t, uint32_t> > result;

    if (intensity.empty()) return result;

    bool got_it = false;
    uint32_t data_size = intensity.size() - 1;

    double threshold_current = threshold_absolute;

    uint32_t offset = 0;
    bool is_relative = false;

    uint32_t segment_lenght = 0;
    uint32_t start_relative_position = 0;
    uint32_t stop_relative_position = 0;
    for (uint32_t i=0; i<=data_size; i++)
    {
        double value = intensity.at(i);

        // Found segment end
        if (((value < threshold_current ||  i==data_size) && got_it)
            || (is_relative && i > stop_relative_position))
        {
            DEBUG("found end: %i relative %i %f", i, is_relative, threshold_current)

            // Add point
            if (is_relative && got_it && segment_lenght > minimum_length)
            {
                result.push_back(std::pair<int, int>(offset, segment_lenght));
            }

            got_it = false;

            // Calculate relative limit for 1st-gen segment
            if (!is_relative)
            {
                stop_relative_position = i;
                threshold_current = getRelativeLimitForSement(
                    start_relative_position,
                    stop_relative_position,
                    intensity,
                    threshold_relative
                );
                DEBUG("limit relative: %f", threshold_current)
                // rescan segment with new threshold
                i = start_relative_position;
                is_relative = true;
                got_it = false;
            }

            // Finish scan for 1st-gen segment, go find next one
            if (is_relative && i > stop_relative_position)
            {
                is_relative = false;
                start_relative_position = i;
                stop_relative_position = i;
                threshold_current = threshold_absolute;
                DEBUG("limit original: %f", threshold_current)
            }
        }

        //  It's segment, go on
        if (value > threshold_current && got_it)
        {
            segment_lenght++;
        }

        // Found segment start
        if (value > threshold_current && !got_it)
        {
            DEBUG("found start: %i relative %i %f", i, is_relative, threshold_current)
            if (!is_relative) start_relative_position = i;
            got_it = true;
            segment_lenght = 0;
            offset = i;
        }
    }

    return result;
    }

    std::vector<std::pair<uint32_t, uint32_t> > IntonCore::intensitySmoothedToSegments(std::vector<double> intensity,
                                                                            std::vector<double> intensity_smoothed,
                                                                            uint32_t minimumLength)
    {
        DEBUG("intensitySmoothedToSegments: %i, %i", intensity.size(), intensity_smoothed.size())

        std::vector<std::pair<uint32_t, uint32_t> > result;

        if (intensity.empty()) return result;

        bool got_it = false;
        uint32_t segment_start = 0;
        uint32_t segment_length = 0;
        double diff = 0.0;
        for (uint32_t i=0; i<intensity.size() && i<intensity_smoothed.size(); i++)
        {
            diff = intensity.at(i) - intensity_smoothed.at(i) - DATA_NORMALIZED_LIMIT;
            // Found segment end
            if (diff < 0 && got_it)
            {
                if (segment_length > minimumLength)
                {
                    result.push_back(std::pair<int, int>(segment_start, segment_length));
                }
                got_it = false;
            }
            // Continue segment
            else if (diff > 0 && got_it)
            {
                segment_length++;
            }
            // Found segment start
            else if (diff > 0 && !got_it)
            {
                got_it = true;
                segment_start = i;
                segment_length = 0;
            }
        }

        return result;
    }
