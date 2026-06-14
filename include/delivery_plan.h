#pragma once
#include <cstdint>
#include "allocator.hpp"
#include <unordered_map>
#include <vector>
#include <utility>
#include <type_traits>

namespace Tasks
{
    // 平滑信号 / Delivery plan
    /*
    * 【题目背景】
    给定一个长度为 n 的整数序列 A = [A1, A2, . . . , An] 以及一个正整数 k。
    对于窗口长度 w（1 ≤ w ≤ k），我们使用大小为 w 的一维滑动均值滤波器对序列进行处理：
    对于所有 i = 1, 2, . . . , n − w + 1，计算Bi(w) =1/w ∑w−1j=0 Ai+j,得到长度为 n − w + 1 的新序列 B(w)。
    如果对于所. 有. w ∈ [1, k]，滤波后的序列 B(w) 中的每一个值都相等（即 B(w) 是常数序列），那么我们称原序列 A 是平滑的。
    你可以任意修改序列 A 中的若干元素（修改后的值可以是任意整数）.
    请问：最少需要修改多少个元素，才能使得修改后的序列成为平滑序列？

    【输入格式】
    从标准输入读入数据。
    第一行包含两个整数 n, k，表示序列长度和窗口大小的上限。
    第二行包含 n 个整数 A1, A2, . . . , An，表示给定的序列。

    【输出格式】
    输出到标准输出。
    输出一个整数，表示最少需要修改的元素数量。

    【数据范围与约定】
    对于全部测试数据，满足：1 ≤ n ≤ 2 × 10^5，1 ≤ k ≤ n，1 ≤ Ai ≤ 10^9
    */

    /*
    *  思路: 题目等价于找出最多的元素个数，直接哈希即可
    *  对于小数据直接排序, 哈希缓存极差
    *  @WARNING: 对于 1`2*10^5， 排序始终优于哈希
    */
    class DeliveryPlan
    {
    public:
        DeliveryPlan() : _pool(), _alloc(_pool), _freq(_alloc) {}
        void input(uint32_t n);
        void solve();
        void show_result() const;

    private:
        static constexpr uint32_t _block_size = 1000000;
        memory_pool::MemoryPool<_block_size> _pool;
        allocator::PoolAllocator<uint32_t, _block_size> _alloc;

        uint32_t _n;
        uint32_t* _src;

        std::unordered_map<uint32_t, uint32_t, std::hash<uint32_t>, std::equal_to<uint32_t>, 
            allocator::PoolAllocator<std::pair<const uint32_t, uint32_t>, _block_size>> _freq;
        uint32_t _res;
        
    };

    // n 较小时
    class DeliveryPlanWithSmallElems
    {
    public:
        DeliveryPlanWithSmallElems() : _pool(), _alloc(_pool) {}
        void input(uint32_t n);
        void solve();
        void show_result() const;

    private:
        static constexpr std::size_t _block_size = 800000;
        memory_pool::MemoryPool<_block_size> _pool;
        allocator::PoolAllocator<uint32_t, _block_size> _alloc;

        uint32_t _n;
        uint32_t _res;
        uint32_t* _src;
    };
}