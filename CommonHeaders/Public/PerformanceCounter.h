#pragma once

#include <chrono>

class PerformanceCounter {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> TimeStart;

public:
    PerformanceCounter()
        : TimeStart()
    {

    }

    void Reset();
    [[nodiscard]] double Elapsed() const;
};
