#include "../include/line_optimization.h"
#include <iostream>
#include <algorithm>
#include <print>

using namespace Tasks;

void LineOptimization::input()
{
    std::cin >> n >> m >> K;
    _adj.resize(n + 1, std::vector<edge, Alloc<edge>>(_alloc));

    for (uint32_t i = 0; i < m; ++i)
    {
        uint32_t u, v, w, c;
        std::cin >> u >> v >> w >> c;
        _adj[u].emplace_back(v, w, c);
    }
}

void LineOptimization::solve()
{
    // 初始化 dp 表：dp[1..n][0..K] = INF
    _dp.resize(n + 1, std::vector<uint32_t, Alloc<uint32_t>>{_alloc});
    for (uint32_t u = 1; u <= n; ++u)
    {
        _dp[u].assign(K + 1, INF);
    }

    // 起点：节点1，使用0次优化，距离0
    _dp[1][0] = 0;

    // 按拓扑序（节点编号递增）转移
    for (uint32_t u = 1; u <= n; ++u)
    {
        // 遍历节点 u 的所有出边
        for (const auto& e : _adj[u])
        {
            uint32_t v = e.v;
            uint32_t w = e.w;
            uint32_t c = e.c;

            for (uint32_t k = 0; k <= K; ++k)
            {
                uint32_t cur = _dp[u][k];
                if (cur == INF) continue;

                // 不优化这条边
                _dp[v][k] = std::min(_dp[v][k], cur + w);

                // 优化这条边（需要还有优化次数可用）
                if (k < K)
                {
                    _dp[v][k + 1] = std::min(_dp[v][k + 1], cur + c);
                }
            }
        }
    }

    // 取节点 n 的所有 k 中的最小值
    _res = *std::min_element(_dp[n].begin(), _dp[n].end());
}

void LineOptimization::show_result() const
{
    std::println("{}", _res);
}