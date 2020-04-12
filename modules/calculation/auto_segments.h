#ifndef AUTO_SEGMENTS_H
#define AUTO_SEGMENTS_H

#include <vector>
#include <cstdint>


namespace IntonCore {

std::vector<std::pair<int, int> > intensityToSegments(
    std::vector<double> intensity,
    double threshold_absolute,
    double threshold_relative,
    uint32_t minimum_length
    );

std::vector<std::pair<int, int> > intensitySmoothedToSegments(
    std::vector<double> intensity,
    std::vector<double> intensity_smoothed,
    uint32_t minimumLength
    );

}

#endif // AUTO_SEGMENTS_H
