#include <iostream>

#include <immintrin.h>
#include <smmintrin.h>
#include <random>

#include "PerformanceCounter.h"

constexpr float SmallNumber = 1e-6;

inline bool IsNearlyZero(const float Value)
{
    return std::abs(Value) < 1e-6;
}

union float4
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

    float& operator[](uint32_t Index)
    {
        return Data[Index];
    }

    std::string ToString()
    {
        return std::format("[{}, {}, {}, {}]", X, Y, Z, W);
    }
};

struct alignas(64) float4x3
{
    // row major
    float4 Data[3];

    static float4x3 GetRandom()
    {
        float4x3 Result;

        std::default_random_engine RandomGenerator(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> Distribution(-10.f, 10.f);

        for (auto & Row : Result.Data)
        {
            for (float& Cell : Row.Data)
            {
                Cell = Distribution(RandomGenerator);
            }
        }

        return Result;
    }

    float4& operator[](uint32_t Index)
    {
        return Data[Index];
    }

    std::string ToString()
    {
        std::string Result{};

        for (float4& Row : Data)
        {
            Result += Row.ToString();
            Result += "\n";
        }

        return Result;
    }
};

int32_t ForwardElimination(float4x3& Matrix)
{
    // I wanted to find better names for iteration counters but i don't find any better than letters used in algorithm.
    for (uint32_t k = 0; k < 3; ++k)
    {
        struct
        {
            uint32_t Id;
            double Value;
        } Pivot {k, Matrix[k][k]};

        for (uint32_t RowId = k + 1; RowId < 3; ++RowId)
        {
            if (std::abs(Matrix[RowId][k]) > Pivot.Value)
            {
                Pivot.Value = Matrix[RowId][k];
                Pivot.Id = RowId;
            }
        }

        // Detect Singular Matrix and return zero row
        if (IsNearlyZero(Matrix[k][Pivot.Id]))
        {
            return static_cast<int32_t>(Pivot.Id);
        }

        if (Pivot.Id != k)
        {
            // TODO: SSE Swap
            std::swap(Matrix[k], Matrix[Pivot.Id]);
        }

        for (uint32_t i = k + 1; i < 3; i++)
        {
            float Factor = Matrix[i][k] / Matrix[k][k];
            __m128 FactorVector = _mm_set1_ps(Factor);
            Matrix[i].SSEData = _mm_sub_ps(Matrix[i].SSEData, _mm_mul_ps(FactorVector, Matrix[k].SSEData));
        }
    }

    return -1;
}

enum class SolutionType
{
    Unique,
    Inconsistent,
    Undetermined
};

inline std::string SolutionTypeToString(SolutionType Type)
{
    switch (Type)
    {
    case SolutionType::Unique:
        return "Unique";
    case SolutionType::Inconsistent:
        return "Inconsistent";
    case SolutionType::Undetermined:
        return "Undetermined";
    }

    return "ERROR";
}

SolutionType GaussianElimination(float4x3& Matrix, float4& Result)
{
    const int32_t ResultFlag = ForwardElimination(Matrix);
    if (ResultFlag == -1)
    {
        Result[2] = Matrix[2][3] / Matrix[2][2];
        Result[1] = (Matrix[1][3] - Matrix[1][2] * Result[2]) / Matrix[1][1];
        Result[0] = (Matrix[0][3] - Matrix[0][2] * Result[2] - Matrix[0][1] * Result[1]) / Matrix[0][0];

        return SolutionType::Unique;
    }

    if (Matrix[ResultFlag][3] == 0)
    {
        return SolutionType::Undetermined;
    }

    return SolutionType::Inconsistent;
}

int32_t main()
{
    float4x3 MatrixUniqueSolution
    { {
            {3, 1, 2, 11},
            {2, 1, 1, 8},
            {1, 2, 1, 11}
        }};

    float4x3 MatrixNoSolutions
    { {
            {1, 1, 1, 1},
            {2, 2, 2, 2},
            {1, 3, 1, 3}
        }};

    float4x3 MatrixInfiniteSolutions
    { {
            {1, 1, 1, 1},
            {2, 2, 2, 2},
            {3, 3, 3, 3}
        }};

    float4 Result{};
    std::printf("=========| Tests |=========\n");

    std::printf("=> Unique Solution\n");
    std::printf("%s\n", MatrixUniqueSolution.ToString().c_str());
    SolutionType ResultType = GaussianElimination(MatrixUniqueSolution, Result);
    std::printf("%s\n", MatrixUniqueSolution.ToString().c_str());
    std::printf("Type: %s, %s\n", SolutionTypeToString(ResultType).c_str(), Result.ToString().c_str());

    std::printf("=> No Solutions\n");
    std::printf("%s\n", MatrixNoSolutions.ToString().c_str());
    ResultType = GaussianElimination(MatrixNoSolutions, Result);
    std::printf("%s\n", MatrixNoSolutions.ToString().c_str());
    std::printf("Type: %s, %s\n", SolutionTypeToString(ResultType).c_str(), Result.ToString().c_str());

    std::printf("=> Inf Solutions\n");
    std::printf("%s\n", MatrixInfiniteSolutions.ToString().c_str());
    ResultType = GaussianElimination(MatrixInfiniteSolutions, Result);
    std::printf("%s\n", MatrixNoSolutions.ToString().c_str());
    std::printf("Type: %s, %s\n", SolutionTypeToString(ResultType).c_str(), Result.ToString().c_str());

    std::printf("=======| Benchmark |=======\n");

    constexpr uint32_t NumTestMatrices = 1000;
    constexpr float BenchmarkTime = 0.1f;

    std::vector<float4x3> MatricesToSolve;
    MatricesToSolve.reserve(NumTestMatrices);
    for (uint32_t MatrixId = 0; MatrixId < NumTestMatrices; ++MatrixId)
    {
        MatricesToSolve.push_back(float4x3::GetRandom());
    }

    PerformanceCounter PerfCounter;
    PerfCounter.Reset();

    float LastTime = PerfCounter.Elapsed();
    for (uint32_t MatrixId = 0; MatrixId < NumTestMatrices; ++MatrixId)
    {
        MatricesToSolve.push_back(float4x3::GetRandom());

        if (PerfCounter.Elapsed() > BenchmarkTime)
        {
            std::printf("Solved Matrices: %i \n", MatrixId - 1);
            std::printf("Time: %f", LastTime);
            break;
        }

        LastTime = PerfCounter.Elapsed();
    }

    return 0;
}
