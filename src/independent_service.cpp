#include "../include/independent_service.h"
#include <limits>
#include <iostream>
#include <cassert>
#include <unordered_map>  

using namespace Tasks;

IndependentService::IndependentService()
    : _pool(),
    _alloc(_pool),
    _head(allocator::PoolAllocator<int32_t, BLOCK_SIZE>(_alloc)),
    _edges(allocator::PoolAllocator<Edge, BLOCK_SIZE>(_alloc)),
    _d(allocator::PoolAllocator<int64_t, BLOCK_SIZE>(_alloc)),
    _edge_info(allocator::PoolAllocator<std::pair<int32_t, int32_t>, BLOCK_SIZE>(_alloc)),
    _ops(allocator::PoolAllocator<Operation, BLOCK_SIZE>(_alloc)),
    _all_centers(allocator::PoolAllocator<int32_t, BLOCK_SIZE>(_alloc)),
    _query_ranges(allocator::PoolAllocator<std::pair<int32_t, int32_t>, BLOCK_SIZE>(_alloc)),
    _results(allocator::PoolAllocator<int64_t, BLOCK_SIZE>(_alloc)),
    _fa(allocator::PoolAllocator<int32_t, BLOCK_SIZE>(_alloc)),
    _dep(allocator::PoolAllocator<int32_t, BLOCK_SIZE>(_alloc)),
    _sz(allocator::PoolAllocator<int32_t, BLOCK_SIZE>(_alloc)),
    _son(allocator::PoolAllocator<int32_t, BLOCK_SIZE>(_alloc)),
    _top(allocator::PoolAllocator<int32_t, BLOCK_SIZE>(_alloc)),
    _dfn(allocator::PoolAllocator<int32_t, BLOCK_SIZE>(_alloc)),
    _rnk(allocator::PoolAllocator<int32_t, BLOCK_SIZE>(_alloc)),
    _edge_val(allocator::PoolAllocator<int64_t, BLOCK_SIZE>(_alloc)),
    _dfs_clock(0),
    _seg_point(nullptr), _seg_edge(nullptr)
{}

void IndependentService::add_edge(int32_t u, int32_t v, int64_t w)
{
    _edges.push_back({ v, _head[u], w });
    _head[u] = _edge_cnt++;
}

// ---------- 树链剖分 ----------
void IndependentService::dfs1(int32_t u, int32_t f)
{
    _fa[u] = f;
    _dep[u] = _dep[f] + 1;
    _sz[u] = 1;
    _son[u] = 0;
    int32_t max_sz = 0;
    for (int32_t e = _head[u]; e != -1; e = _edges[e].next)
    {
        int32_t v = _edges[e].to;
        if (v == f) continue;
        _edge_val[v] = _edges[e].w;   // 边权赋给子节点
        dfs1(v, u);
        _sz[u] += _sz[v];
        if (_sz[v] > max_sz)
        {
            max_sz = _sz[v];
            _son[u] = v;
        }
    }
}

void IndependentService::dfs2(int32_t u, int32_t tp)
{
    _top[u] = tp;
    _dfn[u] = ++_dfs_clock;
    _rnk[_dfs_clock] = u;
    if (_son[u]) dfs2(_son[u], tp);
    for (int32_t e = _head[u]; e != -1; e = _edges[e].next)
    {
        int32_t v = _edges[e].to;
        if (v == _fa[u] || v == _son[u]) continue;
        dfs2(v, v);
    }
}

int32_t IndependentService::lca(int32_t u, int32_t v)
{
    while (_top[u] != _top[v])
    {
        if (_dep[_top[u]] < _dep[_top[v]]) std::swap(u, v);
        u = _fa[_top[u]];
    }
    return _dep[u] < _dep[v] ? u : v;
}

int64_t IndependentService::path_dist(int32_t u, int32_t v)
{
    int64_t res = 0;
    while (_top[u] != _top[v])
    {
        if (_dep[_top[u]] < _dep[_top[v]]) std::swap(u, v);
        res += _seg_edge->query(1, 1, _n, _dfn[_top[u]], _dfn[u]);
        u = _fa[_top[u]];
    }
    if (_dep[u] > _dep[v]) std::swap(u, v);
    if (u != v)
    {
        // 注意：边权点权中，根节点没有边权，所以区间 [dfn[u]+1, dfn[v]]
        res += _seg_edge->query(1, 1, _n, _dfn[u] + 1, _dfn[v]);
    }
    return res;
}

