#include "../include/brake_distance.h"
#include <print>
#include <algorithm>

using namespace Tasks;

void BrakeDistance::solve(uint32_t n)
{
    _result = _alloc.allocate(64);
    auto current = _result;
    while (n > 1)
    {
        if (n == 2 || n == 3 || n == 7)
        {
            --n;
            _alloc.construct(current, 'A');
        }
        else if (n % 2 == 0)
        {
            n /= 2;
            _alloc.construct(current, 'B');
        }
        else if (n % 4 == 1)
        {
            --n;
            _alloc.construct(current, 'A');
        }
        else
        {
            ++n;
            _alloc.construct(current, 'C');
        }
        ++current;
        
    }
    _steps = current - _result;
    _alloc.construct(current, '\0');
    std::reverse(_result, current);
}


void BrakeDistance::show_result() const
{
    std::print("{}\n{}\n", _steps, _result);
}