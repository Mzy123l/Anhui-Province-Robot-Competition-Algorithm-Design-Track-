#include "../include/independent_service.h"
#include <algorithm>
#include <limits>
#include <print>
#include <queue>
#include <tuple>
#include <vector>

using namespace Tasks;

namespace
{
    struct Edge
    {
        int32_t to;
        int32_t weight;
    };

    struct HeapNode
    {
        int32_t dist;
        int32_t center;
        int32_t node;
    };

    struct HeapCmp
    {
        bool operator()(const HeapNode& lhs, const HeapNode& rhs) const
        {
            if (lhs.dist != rhs.dist)
            {
                return lhs.dist > rhs.dist;
            }
            return lhs.center > rhs.center;
        }
    };
}

void IndependentService::solve()
{
    int32_t n = 0;
    std::cin >> n;

    std::vector<int32_t> demand(n + 1);
    for (int32_t i = 1; i <= n; ++i)
    {
        std::cin >> demand[i];
    }

    std::vector<std::vector<Edge>> adj(n + 1);
  std::vector<std::tuple<int32_t, int32_t, int32_t>> edge_list;
    for (int32_t i = 0; i < n - 1; ++i)
    {
        int32_t u = 0;
        int32_t v = 0;
        int32_t w = 0;
        std::cin >> u >> v >> w;
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
        edge_list.push_back({u, v, w});
    }

    int32_t q = 0;
    std::cin >> q;

    const int32_t inf = std::numeric_limits<int32_t>::max() / 4;
    std::vector<int32_t> dist(n + 1, inf);
    std::vector<int32_t> owner(n + 1, 0);

    for (int32_t query = 0; query < q; ++query)
    {
        int32_t op = 0;
        std::cin >> op;
        if (op == 1)
        {
            int32_t u = 0;
            int32_t val = 0;
            std::cin >> u >> val;
            demand[u] = val;
        }
        else if (op == 2)
        {
            int32_t u = 0;
            int32_t v = 0;
            int32_t w = 0;
            std::cin >> u >> v >> w;
            for (auto& edge : edge_list)
            {
                int32_t a = std::get<0>(edge);
                int32_t b = std::get<1>(edge);
                if ((a == u && b == v) || (a == v && b == u))
                {
                    std::get<2>(edge) = w;
                    break;
                }
            }
            for (int32_t node = 1; node <= n; ++node)
            {
                for (auto& edge : adj[node])
                {
                    if ((node == u && edge.to == v) || (node == v && edge.to == u))
                    {
                        edge.weight = w;
                    }
                }
            }
        }
        else
        {
            int32_t k = 0;
            std::cin >> k;
            std::vector<int32_t> centers(k);
            for (int32_t i = 0; i < k; ++i)
            {
                std::cin >> centers[i];
            }

            std::fill(dist.begin(), dist.end(), inf);
            std::fill(owner.begin(), owner.end(), 0);
            std::priority_queue<HeapNode, std::vector<HeapNode>, HeapCmp> heap;

            for (int32_t center : centers)
            {
                dist[center] = 0;
                owner[center] = center;
                heap.push({0, center, center});
            }

            while (!heap.empty())
            {
                const HeapNode cur = heap.top();
                heap.pop();
                if (cur.dist != dist[cur.node] || cur.center != owner[cur.node])
                {
                    continue;
                }
                for (const Edge& edge : adj[cur.node])
                {
                    const int32_t next_dist = cur.dist + edge.weight;
                    const int32_t next_center = cur.center;
                    if (next_dist < dist[edge.to]
                        || (next_dist == dist[edge.to] && next_center < owner[edge.to]))
                    {
                        dist[edge.to] = next_dist;
                        owner[edge.to] = next_center;
                        heap.push({next_dist, next_center, edge.to});
                    }
                }
            }

            std::vector<int64_t> totals(k, 0);
            for (int32_t node = 1; node <= n; ++node)
            {
                const int32_t assigned = owner[node];
                for (int32_t i = 0; i < k; ++i)
                {
                    if (centers[i] == assigned)
                    {
                        totals[i] += demand[node];
                        break;
                    }
                }
            }

            for (int32_t i = 0; i < k; ++i)
            {
                if (i > 0)
                {
                    std::print(" ");
                }
                std::print("{}", totals[i]);
            }
            std::print("\n");
        }
    }
}
