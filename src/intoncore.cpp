#include "intoncore.h"

#include "define.h"
#include "config.h"
#include "storage.h"

using namespace IntonCore;

Core::Core(const std::string& file_path, Config * config):
    config(nullptr), template_storage(nullptr)
{
    this->initialize_variables();
    this->initialize_config(config);
    this->initialize_template_file(file_path);
}

Core::~Core()
{
    delete this->template_storage;
}

void Core::load_processed_file(const std::string &file_path)
{
    // TODO: Implement comparing logic
}

Storage *Core::getTemplate()
{
    return this->template_storage;
}

Storage *Core::reloadTemplate(WaveFile *file)
{
    if (this->template_storage != nullptr)
    {
        delete this->template_storage;
    }

    return this->template_storage = new Storage(file, this->config);
}

void Core::initialize_variables()
{
    this->config = nullptr;
    this->template_storage = nullptr;
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
