#include "../include/taskb.h"
#include <print>
#include <unordered_map>
#include <list>
#include <vector>
#include <string>
#include <cctype>
#include <cstdint>

namespace Tasks {

    // 节点结构，用于LRU双向链表
    struct CacheNode {
        int64_t key;
        int64_t value;
        bool dirty;
    };

    void TaskB::solve(int32_t C, const std::string& opsStr) {
        // 主存：记录被写回修改过的键值对，未记录时默认值为key本身
        std::unordered_map<int64_t, int64_t> memory;

        // 辅助函数：从主存获取值
        auto getMemory = [&](int64_t key) -> int64_t {
            auto it = memory.find(key);
            return (it != memory.end()) ? it->second : key;
            };

        // 缓存数据结构（仅当C > 0时使用）
        std::list<CacheNode> cacheList;                 // MRU在前，LRU在后
        std::unordered_map<int64_t, std::list<CacheNode>::iterator> cacheMap;

        // 解析操作序列
        auto ops = parseOperations(opsStr);

        // 模拟执行每个操作
        for (const auto& op : ops) {
            if (op.empty()) continue;
            const std::string& cmd = op[0];
            if (cmd == "READ") {
                int64_t key = std::stoll(op[1]);
                if (C > 0 && cacheMap.find(key) != cacheMap.end()) {
                    // 命中
                    auto it = cacheMap[key];
                    int64_t val = it->value;
                    // 移动到MRU端
                    cacheList.splice(cacheList.begin(), cacheList, it);
                    std::print("READ {}:Hit,value={}\n", key, val);
                }
                else {
                    // 缺失
                    int64_t val = getMemory(key);
                    if (C > 0) {
                        // 若缓存已满，淘汰LRU
                        if (cacheList.size() == static_cast<size_t>(C)) {
                            auto last = cacheList.back();
                            cacheList.pop_back();
                            cacheMap.erase(last.key);
                            if (last.dirty) {
                                memory[last.key] = last.value;   // 写回主存
                            }
                        }
                        // 加载新节点（干净状态）
                        cacheList.emplace_front(CacheNode{ key, val, false });
                        cacheMap[key] = cacheList.begin();
                    }
                    std::print("READ {}:Miss,loaded,value={}\n", key, val);
                }
            }
            else if (cmd == "WRITE") {
                int64_t key = std::stoll(op[1]);
                int64_t value = std::stoll(op[2]);
                if (C > 0 && cacheMap.find(key) != cacheMap.end()) {
                    // 命中
                    auto it = cacheMap[key];
                    it->value = value;
                    it->dirty = true;
                    // 移动到MRU端
                    cacheList.splice(cacheList.begin(), cacheList, it);
                    std::print("WRITE {} {}:Hit,updated\n", key, value);
                }
                else {
                    // 缺失
                    if (C > 0) {
                        // 先淘汰（如果需要）
                        if (cacheList.size() == static_cast<size_t>(C)) {
                            auto last = cacheList.back();
                            cacheList.pop_back();
                            cacheMap.erase(last.key);
                            if (last.dirty) {
                                memory[last.key] = last.value;
                            }
                        }
                        // 加载并立即更新为写入值（置脏）
                        cacheList.emplace_front(CacheNode{ key, value, true });
                        cacheMap[key] = cacheList.begin();
                    }
                    else {
                        // C == 0：直接写回主存
                        memory[key] = value;
                    }
                    std::print("WRITE {} {}:Miss,loaded and updated\n", key, value);
                }
            }
        }

        // 输出空行
        std::print("\n");

        // 输出最终缓存状态
        if (C == 0 || cacheList.empty()) {
            std::print("Cache is empty.\n");
        }
        else {
            std::print("Cachestate(MRU->LRU):\n");
            for (const auto& node : cacheList) {
                std::print("{}:{}\n", node.key, node.value);
            }
        }
    }

    // 解析操作字符串，返回操作列表（每个操作为字符串数组，如 {"READ","10"}）
    std::vector<std::vector<std::string>> TaskB::parseOperations(const std::string& opsStr) {
        std::vector<std::vector<std::string>> result;
        std::vector<std::string> curOp;
        std::string token;

        auto flushToken = [&]() {
            if (!token.empty()) {
                curOp.push_back(token);
                token.clear();
            }
            };

        auto flushOp = [&]() {
            flushToken();
            if (!curOp.empty()) {
                result.push_back(std::move(curOp));
                curOp.clear();
            }
            };

        for (char ch : opsStr) {
            if (std::isspace(static_cast<unsigned char>(ch))) {
                flushToken();
            }
            else if (ch == ';') {
                flushToken();
                flushOp();
            }
            else {
                token.push_back(ch);
            }
        }
        flushToken();
        flushOp();

        return result;
    }

} // namespace Tasks