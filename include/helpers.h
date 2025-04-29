#ifndef HELPERS_H
#define HELPERS_H

#include "IntonCore_global.h"

#include "config.h"
#include "../modules/wav_file/wavFile.h"
#include <vector>

namespace IntonCore {

class Helpers
{
private:
    Helpers();

public:
    static WaveFile * makeSimpleWaveFileFromRawData(const std::string& path,
                                              const char *data,
                                              uint32_t chunkDataSize,
                                              uint16_t numberOfChannels,
                                              uint32_t sampleRate,
                                              uint16_t significantBitsPerSample,
                                              bool closeFile = true
                                              );
    static WaveFile * makeSimpleWaveFileFromVector(const std::string& path,
                                                   std::vector<double> data,
                                                   uint16_t numberOfChannels,
                                                   uint32_t sampleRate,
                                                   uint16_t significantBitsPerSample,
                                                   bool closeFile = true
                                                   );
    static WaveFile * openWaveFile(const std::string& path);
};

}

#endif // HELPERS_H
