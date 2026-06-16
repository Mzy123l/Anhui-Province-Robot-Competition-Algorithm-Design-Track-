#include "../include/journey_endurance.h"
#include <iostream>
#include <algorithm>
#include <print>

using namespace Tasks;

void JourneyEndurance::input()
{
    std::cin >> N >> M;

    adj = alloc_matrix();
    for (uint32_t i = 0; i < N * N; ++i) adj[i] = INF;

    for (uint32_t i = 0; i < M; ++i)
    {
        uint32_t u, v, w;
        std::cin >> u >> v >> w;
        if (w < mat(adj, u, v))
        {
            mat(adj, u, v) = w;
        }
    }

    std::cin >> Q;
    // 分配结果数组
    results = static_cast<uint32_t*>(_pool.allocate<uint32_t>(Q * N));
    if (!results) std::abort();
}

void JourneyEndurance::solve()
{
    // ---------- 1. 初始化 dp[0] ----------
    dp[0] = alloc_matrix();
    for (uint32_t i = 0; i < N * N; ++i) dp[0][i] = adj[i];

    // ---------- 2. 倍增预处理 ----------
    for (int t = 1; t < MAX_LOG; ++t)
    {
        dp[t] = alloc_matrix();
        uint32_t* prev = dp[t - 1];
        uint32_t* cur = dp[t];

        for (uint32_t i = 1; i <= N; ++i)
        {
            for (uint32_t j = 1; j <= N; ++j)
            {
                uint32_t best = INF;
                for (uint32_t k = 1; k <= N; ++k)
                {
                    uint32_t a = mat(prev, i, k);
                    uint32_t b = mat(prev, k, j);
                    if (a == INF || b == INF) continue;
                    uint32_t val = std::max(a, b);
                    if (val < best) best = val;
                }
                mat(cur, i, j) = best;
            }
        }
    }

    // ---------- 3. 分配工作向量 ----------
    uint32_t* vec_a = static_cast<uint32_t*>(_pool.allocate<uint32_t>(N));
    uint32_t* vec_b = static_cast<uint32_t*>(_pool.allocate<uint32_t>(N));
    if (!vec_a || !vec_b) std::abort();

    // ---------- 4. 处理每个询问 ----------
    for (uint32_t q = 0; q < Q; ++q)
    {
        uint32_t s, K;
        std::cin >> s >> K;

        // 初始化：起点为 0，其余 INF
        for (uint32_t i = 1; i <= N; ++i)
        {
            vec_a[i - 1] = (i == s) ? 0 : INF;
        }

        if (K == 0)
        {
            // K=0 时直接复制
            for (uint32_t j = 1; j <= N; ++j)
            {
                results[q * N + (j - 1)] = vec_a[j - 1];
            }
            continue;
        }

        uint32_t* cur_vec = vec_a;
        uint32_t* next_vec = vec_b;

        for (int bit = 0; bit < MAX_LOG; ++bit)
        {
            if (K & (1ULL << bit))
            {
                // 用 dp[bit] 更新 cur_vec -> next_vec
                for (uint32_t j = 1; j <= N; ++j)
                {
                    uint32_t best = INF;
                    for (uint32_t i = 1; i <= N; ++i)
                    {
                        uint32_t a = cur_vec[i - 1];
                        uint32_t b = mat(dp[bit], i, j);
                        if (a == INF || b == INF) continue;
                        uint32_t val = std::max(a, b);
                        if (val < best) best = val;
                    }
                    next_vec[j - 1] = best;
                }
                std::swap(cur_vec, next_vec);
            }
        }

        // 最终结果在 cur_vec 中，复制到 results
        for (uint32_t j = 1; j <= N; ++j)
        {
            results[q * N + (j - 1)] = cur_vec[j - 1];
        }
    }
}

void JourneyEndurance::show_result() const
{
    for (uint32_t q = 0; q < Q; ++q)
    {
        for (uint32_t j = 1; j <= N; ++j)
        {
            uint32_t val = results[q * N + (j - 1)];
            if (val == INF)
            {
                std::print("-1");
            }
            else
            {
                std::print("{}", val);
            }
            if (j < N) std::print(" ");
        }
        std::println();
    }
}