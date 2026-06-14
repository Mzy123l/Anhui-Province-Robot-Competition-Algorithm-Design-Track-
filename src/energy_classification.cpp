#include "../include/energy_classification.h"
#include <algorithm>
#include <print>
#include <vector>
#include <iostream>

using namespace Tasks;
 
void EnergyClassification::input()
{
    std::cin >> _n >> _m;
    _values = _alloc.allocate(_n);
    for (uint32_t i = 1; i < _n; ++i)
    {
        std::cin >> _values[i];
    }
}

void EnergyClassification::solve()
{
    
    // 找出最大值和最小值
    uint32_t min_all = _values[0];
    uint32_t max_all = _values[0];
    for (uint32_t i = 1; i < _n; ++i)
    {
        if (_values[i] < min_all) 
        { 
            min_all = _values[i];
            continue; // 提前退出加快速度
        }
        if (_values[i] > max_all) max_all = _values[i];
    }

    uint64_t lo = 0;
    uint64_t hi = static_cast<uint64_t>(_n) * (max_all - min_all);

    while (lo < hi)
    {
        const uint64_t mid = lo + (hi - lo) / 2;
        if (min_segments(mid) <= _m)
        {
            hi = mid;
        }
        else
        {
            lo = mid + 1;
        }
    }
    _result = lo;
}

void EnergyClassification::show_result() const
{
    std::println("{}", _result);
}

inline uint32_t EnergyClassification::min_segments(uint64_t limit)
{
    uint32_t segments = 0;
    uint32_t left = 0;
    while (left < _n)
    {
        ++segments;
        uint32_t right = left;
        uint32_t min = _values[left];
        uint32_t max = _values[left];
        while (right + 1 < _n)
        {
            const uint32_t next = right + 1;
            if (_values[next] < min) min = _values[next];
            if (_values[next] > max) max = _values[next];
            const uint64_t risk = static_cast<uint64_t>(next - left + 1) * static_cast<uint64_t>(max - min);
            if (risk <= limit)
            {
                right = next;
            }
            else
            {
                break;
            }
        }
        left = right + 1;
    }
    return segments;
}