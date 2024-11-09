#include <iostream>
#include <chrono>
#include <bitset>
#include <cmath>

#include "PerformanceCounter.h"

const uint32_t NUMBERS_TO_CHECK = 70000000;
// const uint32_t NUMBERS_TO_CHECK = 25;

template<uint32_t NumbersToCheck>
void FindCompositesUsingErato(std::bitset<(NumbersToCheck - 1)/ 2>& Result)
{
    // zero and one is neither prime nor complex number
    uint32_t squareOfNumberToCheck = std::ceil(std::sqrt(NumbersToCheck));
    for (uint32_t BitIndex = 1; BitIndex <= squareOfNumberToCheck; ++BitIndex)
    {
        uint32_t NumberToCheck = 2 * BitIndex + 1;
        if (!Result[BitIndex])
        {
            for (uint32_t multiply = NumberToCheck * NumberToCheck; multiply < NumbersToCheck; multiply += 2 * NumberToCheck)
            {
                Result[(multiply - 1) / 2] = true;
            }
        }
    }


}

int main()
{
    PerformanceCounter PerfCounter;
    PerfCounter.Reset();

    // 8.34 MiB is a bit too much for stack allocation
    std::bitset<(NUMBERS_TO_CHECK - 1) / 2> Result;

    FindCompositesUsingErato<NUMBERS_TO_CHECK>(Result);

    std::printf("Total time: %fms\n", PerfCounter.Elapsed());

    // we are skipping even nubers so we need to add 2 to whole sum
    uint64_t PrimesSum = 2;
    for (size_t BitIndex = 1; BitIndex <= Result.size(); ++BitIndex)
    {
        PrimesSum += !Result[BitIndex] * (2 * BitIndex + 1);
    }

    std::printf("Checksum: %llu (expected: 139601928199359)\n", PrimesSum);
    std::printf("Correct: %s\n", PrimesSum == 139601928199359lu ? "True" : "False");

    return 0;
}
