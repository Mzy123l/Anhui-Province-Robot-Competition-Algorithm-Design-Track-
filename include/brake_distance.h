#pragma once
#include "allocator.hpp"
#include <print>
#include <cstdint>

namespace Tasks
{
    // A.间距调整 / Brake distance
    /*在智能网联汽车队列中，车辆通过V2V通信协调行驶，并实时调整与前车的车间距，
    定义当前车间距为一个正整数，初始间距为1（单位：米）。
    一种自适应间距控制策略允许在每一轮控制周期中，对当前间距执行三种原子操作之一：
        • A操作（线性增加）：令间距 + 1。
        • B操作（倍增扩大）：令间距×2。
        • C操作（回退缩小）：令间距−1，但仅当当前间距严格大于1时允许执行（间距始终保持为正整数）。
        每执行一次操作计为一步。现给定目标间距n，请你构造一条从1到n的操作序列，使总步数最少。
        若存在多条最少步数的序列，输出字典序最小的操作串。*/
    /*
    * 【输入格式】
    * 从标准输入读入数据。
    * 一行一个整数n，表示目标车间距。
    *
    * 【输出格式】
    * 第一行输出一个整数S，表示最少操作步数。
    * 第二行输出一个长度恰好为S的字符串，由A、B、C构成，表示最优操作序列。
    * 
    * 【数据范围与约定】
    * 对于10%的数据，n≤10。
    * 对于30%的数据，n≤500。
    * 对于50%的数据，n≤3000。
    * 对于100%的数据，1≤n≤109。
    */

    /*
    * 思路: 逆向贪心构造法
    */
    class BrakeDistance
    {
    public:
        BrakeDistance() : _pool(), _alloc(_pool), _result() {} // 加快速度，构造函数不初始化_result
        void solve(uint32_t n);
        void show_result() const;

   
        
    private:
        memory_pool::MemoryPool<64> _pool;
        allocator::PoolAllocator<char, 64> _alloc;
        char* _result;
        uint8_t _steps;
    };
}