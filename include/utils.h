#ifndef UTILS_H
#define UTILS_H

#include <memory>
#include <string>
#include <stdexcept>
#include <vector>
#include <list>


namespace IntonCore {

std::vector<double> normalizeVector(std::vector<double> vector, double target_min = 0.0, double target_max = 1.0);
std::vector<double> normalizeVectorByOctaves(std::vector<double> vector, std::list<double> octaves);
std::vector<double> resizeVectorByMinMax(std::vector<double> vector, uint32_t target_size);
std::vector<double> linerSmoothVector(std::vector<double> vector, uint32_t frame);
std::vector<double> cutVectorBySegments(std::vector<double> vector, std::vector<std::pair<uint32_t,uint32_t>> segments, uint32_t segments_data_size);
std::vector<double> calculateVectorDerivative(std::vector<double> vector);
std::vector<double> reinforcedVectorFrequency(std::vector<double> vector, double k);
std::vector<double> highFrequencyReinforcedVectorFrequency(std::vector<double> vector, double k);
std::vector<double> lowFrequencyReinforcedVectorFrequency(std::vector<double> vector, double k);
std::vector<double> multiplyWaveVector(std::vector<double> vector, double k);
std::vector<double> fullWaveVectorCorrection(std::vector<double> vector, double hfk, double lfk, double sk, bool twice = false);
std::vector<double> waveVectorZeroCrossingCorrection(std::vector<double> vector, uint32_t frame_rate, double zero_crossing_target);
std::vector<double> dynamicWaveVectorCorrection(std::vector<double> vector, uint32_t frame_rate, double zero_crossing_target, double strength_target, int frame, int shift);
std::vector<double> dynamicWaveStrengthVectorCorrection(std::vector<double> vector, double strength_target, int frame, int shift);
double calculateVectorRMS(std::vector<double> data);
double calculateVectorZeroCrossing(std::vector<double> data, uint32_t frame_rate);
std::vector<double> calculateDynamicVectorZeroCrossing(std::vector<double> vector, uint32_t frame_rate, uint32_t frame, uint32_t shift);
std::vector<double> calculateDynamicVectorRMS(std::vector<double> vector, uint32_t frame, uint32_t shift);
double calculateVectorGravityCenterSubvector(std::vector<double> data, int from, int to);
double calculateVectorGravityCenter(std::vector<double> data);
std::tuple<double, double> findZeroCrossingK(
    std::vector<double> vector, uint32_t frame_rate, double zero_crossing_target,
    bool twice = false,
    double high_frequency_k = 0.0,
    double low_frequency_k = 0.0,
    int iteration = 0,
    double delta = 0.5
);

std::vector<uint32_t> segmentsToMask(std::vector<std::pair<uint32_t, uint32_t>> segments, uint32_t result_length);

std::vector<std::pair<uint32_t, uint32_t> > invertSegments(std::vector<std::pair<uint32_t, uint32_t> > segments);

uint32_t normalizeValue(uint32_t value, uint32_t source_size, uint32_t target_size);

template<class T>
T getMedianValue(std::vector<T> vector)
{
    if (vector.size() == 0)
        return T(0);

    sort(vector.begin(), vector.end());

    auto vector_size = vector.size();

    T result;

    if (vector_size % 2 == 0)
    {
        result = vector.at(vector_size / 2);
        result += vector.at((vector_size / 2) - 1);
        result /= 2;
    } else {
        result = vector.at((vector_size-1) / 2);
    }

    return result;
}

template<class T>
std::vector<T> resizeVector(std::vector<T> vector, uint32_t target_size)
{
    std::vector<T> result;

    if (vector.empty()) return result;

    auto scale = 1.0 * vector.size() / target_size;

    for (uint32_t i=0; i<target_size; i++)
    {
        result.push_back(vector.at(i*scale));
    }

    return result;
}

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    std::unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

//template<class T>
//std::vector<T> getSubvector(std::vector<T> vector, double from_percent, double to_percent, uint32_t total_length)
//{
//    int from = static_cast<int>(std::ceil(from_percent * total_length));
//    int to = static_cast<int>(std::ceil(to_percent * total_length));
//}

}

#endif // UTILS_H
