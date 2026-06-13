#include "../include/taskb.h"
#include <print>
#include <cstring>
#include <cstdlib>
#include <charconv>

using namespace Tasks;

TaskB::TaskB()
    : _capacity(0)
    , _cacheEnabled(false)
    , _head(nullptr)
    , _tail(nullptr)
    , _freeHead(nullptr)
    , _pool()
    , _alloc(_pool)
{
}

void TaskB::initNodePool(int32_t capacity) {
    for (int32_t i = 0; i < capacity; ++i) {
        Node* node = _alloc.allocate();
        if (!node) return;   // 内存池不够（理论上不会）
        _alloc.construct(node);
        node->prev = node->next = nullptr;
        node->dirty = false;
        node->key = node->value = 0;
        // 插入空闲链表头部
        node->next = _freeHead;
        if (_freeHead) _freeHead->prev = node;
        _freeHead = node;
    }
}

int32_t TaskB::readMemory(int32_t key) const {
    auto it = _memory.find(key);
    if (it != _memory.end())
        return it->second;
    return key;   // 初始值等于 key 本身
}

void TaskB::writeBackMemory(Node* node) {
    if (node->dirty) {
        _memory[node->key] = node->value;
        node->dirty = false;
    }
}

TaskB::Node* TaskB::allocNode() {
    if (!_freeHead) return nullptr;
    Node* node = _freeHead;
    _freeHead = node->next;
    if (_freeHead) _freeHead->prev = nullptr;
    node->prev = node->next = nullptr;
    return node;
}

void TaskB::moveToHead(Node* node) {
    if (node == _head) return;
    // 从当前位置摘除
    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    if (node == _tail) _tail = node->prev;
    // 插入头部
    node->prev = nullptr;
    node->next = _head;
    if (_head) _head->prev = node;
    _head = node;
    if (!_tail) _tail = node;
}

TaskB::Node* TaskB::evictLru() {
    if (!_tail) return nullptr;
    Node* victim = _tail;
    // 从链表中移除
    if (victim->prev) victim->prev->next = nullptr;
    _tail = victim->prev;
    if (_head == victim) _head = nullptr;
    // 从哈希表删除
    _keyToNode.erase(victim->key);
    // 脏数据写回主存
    writeBackMemory(victim);
    return victim;
}

TaskB::Node* TaskB::loadFromMemory(int32_t key) {
    int32_t memVal = readMemory(key);
    Node* node = nullptr;

    if (static_cast<int32_t>(_keyToNode.size()) == _capacity) {
        // 缓存满：淘汰 LRU，复用该节点
        node = evictLru();
        node->key = key;
        node->value = memVal;
        node->dirty = false;
    }
    else {
        node = allocNode();
        if (!node) return nullptr;
        node->key = key;
        node->value = memVal;
        node->dirty = false;
    }

    _keyToNode[key] = node;
    moveToHead(node);
    return node;
}

void TaskB::processRead(int32_t key) {
    auto it = _keyToNode.find(key);
    if (it != _keyToNode.end()) {
        // 命中
        Node* node = it->second;
        moveToHead(node);
        std::print("READ {}:Hit,value={}\n", key, node->value);
    }
    else {
        // 缺失
        Node* node = loadFromMemory(key);
        int32_t val = node ? node->value : readMemory(key);
        std::print("READ {}:Miss,loaded,value={}\n", key, val);
    }
}

void TaskB::processWrite(int32_t key, int32_t value) {
    auto it = _keyToNode.find(key);
    if (it != _keyToNode.end()) {
        // 写命中
        Node* node = it->second;
        node->value = value;
        node->dirty = true;
        moveToHead(node);
        std::print("WRITE {} {}:Hit,updated\n", key, value);
    }
    else {
        // 写缺失：先加载，再更新
        Node* node = loadFromMemory(key);
        if (node) {
            node->value = value;
            node->dirty = true;
            moveToHead(node);
        }
        std::print("WRITE {} {}:Miss,loaded and updated\n", key, value);
    }
}

void TaskB::solve(int32_t capacity, const char* ops) {
    _capacity = capacity;
    _cacheEnabled = (capacity > 0);
    _keyToNode.clear();
    _memory.clear();
    _head = _tail = _freeHead = nullptr;

    if (_cacheEnabled) {
        initNodePool(capacity);
    }

    // 解析操作字符串
    const char* p = ops;
    while (*p) {
        // 跳过空白字符（空格、制表符、换行等）
        while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
            ++p;
        if (!*p) break;

        // 读取命令（READ 或 WRITE）
        char cmd[8] = { 0 };
        int i = 0;
        while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && *p != ';' && i < 7)
            cmd[i++] = *p++;
        cmd[i] = '\0';
        // 跳过命令后的空白
        while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
            ++p;

        if (strcmp(cmd, "READ") == 0) {
            // 读取 key
            int32_t key = 0;
            auto [ptr, ec] = std::from_chars(p, p + 20, key);
            p = ptr;
            // 跳过 key 之后的空白，直到 ';' 或结尾
            while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
                ++p;
            if (*p == ';') ++p;   // 跳过分隔符

            if (_cacheEnabled)
                processRead(key);
            else
                std::print("READ {}:Miss,loaded,value={}\n", key, key);
        }
        else if (strcmp(cmd, "WRITE") == 0) {
            // 读取 key 和 value
            int32_t key = 0, value = 0;
            auto [ptr1, ec1] = std::from_chars(p, p + 20, key);
            p = ptr1;
            while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
                ++p;
            auto [ptr2, ec2] = std::from_chars(p, p + 20, value);
            p = ptr2;
            while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
                ++p;
            if (*p == ';') ++p;

            if (_cacheEnabled)
                processWrite(key, value);
            else
                std::print("WRITE {} {}:Miss,loaded and updated\n", key, value);
        }
        else {
            // 非法命令，跳过直到分号
            while (*p && *p != ';') ++p;
            if (*p == ';') ++p;
        }
    }
}

void TaskB::show_result() const {
    std::print("\n");
    if (!_cacheEnabled || _head == nullptr) {
        std::print("Cache is empty.\n");
        return;
    }
    std::print("Cachestate(MRU->LRU):\n");
    Node* cur = _head;
    while (cur) {
        std::print("{}:{}\n", cur->key, cur->value);
        cur = cur->next;
    }
}