int64_t IndependentService::subtree_sum(int32_t u)
{
    return _seg_point->query(1, 1, _n, _dfn[u], _dfn[u] + _sz[u] - 1);
}

// ---------- 虚树构建----------
void IndependentService::build_virtual_tree(
    const std::vector<int32_t>& nodes,
    std::vector<int32_t>& vt_nodes,
    std::vector<std::vector<int32_t>>& vt_adj,
    std::vector<int32_t>& vt_parent,
    std::vector<int64_t>& vt_dist_from_root,
    std::unordered_map<int32_t, int32_t>& id_map)  
{
    // nodes 已按 dfn 排序
    std::vector<int32_t> all = nodes;
    // 强制加入根节点 1
    all.push_back(1);
    std::sort(all.begin(), all.end(), [this](int32_t a, int32_t b) { return _dfn[a] < _dfn[b]; });
    all.erase(std::unique(all.begin(), all.end()), all.end());

    // 插入所有 LCA
    std::vector<int32_t> tmp = all;
    for (size_t i = 1; i < all.size(); ++i)
    {
        tmp.push_back(lca(all[i - 1], all[i]));
    }
    std::sort(tmp.begin(), tmp.end(), [this](int32_t a, int32_t b) { return _dfn[a] < _dfn[b]; });
    tmp.erase(std::unique(tmp.begin(), tmp.end()), tmp.end());
    all.swap(tmp);

    // 分配虚树节点 ID（清空并重建 id_map）
    id_map.clear();
    vt_nodes.clear();
    vt_adj.clear();
    vt_parent.clear();
    vt_dist_from_root.clear();

    auto add_node = [&](int32_t x) -> int32_t
        {
            int32_t idx = (int)vt_nodes.size();
            vt_nodes.push_back(x);
            vt_adj.emplace_back();
            vt_parent.push_back(-1);
            vt_dist_from_root.push_back(0);
            id_map[x] = idx;          // 记录映射
            return idx;
        };

    for (int32_t x : all)
    {
        add_node(x);
    }

    // 用栈建树
    std::vector<int32_t> stk;
    stk.push_back(all[0]);
    for (size_t i = 1; i < all.size(); ++i)
    {
        int32_t u = all[i];
        int32_t l = lca(u, stk.back());
        while (stk.size() > 1 && _dep[stk[stk.size() - 2]] >= _dep[l])
        {
            int32_t v = stk.back(); stk.pop_back();
            int32_t p = stk.back();
            int32_t vid = id_map[v], pid = id_map[p];
            vt_adj[pid].push_back(vid);
            vt_parent[vid] = pid;
            vt_dist_from_root[vid] = path_dist(vt_nodes[pid], vt_nodes[vid]); // 边权
        }
        if (stk.back() != l)
        {
            if (!id_map.count(l))
            {
                id_map[l] = add_node(l);
            }
            int32_t lid = id_map[l];
            int32_t bid = id_map[stk.back()];
            vt_adj[lid].push_back(bid);
            vt_parent[bid] = lid;
            vt_dist_from_root[bid] = path_dist(vt_nodes[lid], vt_nodes[bid]);
            stk.pop_back();
            stk.push_back(l);
        }
        stk.push_back(u);
    }
    while (stk.size() > 1)
    {
        int32_t v = stk.back(); stk.pop_back();
        int32_t p = stk.back();
        int32_t vid = id_map[v], pid = id_map[p];
        vt_adj[pid].push_back(vid);
        vt_parent[vid] = pid;
        vt_dist_from_root[vid] = path_dist(vt_nodes[pid], vt_nodes[vid]);
    }
    // 此时 stk[0] 为虚树根（一定是 1）
}

