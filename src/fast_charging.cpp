#include "../include/fast_charging.h"
#include <print>

void Tasks::FastCharging::solve(uint32_t _a, uint32_t _b, uint32_t _c)
{
    // 升序排列，确保 a <= b <= c
    if (_a > _b) std::swap(_a, _b);
    if (_a > _c) std::swap(_a, _c);
    if (_b > _c) std::swap(_b, _c);

    _res = 2 * _c - _a - _b;

    if (_res % 2 == 0)
    {
        _res >>= 1;
    }
    else
    {
        _res = (_res + 3) >> 1;
    }
}

void Tasks::FastCharging::show_result() const
{
    std::print("{}\n", _res);
}
