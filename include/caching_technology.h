#pragma once
#include <cstdint>
#include <string>

namespace Tasks {

    class TaskB {
    public:
        TaskB() = default;

        // 主求解函数：接收缓存容量C和操作字符串opsStr
        void solve(int32_t C, const std::string& opsStr);

    private:
        // 解析操作序列，返回操作列表（每条操作为字符串数组）
        std::vector<std::vector<std::string>> parseOperations(const std::string& opsStr);
    };

} // namespace Tasks