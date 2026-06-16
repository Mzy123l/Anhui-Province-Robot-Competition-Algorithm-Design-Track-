#pragma once
#include <cstdint>
#include <limits>
#include "allocator.hpp"

namespace Tasks 
{
    /*
    * 【题目描述】
    在智能电动汽车的充电网络中，N 个充电站由单向行驶路段连接。
    一辆电动车需要完成恰好跨越 K 段路的旅程。电池容量必须不小于途经路段的最大能量消耗值,否则会在半路耗尽电量。
    充电站网络中存在自环与重边，需要仔细规划路线，使得所需的电池容量最小。
    给定一个有向图，包含 N 个节点和 M 条有向边，每条边的能量消耗值为 w。
    有 Q 次询问，每次询问给出起点 s 和整数 K。对于每个终点 j ∈ [1, N]，求从 s 出发,
    恰好经过 K 条边到达 j 的所有路径中，路径上最大边权的最小值。允许重复经过节点和边。
    若无法恰好 K 步到达 j，输出 −1。图中可能存在自环u → u 和重边，即同一对有向节点间可能有多条同向边，权值可能不同。
    但是司机们都是聪明的，它们会在重边走最短的那条，不过，他们可以通过走自环留在起点。不能呆在原地不动，因为这是违章停车。

    【输入格式】
    从标准输入读入数据。
    第一行两个整数 N, M，表示充电站数量和路段数量。
    接下来 M 行，每行三个整数 u, v, w，描述一条从 u 到 v 的有向路段，能量消耗为w。可能存在自环和重边。
    接下来一行一个整数 Q。
    接下来 Q 行，每行两个整数 s, K，表示从 s 出发恰好走 K 步。

    【输出格式】
    输出到标准输出。
    对于每个询问，输出一行 N 个整数，依次表示到达节点 1, 2, . . . , N 所需的最小电
    池容量。若无法到达，输出 −1。

    【数据范围与约定】
    对于 30% 的数据，N ≤ 10，M ≤ N^2，Q ≤ 10，K ≤ 100，1 ≤ w ≤ 10^9。
    对于 100% 的数据，N ≤ 100，M ≤ N^2，Q ≤ 1000，K ≤ 10^9，1 ≤ w ≤ 10^9。
    */
    class JourneyEndurance
    {
    public:
        JourneyEndurance() : _pool(), _alloc(_pool) {}
        void input();
        void solve();
        void show_result() const;

    private:
        static constexpr uint32_t INF = 0x3f3f3f3f;
        static constexpr std::size_t BLOCK_SIZE = 500000000; 
        static constexpr int MAX_LOG = 31;

        memory_pool::MemoryPool<BLOCK_SIZE> _pool;
        allocator::PoolAllocator<char, BLOCK_SIZE> _alloc;

        template<typename T>
        using Alloc = allocator::PoolAllocator<T, BLOCK_SIZE>;

        uint32_t N, M, Q;
        uint32_t* adj;                 // 邻接矩阵（扁平化）
        uint32_t* dp[MAX_LOG];        // 倍增矩阵
        uint32_t* results;            // 所有询问的结果，大小 Q * N

        inline uint32_t& mat(uint32_t* mat, uint32_t i, uint32_t j) const
        {
            return mat[(i - 1) * N + (j - 1)];
        }

        uint32_t* alloc_matrix()
        {
            void* ptr = _pool.allocate<uint32_t>(N * N);
            if (!ptr) std::abort();
            return static_cast<uint32_t*>(ptr);
        }
    };

} 