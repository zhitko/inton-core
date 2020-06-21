#include "storage.h"

#include "define.h"

#include "modules/wav_file/wavFile.h"

using namespace IntonCore;

Storage::Storage(const std::string& file_path, Config *config):
    config(config), file_path(file_path)
{
    DEBUG("Create Storage for %s\n", file_path.c_str())
}

Storage::~Storage()
{
    DEBUG("Free Storage resources\n")
    if (this->wave_file.isExists())
    {
        DEBUG("Free WaveFIle\n")
        WaveFile * wave = this->wave_file.getValue();
        waveCloseFile(wave);
    }
}

WaveFile* Storage::getWaveFile()
{
    RETURN_IF_EXIST(this->wave_file)

    DEBUG("Try to open file %s\n", file_path.c_str())
    WaveFile * wave = waveOpenFile(file_path.c_str());
    this->wave_file.setValue(wave);

    return wave;
}

std::vector<double> Storage::getWave()
{
    RETURN_VALUE_IF_EXIST(this->data_wave)

    DEBUG("Get wave")

    WaveFile * wave_file = this->getWaveFile();

    if (!wave_file) return std::vector<double>();

    uint32_t size = littleEndianBytesToUInt32(wave_file->dataChunk->chunkDataSize);
    uint16_t bits = littleEndianBytesToUInt16(wave_file->formatChunk->significantBitsPerSample);

    std::vector<double> wave_data = waveformDataToVector(wave_file->dataChunk->waveformData, size, bits);

    this->data_wave.setValue(wave_data);

    return wave_data;
}

uint32_t Storage::getWaveFrameRate()
{
    // TODO: implement retrieve it from wave file
    return WAVE_FRAME_RATE;
}

std::vector<double> Storage::getWaveNormalized()
{
    RETURN_VALUE_IF_EXIST(this->data_wave_normalized)

    DEBUG("Get wave normalized")

    auto wave = this->getWave();

    if (wave.empty()) return std::vector<double>();

    auto wave_mormalized = normalizeVector(wave, WAVE_NORMALIZED_MIN, WAVE_NORMALIZED_MAX);

    this->data_wave_normalized.setValue(wave_mormalized);

    return wave_mormalized;
}
