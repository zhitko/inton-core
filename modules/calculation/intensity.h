#ifndef INTENSITY_H
#define INTENSITY_H

#include <vector>
#include <cstdint>


namespace IntonCore {

std::vector<double> waveToIntensive(std::vector<double> wave, uint32_t frame, uint32_t shift);

}

#endif // INTENSITY_H
