#include "../include/keep_health.h"
#include <print>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

using namespace Tasks;

namespace
{
    struct WarehouseBattery
    {
        int32_t soh;
        uint32_t order;
        std::string name;
    };

    struct WarehouseCmp
    {
        bool operator()(const WarehouseBattery& lhs, const WarehouseBattery& rhs) const
        {
            if (lhs.soh != rhs.soh)
            {
                return lhs.soh > rhs.soh;
            }
            return lhs.order > rhs.order;
        }
    };
}

void KeepHealth::solve()
{
    uint32_t q = 0;
    int32_t k = 0;
    std::cin >> q >> k;

    std::priority_queue<WarehouseBattery, std::vector<WarehouseBattery>, WarehouseCmp> warehouse;
    std::unordered_map<std::string, WarehouseBattery> borrowed;
    uint32_t insert_order = 0;

    for (uint32_t i = 0; i < q; ++i)
    {
        int32_t op = 0;
        std::cin >> op;
        if (op == 1)
        {
            std::string name;
            std::cin >> name;
            warehouse.push(WarehouseBattery{0, insert_order++, name});
        }
        else if (op == 2)
        {
            WarehouseBattery battery = warehouse.top();
            warehouse.pop();
            borrowed[battery.name] = battery;
            std::print("{}\n", battery.name);
        }
        else if (op == 3)
        {
            std::string name;
            std::cin >> name;
            WarehouseBattery battery = borrowed[name];
            borrowed.erase(name);
            ++battery.soh;
            if (battery.soh >= k)
            {
                std::print("{}\n", name);
            }
            else
            {
                warehouse.push(battery);
            }
        }
        else
        {
            std::print("{}\n", warehouse.size());
        }
    }
}
