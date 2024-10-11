#include <iostream>
#include <chrono>
#include <bitset>
#include <cmath>

const uint32_t NUMBERS_TO_CHECK = 70000000;

void FindCompositesUsingErato(std::vector<bool>& Result)
{
    // zero and one is neighter prime or complex number
    for (uint32_t BitIndex = 2; BitIndex * BitIndex <= Result.size(); ++BitIndex)
    {
        // uint32_t number
        if (!Result[BitIndex])
        {
            for (uint32_t complexNumber = BitIndex * BitIndex; complexNumber <= Result.size(); complexNumber += BitIndex)
            {
                Result[complexNumber] = true;
            }
        }
    }
}

int main()
{
    const auto StartTime = std::chrono::high_resolution_clock::now();

    // std::vector<bool> is special specialization of std::vector<T> where the values are stored as bitflags.
    std::vector<bool> Result(NUMBERS_TO_CHECK + 1, false);


    FindCompositesUsingErato(Result);

    const std::chrono::duration<double> ProcessingTime = std::chrono::high_resolution_clock::now() - StartTime;
    double ProcessingSeconds = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(ProcessingTime).count()) * 10e-3;
    std::printf("Total time: %fs\n", ProcessingSeconds);

    uint64_t PrimesSum = 0;
    for (size_t BitIndex = 2; BitIndex <= Result.size(); ++BitIndex)
    {
        PrimesSum += !Result[BitIndex] * BitIndex;
    }

    std::printf("Checksum: %lu (expected: 139601928199359)\n", PrimesSum);
    std::printf("Correct: %s\n", PrimesSum == 139601928199359lu ? "True" : "False");

    return 0;
}
