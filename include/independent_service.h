#pragma once

#include "allocator.hpp"
#include <cstdint>
#include <print>
#include <vector>
#include <algorithm>
#include <variant>
#include <memory>
#include <queue>
#include <functional>
#include <unordered_map>

namespace Tasks
{

    class IndependentService
    {
    public:
        IndependentService();

        void input();
        void solve();
        void show_result() const;

    private:

        static constexpr size_t BLOCK_SIZE = 500000000;
        memory_pool::MemoryPool<BLOCK_SIZE> _pool;
        allocator::PoolAllocator<char, BLOCK_SIZE> _alloc;

        // ---------- 图结构 ----------
        struct Edge
        {
            int32_t to;
            int32_t next;
            int64_t w;
        };

        int32_t _n;
        std::vector<int32_t, allocator::PoolAllocator<int32_t, BLOCK_SIZE>> _head;
        std::vector<Edge, allocator::PoolAllocator<Edge, BLOCK_SIZE>> _edges;
        int32_t _edge_cnt;

        std::vector<int64_t, allocator::PoolAllocator<int64_t, BLOCK_SIZE>> _d;
        std::vector<std::pair<int32_t, int32_t>, allocator::PoolAllocator<std::pair<int32_t, int32_t>, BLOCK_SIZE>> _edge_info;

        // ---------- 操作 ----------
        struct OpModNode { int32_t u; int64_t val; };
        struct OpModEdge { int32_t idx; int64_t w; };
        struct OpQueryIndex { int32_t qid; };

        using Operation = std::variant<OpModNode, OpModEdge, OpQueryIndex>;
        std::vector<Operation, allocator::PoolAllocator<Operation, BLOCK_SIZE>> _ops;

        // 集中存储所有查询的 centers 数据
        std::vector<int32_t, allocator::PoolAllocator<int32_t, BLOCK_SIZE>> _all_centers;
        std::vector<std::pair<int32_t, int32_t>, allocator::PoolAllocator<std::pair<int32_t, int32_t>, BLOCK_SIZE>> _query_ranges;

        // ---------- 输出 ----------
        std::vector<int64_t, allocator::PoolAllocator<int64_t, BLOCK_SIZE>> _results;

        // ---------- 树链剖分 ----------
        std::vector<int32_t, allocator::PoolAllocator<int32_t, BLOCK_SIZE>> _fa, _dep, _sz, _son, _top, _dfn, _rnk;
        std::vector<int64_t, allocator::PoolAllocator<int64_t, BLOCK_SIZE>> _edge_val;  // 边权转点权（深度大的节点）
        int32_t _dfs_clock;

        // ---------- 线段树（点权和边权）—— 使用树状数组，不占用内存池 ----------
        class SegTree
        {
        public:
            int32_t n;
            std::vector<int64_t> bit;
            std::vector<int64_t> val; // 当前叶子节点值

            SegTree(int32_t n_) : n(n_), bit(n_ + 2, 0), val(n_ + 1, 0) {}

            void build(int32_t /*p*/, int32_t /*l*/, int32_t /*r*/, const std::vector<int64_t>& arr)
            {
                for (int i = 1; i <= n; ++i)
                {
                    val[i] = arr[i];
                    _add(i, arr[i]);
                }
            }

            void update(int32_t /*p*/, int32_t /*l*/, int32_t /*r*/, int32_t pos, int64_t new_val)
            {
                int64_t delta = new_val - val[pos];
                if (delta != 0)
                {
                    val[pos] = new_val;
                    _add(pos, delta);
                }
            }

            int64_t query(int32_t /*p*/, int32_t /*l*/, int32_t /*r*/, int32_t ql, int32_t qr)
            {
                if (ql > qr) return 0;
                return _sum(qr) - _sum(ql - 1);
            }

        private:
            void _add(int32_t idx, int64_t delta)
            {
                while (idx <= n)
                {
                    bit[idx] += delta;
                    idx += idx & -idx;
                }
            }

            int64_t _sum(int32_t idx)
            {
                int64_t res = 0;
                while (idx > 0)
                {
                    res += bit[idx];
                    idx -= idx & -idx;
                }
                return res;
            }
        };

        std::unique_ptr<SegTree> _seg_point;   // 点权线段树
        std::unique_ptr<SegTree> _seg_edge;    // 边权线段树（转为点权）

        // ---------- 辅助函数 ----------
        void add_edge(int32_t u, int32_t v, int64_t w);
        void dfs1(int32_t u, int32_t f);
        void dfs2(int32_t u, int32_t tp);
        int32_t lca(int32_t u, int32_t v);
        int64_t path_dist(int32_t u, int32_t v);       // 树上距离
        int64_t subtree_sum(int32_t u);            // 点权子树和（通过 dfn 区间）

        // 虚树构建与查询
        void build_virtual_tree(const std::vector<int32_t>& nodes,
            std::vector<int32_t>& vt_nodes,
            std::vector<std::vector<int32_t>>& vt_adj,
            std::vector<int32_t>& vt_parent,
            std::vector<int64_t>& vt_dist_from_root,
            std::unordered_map<int32_t, int32_t>& id_map);
        void compute_query(const int32_t* centers, int32_t k, std::vector<int64_t>& out);
    };

} 