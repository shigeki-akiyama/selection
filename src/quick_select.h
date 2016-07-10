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
static It pivot_middle(It first, It last)
{
    return first + (last - first) / 2;
}

template <class It>
static It pivot_median3(It first, It last)
{
    enum { N_ELEMS = 3 };

#if 1
    auto mid = first + (last - first) / 2;
    It xs[] = { first, mid, last - 1 };
#else
    unused(last);
    It xs[N_ELEMS];
    for (auto i = 0; i < N_ELEMS; i++)
        xs[i] = first + i;
#endif

    std::sort(xs, xs + 3, [](It lhs, It rhs) { return *lhs < *rhs; });
    return xs[1];
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

template <class It, class Pivot>
static void quick_select_base(It first, It nth, It last, Pivot decide_pivot)
{
    auto size = last - first;
    if (size <= 1)
        return;

    It pivot = decide_pivot(first, last);

    auto it = partition(first, pivot, last);

    if (it == nth)
        return;
    else if (nth < it)
        quick_select_base(first, nth, it, decide_pivot);
    else
        quick_select_base(it + 1, nth, last, decide_pivot);
}

template <class It>
static void quick_select_pivot_middle(It first, It nth, It last)
{
    quick_select_base(first, nth, last, pivot_middle<It>);
}

template <class It>
static void quick_select_pivot_median3(It first, It nth, It last)
{
    quick_select_base(first, nth, last, pivot_median3<It>);
}

template <class It>
static void quick_select(It first, It nth, It last)
{
   quick_select_base(first, nth, last, pivot_median3<It>);
}


#endif
