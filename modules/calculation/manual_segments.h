#ifndef MANUAL_SEGMENTS_H
#define MANUAL_SEGMENTS_H

#include <vector>
#include <stdint.h>

struct WaveFile;

namespace IntonCore {

std::vector<std::pair<uint32_t, uint32_t> > waveFileToManualSegmants(WaveFile* waveFile, char marker);

}

#endif // MANUAL_SEGMENTS_H
