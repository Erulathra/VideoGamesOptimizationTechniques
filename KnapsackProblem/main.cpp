#include <cassert>
#include <fstream>
#include <iostream>
#include <queue>
#include <random>
#include <vector>

#include "lazycsv.hpp"
#include "PerformanceCounter.h"

struct PackItem
{
    float Point;
    float Weight;
};

// 16 bytes of data. Cache likes that.
struct GraphNode
{
    int32_t ItemIndex;
    float PointsSum;
    float Bound;
    float CurrentWeight;

    GraphNode(int32_t InItemIndex, float InPointsSum, float InBound, float InCurrentWeight)
        : ItemIndex(InItemIndex),
          PointsSum(InPointsSum),
          Bound(InBound),
          CurrentWeight(InCurrentWeight)
    {
    }

    GraphNode()
        : GraphNode(-1, 0.f, 0.f, 0.f)
    {
    }
};

std::vector<PackItem> LoadData(std::string_view Path)
{
    std::vector<PackItem> Result;
    lazycsv::parser<lazycsv::mmap_source, lazycsv::has_header<false>> CSVParser{Path.data()};

    for (const auto row : CSVParser)
    {
        const auto [point, weight] = row.cells(0, 1);
        Result.emplace_back(std::stof(point.trimed().data()), std::stof(weight.trimed().data()));
    }

    return Result;
}

std::vector<PackItem> GenerateData(const uint32_t Num, const float MaxPoint)
{
    std::vector<PackItem> Result;
    Result.reserve(Num);

    std::default_random_engine RandomGenerator(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::normal_distribution<float> Distribution(0, 1.f);

    for (int ItemId = 0; ItemId < Num; ++ItemId)
    {
        const float Point = MaxPoint * Distribution(RandomGenerator);
        const float Weight = Distribution(RandomGenerator);
        Result.emplace_back(Point, Weight);
    }

    return Result;
}

inline float CalculateBound(const GraphNode& Node, const std::vector<PackItem>& Items, const float MaxWeight)
{
    if (Node.CurrentWeight > MaxWeight)
    {
        return 0;
    }

    float Bound = Node.PointsSum;

    auto ItemIterator = Items.begin() + Node.ItemIndex + 1;
    float TotalWeight = Node.CurrentWeight;

    for (; ItemIterator != Items.end(); ++ItemIterator)
    {
        if (TotalWeight + ItemIterator->Weight > MaxWeight)
        {
            break;
        }

        Bound += ItemIterator->Point;
        TotalWeight += ItemIterator->Weight;
    }

    if (ItemIterator != Items.end())
    {
        const float RemainingWeight = MaxWeight - TotalWeight;
        Bound += (RemainingWeight / ItemIterator->Weight) * ItemIterator->Point;
    }

    return Bound;
}

float SolveKnapsack(std::vector<PackItem>& Items, const float MaxWeight)
{
    assert(!Items.empty());
    assert(MaxWeight > 0);

    std::ranges::sort_heap(Items, [](const PackItem& LeftItem, const PackItem& RightItem) -> bool
    {
        const float RatioLeft = LeftItem.Point / LeftItem.Weight;
        const float RatioRight = RightItem.Point / RightItem.Weight;
        return RatioLeft < RatioRight;
    });

    const uint32_t NumItems = Items.size();

    float MaxPointsSum = 0.f;
    std::queue<GraphNode> NodesToProcessQueue;

    // Emplace first node
    NodesToProcessQueue.emplace();

    while (!NodesToProcessQueue.empty())
    {
        GraphNode CurrentNode = NodesToProcessQueue.front();
        NodesToProcessQueue.pop();

        if (CurrentNode.ItemIndex < 0)
        {
            CurrentNode.ItemIndex = 0;
        }

        if (CurrentNode.ItemIndex == NumItems - 1)
        {
            // nothing to process skip this node
            continue;
        }

        const auto ChildItemIt = Items.begin() + CurrentNode.ItemIndex + 1;

        GraphNode ChildNode
        {
            CurrentNode.ItemIndex + 1,
            CurrentNode.PointsSum + ChildItemIt->Point,
            0.f,
            CurrentNode.CurrentWeight + ChildItemIt->Weight,
        };

        if (ChildNode.CurrentWeight <= MaxWeight && ChildNode.PointsSum > MaxPointsSum)
        {
            MaxPointsSum = ChildNode.PointsSum;
        }

        ChildNode.Bound = CalculateBound(ChildNode, Items, MaxWeight);

        if (ChildNode.Bound > MaxPointsSum)
        {
            NodesToProcessQueue.push(ChildNode);
        }

        ChildNode.CurrentWeight = CurrentNode.CurrentWeight;
        ChildNode.PointsSum = CurrentNode.PointsSum;
        ChildNode.Bound = CalculateBound(ChildNode, Items, MaxWeight);

        if (ChildNode.Bound > MaxPointsSum)
        {
            NodesToProcessQueue.push(ChildNode);
        }
    }

    return MaxPointsSum;
}

#define TEST_MODE 0
#define NUM_TESTS 30
#define PROBE_SIZE 1000
#define MAX_WEIGHT 4.f

int32_t main(int32_t argc, char** argv)
{
    std::printf("|===== Knapsack Solver ====|\n");
#if TEST_MODE
    std::vector<PackItem> Items = LoadData("res/test_data.csv");
    const float MaxWeight = MAX_WEIGHT;

    PerformanceCounter PerfCount;
    PerfCount.Reset();
    float Result = SolveKnapsack(Items, MaxWeight);
    double ProcessingTime = PerfCount.Elapsed();

    std::printf("Set size: %lu \n", Items.size());
    std::printf("Result: %f\n", Result);
    std::printf("Processing Time: %fms\n", ProcessingTime);
#else
    std::ofstream TimingResultsFile {"res/KnapsackBenchResults.csv"};
    PerformanceCounter PerfCounter;

    for (uint32_t TestIndex = 1; TestIndex < NUM_TESTS + 1; ++TestIndex)
    {
        double AverageTime = 0.f;
        double GenerateDataTime = 0.f;

        for (uint32_t ProbeIndex = 0; ProbeIndex < PROBE_SIZE; ++ProbeIndex)
        {
            PerfCounter.Reset();
            std::vector<PackItem> Data = GenerateData(TestIndex, 100.f);
            GenerateDataTime += PerfCounter.Elapsed();

            PerfCounter.Reset();
            float Result = SolveKnapsack(Data, MAX_WEIGHT);
            AverageTime += PerfCounter.Elapsed();
        }
        AverageTime /= PROBE_SIZE;

        auto PrintResult = [&](std::ostream& os)
        {
            std::print(os, "{},{},{}\n", TestIndex, AverageTime, GenerateDataTime);
        };

        PrintResult(std::cout);
        PrintResult(TimingResultsFile);
    }
#endif

    return 0;
}
