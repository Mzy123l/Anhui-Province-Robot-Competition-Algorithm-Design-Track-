#pragma once
#include <cstdint>
#include "allocator.hpp"

namespace Tasks
{
    // 能源划分 / Energy classification
    /*
    * 【题目描述】某新能源公司运营着一条由 n 个光伏阵列组成的发电带。
    第 i 个光伏阵列在某一时刻的实际出力（单位：kW）为 ai。
    为了接入电网，公司需要将这 n 个阵列依次划分成恰好 m 个连续的运行周期，每个周期至少包含一个阵列。
    对于每个运行周期，定义其波动风险值为：周期长度 × ( 周期内的最大出力 - 周期内的最小出力)
    整个调度方案的总风险值定义为所有周期中波动风险值的最大值。
    请你设计一个划分方案，使得总风险值最小，并输出该最小值。

    【输入格式】
    从标准输入读入数据。
    第一行：两个整数 n, m。
    第二行：n 个整数 a1, a2, . . . , an 表示每个光伏阵列的实时出力

    【输出格式】
    输出到标准输出。
    输出一个整数，表示最小可能的总风险值。

    【数据范围与约定】
    对于 40% 的数据，1 ≤ m ≤ n ≤ 1000，1 ≤ ai ≤ 1000。
    对于 100% 的数据，1 ≤ m ≤ n ≤ 10^5，1 ≤ ai ≤ 10^9。
    */
    class EnergyClassification
    {
    public:
        EnergyClassification() : _pool(), _alloc(_pool) {}
        void input();
        void solve();
        void show_result() const;
    private:
        inline uint32_t min_segments(uint64_t limit);
    private:
        static constexpr uint32_t _block_size = 400000;
        uint32_t* _values;
        uint64_t _result;
        uint32_t _n;
        uint32_t _m;

        memory_pool::MemoryPool<_block_size> _pool;
        allocator::PoolAllocator<uint32_t, _block_size> _alloc;

    };
}