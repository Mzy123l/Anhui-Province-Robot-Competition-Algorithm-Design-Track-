#pragma once

#include "allocator.hpp"
#include <cstdint>
#include <print>
#include <vector>
#include <algorithm>
#include <variant>

namespace Tasks {

    class IndependentService
    {
    public:
        IndependentService();

        void input();
        void solve();
        void show_result() const;

    private:
        static constexpr size_t BLOCK_SIZE = 131072;  // 128KB 块，总池 ≤ 16MB
        memory_pool::MemoryPool<BLOCK_SIZE> _pool;
        allocator::PoolAllocator<char, BLOCK_SIZE> _alloc;

        // ---------- 图结构 ----------
        struct Edge
        {
            int to;
            int next;
            int64_t w;
        };

        int _n;
        std::vector<int, allocator::PoolAllocator<int, BLOCK_SIZE>> _head;
        std::vector<Edge, allocator::PoolAllocator<Edge, BLOCK_SIZE>> _edges;
        int _edge_cnt;

        std::vector<int64_t, allocator::PoolAllocator<int64_t, BLOCK_SIZE>> _d;
        std::vector<std::pair<int, int>, allocator::PoolAllocator<std::pair<int, int>, BLOCK_SIZE>> _edge_info;

        // ---------- 操作 ----------
        struct OpModNode { int u; int64_t val; };
        struct OpModEdge { int idx; int64_t w; };
        struct OpQueryIndex { int qid; };  // 查询索引

        using Operation = std::variant<OpModNode, OpModEdge, OpQueryIndex>;
        std::vector<Operation, allocator::PoolAllocator<Operation, BLOCK_SIZE>> _ops;

        // 集中存储所有查询的 centers 数据
        std::vector<int, allocator::PoolAllocator<int, BLOCK_SIZE>> _all_centers;
        std::vector<std::pair<int, int>, allocator::PoolAllocator<std::pair<int, int>, BLOCK_SIZE>> _query_ranges; // (start, len)

        // ---------- 输出 ----------
        std::vector<int64_t, allocator::PoolAllocator<int64_t, BLOCK_SIZE>> _results;

        // ---------- Dijkstra 辅助 ----------
        struct HeapNode
        {
            int64_t dist;
            int node;
            bool operator>(const HeapNode& o) const { return dist > o.dist; }
        };

        std::vector<int64_t, allocator::PoolAllocator<int64_t, BLOCK_SIZE>> _dist;
        std::vector<int, allocator::PoolAllocator<int, BLOCK_SIZE>> _center_id;
        std::vector<bool, allocator::PoolAllocator<bool, BLOCK_SIZE>> _vis;

        void add_edge(int u, int v, int64_t w);
        void dijkstra(const int* centers, int k, std::vector<int64_t, allocator::PoolAllocator<int64_t, BLOCK_SIZE>>& out);
    };

} // namespace Tasks