#include "../include/line_optimization.h"
#include <algorithm>
#include <limits>
#include <print>
#include <tuple>
#include <vector>

using namespace Tasks;

void LineOptimization::solve()
{
    int32_t n = 0;
    int32_t m = 0;
    int32_t k = 0;
    std::cin >> n >> m >> k;

    std::vector<std::vector<std::tuple<int32_t, int32_t, int32_t>>> adj(static_cast<std::size_t>(n + 1));
    for (int32_t i = 0; i < m; ++i)
    {
        int32_t u = 0;
        int32_t v = 0;
        int32_t w = 0;
        int32_t c = 0;
        std::cin >> u >> v >> w >> c;
        adj[static_cast<std::size_t>(u)].emplace_back(v, w, c);
    }

    const int64_t inf = std::numeric_limits<int64_t>::max() / 4;
    std::vector<std::vector<int64_t>> dp(
        static_cast<std::size_t>(n + 1),
        std::vector<int64_t>(static_cast<std::size_t>(k + 1), inf));
    dp[1][0] = 0;

    for (int32_t u = 1; u <= n; ++u)
    {
        for (const auto& edge : adj[static_cast<std::size_t>(u)])
        {
            const int32_t v = std::get<0>(edge);
            const int32_t w = std::get<1>(edge);
            const int32_t c = std::get<2>(edge);
            for (int32_t used = k; used >= 0; --used)
            {
                if (dp[u][used] >= inf)
                {
                    continue;
                }
                dp[v][used] = std::min(dp[v][used], dp[u][used] + w);
                if (used < k)
                {
                    dp[v][used + 1] = std::min(dp[v][used + 1], dp[u][used] + c);
                }
            }
        }
    }

    int64_t answer = inf;
    for (int32_t used = 0; used <= k; ++used)
    {
        answer = std::min(answer, dp[n][used]);
    }
    std::print("{}\n", answer);
}
