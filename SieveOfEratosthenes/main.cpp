#include <iostream>
#include <chrono>
#include <bitset>
#include <cmath>

#include "../CommonHeaders/Public/PerformanceCounter.h"

const uint32_t NUMBERS_TO_CHECK = 70000000;

template<uint32_t NumbersToCheck>
void FindCompositesUsingErato(std::bitset<NumbersToCheck>& Result)
{
    // zero and one is neither prime nor complex number
    uint32_t squareOfNumberToCheck = std::ceil(std::sqrt(NumbersToCheck));
    for (uint32_t BitIndex = 2; BitIndex <= squareOfNumberToCheck; ++BitIndex)
    {
        uint32_t NumberToCheck = BitIndex;
        if (!Result[BitIndex])
        {
            for (uint32_t multiply = NumberToCheck * NumberToCheck; multiply <= NumbersToCheck; multiply += NumberToCheck)
            {
                Result[multiply] = true;
            }
        }
    }
}

int main()
{
    PerformanceCounter PerfCounter;
    PerfCounter.Reset();

    // 8.34 MiB is a bit too much for stack allocation
    auto Result = std::make_unique<std::bitset<(NUMBERS_TO_CHECK + 1)>>();

    FindCompositesUsingErato<(NUMBERS_TO_CHECK + 1)>(*Result);

    std::printf("Total time: %fms\n", PerfCounter.Elapsed());

    uint64_t PrimesSum = 0;
    for (size_t BitIndex = 2; BitIndex <= Result->size(); ++BitIndex)
    {
        PrimesSum += !(*Result)[BitIndex] * BitIndex;
    }

    std::printf("Checksum: %llu (expected: 139601928199359)\n", PrimesSum);
    std::printf("Correct: %s\n", PrimesSum == 139601928199359lu ? "True" : "False");

    return 0;
}
