#include "intoncore.h"

#include "define.h"
#include "config.h"
#include "storage.h"

using namespace IntonCore;

Core::Core(const std::string& file_path, Config * config):
    config(nullptr), template_storage(nullptr), record_storage(nullptr)
{
    this->initialize_variables();
    this->initialize_config(config);
    this->initialize_template_file(file_path);
}

Core::~Core()
{
    if (this->template_storage != nullptr) delete this->template_storage;
    if (this->record_storage != nullptr) delete this->record_storage;
}

void Core::load_processed_file(const std::string &file_path)
{
    // TODO: Implement comparing logic
}

Storage *Core::getTemplate()
{
    return this->template_storage;
}

Storage *Core::getRecord()
{
    return this->record_storage;
}

Storage *Core::reloadTemplate(WaveFile *file)
{
    if (this->template_storage != nullptr)
    {
        delete this->template_storage;
    }

    return this->template_storage = new Storage(file, this->config);
}

Storage *Core::reloadTemplate(const std::string& file_path)
{
    if (this->template_storage != nullptr)
    {
        delete this->template_storage;
    }

    return this->template_storage = new Storage(file_path, this->config);
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
    this->template_storage = nullptr;
    this->record_storage = nullptr;
}

void Core::initialize_template_file(const std::string &file_path)
{
    this->template_storage = new Storage(file_path, this->config);
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

    if (is_changed && template_storage) {
        DEBUG("setIntensityConfig clean Pitch")
        if (template_storage != nullptr) template_storage->cleanPitch();
        if (record_storage != nullptr) record_storage->cleanPitch();
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

    if (is_changed && template_storage) {
        DEBUG("setIntensityConfig clean Intensity")
        if (template_storage != nullptr) template_storage->cleanIntensity();
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

    if (is_changed && template_storage) {
        DEBUG("setIntensityConfig clean Segments")
        if (template_storage != nullptr) template_storage->cleanSegments();
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

    if (is_changed && template_storage) {
        DEBUG("setIntensityConfig clean Segments")
        if (template_storage != nullptr) template_storage->cleanSegments();
        if (record_storage != nullptr) record_storage->cleanSegments();
    }
}
