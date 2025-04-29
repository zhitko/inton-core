#include "spectrum.h"

#include "define.h"
#include "math.h"

#include "SPTK/conversion/waveform_to_spectrum.h"
#include "SPTK/window/data_windowing.h"
#include "SPTK/window/standard_window.h"


std::vector<double> IntonCore::waveToSpectrum(
        std::vector<double> waveform,
        uint32_t frame_length,
        uint32_t fft_length,
        uint32_t output_format,
        double epsilon,
        double relative_floor_in_decibels
)
{
    DEBUG("waveToSpectrum %i, %i, %i, %f, %f", frame_length, fft_length, output_format, epsilon, relative_floor_in_decibels)
    if (waveform.empty()) return std::vector<double>();

    if (frame_length > fft_length) frame_length = fft_length;

    // --- StandardWindow init ---

    auto window_type = sptk::StandardWindow::kHamming;
    auto normalization_type = sptk::DataWindowing::NormalizationType::kPower;

    sptk::StandardWindow standard_window(frame_length, window_type, false);
    sptk::DataWindowing data_windowing(&standard_window, frame_length, normalization_type);

    if (!data_windowing.IsValid()) {
        ERROR("Failed to initialize set DataWindowing")
        return std::vector<double>();
    }

    // --- WaveformToSpectrum init ---

    sptk::SpectrumToSpectrum::InputOutputFormats of = static_cast<sptk::SpectrumToSpectrum::InputOutputFormats>(output_format);
    sptk::SpectrumToSpectrum::InputOutputFormats output_format_enum(of);

    sptk::WaveformToSpectrum waveform_to_spectrum(
        frame_length,
        fft_length,
        output_format_enum,
        epsilon,
        relative_floor_in_decibels
    );

    sptk::WaveformToSpectrum::Buffer buffer;
    if (!waveform_to_spectrum.IsValid()) {
        ERROR("Failed to initialize set WaveformToSpectrum")
        return std::vector<double>();
    }

    const int output_length(fft_length / 2 + 1);
    std::vector<double> output(output_length);

    // --- StandardWindow run ---

    int parts = waveform.size() / frame_length;

    std::vector<double> result(parts * output_length);

    for (auto i = result.begin(); i != result.end(); i++) {
        *i = 0;
    }

    for (int it=0; it<parts; it++)
    {
        std::vector<double> data_sequence(
                    waveform.begin() + it*frame_length,
                    waveform.begin() + it*frame_length + frame_length
        );
        std::vector<double> windowed_data_sequence(fft_length);

        if (!data_windowing.Run(data_sequence, &windowed_data_sequence)) {
            WARN("Failed to run DataWindowing")
            continue;
        }

        // --- WaveformToSpectrum run ---

        if (!waveform_to_spectrum.Run(windowed_data_sequence, &output, &buffer)) {
            WARN("Failed to run WaveformToSpectrum")
            continue;
        }

        for (auto i = 0; i < output.size(); i++) {
            if (!isinf(output.at(i))) {
                result.at(i + it*output_length) = output.at(i);
            }
        }
    }

    return result;
}


std::vector<double> IntonCore::waveToSpectrum2D(
    std::vector<double> spectrum,
    uint32_t fft_length
    )
{
    DEBUG("waveToSpectrum2D %i", fft_length)
    if (spectrum.empty()) return std::vector<double>();

    const int output_length(fft_length / 2 + 1);

    std::vector<double> result(output_length);

    for (auto i = result.begin(); i != result.end(); i++) {
        *i = 0.0;
    }

    int parts = spectrum.size() / output_length;

    for (int it=0; it<parts; it++)
    {
        for (auto i = 0; i < output_length; i++) {
            int pos = it*output_length+i;
            if (!isinf(spectrum.at(pos))) {
                result.at(i) += spectrum.at(pos);
            }
        }
    }

    for (auto i = 0; i < result.size(); i++) {
        result.at(i) /= parts;
    }

    return result;
}
