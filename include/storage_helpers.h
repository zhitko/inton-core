#ifndef STORAGE_HELPERS_H
#define STORAGE_HELPERS_H

#include "IntonCore_global.h"

#include <string>

#define RETURN_IF_EXIST(storage) \
    if ((storage).isExists()) return ((storage).getValue());
#define RETURN_VALUE_IF_EXIST(storage) \
    if ((storage).isExists()) return *((storage).getValue());

struct DistributionMoments
{
    long count;
    double mean;
    double variance;
    double skewness;
    double kurtosis;
};

#endif // STORAGE_HELPERS_H