// ---------- 查询处理（利用 id_map 消除 std::find）----------
void IndependentService::compute_query(const int* centers, int32_t k, std::vector<int64_t>& out)
{
    out.assign(k, 0);
    if (k == 0) return;

    // 1. 收集服务中心并按 dfn 排序
    std::vector<int32_t> sorted_centers(centers, centers + k);
    std::sort(sorted_centers.begin(), sorted_centers.end(),
        [this](int32_t a, int32_t b) { return _dfn[a] < _dfn[b]; });

    // 2. 构建虚树（包含根节点 1），同时获得 id_map
    std::vector<int32_t> vt_nodes;
    std::vector<std::vector<int32_t>> vt_adj;
    std::vector<int32_t> vt_parent;
    std::vector<int64_t> vt_dist_from_root;
    std::unordered_map<int32_t, int32_t> id_map;   // 原节点 -> 虚树索引
    build_virtual_tree(sorted_centers, vt_nodes, vt_adj, vt_parent, vt_dist_from_root, id_map);

    int32_t vt_sz = (int)vt_nodes.size();

    // 3. 建立服务中心到输出下标的映射
    std::unordered_map<int32_t, int32_t> center_to_idx;
    for (int32_t i = 0; i < k; ++i)
    {
        center_to_idx[centers[i]] = i;
    }

    // 4. 虚树上多源 Dijkstra
    const int64_t INF = std::numeric_limits<int64_t>::max();
    std::vector<int64_t> dist(vt_sz, INF);
    std::vector<int32_t> belong(vt_sz, -1);
    using P = std::pair<int64_t, int32_t>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;

    // 初始化服务中心对应的虚树节点（利用 id_map 直接定位）
    for (int32_t i = 0; i < k; ++i)
    {
        int32_t u = centers[i];
        auto it = id_map.find(u);
        if (it != id_map.end())
        {
            int32_t idx = it->second;
            dist[idx] = 0;
            belong[idx] = i;           // 直接使用输出下标
            pq.push({ 0, idx });
        }
    }

    // 构建双向邻接表（方便 Dijkstra）
    std::vector<std::vector<int32_t>> vt_adj2(vt_sz);
    std::vector<std::vector<int64_t>> vt_w2(vt_sz);
    for (int32_t u = 0; u < vt_sz; ++u)
    {
        for (int32_t v : vt_adj[u])
        {
            int64_t w = vt_dist_from_root[v];
            vt_adj2[u].push_back(v);
            vt_w2[u].push_back(w);
            vt_adj2[v].push_back(u);
            vt_w2[v].push_back(w);
        }
    }

    while (!pq.empty())
    {
        auto [d, u] = pq.top(); pq.pop();
        if (d != dist[u]) continue;
        for (size_t i = 0; i < vt_adj2[u].size(); ++i)
        {
            int32_t v = vt_adj2[u][i];
            int64_t nd = d + vt_w2[u][i];
            if (nd < dist[v])
            {
                dist[v] = nd;
                belong[v] = belong[u];
                pq.push({ nd, v });
            }
            else if (nd == dist[v] && belong[u] < belong[v])
            {
                belong[v] = belong[u];
            }
        }
    }

    // 5. 通过虚树 DFS 统计每个服务中心的总点权
    std::vector<std::vector<int32_t>> children(vt_sz);
    int32_t root = -1;
    for (int32_t i = 0; i < vt_sz; ++i)
    {
        if (vt_parent[i] == -1) root = i;
        else children[vt_parent[i]].push_back(i);
    }

    std::function<void(int)> dfs = [&](int32_t u) 
        {
       
            int64_t total = subtree_sum(vt_nodes[u]);
        
            for (int32_t v : children[u])
            {
           
                total -= subtree_sum(vt_nodes[v]);
          
                dfs(v);
    
            }
       
            out[belong[u]] += total;
      
        };
    dfs(root);
}


