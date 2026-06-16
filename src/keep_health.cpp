#include "../include/keep_health.h"
#include <iostream>
#include <print>
#include <utility>

using namespace Tasks;

uint32_t KeepHealth::TimeStamp = 0;

KeepHealth::_battery::_battery(const _string& name, uint32_t count): _charge_count(count), _name(name)
{

    _time_stamp = ++TimeStamp;
}

bool KeepHealth::_battery::Compare::operator()(const _battery& a, const _battery& b) const
{
    if (a._charge_count != b._charge_count)
    {
        return a._charge_count > b._charge_count;
    }
    return a._time_stamp > b._time_stamp;
}

void KeepHealth::parse_stringline(char* head)
{
    
    char* current = head;
    while ((*current) != '\0')
    {
        switch (*current)
        {
        case '1':
        {
            ++current;
            while ((*current) == ' ')
            {
                ++current;
            }
            _src.emplace_back(1, current);
            return;
        }
        case '2':
        {
            _src.emplace_back(2, "");
            return;
        }
        case '3':
        {
            ++current;
            while ((*current) == ' ')
            {
                ++current;
            }
            _src.emplace_back(3, current);
            return;
        }
        case '4':
        {
            _src.emplace_back(4, "");
            return;
        }
        default:
            ++current;
        }
    }
    
}
void KeepHealth::input()
{
    _src.reserve(Q);
    char _input[16];
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    for (uint32_t i = 0; i < Q; ++i)
    {
        std::cin.getline(_input, 16);
        parse_stringline(_input);
    }

}
void KeepHealth::solve()
{
    _res.reserve(Q);
    std::vector<_battery, Alloc<_battery>> _temp{ Alloc<_battery>(_alloc) };
    _temp.reserve(Q);
    _outer.reserve(Q);
    _warehouse = std::priority_queue<_battery, std::vector<_battery, Alloc<_battery>>, 
        _battery::Compare>{ _battery::Compare {}, std::move(_temp) };


    for (const auto& op : _src)
    {
        switch (op.first)
        {
        case 1: _buy(op.second); break;
        case 2: _use(); break;
        case 3: _return(op.second); break;
        case 4: _check(); break;
        }
    }

}
void KeepHealth::_buy(const _string& str)
{
    _warehouse.emplace(str);
}
void KeepHealth::_use()
{
    const _battery& top = _warehouse.top();
    _outer[top._name] = top._charge_count;
    _res.emplace_back(top._name);
    _warehouse.pop();
}
void KeepHealth::_return(const _string& str)
{
    auto str_view = _string_view(str);
    uint32_t count = ++_outer[str_view];
    if (count == K)
    {
        _res.emplace_back(str_view);
    }
    else
    {
        _warehouse.emplace(str, count);
    }
}
void KeepHealth::_check()
{
    _res.emplace_back(static_cast<uint32_t>(_warehouse.size()));
}
void KeepHealth::show_result() const
{
    for (const auto& res : _res)
    {
        std::visit([](const auto& val) 
            {
                std::println ("{}", val);
            }, res);
    }
}