#ifndef MANUAL_SEGMENTS_H
#define MANUAL_SEGMENTS_H

#include <vector>

struct WaveFile;

namespace IntonCore {

std::vector<std::pair<int, int> > waveFileToManualSegmants(WaveFile* waveFile, char marker);

}

#endif // MANUAL_SEGMENTS_H
