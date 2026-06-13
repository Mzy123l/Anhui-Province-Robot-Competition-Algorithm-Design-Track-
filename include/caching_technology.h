#pragma once

#include <cstdint>
#include <unordered_map>
#include "allocator.hpp"

namespace Tasks {

    class TaskB {
    public:
        TaskB();
        ~TaskB() = default;

        // ops: 原始操作字符串（以 '\0' 结尾）
        void solve(int32_t capacity, const char* ops);
        void show_result() const;

    private:
        struct Node {
            int32_t key;
            int32_t value;
            bool dirty;
            Node* prev;
            Node* next;
        };

        // 初始化空闲节点池（capacity 个节点）
        void initNodePool(int32_t capacity);
        // 从主存读取值（key 对应的当前值）
        int32_t readMemory(int32_t key) const;
        // 将脏节点写回主存
        void writeBackMemory(Node* node);
        // 获取一个空闲节点（从空闲链表头部）
        Node* allocNode();
        // 将节点移动到 MRU 端（链表头部）
        void moveToHead(Node* node);
        // 淘汰 LRU 节点（链表尾部），返回被淘汰的节点（复用）
        Node* evictLru();
        // 从主存加载指定 key 到缓存（可能引发淘汰）
        Node* loadFromMemory(int32_t key);
        // 处理读操作
        void processRead(int32_t key);
        // 处理写操作
        void processWrite(int32_t key, int32_t value);

    private:
        int32_t _capacity;                     // 缓存容量
        bool _cacheEnabled;                    // 是否启用缓存（C>0）
        Node* _head;                           // MRU 端
        Node* _tail;                           // LRU 端
        Node* _freeHead;                       // 空闲节点链表头
        std::unordered_map<int32_t, Node*> _keyToNode;   // 键到节点的映射
        mutable std::unordered_map<int32_t, int32_t> _memory; // 被修改过的主存值

        // 内存池（容纳 Node 节点）
        static constexpr std::size_t BLOCK_SIZE = 65536;   // 足够容纳 1000 个 Node
        memory_pool::MemoryPool<BLOCK_SIZE> _pool;
        allocator::PoolAllocator<Node, BLOCK_SIZE> _alloc;
    };

} // namespace Tasks