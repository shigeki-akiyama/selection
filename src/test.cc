
#include "quick_select.h"
#include <vector>
#include <random>
#include <chrono>
#include <cstdio>
#include <cstdlib>

enum {
    DEBUG = 0,
    TIMES = 10000,

    DIST_UNIFORM = 0,
    DIST_NORMAL  = 1,
    DIST_LAST    = 2,
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
static std::vector<T> gen_random_vector(size_t size, unsigned dist, 
                                        unsigned long seed)
{
    std::mt19937 engine(seed);
    std::uniform_real_distribution<T> dist_uniform(-6.0f, 6.0f);
    std::normal_distribution<T> dist_normal(0.0f, 5.0f);

    std::vector<T> xs(size);
    for (auto& x : xs)
        if (dist == DIST_UNIFORM)
            x = dist_uniform(engine);
        else
            x = dist_normal(engine);

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
    std::printf("%-20s    %9zu  %11.3f  %11.3f  %11.3f\n",
                name, buf.size(), avg * factor, min * factor, max * factor);
}

int real_main(size_t n_elems, unsigned dist, size_t seed)
{
    using elem_type = f64;

    size_t idx = n_elems / 2;
    unused(idx);

    //std::random_device seed_gen;
    //auto seed = seed_gen();
    auto values = gen_random_vector<elem_type>(n_elems, dist, seed);

    auto buf = values;

    if (0) {
        auto nth = buf.begin() + idx;
        test_quick_select(buf.begin(), nth, buf.end());
    }

    bench_params<elem_type> bp = { values, buf, idx };

    std::printf("# %-20s  %9s  %11s  %11s  %11s\n",
                "name", "size", "avg", "min", "max");

    using iterator = decltype(buf)::iterator;

    benchmark("quick_select_middle", bp, 
              quick_select_pivot_middle<iterator>);
    benchmark("quick_select_median3", bp, 
              quick_select_pivot_median3<iterator>);
    benchmark("std::nth_element", bp, std::nth_element<iterator>);
    benchmark("std::sort", bp, 
              [](iterator first, iterator nth, iterator last) {
                  unused(nth);
                  return std::sort(first, last);
              });
    benchmark("std::partial_sort", bp, 
              [](iterator first, iterator nth, iterator last) {
                  return std::partial_sort(first, nth + 1, last);
              });

    return 0;

}

int main(int argc, char *argv[])
{
    size_t n_elems  = (argc >= 2) ? std::atoi(argv[1]) : 500;
    int dist        = (argc >= 3) ? std::atoi(argv[2]) : DIST_NORMAL;
    size_t seed     = (argc >= 4) ? std::atoi(argv[3]) : 0;

    if (dist < 0 || dist >= DIST_LAST) {
        std::printf("Usage: %s n_elems dist seed", argv[0]);
        return 0;
    }

    return real_main(n_elems, unsigned(dist), seed);
}

