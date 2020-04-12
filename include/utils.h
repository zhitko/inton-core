#ifndef UTILS_H
#define UTILS_H

#include <memory>
#include <string>
#include <stdexcept>
#include <vector>


namespace IntonCore {

std::vector<double> normalizeVector(std::vector<double> vector, double target_min = 0.0, double target_max = 1.0);
std::vector<double> resizeVectorByMinMax(std::vector<double> vector, uint32_t target_size);
std::vector<double> linerSmoothVector(std::vector<double> vector, uint32_t frame);

std::vector<int> segmentsToMask(std::vector<std::pair<int, int>> segments, uint32_t result_length);

std::vector<std::pair<int, int> > invertSegments(std::vector<std::pair<int, int> > segments);

int normalizeValue(int value, int source_size, int target_size);

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
