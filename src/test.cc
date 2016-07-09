
#include "quick_select.h"
#include <vector>
#include <random>
#include <chrono>
#include <cstdio>
#include <cstdlib>

enum {
    DEBUG = 0,
    N_ELEMS = 500,
    TIMES = 10000,
};

using f64 = double;

double now()
{
    using namespace std::chrono;
    auto tp = high_resolution_clock::now().time_since_epoch();
    return duration_cast<duration<double>>(tp).count();
}

template <class It>
static void test_partition(It first, It last, int pivot_idx)
{
    if (DEBUG)
        print_values("before", first, last);

    auto pivot = first + pivot_idx;
    auto pvalue = *pivot;

    auto it = partition(first, pivot, last);
   
    if (DEBUG)
        print_values("after", first, last);

    if (DEBUG) {
        auto idx = it - first;
        std::printf("*pivot = %d, idx = %zd, *it = %d\n", pvalue, idx, *it);
    }
}

template <class It>
static bool validate_select(It first, It nth, It last)
{
    unused(last);

    auto value = *nth;

    for (auto it = first; it != nth; ++it)
        if (*it > value)
            return false;

    return true;
}

template <class It>
static void test_quick_select(It first, It nth, It last)
{
    if (DEBUG)
        print_values("before", first, last);

    quick_select(first, nth, last);

    if (DEBUG)
        print_values("after", first, last);

    if (validate_select(first, nth, last))
        std::printf("test: PASS\n");
    else
        std::printf("test: FAIL\n");
}

template <class T>
static std::vector<T> gen_random_vector(size_t size, unsigned long seed)
{
    std::mt19937 engine(seed);
    std::uniform_real_distribution<T> dist(-6.0f, 6.0f);

    std::vector<T> xs(size);
    for (auto& x : xs)
        x = dist(engine);

    return xs;
}

template <class T>
struct bench_params {
    const std::vector<T>& values;
    std::vector<T>& buf;
    size_t idx;
};

template <class T, class F>
void benchmark(const char *name, const bench_params<T>& p, F select)
{
    auto buf = p.buf;
  
    auto sum = 0.0;
    auto min = std::numeric_limits<double>::max();
    auto max = std::numeric_limits<double>::min();
    for (size_t i = 0; i < TIMES; i++) {
        buf = p.values;

        auto t0 = now();
        auto nth = buf.begin() + p.idx;
        select(buf.begin(), nth, buf.end());
        auto t1 = now();

        if (!validate_select(buf.begin(), nth, buf.end())) {
            std::printf("ERROR\n");
            return;
        }

        auto t = t1 - t0;
        sum += t;
        min = std::min(min, t);
        max = std::max(max, t);
    }

    auto factor = 1e6;
    auto avg = sum / TIMES;
    std::printf("%-20s  %6.3f  %6.3f  %6.3f\n",
                name, avg * factor, min * factor, max * factor);
}

int main(int argc, char *argv[])
{
    using elem_type = f64;

    unused(argc, argv);
    size_t idx = N_ELEMS / 2;
    size_t seed = (argc >= 2) ? std::atoi(argv[1]) : 0;
    unused(idx);

    //std::random_device seed_gen;
    //auto seed = seed_gen();
    auto values = gen_random_vector<elem_type>(N_ELEMS, seed);

    auto buf = values;

    if (0) {
        auto nth = buf.begin() + idx;
        test_quick_select(buf.begin(), nth, buf.end());
    }

    bench_params<elem_type> bp = { values, buf, idx };

    std::printf("# %-20s  %-6s  %-6s  %-6s\n",
                "name", "avg", "min", "max");

    using iterator = decltype(buf)::iterator;
    benchmark("quick_select", bp, quick_select<iterator>);
    benchmark("std::nth_element", bp, std::nth_element<iterator>);

    return 0;
}

