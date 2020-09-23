#include "storage.h"

#include "define.h"

#include "config.h"

#include "3party/alglib-3.16.0/src/ap.h"
#include "3party/alglib-3.16.0/src/statistics.h"

using namespace IntonCore;

DistributionMoments calculateDistributionMoments(std::vector<std::pair<uint32_t, uint32_t> > segments)
{
    DEBUG("Calculate DistributionMoments")

    DistributionMoments distributionMoments;

    distributionMoments.count = segments.size();

    DEBUG("DistributionMoments set vector values")
    alglib::real_1d_array x;
    x.setlength(distributionMoments.count);
    for(long i=0; i<distributionMoments.count; i++)
    {
        x[i] = segments.at(i).second;
    }

    DEBUG("DistributionMoments calculation")
    alglib::samplemoments(
        x,
        distributionMoments.count,
        distributionMoments.mean,
        distributionMoments.variance,
        distributionMoments.skewness,
        distributionMoments.kurtosis
    );

    return distributionMoments;
}

long Storage::getConsonantsAndSilenceCount()
{
    if (this->data_consonants_and_silence_length_distribution_moments.isExists())
        return this->data_consonants_and_silence_length_distribution_moments.getValue()->count;

    DEBUG("Calculate metrics consonants and silence count")

    auto segments = this->getAutoSegmentsByIntensitySmoothedInverted();

    if (segments.empty()) return 0;

    auto distributionMoments = calculateDistributionMoments(segments);

    this->data_consonants_and_silence_length_distribution_moments.setValue(distributionMoments);

    return distributionMoments.count;
}

double Storage::getConsonantsAndSilenceLengthMean()
{
    if (this->data_consonants_and_silence_length_distribution_moments.isExists())
        return this->data_consonants_and_silence_length_distribution_moments.getValue()->mean;

    DEBUG("Calculate metrics consonants and silence mean")

    auto segments = this->getAutoSegmentsByIntensitySmoothedInverted();

    if (segments.empty()) return 0;

    auto distributionMoments = calculateDistributionMoments(segments);

    this->data_consonants_and_silence_length_distribution_moments.setValue(distributionMoments);

    return distributionMoments.mean;
}

double Storage::getConsonantsAndSilenceLengthVariance()
{
    if (this->data_consonants_and_silence_length_distribution_moments.isExists())
        return this->data_consonants_and_silence_length_distribution_moments.getValue()->variance;

    DEBUG("Calculate metrics consonants and silence variance")

    auto segments = this->getAutoSegmentsByIntensitySmoothedInverted();

    if (segments.empty()) return 0;

    auto distributionMoments = calculateDistributionMoments(segments);

    this->data_consonants_and_silence_length_distribution_moments.setValue(distributionMoments);

    return distributionMoments.variance;
}

double Storage::getConsonantsAndSilenceLengthSkewness()
{
    if (this->data_consonants_and_silence_length_distribution_moments.isExists())
        return this->data_consonants_and_silence_length_distribution_moments.getValue()->skewness;

    DEBUG("Calculate metrics consonants and silence skewness")

    auto segments = this->getAutoSegmentsByIntensitySmoothedInverted();

    if (segments.empty()) return 0;

    auto distributionMoments = calculateDistributionMoments(segments);

    this->data_consonants_and_silence_length_distribution_moments.setValue(distributionMoments);

    return distributionMoments.skewness;
}

double Storage::getConsonantsAndSilenceLengthKurtosis()
{
    if (this->data_consonants_and_silence_length_distribution_moments.isExists())
        return this->data_consonants_and_silence_length_distribution_moments.getValue()->kurtosis;

    DEBUG("Calculate metrics consonants and silence kurtosis")

    auto segments = this->getAutoSegmentsByIntensitySmoothedInverted();

    if (segments.empty()) return 0;

    auto distributionMoments = calculateDistributionMoments(segments);

    this->data_consonants_and_silence_length_distribution_moments.setValue(distributionMoments);

    return distributionMoments.kurtosis;
}

long Storage::getVowelsCount()
{
    if (this->data_vowels_length_distribution_moments.isExists())
        return this->data_vowels_length_distribution_moments.getValue()->count;

    DEBUG("Calculate metrics vowels count")

    auto segments = this->getAutoSegmentsByIntensitySmoothed();

    if (segments.empty()) return 0;

    auto distributionMoments = calculateDistributionMoments(segments);

    this->data_vowels_length_distribution_moments.setValue(distributionMoments);

    return distributionMoments.count;
}

double Storage::getVowelsLengthMean()
{
    if (this->data_vowels_length_distribution_moments.isExists())
        return this->data_vowels_length_distribution_moments.getValue()->mean;

    DEBUG("Calculate metrics vowels mean")

    auto segments = this->getAutoSegmentsByIntensitySmoothed();

    if (segments.empty()) return 0;

    auto distributionMoments = calculateDistributionMoments(segments);

    this->data_vowels_length_distribution_moments.setValue(distributionMoments);

    return distributionMoments.mean;
}

double Storage::getVowelsLengthVariance()
{
    if (this->data_vowels_length_distribution_moments.isExists())
        return this->data_vowels_length_distribution_moments.getValue()->variance;

    DEBUG("Calculate metrics vowels variance")

    auto segments = this->getAutoSegmentsByIntensitySmoothed();

    if (segments.empty()) return 0;

    auto distributionMoments = calculateDistributionMoments(segments);

    this->data_vowels_length_distribution_moments.setValue(distributionMoments);

    return distributionMoments.variance;
}

double Storage::getVowelsLengthSkewness()
{
    if (this->data_vowels_length_distribution_moments.isExists())
        return this->data_vowels_length_distribution_moments.getValue()->skewness;

    DEBUG("Calculate metrics vowels skewness")

    auto segments = this->getAutoSegmentsByIntensitySmoothed();

    if (segments.empty()) return 0;

    auto distributionMoments = calculateDistributionMoments(segments);

    this->data_vowels_length_distribution_moments.setValue(distributionMoments);

    return distributionMoments.skewness;
}

double Storage::getVowelsLengthKurtosis()
{
    if (this->data_vowels_length_distribution_moments.isExists())
        return this->data_vowels_length_distribution_moments.getValue()->kurtosis;

    DEBUG("Calculate metrics vowels kurtosis")

    auto segments = this->getAutoSegmentsByIntensitySmoothed();

    if (segments.empty()) return 0;

    auto distributionMoments = calculateDistributionMoments(segments);

    this->data_vowels_length_distribution_moments.setValue(distributionMoments);

    return distributionMoments.kurtosis;
}
