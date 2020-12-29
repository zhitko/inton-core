#include "helpers.h"

using namespace IntonCore;

Helpers::Helpers()
{

}

WaveFile * Helpers::makeSimpleWaveFileFromRawData(const std::string& path,
                                      const char *data,
                                      uint32_t chunkDataSize,
                                      uint16_t numberOfChannels,
                                      uint32_t sampleRate,
                                      uint16_t significantBitsPerSample,
                                      bool closeFile)
{
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
    if(closeFile) waveCloseFile(waveFile);
    return waveFile;
}

WaveFile * Helpers::openWaveFile(const std::string& path)
{
    WaveFile *waveFile = waveOpenFile(path.c_str());
    return waveFile;
}
