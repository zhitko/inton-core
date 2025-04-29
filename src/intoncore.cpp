#include "intoncore.h"

#include "define.h"
#include "config.h"
#include "storage.h"

using namespace IntonCore;

Core::Core(Config * config):
    config(nullptr), record_storage(nullptr)
{
    this->initialize_variables();
    this->initialize_config(config);
}

Core::Core(const std::string& file_path, Config * config):
    config(nullptr), record_storage(nullptr)
{
    this->initialize_variables();
    this->initialize_config(config);
    this->reloadTemplate(file_path);
}

Core::~Core()
{
    if (this->templates_storage.size() != 0) this->clean_templates();
    if (this->record_storage != nullptr) delete this->record_storage;
}

void Core::clean_templates()
{
    for (const auto& [key, value] : this->templates_storage)
    {
        delete value;
    }
    this->templates_storage.clear();
}

Storage *Core::getTemplate(const std::string& key)
{
    return this->templates_storage.at(key);
}

Storage *Core::popTemplate(const std::string& key)
{
    if (this->templates_storage.count(key) == 0) return nullptr;
    auto storage = this->templates_storage.extract(key);
    return storage.mapped();
}

void Core::setTemplate(Storage * storage, const std::string& key)
{
    if (storage == nullptr) return;
    if (this->templates_storage.count(key) != 0)
    {
        auto t = this->templates_storage.extract(key);
        delete t.mapped();
    }
    this->templates_storage[key] = storage;
}

Storage *Core::getRecord()
{
    return this->record_storage;
}

Storage *Core::popRecord()
{
    if (this->record_storage == nullptr) return nullptr;
    auto storage = this->record_storage;
    this->record_storage = nullptr;
    return storage;
}

void Core::setRecord(Storage * storage)
{
    if (storage == nullptr) return;
    if (this->record_storage != nullptr)
    {
        delete this->record_storage;
    }

    this->record_storage = storage;
}

Storage *Core::reloadTemplate(WaveFile *file, const std::string& key)
{
    if (this->templates_storage.count(key) != 0)
    {
        auto t = this->templates_storage.extract(key);
        delete t.mapped();
    }

    return this->templates_storage[key] = new Storage(file, this->config);
}

Storage *Core::reloadTemplate(const std::string& file_path, const std::string& key)
{
    if (this->templates_storage.count(key) != 0)
    {
        auto t = this->templates_storage.extract(key);
        delete t.mapped();
    }

    return this->templates_storage[key] = new Storage(file_path, this->config);
}

Storage *Core::reloadRecord(WaveFile *file)
{
    if (this->record_storage != nullptr)
    {
        delete this->record_storage;
    }

    return this->record_storage = new Storage(file, this->config);
}

Storage *Core::reloadRecord(const std::string& file_path)
{
    if (this->record_storage != nullptr)
    {
        delete this->record_storage;
    }

    return this->record_storage = new Storage(file_path, this->config);
}

void Core::initialize_variables()
{
    this->config = nullptr;
//    this->template_storage = nullptr;
    this->record_storage = nullptr;
}

void Core::initialize_config(Config * config)
{
    if (config)
    {
        this->config = config;
    }
    else
    {
        this->config = new Config();
    }
}

void Core::setWaveConfig(
    double wave_correction_high_frequency,
    double wave_correction_low_frequency,
    double wave_correction_strength,
    double wave_dynamic_zero_crossing_frame_sec,
    double wave_dynamic_zero_crossing_shift_sec,
    double wave_dynamic_zero_crossing_target,
    double wave_dynamic_strength_target,
    int wave_correction_type
) {
    if (this->config == nullptr)
    {
        initialize_config(nullptr);
    }

    auto is_changed = false;

    DEBUG("setWaveCorrectionHighFrequency %f", wave_correction_high_frequency)
    is_changed = this->config->setWaveCorrectionHighFrequency(wave_correction_high_frequency) || is_changed;
    DEBUG("setWaveCorrectionLowFrequency %f", wave_correction_low_frequency)
    is_changed = this->config->setWaveCorrectionLowFrequency(wave_correction_low_frequency) || is_changed;
    DEBUG("setWaveCorrectionStrength %f", wave_correction_strength)
    is_changed = this->config->setWaveCorrectionStrength(wave_correction_strength) || is_changed;
    DEBUG("setWaveDynamicZeroCrossingFrameSec %f", wave_dynamic_zero_crossing_frame_sec)
    is_changed = this->config->setWaveDynamicZeroCrossingFrameSec(wave_dynamic_zero_crossing_frame_sec) || is_changed;
    DEBUG("setWaveDynamicZeroCrossingShiftSec %f", wave_dynamic_zero_crossing_shift_sec)
    is_changed = this->config->setWaveDynamicZeroCrossingShiftSec(wave_dynamic_zero_crossing_shift_sec) || is_changed;
    DEBUG("setWaveDynamicZeroCrossingTarget %f", wave_dynamic_zero_crossing_target)
    is_changed = this->config->setWaveDynamicZeroCrossingTarget(wave_dynamic_zero_crossing_target) || is_changed;
    DEBUG("setWaveDynamicStrengthTarget %f", wave_dynamic_strength_target)
    is_changed = this->config->setWaveDynamicStrengthTarget(wave_dynamic_strength_target) || is_changed;
    DEBUG("setWaveCorrectionType %i", wave_correction_type)
    is_changed = this->config->setWaveCorrectionType(wave_correction_type) || is_changed;

    if (is_changed) {
        DEBUG("setIntensityConfig clean Wave")
        if (templates_storage.size() > 0)
        {
            for (const auto& [key, value] : this->templates_storage)
            {
                value->cleanWave();
            }
        }
        if (record_storage != nullptr) record_storage->cleanWave();
    }
}

