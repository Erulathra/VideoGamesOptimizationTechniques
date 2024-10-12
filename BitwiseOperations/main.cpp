#include <bitset>
#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>
#include <unordered_set>

void FindNumbers(std::vector<uint64_t>& Result, uint32_t NumBits);

int main()
{
    const auto StartTime = std::chrono::high_resolution_clock::now();

    // Reserve space. For first bit we have NumBits places, for second bit we have (NumBits - 1) places etc.
    constexpr uint32_t NumBits = 31;
    constexpr uint32_t ExpectedNumberOfVariations = NumBits * (NumBits - 1);

    std::vector<uint64_t> Result;
    Result.reserve(ExpectedNumberOfVariations);

    FindNumbers(Result, NumBits);

    const auto ProcessingTime = std::chrono::high_resolution_clock::now() - StartTime;
    std::printf("Total time: %fms\n", std::chrono::duration_cast<std::chrono::nanoseconds>(ProcessingTime).count() * 1e-6);

    uint64_t CheckSum = 0;
    for (const uint64_t Number : Result)
    {
        CheckSum += Number;
    }

    std::printf("Checksum: %llu (expected: 64424509410)\n", CheckSum);
    std::printf("Correct: %s\n", CheckSum == 64424509410llu ? "True" : "False");

    return 0;
}

void FindNumbers(std::vector<uint64_t>& Result, uint32_t NumBits)
{
    for (uint32_t FirstBitIndex = 0; FirstBitIndex < NumBits; FirstBitIndex++)
    {
        for (uint32_t SecondBitIndex = 0; SecondBitIndex < FirstBitIndex; SecondBitIndex++)
        {
            uint64_t BothBits = 1llu << FirstBitIndex | 1llu << SecondBitIndex;
            Result.push_back(BothBits);
        }
    }
}
