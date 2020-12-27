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
