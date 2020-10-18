#include "helpers.h"

#include "modules/wav_file/wavFile.h"

using namespace IntonCore;

Helpers::Helpers()
{

}

void Helpers::makeSimpleWaveFileFromRawData(const std::string& path,
                                      const char *data,
                                      uint32_t chunkDataSize,
                                      uint16_t numberOfChannels,
                                      uint32_t sampleRate,
                                      uint16_t significantBitsPerSample){
    WaveFile *waveFile = makeWaveFileFromRawData(
                (char *)data,
                chunkDataSize,
                numberOfChannels,
                sampleRate,
                significantBitsPerSample,
                0,
                nullptr,
                nullptr,
                nullptr
    );
    saveWaveFile(waveFile, path.c_str());
    waveCloseFile(waveFile);
}
