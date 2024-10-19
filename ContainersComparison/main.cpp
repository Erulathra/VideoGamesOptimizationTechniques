#include <chrono>
#include <thread>

#include "PerformanceCounter.h"

int32_t main(int argc, char **argv)
{
    PerformanceCounter performanceCounter;
    performanceCounter.Reset();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::printf("Elapsed: %f", performanceCounter.Elapsed());
}