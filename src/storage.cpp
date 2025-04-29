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
    this->cleanWave();
    this->cleanIntensity();
    if (this->data_manual_segments_p.isExists())
        this->data_manual_segments_p.clear();
    if (this->data_manual_segments_t.isExists())
        this->data_manual_segments_t.clear();
    if (this->data_manual_segments_n.isExists())
        this->data_manual_segments_n.clear();
    this->cleanSegments();
    if (this->data_consonants_and_silence_length_distribution_moments.isExists())
        this->data_consonants_and_silence_length_distribution_moments.clear();
    if (this->data_silence_length_distribution_moments.isExists())
        this->data_silence_length_distribution_moments.clear();
    if (this->data_vowels_length_distribution_moments.isExists())
        this->data_vowels_length_distribution_moments.clear();
    this->cleanPitch();
}

std::string Storage::getFilePath()
{
    return this->file_path;
}

WaveFile* Storage::getWaveFile()
{
    RETURN_IF_EXIST(this->wave_file)

    DEBUG("Try to open file %s\n", file_path.c_str())
    WaveFile * wave = waveOpenFile(file_path.c_str());
    if (wave != nullptr) this->wave_file.setValue(wave);

    return wave;
}
