#include "storage.h"

#include "define.h"

#include "modules/wav_file/wavFile.h"

using namespace IntonCore;

Storage::Storage(const std::string& file_path, Config *config):
    config(config), file_path(file_path)
{
    DEBUG("Create Storage for %s\n", file_path.c_str())
}

Storage::Storage(WaveFile *file, Config *config):
    config(config)
{
    this->file_path = file->filePath;
    this->wave_file.setValue(file);
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
    this->clear();
}

void Storage::clear()
{
    if (this->data_wave.isExists())
        this->data_wave.clear();
    if (this->data_wave_normalized.isExists())
        this->data_wave_normalized.clear();
    if (this->data_intensity.isExists())
        this->data_intensity.clear();
    if (this->data_intensity_normalized.isExists())
        this->data_intensity_normalized.clear();
    if (this->data_intensity_normalized_smoothed.isExists())
        this->data_intensity_normalized_smoothed.clear();
    if (this->data_intensity_normalized_double_smoothed.isExists())
        this->data_intensity_normalized_double_smoothed.clear();
    if (this->data_manual_segments_p.isExists())
        this->data_manual_segments_p.clear();
    if (this->data_manual_segments_t.isExists())
        this->data_manual_segments_t.clear();
    if (this->data_manual_segments_n.isExists())
        this->data_manual_segments_n.clear();
    if (this->data_auto_segments_by_intensity.isExists())
        this->data_auto_segments_by_intensity.clear();
    if (this->data_auto_segments_by_intensity_mask.isExists())
        this->data_auto_segments_by_intensity_mask.clear();
    if (this->data_auto_segments_by_intensity_inverted.isExists())
        this->data_auto_segments_by_intensity_inverted.clear();
    if (this->data_auto_segments_by_intensity_smoothed.isExists())
        this->data_auto_segments_by_intensity_smoothed.clear();
    if (this->data_auto_segments_by_intensity_smoothed_mask.isExists())
        this->data_auto_segments_by_intensity_smoothed_mask.clear();
    if (this->data_auto_segments_by_intensity_smoothed_inverted.isExists())
        this->data_auto_segments_by_intensity_smoothed_inverted.clear();
    if (this->data_auto_segments_by_intensity_double_smoothed.isExists())
        this->data_auto_segments_by_intensity_double_smoothed.clear();
    if (this->data_auto_segments_by_intensity_double_smoothed_mask.isExists())
        this->data_auto_segments_by_intensity_double_smoothed_mask.clear();
    if (this->data_auto_segments_by_intensity_double_smoothed_inverted.isExists())
        this->data_auto_segments_by_intensity_double_smoothed_inverted.clear();
    if (this->data_consonants_and_silence_length_distribution_moments.isExists())
        this->data_consonants_and_silence_length_distribution_moments.clear();
    if (this->data_silence_length_distribution_moments.isExists())
        this->data_silence_length_distribution_moments.clear();
    if (this->data_vowels_length_distribution_moments.isExists())
        this->data_vowels_length_distribution_moments.clear();
    if (this->data_pitch.isExists())
        this->data_pitch.clear();
}

WaveFile* Storage::getWaveFile()
{
    RETURN_IF_EXIST(this->wave_file)

    DEBUG("Try to open file %s\n", file_path.c_str())
    WaveFile * wave = waveOpenFile(file_path.c_str());
    if (wave != nullptr) this->wave_file.setValue(wave);

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
