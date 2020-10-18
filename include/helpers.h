#ifndef HELPERS_H
#define HELPERS_H

#include "IntonCore_global.h"

#include "config.h"

namespace IntonCore {

class Helpers
{
private:
    Helpers();

public:
    static void makeSimpleWaveFileFromRawData(const std::string& path,
                                              const char *data,
                                              uint32_t chunkDataSize,
                                              uint16_t numberOfChannels,
                                              uint32_t sampleRate,
                                              uint16_t significantBitsPerSample
                                              );
};

}

#endif // HELPERS_H
