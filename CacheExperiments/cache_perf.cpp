#include <chrono>
#include <random>
#include <cstring>

#include <cstdio>
#include <thread>

class Stopwatch
{
    std::chrono::time_point<std::chrono::system_clock> start_time;

public:
    Stopwatch()
        : start_time(std::chrono::high_resolution_clock::now())
    {
    }

    inline uint64_t read() const { return (std::chrono::high_resolution_clock::now() - start_time).count(); }
};

size_t RANDOM32BITADDRESS()
{
    static std::default_random_engine generator;
    std::uniform_int_distribution<size_t> distribution(0, 0xffffffff);
    return distribution(generator);
}

template<typename TestData>
TestData TestMemAccess(size_t sampleCount, TestData *vector, TestData **list, size_t size, uint64_t &seqTime, uint64_t &randTime)
{
    TestData sample;
    //! GENERATE RANDOM ADDRESS LIST
    for (size_t i = 0; i < sampleCount; ++i)
    {
        size_t index = RANDOM32BITADDRESS() % (sampleCount - 1);
        list[i] = &vector[index];
    }
    //! COUNT ACCESS TIME FOR RANDOM ADDRESS
    Stopwatch randCount;
    for (size_t i = 0; i < sampleCount; ++i)
    {
        memcpy(&sample, list[i], size);
    }
    randTime += randCount.read();
    //! GENERATE SEQUENCE ADDRESS LIST
    for (size_t i = 0; i < sampleCount; ++i)
    {
        list[i] = &vector[i];
    }
    //! COUNT ACCESS TIME FOR SEQUENCE ADDRESS
    Stopwatch seqCount;
    for (size_t i = 0; i < sampleCount; ++i)
    {
        memcpy(&sample, list[i], size);
    }
    seqTime += seqCount.read();
    return sample;
}

//! Choose the L2 size for your CPU:
#define L2_CACHE_SIZE (1 * 1024 * 1024)

#define TEST_COUNT 64

struct Data16
{
    float a;
    float b;
    float c;
    float d;
};

struct Data20
{
    float a;
    float b;
    float c;
    float d;
    float e;
};

Data16 vectorData16[L2_CACHE_SIZE * 4];
Data16 *listData16[L2_CACHE_SIZE * 4];
Data20 vectorData20[L2_CACHE_SIZE * 4];
Data20 *listData20[L2_CACHE_SIZE * 4];

void TEST(size_t sampleCount)
{
    uint32_t count = TEST_COUNT;
    uint64_t seqReadTimeData16 = 0.0;
    uint64_t randReadTimeData16 = 0.0;

    while (count--)
    {
        TestMemAccess<Data16>(sampleCount, vectorData16, listData16, sizeof(Data16), seqReadTimeData16, randReadTimeData16);
    }

    uint64_t seqReadTimeData20 = 0.0;
    uint64_t randReadTimeData20 = 0.0;
    count = TEST_COUNT;

    while (count--)
    {
        TestMemAccess<Data20>(sampleCount, vectorData20, listData20, sizeof(Data20), seqReadTimeData20, randReadTimeData20);
    }

    printf("TEST Data Packed 16 \n");
    printf("Sequence read from vector %f ms \n", static_cast<double>(seqReadTimeData16) * 1e-6 / TEST_COUNT);
    printf("Random read from vector %f ms \n", static_cast<double>(randReadTimeData16) * 1e-6 / TEST_COUNT);
    printf("TEST Data Packed 20 \n");
    printf("Sequence read from vector %f ms \n", static_cast<double>(seqReadTimeData20) * 1e-6 / TEST_COUNT);
    printf("Random read from vector %f ms \n", static_cast<double>(randReadTimeData20) * 1e-6 / TEST_COUNT);
}

//!===========MAIN===========
int main()
{
    printf("TEST quarter cache used \n");
    TEST((L2_CACHE_SIZE / 4) / 16);
    printf("---\n");
    printf("TEST half cache used \n");
    TEST((L2_CACHE_SIZE / 2) / 16);
    printf("---\n");
    printf("TEST full cache used \n");
    TEST(L2_CACHE_SIZE / 16);
    printf("---\n");
    printf("TEST double cache used \n");
    TEST((L2_CACHE_SIZE * 2) / 16);
    printf("---\n");
    printf("TEST quadruple cache used \n");
    TEST((L2_CACHE_SIZE * 4) / 16);
    return 0;
}
