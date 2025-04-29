#ifndef INTONCORE_H
#define INTONCORE_H

#define DEFAULT_KEY "default"

#include "IntonCore_global.h"
#include "config.h"
#include "storage.h"
#include "helpers.h"

#include <string>
#include <list>
#include <map>
#include <string>


namespace IntonCore {

class Core
{
public:
    Core(Config * config = nullptr);
    Core(const std::string& file_path, Config * config = nullptr);
    virtual ~Core();

    Storage * getTemplate(const std::string& key = DEFAULT_KEY);
    Storage * popTemplate(const std::string& key = DEFAULT_KEY);
    void setTemplate(Storage * storage, const std::string& key = DEFAULT_KEY);
    Storage * reloadTemplate(WaveFile * file, const std::string& key = DEFAULT_KEY);
    Storage * reloadTemplate(const std::string& file_path, const std::string& key = DEFAULT_KEY);

    Storage * getRecord();
    Storage * popRecord();
    void setRecord(Storage * storage);
    Storage * reloadRecord(WaveFile * file);
    Storage * reloadRecord(const std::string& file_path);

    void setWaveConfig(double wave_correction_high_frequency,
                       double wave_correction_low_frequency,
                       double wave_correction_strength,
                       double wave_dynamic_zero_crossing_frame_sec,
                       double wave_dynamic_zero_crossing_shift_sec,
                       double wave_dynamic_zero_crossing_target,
                       double wave_dynamic_strength_target,
                       int wave_correction_type);

    void setPitchConfig(uint32_t atype,
                        uint32_t otype,
                        uint32_t frame_shift,
                        double thresh_rapt,
                        double thresh_swipe,
                        double sample_freq,
                        double min_freq,
                        double max_freq);

    void setIntensityConfig(uint32_t frame,
                            uint32_t shift,
                            uint32_t smooth_frame,
                            uint32_t double_smooth_frame);

    void setSpectrumConfig(uint32_t frame_length,
                           uint32_t fft_length,
                           uint32_t output_format);

    void setSegmentsConfig(uint32_t segments_limit);
    void setOctavesConfig(std::list<double> octaves);

private:
    void initialize_variables();
    void initialize_config(Config * config);
    Storage * initialize_store(Config * config, std::string wave_file_path);
    void clean_templates();

protected:
    Config * config;
    Storage * record_storage;
    std::map<std::string, Storage *> templates_storage;
};

}

#endif // INTONCORE_H
