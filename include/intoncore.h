#ifndef INTONCORE_H
#define INTONCORE_H

#include "IntonCore_global.h"
#include "config.h"
#include "storage.h"
#include "helpers.h"

#include <string>
#include <list>


namespace IntonCore {

class Core
{
public:
    Core(const std::string& file_path, Config * config = nullptr);
    virtual ~Core();

    void load_processed_file(const std::string& file_path);

    Storage * getTemplate();
    Storage * reloadTemplate(WaveFile * file);
    Storage * reloadTemplate(const std::string& file_path);
    Storage * getRecord();
    Storage * reloadRecord(WaveFile * file);
    Storage * reloadRecord(const std::string& file_path);

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

    void setSegmentsConfig(uint32_t segments_limit);
    void setOctavesConfig(std::list<double> octaves);

private:
    void initialize_variables();
    void initialize_template_file(const std::string& file_path);
    void initialize_config(Config * config);
    Storage * initialize_store(Config * config, std::string wave_file_path);

protected:
    Config * config;
    Storage * record_storage;
    Storage * template_storage;
};

}

#endif // INTONCORE_H
