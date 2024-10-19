#include "PerformanceCounter.h"

void PerformanceCounter::Reset()
{
    TimeStart = std::chrono::high_resolution_clock::now();
}

double PerformanceCounter::Elapsed() const
{
    const auto Duration = std::chrono::high_resolution_clock::now() - TimeStart;
    return static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(Duration).count()) * 1e-6 ;
}
