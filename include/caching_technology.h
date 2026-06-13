#pragma once

#include <cstdint>
#include <unordered_map>
#include "allocator.hpp"

namespace Tasks {

    class TaskB {
    public:
        TaskB();
        ~TaskB() = default;

        void solve(int32_t capacity, const std::string& opsStr);
        void show_result() const;

    private:
        struct Node {
            int32_t key;
            int32_t value;
            bool dirty;
            Node* prev;
            Node* next;
        };

        void initMemory(int32_t capacity);
        int32_t readMemory(int32_t key);
        void writeBackMemory(Node* node);
        Node* getFreeNode();
        void moveToHead(Node* node);
        Node* evictLru();
        Node* loadFromMemory(int32_t key);
        void processRead(int32_t key);
        void processWrite(int32_t key, int32_t value);

    private:
        int32_t _capacity;
        bool _cacheEnabled;
        Node* _head;
        Node* _tail;
        Node* _freeHead;
        std::unordered_map<int32_t, Node*> _keyToNode;
        std::unordered_map<int32_t, int32_t> _memory;   // 记录被修改的主存值

        // 内存池相关：固定大小足够容纳最多1000个节点（约64字节/节点 -> 64KB）
        static constexpr std::size_t BLOCK_SIZE = 65536;
        memory_pool::MemoryPool<BLOCK_SIZE> _pool;
        allocator::PoolAllocator<Node, BLOCK_SIZE> _alloc;

        // 输出结果暂存
        std::vector<std::string> _outputLines;
    };

} // namespace Tasks