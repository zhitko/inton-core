#include "pitch.h"

#include <cmath>
#include <algorithm>  // std::transform

#include "define.h"
#include "modules/wav_file/wavFile.h"

#include "SPTK/analysis/pitch_extraction.h"
#include "SPTK/utils/sptk_utils.h"

const sptk::PitchExtraction::Algorithms kDefaultAlgorithm(
    sptk::PitchExtraction::Algorithms::kRapt);

std::vector<double> IntonCore::waveToPitch(
        std::vector<double> waveform,
        uint32_t atype,
        uint32_t otype,
        uint32_t frame_shift,
        double thresh_rapt,
        double thresh_swipe,
        double sample_freq,
        double min_freq,
        double max_freq
)
{
    DEBUG("waveToPitch %i, %i", atype, otype)
    if (waveform.empty()) return std::vector<double>();

    sptk::PitchExtraction::Algorithms alg = static_cast<sptk::PitchExtraction::Algorithms>(atype);

    sptk::PitchExtraction::Algorithms algorithm(alg);
    std::vector<double> voicing_thresholds{
        kDefaultVoicingThresholdForRapt, kDefaultVoicingThresholdForSwipe,
        kDefaultVoicingThresholdForReaper, kDefaultVoicingThresholdForWorld,
    };

    const double sampling_rate_in_hz(1000.0 * sample_freq);

    DEBUG("waveToPitch frame_shift = %i", frame_shift)
    DEBUG("waveToPitch sampling_rate_in_hz = %f", sampling_rate_in_hz)
    DEBUG("waveToPitch min_freq = %f", min_freq)
    DEBUG("waveToPitch max_freq = %f", max_freq)
    DEBUG("waveToPitch voicing_thresholds = %f", voicing_thresholds[algorithm])
    DEBUG("waveToPitch algorithm = %i", algorithm)

    sptk::PitchExtraction pitch_extraction(
        frame_shift,
        sampling_rate_in_hz,
        min_freq,
        max_freq,
        voicing_thresholds[algorithm],
        algorithm
    );

    if (!pitch_extraction.IsValid()) {
        ERROR("Failed to initialize set PitchExtraction")
        return std::vector<double>();
    }

    std::vector<double> f0;
    if (!pitch_extraction.Run(waveform, &f0, NULL, NULL)) {
        ERROR("Failed to extract pitch")
        return std::vector<double>();
    }

    switch (otype) {
      case 0: {
        std::transform(f0.begin(), f0.end(), f0.begin(),
                       [sampling_rate_in_hz](double x) {
                         return (0.0 < x) ? sampling_rate_in_hz / x : 0.0;
                       });
        break;
      }
      case 1: {
        // nothing to do
        break;
      }
      case 2: {
        std::transform(f0.begin(), f0.end(), f0.begin(), [](double x) {
          return (0.0 < x) ? std::log(x) : sptk::kLogZero;
        });
        break;
      }
      default: { break; }
    }

    return f0;

}
