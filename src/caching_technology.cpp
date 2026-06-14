#include "../include/caching_technology.h"
#include <string>
#include <print>
using namespace Tasks;

void CachingTechnology::solve(char* head, uint16_t C)
{
    _c = C;
    _result.reserve(4000);
    char* current = head;
    bool is_read;
    bool has_set_key = false;
    int32_t key;
    int32_t value;
    while (*current != '\0')
    {
        if (*current == 'A')
        {
            is_read = true;
            ++current;
            continue;
        }
        if (*current == 'I')
        {
            is_read = false;
            ++current;
            continue;
        }
        if (*current == ';')
        {
            ++current;
            has_set_key = false;
            continue;
        }
        if ((*current) >= '0' && (*current) <= '9')
        {
            if (has_set_key)
            {
                value = std::stoi(current);
                write(key, value);
                while ((*current) != ';' && (*current) != '\0')
                {
                    ++current;
                }
                has_set_key = false;
            }
            else
            {
                key = std::stoi(current);
                if (is_read)
                {
                    read(key);
                    while ((*current) != ';' && (*current) != '\0')
                    {
                        ++current;
                    }
                    has_set_key = false;
                }
                else
                {
                    has_set_key = true;
                    while ((*current) != ' ')
                    {
                        ++current;
                    }
                }
            }

        }
        ++current;
    }
}

inline void CachingTechnology::read(int32_t key)
{
    if (!_map.contains(key))
    {
        _result.emplace_back(true, false, key, key);
        _list.emplace_front(key, key, true, true);
        _map[key] = _list.begin();
        
        if (_current_count == _c)
        {
            int32_t back_key = _list.back()._key;
            _list.back()._inqueue = false;
            _list.back()._dirty = false;
            _list_outer.push_front(_list.back());
            _list.pop_back();
            _map[back_key] = _list_outer.begin();
            return;
        }
        ++_current_count;
        return;
    }
    list::iterator it = _map[key];
    auto current = *it;
    if (current._inqueue)
    {
        _result.emplace_back(true, true, key, current._value);
        _list.emplace_front(key, current._value, false, true);
        _list.erase(it);
    }
    else
    {
        _result.emplace_back(true, false, key, current._value);
        _list.emplace_front(key, current._value, false, true);
        _list_outer.erase(it);
        if (_current_count == _c)
        {
            int32_t back_key = _list.back()._key;
            _list.back()._inqueue = false;
            _list.back()._dirty = false;
            _list_outer.push_front(_list.back());
            _list.pop_back();
            _map[back_key] = _list_outer.begin();
        }
    }


    _map[key] = _list.begin();
}

inline void CachingTechnology::write(int32_t key, int32_t value)
{
    if (!_map.contains(key))
    {
        _result.emplace_back(false, false, key, value);
        _list.emplace_front(key, value, true, true);
        _map[key] = _list.begin();

        if (_current_count == _c)
        {
            int32_t back_key = _list.back()._key;
            _list.back()._inqueue = false;
            _list.back()._dirty = false;
            _list_outer.push_front(_list.back());
            _list.pop_back();
            _map[back_key] = _list_outer.begin();
            return;
        }
        ++_current_count;
        return;
    }
    list::iterator it = _map[key];
    auto current = *it;
    if (current._inqueue)
    {
        _result.emplace_back(false, true, key, value);
        current._dirty = true;
        _list.emplace_front(key, value, true, true);
        _list.erase(it);
    }
    else
    {
        _result.emplace_back(false, false, key, value);
        _list.emplace_front(key, value, true, true);
        _list_outer.erase(it);
        if (_current_count == _c)
        {
            int32_t back_key = _list.back()._key;
            _list.back()._inqueue = false;
            _list.back()._dirty = false;
            _list_outer.push_front(_list.back());
            _list.pop_back();
            _map[back_key] = _list_outer.begin();
        }
    }
    
    _map[key] = _list.begin();
}
void CachingTechnology::show_result() const
{

    for (const auto& elem : _result)
    {

        if (std::get<_is_read>(elem))
        {
            if (std::get<_is_hit>(elem))
            {
                std::println("READ {}:Hit,value={}", std::get<_key>(elem), std::get<_value>(elem));
            }
            else
            {
                std::println("READ {}:Miss,loaded,value={}", std::get<_key>(elem), std::get<_value>(elem));
            }
        }
        else
        {
            if (std::get<_is_hit>(elem))
            {
                std::println("WRITE {} {}:Hit,updated", std::get<_key>(elem), std::get<_value>(elem));

            }
            else
            {
                std::println("WRITE {} {}:Miss,loaded and updated", std::get<_key>(elem), std::get<_value>(elem));
            }
        }
    }
    std::println();
    if (_c == 0)
    {
        std::println("Cache is empty.");
    }
    else
    {
        std::println("Cachestate(MRU‐>LRU):");
        for (const auto& it : _list)
        {
            if (!it._inqueue)return;
            std::println("{}:{}", it._key, it._value);
        }
    }
    
}