void Core::setPitchConfig(
        uint32_t atype,
        uint32_t otype,
        uint32_t frame_shift,
        double thresh_rapt,
        double thresh_swipe,
        double sample_freq,
        double min_freq,
        double max_freq)
{
    if (this->config == nullptr)
    {
        initialize_config(nullptr);
    }

    auto is_changed = false;

    DEBUG("setPitchAlgorithmType %i", atype)
    is_changed = this->config->setPitchAlgorithmType(atype) || is_changed;
    DEBUG("setPitchOutputType %i", otype)
    is_changed = this->config->setPitchOutputType(otype) || is_changed;
    DEBUG("setPitchFrameShift %i", frame_shift)
    is_changed = this->config->setPitchFrameShift(frame_shift) || is_changed;
    DEBUG("setPitchThresholdForRapt %f", thresh_rapt)
    is_changed = this->config->setPitchThresholdForRapt(thresh_rapt) || is_changed;
    DEBUG("setPitchThresholdForSwipe %f", thresh_swipe)
    is_changed = this->config->setPitchThresholdForSwipe(thresh_swipe) || is_changed;
    DEBUG("setPitchSamplingFrequency %f", sample_freq)
    is_changed = this->config->setPitchSamplingFrequency(sample_freq) || is_changed;
    DEBUG("setPitchMinFrequency %f", min_freq)
    is_changed = this->config->setPitchMinFrequency(min_freq) || is_changed;
    DEBUG("setPitchMaxFrequency %f", max_freq)
    is_changed = this->config->setPitchMaxFrequency(max_freq) || is_changed;

    if (is_changed) {
        DEBUG("setIntensityConfig clean Pitch")
        if (templates_storage.size() > 0)
        {
            for (const auto& [key, value] : this->templates_storage)
            {
                value->cleanPitch();
            }
        }
        if (record_storage != nullptr) record_storage->cleanPitch();
    }
}

void Core::setSpectrumConfig(uint32_t frame_length,
                             uint32_t fft_length,
                             uint32_t output_format)
{
    if (this->config == nullptr)
    {
        initialize_config(nullptr);
    }

    auto is_changed = false;

    DEBUG("setIntensityConfig %i", frame_length)
    is_changed = this->config->setSpectrumFrameLength(frame_length) || is_changed;
    DEBUG("setIntensityConfig %i", fft_length)
    is_changed = this->config->setSpectrumFftLength(fft_length) || is_changed;
    DEBUG("setIntensityConfig %i", output_format)
    is_changed = this->config->setSpectrumOutputFormat(output_format) || is_changed;

    if (is_changed) {
        DEBUG("setSpectrumConfig clean Spectrum")
        if (templates_storage.size() > 0)
        {
            for (const auto& [key, value] : this->templates_storage)
            {
                value->cleanSpectrum();
            }
        }
        if (record_storage != nullptr) record_storage->cleanSpectrum();
    }
}

void Core::setIntensityConfig(uint32_t frame,
                              uint32_t shift,
                              uint32_t smooth_frame,
                              uint32_t double_smooth_frame)
{
    if (this->config == nullptr)
    {
        initialize_config(nullptr);
    }

    auto is_changed = false;

    DEBUG("setIntensityConfig %i", frame)
    is_changed = this->config->setIntensityFrame(frame) || is_changed;
    DEBUG("setIntensityConfig %i", shift)
    is_changed = this->config->setIntensityShift(shift) || is_changed;
    DEBUG("setIntensityConfig %i", smooth_frame)
    is_changed = this->config->setIntensitySmoothFrame(smooth_frame) || is_changed;
    DEBUG("setIntensityConfig %i", double_smooth_frame)
    is_changed = this->config->setIntensityDoubleSmoothFrame(double_smooth_frame) || is_changed;

    if (is_changed) {
        DEBUG("setIntensityConfig clean Intensity")
        if (templates_storage.size() > 0)
        {
            for (const auto& [key, value] : this->templates_storage)
            {
                value->cleanIntensity();
            }
        }
        if (record_storage != nullptr) record_storage->cleanIntensity();
    }
}

void Core::setSegmentsConfig(uint32_t segments_limit)
{
    if (this->config == nullptr)
    {
        initialize_config(nullptr);
    }

    auto is_changed = false;

    DEBUG("setSegmentsConfig %i", segments_limit)
    is_changed = this->config->setSegmentsByIntensityMinimumLength(segments_limit) || is_changed;

    if (is_changed) {
        DEBUG("setIntensityConfig clean Segments")
        if (templates_storage.size() > 0)
        {
            for (const auto& [key, value] : this->templates_storage)
            {
                value->cleanSegments();
            }
        }
        if (record_storage != nullptr) record_storage->cleanSegments();
    }
}

void Core::setOctavesConfig(std::list<double> octaves)
{
    if (this->config == nullptr)
    {
        initialize_config(nullptr);
    }

    auto is_changed = false;


    DEBUG("setIntensityConfig %lu", octaves.size())
    is_changed = this->config->setOctaves(octaves) || is_changed;

    if (is_changed) {
        DEBUG("setIntensityConfig clean Segments")
        if (templates_storage.size() > 0)
        {
            for (const auto& [key, value] : this->templates_storage)
            {
                value->cleanSegments();
            }
        }
        if (record_storage != nullptr) record_storage->cleanSegments();
    }
}
