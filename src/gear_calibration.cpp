#include "../include/gear_calibration.h"
#include <iostream>
#include <print>

using namespace Tasks;


bool GearCalibration::_state::operator==(const _state& other) const
{
    return p[0] == other.p[0] && p[1] == other.p[1] && p[2] == other.p[2] && p[3] == other.p[3];
}

std::size_t GearCalibration::_state::hash::operator()(const _state& s)const
{
    return (std::hash<int>()(s.p[0])) ^ (std::hash<int>()(s.p[1]) << 1) ^ (std::hash<int>()(s.p[2]) << 2) ^ (std::hash<int>()(s.p[3]) << 3);
}

bool GearCalibration::is_target_reached(const int16_t* p) const
{
    return p[0] >= _target[0] && p[1] >= _target[1] && p[2] >= _target[2] && p[3] >= _target[3];
}
int32_t GearCalibration::estimate_min_cost(const int16_t* p) const
{
    int16_t need[4];
    for (uint8_t i = 0; i < static_cast<uint8_t>(4); ++i)
    {
        need[i] = std::max(_target[i] - p[i], 0);
    }

    if (need[0] == 0 && need[1] == 0 && need[2] == 0 && need[3] == 0)
        return 0;

    // 寻找单位成本总产出最高的产线, 乐观估计
    double max_efficiency = 0.0;
    for (uint8_t i = 0; i < _m; ++i)
    {
        if (_lines[i].cost == 0) continue; // 避免除以0
        // 总产出 = 四种元素产量之和
        double total_output = _lines[i].prod[0] + _lines[i].prod[1] + _lines[i].prod[2] + _lines[i].prod[3];
        double efficiency = total_output / static_cast<double>(_lines[i].cost);
        if (efficiency > max_efficiency)
        {
            max_efficiency = efficiency;
        }
    }
    // 如果没有有效的产线（例如所有产线成本均为0但产量也为0），则无法填补
    if (max_efficiency == 0.0)
    {
        return INT_MAX; // 返回无穷大，表示不可能
    }
    // 总缺口（四种元素缺口之和）
    double total_needed = need[0] + need[1] + need[2] + need[3];
    // 最少需要的成本 = 总缺口 / 最高效率，向上取整
    double est = total_needed / max_efficiency;
    return static_cast<int>(std::ceil(est));
}
void GearCalibration::dfs(uint8_t start, int32_t current_total_cost, const int16_t prod[4])
{
    // 如果当前成本已经 ≥ 已知最优解，直接返回
    if (current_total_cost >= best_cost) return;

    // 当前成本 + 估算最小剩余成本 ≥ 已知最优解，则不可能更优
    if (current_total_cost + estimate_min_cost(prod) >= best_cost) return;

    // ----- 检查是否达到目标 -----
    if (is_target_reached(prod))
    {
        // 更新最优解
        if (current_total_cost < best_cost)
        {
            best_cost = current_total_cost;
        }
        return;
    }
    // 将当前产量打包成 _state 对象
    _state cur_state;
    std::memcpy(cur_state.p, prod, sizeof(int16_t) * 4);

    // 查找是否已经访问过相同的产量组合
    auto it = _map.find(cur_state);
    if (it != _map.end() && it->second <= current_total_cost)
    {
        // 如果之前到达该状态的花费 ≤ 当前花费，则当前路径不可能更优
        return;
    }
    // 否则，记录当前状态的最小花费
    _map[cur_state] = current_total_cost;

    // 递归枚举
    for (uint8_t i = start; i < _m; ++i)
    {
        int16_t new_prod[4];
        for (uint8_t j = 0; j < static_cast<uint8_t>(4); ++j)
        {
            new_prod[j] = prod[j] + _lines[i].prod[j];
        }
        dfs(i + 1, current_total_cost + _lines[i].cost, new_prod);
    }
}

void GearCalibration::solve()
{
    int16_t start[4] = { 0,0,0,0 };
    dfs(0, 0, start);
}
void GearCalibration::input(int16_t* target, uint16_t m)
{
    for (uint8_t i = 0; i < static_cast<uint8_t>(4); ++i)
    {
        _target[i] = target[i];
    }
    _m = m;
    _lines.resize(_m);
    for (uint8_t i = 0; i < _m; ++i)
    {
        for (uint8_t j = 0; j < static_cast<uint8_t>(4); ++j)
        {
            std::cin >> _lines[i].prod[j];
        }
        std::cin >> _lines[i].cost;
    }
}

void GearCalibration::show_result() const
{
    if (best_cost == INT_MAX)
        std::println("Impossible");
    else
        std::println("{}", best_cost);
}



void GearCalibrationWithSmallElems::solve()
{
    best_cost_ = INT_MAX;
    dfs(0, 0);
}

void GearCalibrationWithSmallElems::dfs(int32_t idx, int32_t current_cost)
{
    // 检查是否已达到目标
    if (current_prod_[0] >= target_[0] &&
        current_prod_[1] >= target_[1] &&
        current_prod_[2] >= target_[2] &&
        current_prod_[3] >= target_[3])
    {
        if (current_cost < best_cost_)
        {
            best_cost_ = current_cost;
        }
        return;
    }

    // 剪枝：当前成本已不小于已知最优解
    if (current_cost >= best_cost_) return;

    // 超出产线范围
    if (idx >= static_cast<int32_t>(lines_.size())) return;

    // ---- 不选当前产线 ----
    dfs(idx + 1, current_cost);

    // ---- 选当前产线 ----
    // 备份当前产量，便于回溯
    int32_t backup[4];
    std::memcpy(backup, current_prod_, sizeof(backup));

    // 累加产量
    for (int j = 0; j < 4; ++j)
    {
        current_prod_[j] += lines_[idx].prod[j];
    }
    dfs(idx + 1, current_cost + lines_[idx].cost);

    // 回溯恢复产量
    std::memcpy(current_prod_, backup, sizeof(backup));
}
void GearCalibrationWithSmallElems::input(int16_t* target, uint16_t m)
{
    for (uint8_t i = 0; i < static_cast<uint8_t>(4); ++i)
    {
        target_[i] = target[i];
    }
    _m = m;
    lines_.resize(_m);
    for (uint8_t i = 0; i < _m; ++i)
    {
        for (uint8_t j = 0; j < static_cast<uint8_t>(4); ++j)
        {
            std::cin >> lines_[i].prod[j];
        }
        std::cin >> lines_[i].cost;
    }
}

void GearCalibrationWithSmallElems::show_result() const
{
    if (best_cost_ == INT_MAX)
        std::println("Impossible");
    else
        std::println("{}", best_cost_);
}