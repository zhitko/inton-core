#ifndef INTONCORE_H
#define INTONCORE_H

#include "IntonCore_global.h"
#include "config.h"
#include "storage.h"
#include "helpers.h"

#include <string>


namespace IntonCore {

class Core
{
public:
    Core(const std::string& file_path, Config * config = nullptr);
    virtual ~Core();

    void load_processed_file(const std::string& file_path);

    Storage * getTemplate();
    Storage * reloadTemplate(WaveFile * file);

    void setPitchConfig(uint32_t atype,
                        uint32_t otype,
                        uint32_t frame_shift,
                        double thresh_rapt,
                        double thresh_swipe,
                        double sample_freq,
                        double min_freq,
                        double max_freq);

private:
    void initialize_variables();
    void initialize_template_file(const std::string& file_path);
    void initialize_config(Config * config);
    Storage * initialize_store(Config * config, std::string wave_file_path);

protected:
    Config * config;
    Storage * template_storage;
};

}

#endif // INTONCORE_H
