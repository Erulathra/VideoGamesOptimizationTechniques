#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>

#include <immintrin.h>
#include <smmintrin.h>
#include <random>

#include "PerformanceCounter.h"


#if !NDEBUG
constexpr uint32_t NumTests = 100;
constexpr uint64_t NumComponents = 131072;
#else
constexpr uint32_t NumTests = 1000;
constexpr uint64_t NumComponents = 1048576;
#endif

union SSEVector
{
    struct
    {
        float X;
        float Y;
        float Z;
        float W;
    };

    float Data[4];
    __m128 SSEData;
};

void GenerateRandomVector(std::vector<SSEVector>& Vector)
{
    std::default_random_engine RandomGenerator(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> Distribution(-1.f, 1.f);

    Vector.clear();
    const uint32_t NumVectors = std::ceil(NumComponents / 4.f);

    Vector.resize(NumVectors);

    for (SSEVector& SubVector : Vector)
    {
        SubVector.X = Distribution(RandomGenerator);
        SubVector.Y = Distribution(RandomGenerator);
        SubVector.Z = Distribution(RandomGenerator);
        SubVector.W = Distribution(RandomGenerator);
    }

    // Clear last not full vector
    const uint32_t NumFilledComponents = NumVectors * 4;
    const uint32_t SurplusComponents = NumFilledComponents - NumComponents;
    if (SurplusComponents > 0)
    {
        for (uint32_t ComponentId = 3; ComponentId > 0; --ComponentId)
        {
            Vector.rbegin()->Data[ComponentId] = 0;
        }
    }
}

double RunTest(std::function<float(const std::vector<SSEVector>& Left, const std::vector<SSEVector>& Right)> Function)
{
    PerformanceCounter PerfCounter;
    double CumulativeTime = 0.f;

    for (uint64_t TestId = 0; TestId < NumTests; ++TestId)
    {
        std::vector<SSEVector> VecA, VecB;
        GenerateRandomVector(VecA);
        GenerateRandomVector(VecB);

        PerfCounter.Reset();
        float Result = Function(VecA, VecB);
        CumulativeTime += PerfCounter.Elapsed();
    }

    return CumulativeTime / NumTests;
}

float DotProduct(const std::vector<SSEVector>& Left, const std::vector<SSEVector>& Right)
{
    assert(Left.size() == Right.size());

    float Sum = 0.f;

    for (uint32_t VectorId = 0; VectorId < Left.size(); ++VectorId)
    {
        for (uint32_t ComponentId = 0; ComponentId < 4; ++ComponentId)
        {
            Sum += Left[VectorId].Data[ComponentId] * Right[VectorId].Data[ComponentId];
        }
    }

    return Sum;
}

float DotProductUnrolled(const std::vector<SSEVector>& Left, const std::vector<SSEVector>& Right)
{
    assert(Left.size() == Right.size());

    float Sum = 0.f;

    for (uint32_t VectorId = 0; VectorId < Left.size(); ++VectorId)
    {
        Sum += Left[VectorId].X * Right[VectorId].X;
        Sum += Left[VectorId].Y * Right[VectorId].Y;
        Sum += Left[VectorId].Z * Right[VectorId].Z;
        Sum += Left[VectorId].W * Right[VectorId].W;
    }

    return Sum;
}

float DotProductSSE(const std::vector<SSEVector>& Left, const std::vector<SSEVector>& Right)
{
    assert(Left.size() == Right.size());

    float Sum = 0.f;

    for (uint32_t VectorId = 0; VectorId < Left.size(); ++VectorId)
    {
        Sum += _mm_cvtss_f32(_mm_dp_ps(Left[VectorId].SSEData, Right[VectorId].SSEData, 0xF1));
    }

    return Sum;
}

float DotProductSSEUnrolled(const std::vector<SSEVector>& Left, const std::vector<SSEVector>& Right)
{
    assert(Left.size() == Right.size());

    float Sum = 0.f;

    const uint32_t NumVectors = Left.size();
    const uint32_t NumIterations = std::floor(NumVectors / 4);

    for (uint32_t PackId = 0; PackId < NumIterations; ++PackId)
    {
        const uint32_t Offset = PackId * 4;
        Sum += _mm_cvtss_f32(_mm_dp_ps(Left[Offset].SSEData, Right[Offset].SSEData, 0xF1));
        Sum += _mm_cvtss_f32(_mm_dp_ps(Left[Offset + 1].SSEData, Right[Offset + 1].SSEData, 0xF1));
        Sum += _mm_cvtss_f32(_mm_dp_ps(Left[Offset + 2].SSEData, Right[Offset + 2].SSEData, 0xF1));
        Sum += _mm_cvtss_f32(_mm_dp_ps(Left[Offset + 3].SSEData, Right[Offset + 3].SSEData, 0xF1));
    }

    const uint32_t NumProcessedVectors = NumIterations * 4;
    for (uint32_t VectorId = NumProcessedVectors; VectorId < NumVectors; ++VectorId)
    {
        Sum += _mm_cvtss_f32(_mm_dp_ps(Left[VectorId].SSEData, Right[VectorId].SSEData, 0xF1));
    }

    return Sum;
}

int32_t main()
{
    std::vector<SSEVector> VecA, VecB;
    GenerateRandomVector(VecA);
    GenerateRandomVector(VecB);

    std::printf("=======| Unit Tests |=======\n");
    std::printf("DotProduct: %f\n", DotProduct(VecA, VecB));
    std::printf("DotProduct Unrolled: %f\n", DotProductUnrolled(VecA, VecB));
    std::printf("DotProduct SSE: %f\n", DotProductSSE(VecA, VecB));
    std::printf("DotProduct SSE Unrolled: %f\n", DotProductSSEUnrolled(VecA, VecB));

    std::printf("=======| Perf Tests |=======\n");
    std::printf("DotProduct: %fms\n", RunTest(&DotProduct));
    std::printf("DotProduct Unrolled: %fms\n", RunTest(&DotProductUnrolled));
    std::printf("DotProduct SSE: %fms\n", RunTest(&DotProductSSE));
    std::printf("DotProduct SSE Unrolled: %fms\n", RunTest(&DotProductSSEUnrolled));
}
