#include "../include/taskb.h"
#include <print>
#include <sstream>
#include <vector>
#include <cstring>

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

void TaskB::initMemory(int32_t capacity) {
    _capacity = capacity;
    _cacheEnabled = (capacity > 0);
    if (!_cacheEnabled) return;

    // 预先分配 capacity 个节点放入空闲链表
    for (int32_t i = 0; i < capacity; ++i) {
        Node* node = _alloc.allocate();
        if (!node) {
            // 内存池不足，但 capacity <= 1000 且 BLOCK_SIZE 足够大，不会发生
            std::print("FATAL: memory pool exhausted\n");
            return;
        }
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

int32_t TaskB::readMemory(int32_t key) {
    auto it = _memory.find(key);
    if (it != _memory.end()) {
        return it->second;
    }
    return key;   // 初始值等于 key 本身
}

void TaskB::writeBackMemory(Node* node) {
    if (node->dirty) {
        _memory[node->key] = node->value;
        node->dirty = false;
    }
}

TaskB::Node* TaskB::getFreeNode() {
    if (!_freeHead) return nullptr;
    Node* node = _freeHead;
    _freeHead = node->next;
    if (_freeHead) _freeHead->prev = nullptr;
    node->prev = node->next = nullptr;
    return node;
}

void TaskB::moveToHead(Node* node) {
    if (node == _head) return;   // 已经是 MRU

    // 从当前位置摘下
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
    // 从哈希表中删除
    _keyToNode.erase(victim->key);
    // 脏数据写回主存
    writeBackMemory(victim);
    return victim;
}

TaskB::Node* TaskB::loadFromMemory(int32_t key) {
    int32_t memValue = readMemory(key);
    Node* node = nullptr;

    // 缓存已满则淘汰一个，复用该节点
    if (static_cast<int32_t>(_keyToNode.size()) == _capacity) {
        node = evictLru();
        // 重用节点，重置内容
        node->key = key;
        node->value = memValue;
        node->dirty = false;
    }
    else {
        // 有空闲节点则分配
        node = getFreeNode();
        if (!node) return nullptr;
        node->key = key;
        node->value = memValue;
        node->dirty = false;
    }

    // 插入哈希表
    _keyToNode[key] = node;
    // 移动到 MRU 端
    moveToHead(node);
    return node;
}

void TaskB::processRead(int32_t key) {
    auto it = _keyToNode.find(key);
    if (it != _keyToNode.end()) {
        // 命中
        Node* node = it->second;
        moveToHead(node);
        _outputLines.push_back(std::format("READ {}:Hit,value={}", key, node->value));
    }
    else {
        // 缺失
        Node* node = loadFromMemory(key);
        int32_t val = node ? node->value : readMemory(key);
        _outputLines.push_back(std::format("READ {}:Miss,loaded,value={}", key, val));
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
        _outputLines.push_back(std::format("WRITE {} {}:Hit,updated", key, value));
    }
    else {
        // 写缺失：先加载（可能淘汰），再更新
        Node* node = loadFromMemory(key);
        if (node) {
            node->value = value;
            node->dirty = true;
            moveToHead(node);
        }
        _outputLines.push_back(std::format("WRITE {} {}:Miss,loaded and updated", key, value));
    }
}

void TaskB::solve(int32_t capacity, const std::string& opsStr) {
    initMemory(capacity);
    _outputLines.clear();
    _keyToNode.clear();
    _memory.clear();

    if (!_cacheEnabled) {
        // 容量为 0 的特殊处理：无缓存，直接操作主存并输出
        std::vector<std::string> operations;
        std::stringstream ss(opsStr);
        std::string opToken;
        while (std::getline(ss, opToken, ';')) {
            // 去掉前后空白
            size_t start = opToken.find_first_not_of(" \t\n\r");
            if (start == std::string::npos) continue;
            size_t end = opToken.find_last_not_of(" \t\n\r");
            std::string trimmed = opToken.substr(start, end - start + 1);
            std::istringstream iss(trimmed);
            std::string cmd;
            iss >> cmd;
            if (cmd == "READ") {
                int32_t key;
                iss >> key;
                int32_t val = readMemory(key);
                _outputLines.push_back(std::format("READ {}:Miss,loaded,value={}", key, val));
            }
            else if (cmd == "WRITE") {
                int32_t key, val;
                iss >> key >> val;
                _memory[key] = val;    // 直接更新主存
                _outputLines.push_back(std::format("WRITE {} {}:Miss,loaded and updated", key, val));
            }
        }
        return;
    }

    // 正常缓存模式：解析并执行操作
    std::vector<std::string> operations;
    std::stringstream ss(opsStr);
    std::string opToken;
    while (std::getline(ss, opToken, ';')) {
        size_t start = opToken.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) continue;
        size_t end = opToken.find_last_not_of(" \t\n\r");
        std::string trimmed = opToken.substr(start, end - start + 1);
        std::istringstream iss(trimmed);
        std::string cmd;
        iss >> cmd;
        if (cmd == "READ") {
            int32_t key;
            iss >> key;
            processRead(key);
        }
        else if (cmd == "WRITE") {
            int32_t key, value;
            iss >> key >> value;
            processWrite(key, value);
        }
    }
}

void TaskB::show_result() const {
    // 输出所有操作结果
    for (const auto& line : _outputLines) {
        std::print("{}\n", line);
    }
    std::print("\n");

    // 输出缓存最终状态
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