#ifndef AKI_QUICK_SELECT_H
#define AKI_QUICK_SELECT_H

#include <algorithm>
#include <cstdio>

template <class... Args> void unused(Args&&...) {}

template <class It>
static void print_values(const char *name, It first, It last)
{
    std::printf("%-10s : ", name);

    std::for_each(first, last, [](int n) {
        std::printf("%d ", n);
    });

    std::puts("");
}

template <class It>
static It decide_pivot(It first, It nth, It last)
{
    unused(first, nth, last);
    
    auto mid = first + (last - first) / 2;
    unused(mid);
#if 0

    if (*first < *mid && *mid < *(last - 1))
        return mid;
    else if (*mid < *first && *first < *(last - 1))
        return first;
    else
        return last;
#else
    return mid; //first;
#endif
}

template <class It>
static It partition(It first, It pivot, It last)
{
    auto pvalue = *pivot;

    std::swap(*pivot, *(last - 1));

    auto store_it = first;
    for (auto it = first; it != last - 1; ++it) {
        if (*it <= pvalue) {
            std::swap(*it, *store_it);
            ++store_it;
        }
    }

    std::swap(*(last - 1), *store_it);

    return store_it;
}

template <class It>
static void quick_select(It first, It nth, It last)
{
    int debug = 0;

    if (last - first <= 1)
        return;

    auto pivot = decide_pivot(first, nth, last);

    if (debug)
        print_values("", first, last);

    auto it = partition(first, pivot, last);

    if (debug) {
        print_values("", first, last);
        std::printf("pivot = %zd, idx = %zd\n", pivot - first, it - first);

        if (nth < it)
            printf("quick_select(%zd, %zd, %zd)\n",
                   first - first, nth - first, it - first);
        else if (nth > it)
            printf("quick_select(%zd, %zd, %zd)\n",
                   it + 1 - first, nth - first, last - first);
    }

    if (it == nth)
        return;
    else if (nth < it)
        quick_select(first, nth, it);
    else
        quick_select(it + 1, nth, last);
}

#endif
