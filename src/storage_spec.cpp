#include "storage.h"

#include "define.h"
#include "config.h"

#include "modules/calculation/spectrum.h"

using namespace IntonCore;


std::vector<double> Storage::getSpectrum()
{
    RETURN_VALUE_IF_EXIST(this->data_spectrum)

    DEBUG("Get spectrum 3D")

    auto data = this->getWaveCorrected();

    if (data.empty()) return std::vector<double>();

    auto result = waveToSpectrum(
        data,
        config->getSpectrumFrameLength(),
        config->getSpectrumFftLength(),
        config->getSpectrumOutputFormat(),
        config->getSpectrumEpsilon(),
        config->getSpectrumRelativeFloorInDecibels()
        );

    this->data_spectrum.setValue(result);

    return result;
}

std::vector<double> Storage::getSpectrumNormalized()
{
    RETURN_VALUE_IF_EXIST(this->data_spectrum_normalized)

    DEBUG("Get spectrum normalized")

    auto data = this->getSpectrum();

    if (data.empty()) return std::vector<double>();

    auto result = normalizeVector(data);

    this->data_spectrum_normalized.setValue(result);

    return result;
}


std::vector<double> Storage::getSpectrum2D()
{
    RETURN_VALUE_IF_EXIST(this->data_spectrum_2d)

    DEBUG("Get spectrum 2D")

    auto data = this->getSpectrum();

    if (data.empty()) return std::vector<double>();

    auto result = waveToSpectrum2D(
        data,
        config->getSpectrumFftLength()
    );

    this->data_spectrum_2d.setValue(result);

    return result;
}

std::vector<double> Storage::getSpectrum2DNormalized()
{
    RETURN_VALUE_IF_EXIST(this->data_spectrum_2d_normalized)

    DEBUG("Get spectrum 2D normalized")

    auto data = this->getSpectrum2D();

    if (data.empty()) return std::vector<double>();

    auto result = normalizeVector(data);

    this->data_spectrum_2d_normalized.setValue(result);

    return result;
}

double Storage::getSpectrum2DGravityCenter()
{
    RETURN_VALUE_IF_EXIST(this->data_spectrum_2d_gravity_center)

    DEBUG("Get spectrum 2D gravity center")

    auto data = this->getSpectrum2DNormalized();

    if (data.empty()) return 0.0;

    auto result = calculateVectorGravityCenter(data);

    this->data_spectrum_2d_gravity_center.setValue(result);

    return result;
}

double Storage::getSpectrum2DRMS()
{
    RETURN_VALUE_IF_EXIST(this->data_spectrum_2d_rms)

    DEBUG("Get spectrum 2D RMS")

    auto data = this->getSpectrum2DNormalized();

    if (data.empty()) return 0.0;

    auto result = calculateVectorRMS(data);

    this->data_spectrum_2d_rms.setValue(result);

    return result;
}

void Storage::cleanSpectrum()
{
    if (this->data_spectrum.isExists())
        this->data_spectrum.clear();
    if (this->data_spectrum_normalized.isExists())
        this->data_spectrum_normalized.clear();
    if (this->data_spectrum_2d.isExists())
        this->data_spectrum_2d.clear();
    if (this->data_spectrum_2d_normalized.isExists())
        this->data_spectrum_2d_normalized.clear();
    if (this->data_spectrum_2d_gravity_center.isExists())
        this->data_spectrum_2d_gravity_center.clear();
    if (this->data_spectrum_2d_rms.isExists())
        this->data_spectrum_2d_rms.clear();
}
