#include <bitset>
#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <random>

#include "PerformanceCounter.h"

using DataBlock = uint32_t;
constexpr uint64_t NumBlocksBig = 1e6;
constexpr uint64_t  NumBlocksSmall = 1e4;

template <typename ContainerType>
double TestPushBack(ContainerType& Container, uint64_t NumBlocks)
{
    PerformanceCounter PerfCounter;
    PerfCounter.Reset();

    for (DataBlock sampleIndex = 0; sampleIndex < NumBlocks; ++sampleIndex)
    {
        Container.push_back(sampleIndex);
    }

    return PerfCounter.Elapsed();
}

struct InterleavedTestResults
{
    double VectorTime = 0.;
    double ListTime = 0.;
    double DequeTime = 0.;
};

InterleavedTestResults TestInterleavedPushBack(std::vector<DataBlock>& Vector, std::list<DataBlock>& List, std::deque<DataBlock>& Deque, uint64_t NumBlocks)
{
    InterleavedTestResults Result;
    PerformanceCounter PerfConter;

    for (DataBlock sampleIndex = 0; sampleIndex < NumBlocks; ++sampleIndex)
    {
        PerfConter.Reset();
        Vector.push_back(sampleIndex);
        Result.VectorTime += PerfConter.Elapsed();

        PerfConter.Reset();
        List.push_back(sampleIndex);
        Result.ListTime += PerfConter.Elapsed();

        PerfConter.Reset();
        Deque.push_back(sampleIndex);
        Result.DequeTime += PerfConter.Elapsed();
    }

    return Result;
}

struct InsertTestResults
{
    double IteratorMovement = 0.;
    double Insert = 0.;
};

template <typename ContainerIteratorType>
inline ContainerIteratorType MoveIterator(ContainerIteratorType ContainerBegin, size_t Index)
{
    return ContainerBegin + Index;
}

template <>
inline std::list<DataBlock>::iterator MoveIterator(std::list<DataBlock>::iterator ContainerBegin, size_t Index)
{
    for (int i = 0; i < Index; ++i)
    {
        ++ContainerBegin;
    }

    return ContainerBegin;
}

template<bool bAscending = true>
void GenerateRandomIndexes(std::vector<size_t>& RandomNumbers, uint64_t NumBlocks)
{
    RandomNumbers.reserve(NumBlocks);

    std::default_random_engine RandomGenerator;
    std::uniform_int_distribution<size_t> Distribution(0, std::numeric_limits<size_t>::max());

    // In first iteration we don't have any elements, so we need to add it at begin;
    RandomNumbers.push_back(0);

    for (int SampleIndex = 1; SampleIndex < NumBlocks - 1; ++SampleIndex)
    {
        const size_t RandomNumber = Distribution(RandomGenerator);
        if constexpr (bAscending)
        {
            RandomNumbers.push_back(RandomNumber % SampleIndex);
        }
        else
        {
            RandomNumbers.push_back(RandomNumber % (NumBlocks - SampleIndex));
        }
    }

    RandomNumbers.push_back(0);
}

template <typename ContainerType>
InsertTestResults TestInsert(ContainerType& Container, uint64_t NumBlocks)
{
    std::vector<size_t> RandomNumbers;
    GenerateRandomIndexes(RandomNumbers, NumBlocks);

    InsertTestResults Result;
    PerformanceCounter PerfCounter;

    for (size_t sampleIndex = 0; sampleIndex < NumBlocks; ++sampleIndex)
    {
        PerfCounter.Reset();
        auto It = MoveIterator(Container.begin(), RandomNumbers[sampleIndex]);
        Result.IteratorMovement += PerfCounter.Elapsed();

        PerfCounter.Reset();
        Container.insert(It, sampleIndex);
        Result.Insert += PerfCounter.Elapsed();
    }

    return Result;
}

template <typename ContainerType>
double TestErase(ContainerType& Container, uint64_t NumBlocks)
{
    std::vector<size_t> RandomNumbers;
    GenerateRandomIndexes<false>(RandomNumbers, NumBlocks);

    for (size_t sampleIndex = 0; sampleIndex < NumBlocks; ++sampleIndex)
    {
        Container.push_back(sampleIndex);
    }

    double Result = 0.;
    PerformanceCounter PerfCounter;

    for (size_t sampleIndex = 0; sampleIndex < NumBlocks; ++sampleIndex)
    {
        auto It = MoveIterator(Container.begin(), RandomNumbers[sampleIndex]);
        PerfCounter.Reset();
        Container.erase(It);
        Result += PerfCounter.Elapsed();
    }

    return Result;
}

int32_t main(int argc, char **argv)
{
    {
        std::printf("block push_back (%lu items)\n", NumBlocksBig);
        std::vector<DataBlock> Vector;
        std::printf("vector: %fms\n", TestPushBack(Vector, NumBlocksBig));
        std::list<DataBlock> List;
        std::printf("list: %fms\n", TestPushBack(List, NumBlocksBig));
        std::deque<DataBlock> Deque;
        std::printf("deque: %fms\n", TestPushBack(Deque, NumBlocksBig));
        std::printf("\n");
    }

    {
        std::printf("interleaved push_back (%lu items)\n", NumBlocksBig);
        std::vector<DataBlock> Vector;
        std::list<DataBlock> List;
        std::deque<DataBlock> Deque;
        InterleavedTestResults Results = TestInterleavedPushBack(Vector, List, Deque, NumBlocksBig);

        std::printf("vector: %fms\n", Results.VectorTime);
        std::printf("list: %fms\n", Results.ListTime);
        std::printf("deque: %fms\n", Results.DequeTime);
        std::printf("\n");
    }

    {
        std::vector<DataBlock> Vector;
        InsertTestResults VectorResult = TestInsert(Vector, NumBlocksSmall);
        std::list<DataBlock> List;
        InsertTestResults ListResult = TestInsert(List, NumBlocksSmall);
        std::deque<DataBlock> Deque;
        InsertTestResults DequeResult = TestInsert(Deque, NumBlocksSmall);

        std::printf("insert (%lu items)\n", NumBlocksSmall);
        std::printf("vector: %fms\n", VectorResult.Insert);
        std::printf("list: %fms\n", ListResult.Insert);
        std::printf("deque: %fms\n", DequeResult.Insert);
        std::printf("\n");

        std::printf("iterator movement (%lu items)\n", NumBlocksSmall);
        std::printf("vector: %fms\n", VectorResult.IteratorMovement);
        std::printf("list: %fms\n", ListResult.IteratorMovement);
        std::printf("deque: %fms\n", DequeResult.IteratorMovement);
        std::printf("\n");
    }

    {
        std::vector<DataBlock> Vector;
        std::list<DataBlock> List;
        std::deque<DataBlock> Deque;

        std::printf("erase (%lu items)\n", NumBlocksSmall);
        std::printf("vector: %fms\n", TestErase(Vector, NumBlocksSmall));
        std::printf("list: %fms\n", TestErase(List, NumBlocksSmall));
        std::printf("deque: %fms\n", TestErase(Deque, NumBlocksSmall));
        std::printf("\n");
    }
}