#include "../include/delivery_plan.h"
#include <iostream>
#include <print>
#include <algorithm>

using namespace Tasks;

void DeliveryPlan::input(uint32_t n)
{

    _n = n;
    _src = _alloc.allocate(_n);
    for (uint32_t i = 0; i < _n; ++i)
    {
        std::cin >> _src[i];
    }
}

void DeliveryPlan::solve()
{
    _freq.reserve(_n);
    uint32_t mode = _src[0];
    uint32_t max_count = 0;

    for (uint32_t i = 0; i < _n; ++i)
    {
        auto current = _src[i];
        auto count = ++_freq[current];
        if (count > max_count)
        {
            max_count = count;
            mode = current;
        }
    }
    _res = _n - max_count;
}

void DeliveryPlan::show_result() const
{
    std::println("{}", _res);
}

void DeliveryPlanWithSmallElems::input(uint32_t n)
{
    _n = n;
    _src = _alloc.allocate(_n);
    for (uint32_t i = 0; i < _n; ++i)
    {
        std::cin >> _src[i];
    }
}

void DeliveryPlanWithSmallElems::solve()
{
    std::sort(_src, _src + _n);
    uint32_t max_count = 0;
    for (uint32_t i = 0; i < _n; )
    {
        uint32_t j = i;
        while (j < _n && _src[j] == _src[i]) ++j;
        max_count = std::max(max_count, static_cast<uint32_t>(j - i));
        i = j;
    }
    _res = _n - max_count;
}

void Tasks::DeliveryPlanWithSmallElems::show_result() const
{
    std::println("{}", _res);
}
