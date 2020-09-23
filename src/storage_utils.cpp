#include "storage.h"

#include "define.h"

#include "config.h"

using namespace IntonCore;

uint32_t Storage::convertIntensityPointsToWavePoints(uint32_t value)
{
    return value * this->config->intensityShift();
}

double Storage::convertIntensityPointsToSec(uint32_t value)
{
    return this->convertWavePointsToSec(
        this->convertIntensityPointsToWavePoints(value)
        );
}

double Storage::convertWavePointsToSec(uint32_t value)
{
    return (1.0 * value) / this->getWaveFrameRate();
}
