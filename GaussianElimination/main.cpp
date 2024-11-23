#include <iostream>

#include <immintrin.h>
#include <smmintrin.h>
#include <random>

#include "PerformanceCounter.h"

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

    float& operator[](int32_t Index)
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

    float4& operator[](int32_t Index)
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

void GaussianElimination(float4x3& Matrix, float4& Result)
{
    for (int k = 0; k < 2; k++)
    {
        for (int i = k + 1; i < 3; i++)
        {
            float Factor = Matrix[i][k] / Matrix[k][k];

            __m128 FactorVector = _mm_set1_ps(Factor);
            Matrix[i].SSEData = _mm_sub_ps(Matrix[i].SSEData, _mm_mul_ps(FactorVector, Matrix[k].SSEData));
        }
    }

    Result[2] = Matrix[2][3] / Matrix[2][2];
    Result[1] = (Matrix[1][3] - Matrix[1][2] * Result[2]) / Matrix[1][1];
    Result[0] = (Matrix[0][3] - Matrix[0][2] * Result[2] - Matrix[0][1] * Result[1]) / Matrix[0][0];
}

int32_t main()
{
    float4x3 TestMatrix{};
    TestMatrix.Data[0] = {2, 1, 1, 9};
    TestMatrix.Data[1] = {1, 3, 1, 9};
    TestMatrix.Data[2] = {1, 1, 4, 16};

    float4 Result{};

    std::printf("%s\n", TestMatrix.ToString().c_str());
    GaussianElimination(TestMatrix, Result);
    std::printf("%s\n", TestMatrix.ToString().c_str());
    std::printf("%s\n", Result.ToString().c_str());

    return 0;
}
