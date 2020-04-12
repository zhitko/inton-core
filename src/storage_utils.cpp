#include "storage.h"

#include "define.h"

#include "config.h"

using namespace IntonCore;

int Storage::convertIntensityPointsToWavePoints(int value)
{
    return value * this->config->intensityShift();
}

double Storage::convertIntensityPointsToSec(int value)
{
    return this->convertWavePointsToSec(
        this->convertIntensityPointsToWavePoints(value)
    );
}

double Storage::convertWavePointsToSec(int value)
{
    return 1.0 * value / this->getWaveFrameRate();
}
