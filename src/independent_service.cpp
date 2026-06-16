#include "../include/independent_service.h"
#include <limits>
#include <iostream>
namespace Tasks {

    IndependentService::IndependentService()
        : _pool(),
        _alloc(_pool),
        _head(allocator::PoolAllocator<int, BLOCK_SIZE>(_alloc)),
        _edges(allocator::PoolAllocator<Edge, BLOCK_SIZE>(_alloc)),
        _d(allocator::PoolAllocator<int64_t, BLOCK_SIZE>(_alloc)),
        _edge_info(allocator::PoolAllocator<std::pair<int, int>, BLOCK_SIZE>(_alloc)),
        _ops(allocator::PoolAllocator<Operation, BLOCK_SIZE>(_alloc)),
        _all_centers(allocator::PoolAllocator<int, BLOCK_SIZE>(_alloc)),
        _query_ranges(allocator::PoolAllocator<std::pair<int, int>, BLOCK_SIZE>(_alloc)),
        _results(allocator::PoolAllocator<int64_t, BLOCK_SIZE>(_alloc)),
        _dist(allocator::PoolAllocator<int64_t, BLOCK_SIZE>(_alloc)),
        _center_id(allocator::PoolAllocator<int, BLOCK_SIZE>(_alloc)),
        _vis(allocator::PoolAllocator<bool, BLOCK_SIZE>(_alloc))
    {}

    void IndependentService::add_edge(int u, int v, int64_t w)
    {
        _edges.push_back({ v, _head[u], w });
        _head[u] = _edge_cnt++;
    }

    void IndependentService::input()
    {
        std::cin >> _n;

        _d.resize(_n + 1);
        for (int i = 1; i <= _n; ++i) std::cin >> _d[i];

        _head.assign(_n + 1, -1);
        _edges.clear();
        _edge_cnt = 0;
        _edge_info.clear();
        _edges.reserve(2 * (_n - 1));

        for (int i = 0; i < _n - 1; ++i)
        {
            int u, v; int64_t w;
            std::cin >> u >> v >> w;
            add_edge(u, v, w);
            add_edge(v, u, w);
            _edge_info.push_back({ u, v });
        }

        int q; std::cin >> q;
        _ops.clear();
        _ops.reserve(q);
        _all_centers.clear();
        _query_ranges.clear();

        size_t total_out = 0;

        for (int i = 0; i < q; ++i)
        {
            int type; std::cin >> type;
            if (type == 1)
            {
                int u; int64_t val;
                std::cin >> u >> val;
                _ops.emplace_back(OpModNode{ u, val });
            }
            else if (type == 2)
            {
                int u, v; int64_t w;
                std::cin >> u >> v >> w;
                // 在 _edge_info 中查找边 (u,v) 或 (v,u) 的索引
                int idx = -1;
                for (size_t i = 0; i < _edge_info.size(); ++i)
                {
                    if ((_edge_info[i].first == u && _edge_info[i].second == v) ||
                        (_edge_info[i].first == v && _edge_info[i].second == u))
                    {
                        idx = i;
                        break;
                    }
                }
                // 理论上一定存在，可加断言
                _ops.emplace_back(OpModEdge{ idx, w });
            }
            else
            { // type == 3
                int k; std::cin >> k;
                int start = (int)_all_centers.size();
                _all_centers.resize(start + k);
                for (int j = 0; j < k; ++j) std::cin >> _all_centers[start + j];
                _query_ranges.push_back({ start, k });
                int qid = (int)_query_ranges.size() - 1;
                _ops.emplace_back(OpQueryIndex{ qid });
                total_out += k + 1; // k 个结果 + 分隔符 -1
            }
        }

        _results.reserve(total_out);

        // 准备 Dijkstra 临时空间
        _dist.resize(_n + 1);
        _center_id.resize(_n + 1);
        _vis.assign(_n + 1, false);
    }

    void IndependentService::dijkstra(
        const int* centers, int k,
        std::vector<int64_t, allocator::PoolAllocator<int64_t, BLOCK_SIZE>>& out)
    {
        out.assign(k, 0);
        if (k == 0) return;

        const int64_t INF = std::numeric_limits<int64_t>::max();
        for (int i = 1; i <= _n; ++i)
        {
            _dist[i] = INF;
            _center_id[i] = -1;
            _vis[i] = false;
        }

        // 堆
        std::vector<HeapNode, allocator::PoolAllocator<HeapNode, BLOCK_SIZE>> heap{
            allocator::PoolAllocator<HeapNode, BLOCK_SIZE>(_alloc) };
        heap.clear();

        for (int i = 0; i < k; ++i)
        {
            int s = centers[i];
            _dist[s] = 0;
            _center_id[s] = i;
            heap.push_back({ 0, s });
        }
        std::make_heap(heap.begin(), heap.end(), std::greater<HeapNode>());

        while (!heap.empty())
        {
            std::pop_heap(heap.begin(), heap.end(), std::greater<HeapNode>());
            HeapNode cur = heap.back();
            heap.pop_back();
            int u = cur.node;
            if (_vis[u]) continue;
            _vis[u] = true;

            for (int e = _head[u]; e != -1; e = _edges[e].next)
            {
                int v = _edges[e].to;
                int64_t nd = _dist[u] + _edges[e].w;
                if (nd < _dist[v])
                {
                    _dist[v] = nd;
                    _center_id[v] = _center_id[u];
                    heap.push_back({ nd, v });
                    std::push_heap(heap.begin(), heap.end(), std::greater<HeapNode>());
                }
                else if (nd == _dist[v] && _center_id[u] < _center_id[v])
                {
                    _center_id[v] = _center_id[u];
                }
            }
        }

        for (int u = 1; u <= _n; ++u)
        {
            int cid = _center_id[u];
            if (cid != -1)
            {
                out[cid] += _d[u];
            }
        }
    }

    void IndependentService::solve()
    {
        for (const auto& op : _ops)
        {
            std::visit([this](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, OpModNode>)
                {
                    _d[arg.u] = arg.val;
                }
                else if constexpr (std::is_same_v<T, OpModEdge>)
                {
                    int idx = arg.idx;
                    int u = _edge_info[idx].first;
                    int v = _edge_info[idx].second;
                    int64_t w = arg.w;
                    for (int e = _head[u]; e != -1; e = _edges[e].next)
                    {
                        if (_edges[e].to == v)
                        {
                            _edges[e].w = w;
                            break;
                        }
                    }
                    for (int e = _head[v]; e != -1; e = _edges[e].next)
                    {
                        if (_edges[e].to == u)
                        {
                            _edges[e].w = w;
                            break;
                        }
                    }
                }
                else if constexpr (std::is_same_v<T, OpQueryIndex>)
                {
                    auto [start, len] = _query_ranges[arg.qid];
                    const int* centers = &_all_centers[start];
                    std::vector<int64_t, allocator::PoolAllocator<int64_t, BLOCK_SIZE>> res{
                        allocator::PoolAllocator<int64_t, BLOCK_SIZE>(_alloc) };
                    dijkstra(centers, len, res);
                    for (auto v : res) _results.push_back(v);
                    _results.push_back(-1); // 分隔符
                }
                }, op);
        }
    }

    void IndependentService::show_result() const
    {
        for (size_t i = 0; i < _results.size(); ++i)
        {
            if (_results[i] == -1)
            {
                std::print("\n");
            }
            else
            {
                if (i > 0 && _results[i - 1] != -1) std::print(" ");
                std::print("{}", _results[i]);
            }
        }
    }

} // namespace Tasks