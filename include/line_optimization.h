#pragma once
#include <limits>
#include <cstdint>
#include "allocator.hpp"
#include <vector>
#include <array>

namespace Tasks
{
    /*
    * 【题目描述】
    某电动汽车工厂拥有一条由多道工序组成的流水生产线。
    生产线可以抽象为一张有向无环图，每个节点代表一个生产状态，每条有向边代表一道工序，工序的耗时即为边的长度。
    工厂的工程师可以投入资源对若干道工序进行优化，将它们的耗时分别缩短。
    请帮助工程师选择最优的优化方案，使得从原材料（节点 1）到成品整车（节点 n）的总耗时最小。
    形式化地，给定一张有向无环图，节点编号为 1 到 n，保证对所有边 (u, v) 均有 u < v 。
    图中有 m 条边，每条边由四个整数 u, v, w, c 描述，表示一条从 u 指向 v 的工序，
    原始耗时为 w，优化后的耗时为 c，满足 0 < c < w。
    你可以选择最多 K 条边，将其中每条边的耗时从 w 改为 c。
    求在此操作后，从节点 1 到节点 n 的最短路径的长度的最小值。

    【输入格式】
    从标准输入读入数据。
    第一行包含三个正整数 n, m, K，分别表示图的节点数、边数和最多可优化的边数。
    接下来 m 行，每行包含四个正整数 u, v, w, c，描述一条有向边及其优化前后的耗时。保证 u < v，c < w。

    【输出格式】
    输出到标准输出。
    输出一行一个整数，表示优化最多 K 条边之后，从节点 1 到节点 n 的最短路径的最小可能长度。

    【数据范围与约定】
    对于 30% 的数据，n ≤ 2000，m ≤ 10000，K = 1，1 ≤ c < w ≤ 104。
    对于 100% 的数据，n ≤ 2 × 10^5，m ≤ 5 × 10^5，K ≤ 100，1 ≤ c < w ≤ 10^4。
    */

    /*
    * 思路: 动态规划 + 拓扑序转移
    * dp[u][k] 表示从节点1到节点u，恰好使用了k次优化时的最短路径长度
    */
    class LineOptimization
    {
    public:
        LineOptimization() :_pool(), _alloc(_pool),
            _adj(Alloc<std::vector<edge, Alloc<edge>>>(_alloc)),
            _dp(Alloc<std::vector<uint32_t, Alloc<uint32_t>>>(_alloc))
        {}
        void input();
        void solve();
        void show_result() const;
    private:
        static constexpr uint32_t INF = std::numeric_limits<uint32_t>::max();

        static constexpr std::size_t _block_size = 209715200;
        memory_pool::MemoryPool<_block_size> _pool;
        allocator::PoolAllocator<char, _block_size> _alloc;

        template <typename T>
        using Alloc = allocator::PoolAllocator<T, _block_size>;

        struct edge
        {
            edge(uint32_t v, uint32_t w, uint32_t c) :v(v), w(w), c(c) {}
            uint32_t v, w, c;
        };
        uint32_t n, m, K, _res;
        // 邻接表存储图，只存出边
        std::vector<std::vector<edge, Alloc<edge>>, Alloc<std::vector<edge, Alloc<edge>>>> _adj;
        // dp[u][k] 使用自定义分配器
        std::vector<std::vector<uint32_t, Alloc<uint32_t>>, Alloc<std::vector<uint32_t, Alloc<uint32_t>>>> _dp;
    };
}