void IndependentService::input()
{
    std::cin >> _n;

    _d.resize(_n + 1);
    for (int32_t i = 1; i <= _n; ++i) std::cin >> _d[i];

    _head.assign(_n + 1, -1);
    _edges.clear();
    _edge_cnt = 0;
    _edge_info.clear();
    _edges.reserve(2 * (_n - 1));

    for (int32_t i = 0; i < _n - 1; ++i)
    {
        int32_t u, v; int64_t w;
        std::cin >> u >> v >> w;
        add_edge(u, v, w);
        add_edge(v, u, w);
        _edge_info.push_back({ u, v });
    }

    // 树链剖分预处理
    _fa.resize(_n + 1);
    _dep.resize(_n + 1);
    _sz.resize(_n + 1);
    _son.resize(_n + 1);
    _top.resize(_n + 1);
    _dfn.resize(_n + 1);
    _rnk.resize(_n + 1);
    _edge_val.resize(_n + 1, 0);
    _dfs_clock = 0;
    dfs1(1, 0);
    dfs2(1, 1);

    // 构建点权线段树（树状数组）
    std::vector<int64_t> point_arr(_n + 1);
    for (int32_t i = 1; i <= _n; ++i) point_arr[_dfn[i]] = _d[i];
    _seg_point = std::make_unique<SegTree>(_n);
    _seg_point->build(1, 1, _n, point_arr);

    // 构建边权线段树（树状数组）
    std::vector<int64_t> edge_arr(_n + 1, 0);
    for (int32_t i = 2; i <= _n; ++i) edge_arr[_dfn[i]] = _edge_val[i];
    _seg_edge = std::make_unique<SegTree>(_n);
    _seg_edge->build(1, 1, _n, edge_arr);

    int32_t q; std::cin >> q;
    _ops.clear();
    _ops.reserve(q);
    _all_centers.clear();
    _query_ranges.clear();

    size_t total_out = 0;

    for (int32_t i = 0; i < q; ++i)
    {
        int32_t type; std::cin >> type;
        if (type == 1)
        {
            int32_t u; int64_t val;
            std::cin >> u >> val;
            _ops.emplace_back(OpModNode{ u, val });
        }
        else if (type == 2)
        {
            int32_t u, v; int64_t w;
            std::cin >> u >> v >> w;
            int32_t idx = -1;
            for (size_t i = 0; i < _edge_info.size(); ++i)
            {
                if ((_edge_info[i].first == u && _edge_info[i].second == v) ||
                    (_edge_info[i].first == v && _edge_info[i].second == u))
                {
                    idx = i;
                    break;
                }
            }
            _ops.emplace_back(OpModEdge{ idx, w });
        }
        else
        {
            int32_t k; std::cin >> k;
            int32_t start = (int)_all_centers.size();
            _all_centers.resize(start + k);
            for (int32_t j = 0; j < k; ++j) std::cin >> _all_centers[start + j];
            _query_ranges.push_back({ start, k });
            int32_t qid = (int)_query_ranges.size() - 1;
            _ops.emplace_back(OpQueryIndex{ qid });
            total_out += k + 1; // k 个结果 + 分隔符 -1
        }
    }

    _results.reserve(total_out);
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
                // 更新点权线段树
                _seg_point->update(1, 1, _n, _dfn[arg.u], arg.val);
            }
            else if constexpr (std::is_same_v<T, OpModEdge>)
            {
                int32_t idx = arg.idx;
                int32_t u = _edge_info[idx].first;
                int32_t v = _edge_info[idx].second;
                int64_t w = arg.w;
                // 更新邻接表中的边权
                for (int32_t e = _head[u]; e != -1; e = _edges[e].next)
                {
                    if (_edges[e].to == v)
                    {
                        _edges[e].w = w;
                        break;
                    }
                }
                for (int32_t e = _head[v]; e != -1; e = _edges[e].next)
                {
                    if (_edges[e].to == u)
                    {
                        _edges[e].w = w;
                        break;
                    }
                }
                // 更新边权线段树：找出深度大的节点
                int32_t deeper = (_dep[u] > _dep[v]) ? u : v;
                _seg_edge->update(1, 1, _n, _dfn[deeper], w);
            }
            else if constexpr (std::is_same_v<T, OpQueryIndex>)
            {
                auto [start, len] = _query_ranges[arg.qid];
                const int* centers = &_all_centers[start];
                std::vector<int64_t> res;  
                compute_query(centers, len, res);
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

