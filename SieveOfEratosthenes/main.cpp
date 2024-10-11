#include <iostream>
#include <chrono>
#include <bitset>
#include <cmath>

const uint32_t NUMBERS_TO_CHECK = 70000000;
const uint32_t L1_CACHE_SIZE = 32768;

template<uint32_t NumbersToCheck>
void FindCompositesUsingErato(std::bitset<NumbersToCheck>& Result)
{
    // zero and one is neighter prime or complex number
    uint32_t squareOfNumberToCheck = std::ceil(std::sqrt(NumbersToCheck));
    for (uint32_t BitIndex = 0; BitIndex <= squareOfNumberToCheck; ++BitIndex)
    {
        uint32_t NumberToCheck = BitIndex * 2 + 1;
        if (!Result[BitIndex])
        {
            for (uint32_t complexNumber = NumberToCheck * NumberToCheck; complexNumber <= NumbersToCheck; complexNumber += NumberToCheck)
            {
                Result[(complexNumber - 1) * 0.5] = true;
            }
        }
    }
}

template<uint32_t NumbersToCheck, uint64_t CacheSize>
void FindCompositesUsingSegmentedErato(std::bitset<NumbersToCheck>& Result)
{
    uint32_t squareOfNumberToCheck = std::ceil(std::sqrt(NumbersToCheck));
    uint32_t segmentSize = std::min(squareOfNumberToCheck, CacheSize);

    uint32_t numSegements = std::ceil()

    for (int i = 0; i < MAX; ++i)
    {
        
    }
}

int main()
{
    const auto StartTime = std::chrono::high_resolution_clock::now();

    // 8.34 MiB is a bit to much for stack allocation
    auto Result = std::make_unique<std::bitset<(NUMBERS_TO_CHECK + 1)>>();

    FindCompositesUsingErato<(NUMBERS_TO_CHECK + 1)>(*Result);

    const std::chrono::duration<double> ProcessingTime = std::chrono::high_resolution_clock::now() - StartTime;
    const double ProcessingSeconds = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(ProcessingTime).count()) * 10e-3;
    std::printf("Total time: %fs\n", ProcessingSeconds);

    uint64_t PrimesSum = 0;
    for (size_t BitIndex = 0; BitIndex <= Result->size(); ++BitIndex)
    {
        PrimesSum += !(*Result)[BitIndex] * (BitIndex * 2 + 1) ;
    }

    std::printf("Checksum: %lu (expected: 139601928199359)\n", PrimesSum);
    std::printf("Correct: %s\n", PrimesSum == 139601928199359lu ? "True" : "False");

    return 0;
}
