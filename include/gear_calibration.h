#pragma once
#include "allocator.hpp"
#include <unordered_map>
#include <vector>

namespace Tasks
{
    // 出厂校准 / Gear calibration
    /*
    * 【题目描述】
    随着新能源汽车产业的爆发式增长，退役动力电池的回收利用已成为关键环节。
    电池中含有四种关键金属元素：Li（锂）、Co（钴）、Ni（镍）、Mn（锰）。
    你负责运营一家电池回收工厂，现有 m 条不同的回收产线。
    每条产线最多只能启用一次，启用后会产出一定数量的四种元素，并产生相应的运营成本。
    集团总部给出了本月的最低回收指标，要求四种元素的最终库存量均不低于目标值。
    请你计算：满足指标所需的最小总成本。如果无论如何都无法达成目标，请输出 Impossible。

    【输入格式】
    从标准输入读入数据。
    输入共 m + 2 行：第 1 行为四个整数 Limin，Comin，N imin，Mnmin，表示目标产量；
    第 2 行为整数 m，表示产线数量；第 3 行至第 m + 2 行每行为 Lii，Coi，N ii，Mni，costi，依次为第 i 条产线的四种元素产量与运营成本。

    【输出格式】
    输出到标准输出。
    输出仅一行：若可达标，输出一个整数最小总成本；否则输出字符串 Impossible。

    【数据范围与约定】
    对于 40% 的数据，1 ≤ m ≤ 3，Limin, Comin, N imin, Mnmin, Lii, Coi, N ii, Mni ≤ 10。
    对于全部数据，1 ≤ m ≤ 100，Limin, Comin, N imin, Mnmin, Lii, Coi, N ii, Mni ≤ 30，保证答案小于 2147483647。
    */

    /*
    * 思路：IDA* + 状态压缩
    * @WARNING: 哈希具有巨大的常数开销，因此小数据集直接枚举
    */
    class GearCalibration
    {
    public:
        GearCalibration() :_pool(), _alloc(_pool),
            _map(allocator::PoolAllocator<std::pair <const _state, int32_t>, _block_size>(_alloc)),
            _lines(allocator::PoolAllocator<_line, _block_size>(_alloc))
        {}
        void input(int16_t*, uint16_t);
        void solve();
        void show_result() const;

    private:
        void dfs(uint8_t start, int32_t current_total_cost, const int16_t prod[4]);
        int32_t estimate_min_cost(const int16_t* p) const;
        bool is_target_reached(const int16_t* p) const;
    private:

        static constexpr std::size_t _block_size = 65536;
        // 产线总数
        uint16_t _m;
        // 目标需求量（Li, Co, Ni, Mn）
        int16_t _target[4];

        // 记录到目前为止找到的最小总成本，初始化为无穷大
        int32_t best_cost = INT_MAX;

        memory_pool::MemoryPool <_block_size> _pool;
        allocator::PoolAllocator <int32_t, _block_size> _alloc;

        // 状态结构体，用于哈希表键
        struct _state
        {
            uint16_t p[4];
            bool operator==(const _state& other) const;
            struct hash
            {
                std::size_t operator()(const _state& s) const;
            };
        };

        // 全局哈希表，key=_state（当前累计产量），value=达到该状态的最小成本
        // 用于状态去重剪枝：如果再次到达相同的产量但成本更高，则无需继续
        std::unordered_map <_state, int32_t, _state::hash, std::equal_to<_state>, 
            allocator::PoolAllocator<std::pair <const _state, int32_t>, _block_size> > _map;

        // 生产线结构体
        struct _line
        {
            uint16_t prod[4];
            int32_t cost;
        };
        std::vector <_line, allocator::PoolAllocator<_line, _block_size>> _lines;
    };

    // 小数据集
    class GearCalibrationWithSmallElems
    {
    public:
        // 产线数据结构
        struct Line
        {
            int16_t prod[4];    // Li, Co, Ni, Mn 产量
            int32_t cost;       // 成本
        };

        // 构造函数：设置四种元素的目标需求量
        GearCalibrationWithSmallElems() :_pool(), _alloc(_pool), lines_(allocator::PoolAllocator<Line, _block_size>(_alloc)) {}

    public:
        // 求解最小成本，若无解返回 -1
        void input(int16_t* target, uint16_t m);
        void solve();
        void show_result() const;

    private:
        // 产线总数
        uint16_t _m;
        int16_t target_[4];                          // 目标需求量
        constexpr static std::size_t _block_size = 8000000;
        memory_pool::MemoryPool <_block_size> _pool;
        allocator::PoolAllocator <int32_t, _block_size> _alloc;
        std::vector<Line, allocator::PoolAllocator<Line, _block_size>> lines_;
        int32_t best_cost_ = INT_MAX;                          // 当前最优解
        int16_t current_prod_[4];                    // 当前累计产量

        // 深度优先搜索（暴力枚举所有子集）
        void dfs(int32_t idx, int32_t current_cost);
    };
}