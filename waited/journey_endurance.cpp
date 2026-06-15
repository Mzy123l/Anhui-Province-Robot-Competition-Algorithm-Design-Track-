#include "../include/journey_endurance.h"
#include <algorithm>
#include <limits>
#include <print>
#include <vector>

using namespace Tasks;

namespace
{
    constexpr int64_t kInf = static_cast<int64_t>(4e18);

    std::vector<std::vector<int64_t>> min_max_mul(
        const std::vector<std::vector<int64_t>>& lhs,
        const std::vector<std::vector<int64_t>>& rhs)
    {
        const std::size_t n = lhs.size();
        std::vector<std::vector<int64_t>> result(n, std::vector<int64_t>(n, kInf));
        for (std::size_t i = 0; i < n; ++i)
        {
            for (std::size_t j = 0; j < n; ++j)
            {
                int64_t best = kInf;
                for (std::size_t k = 0; k < n; ++k)
                {
                    if (lhs[i][k] == kInf || rhs[k][j] == kInf)
                    {
                        continue;
                    }
                    best = std::min(best, std::max(lhs[i][k], rhs[k][j]));
                }
                result[i][j] = best;
            }
        }
        return result;
    }

    std::vector<int64_t> min_max_mul_vec(
        const std::vector<int64_t>& vec,
        const std::vector<std::vector<int64_t>>& mat)
    {
        const std::size_t n = vec.size();
        std::vector<int64_t> result(n, kInf);
        for (std::size_t j = 0; j < n; ++j)
        {
            int64_t best = kInf;
            for (std::size_t i = 0; i < n; ++i)
            {
                if (vec[i] == kInf || mat[i][j] == kInf)
                {
                    continue;
                }
                best = std::min(best, std::max(vec[i], mat[i][j]));
            }
            result[j] = best;
        }
        return result;
    }
}

void JourneyEndurance::solve()
{
    int32_t n = 0;
    int32_t m = 0;
    std::cin >> n >> m;

    std::vector<std::vector<int64_t>> base(
        static_cast<std::size_t>(n),
        std::vector<int64_t>(static_cast<std::size_t>(n), kInf));

    for (int32_t i = 0; i < m; ++i)
    {
        int32_t u = 0;
        int32_t v = 0;
        int64_t w = 0;
        std::cin >> u >> v >> w;
        const std::size_t ui = static_cast<std::size_t>(u - 1);
        const std::size_t vi = static_cast<std::size_t>(v - 1);
        base[ui][vi] = std::min(base[ui][vi], w);
    }

    std::vector<std::vector<int64_t>> powers;
    powers.push_back(base);
    for (int32_t bit = 1; bit < 31; ++bit)
    {
        powers.push_back(min_max_mul(powers.back(), powers.back()));
    }

    int32_t q = 0;
    std::cin >> q;
    for (int32_t query = 0; query < q; ++query)
    {
        int32_t s = 0;
        int64_t k = 0;
        std::cin >> s >> k;
        std::vector<int64_t> state(static_cast<std::size_t>(n), kInf);
        state[static_cast<std::size_t>(s - 1)] = 0;

        for (int32_t bit = 0; bit < 31; ++bit)
        {
            if ((k >> bit) & 1)
            {
                state = min_max_mul_vec(state, powers[bit]);
            }
        }

        for (int32_t j = 1; j <= n; ++j)
        {
            if (j > 1)
            {
                std::print(" ");
            }
            const int64_t value = state[static_cast<std::size_t>(j - 1)];
            if (value == kInf)
            {
                std::print("-1");
            }
            else
            {
                std::print("{}", value);
            }
        }
        std::print("\n");
    }
}
