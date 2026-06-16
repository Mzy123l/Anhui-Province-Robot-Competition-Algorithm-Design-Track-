<<<<<<< HEAD
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
        if (!node) return;   // ÄÚ´æ³Ø²»¹»£¨ÀíÂÛÉÏ²»»á£©
        _alloc.construct(node);
        node->prev = node->next = nullptr;
        node->dirty = false;
        node->key = node->value = 0;
        // ²åÈë¿ÕÏÐÁ´±íÍ·²¿
        node->next = _freeHead;
        if (_freeHead) _freeHead->prev = node;
        _freeHead = node;
    }
}

int32_t TaskB::readMemory(int32_t key) const {
    auto it = _memory.find(key);
    if (it != _memory.end())
        return it->second;
    return key;   // ³õÊ¼ÖµµÈÓÚ key ±¾Éí
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
    // ´Óµ±Ç°Î»ÖÃÕª³ý
    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    if (node == _tail) _tail = node->prev;
    // ²åÈëÍ·²¿
    node->prev = nullptr;
    node->next = _head;
    if (_head) _head->prev = node;
    _head = node;
    if (!_tail) _tail = node;
}

TaskB::Node* TaskB::evictLru() {
    if (!_tail) return nullptr;
    Node* victim = _tail;
    // ´ÓÁ´±íÖÐÒÆ³ý
    if (victim->prev) victim->prev->next = nullptr;
    _tail = victim->prev;
    if (_head == victim) _head = nullptr;
    // ´Ó¹þÏ£±íÉ¾³ý
    _keyToNode.erase(victim->key);
    // ÔàÊý¾ÝÐ´»ØÖ÷´æ
    writeBackMemory(victim);
    return victim;
}

TaskB::Node* TaskB::loadFromMemory(int32_t key) {
    int32_t memVal = readMemory(key);
    Node* node = nullptr;

    if (static_cast<int32_t>(_keyToNode.size()) == _capacity) {
        // »º´æÂú£ºÌÔÌ­ LRU£¬¸´ÓÃ¸Ã½Úµã
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
        // ÃüÖÐ
        Node* node = it->second;
        moveToHead(node);
        std::print("READ {}:Hit,value={}\n", key, node->value);
    }
    else {
        // È±Ê§
        Node* node = loadFromMemory(key);
        int32_t val = node ? node->value : readMemory(key);
        std::print("READ {}:Miss,loaded,value={}\n", key, val);
    }
}

void TaskB::processWrite(int32_t key, int32_t value) {
    auto it = _keyToNode.find(key);
    if (it != _keyToNode.end()) {
        // Ð´ÃüÖÐ
        Node* node = it->second;
        node->value = value;
        node->dirty = true;
        moveToHead(node);
        std::print("WRITE {} {}:Hit,updated\n", key, value);
    }
    else {
        // Ð´È±Ê§£ºÏÈ¼ÓÔØ£¬ÔÙ¸üÐÂ
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

    // ½âÎö²Ù×÷×Ö·û´®
    const char* p = ops;
    while (*p) {
        // Ìø¹ý¿Õ°××Ö·û£¨¿Õ¸ñ¡¢ÖÆ±í·û¡¢»»ÐÐµÈ£©
        while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
            ++p;
        if (!*p) break;

        // ¶ÁÈ¡ÃüÁî£¨READ »ò WRITE£©
        char cmd[8] = { 0 };
        int i = 0;
        while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && *p != ';' && i < 7)
            cmd[i++] = *p++;
        cmd[i] = '\0';
        // Ìø¹ýÃüÁîºóµÄ¿Õ°×
        while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
            ++p;

        if (strcmp(cmd, "READ") == 0) {
            // ¶ÁÈ¡ key
            int32_t key = 0;
            auto [ptr, ec] = std::from_chars(p, p + 20, key);
            p = ptr;
            // Ìø¹ý key Ö®ºóµÄ¿Õ°×£¬Ö±µ½ ';' »ò½áÎ²
            while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
                ++p;
            if (*p == ';') ++p;   // Ìø¹ý·Ö¸ô·û

            if (_cacheEnabled)
                processRead(key);
            else
                std::print("READ {}:Miss,loaded,value={}\n", key, key);
        }
        else if (strcmp(cmd, "WRITE") == 0) {
            // ¶ÁÈ¡ key ºÍ value
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
            // ·Ç·¨ÃüÁî£¬Ìø¹ýÖ±µ½·ÖºÅ
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
=======
#include "../include/caching_technology.h"
#include <string>
#include <print>
using namespace Tasks;

void CachingTechnology::solve(char* head, uint16_t C)
{
    _c = C;
    _result.reserve(4000);
    char* current = head;
    bool is_read;
    bool has_set_key = false;
    int32_t key;
    int32_t value;
    while (*current != '\0')
    {
        if (*current == 'A')
        {
            is_read = true;
            ++current;
            continue;
        }
        if (*current == 'I')
        {
            is_read = false;
            ++current;
            continue;
        }
        if (*current == ';')
        {
            ++current;
            has_set_key = false;
            continue;
        }
        if ((*current) >= '0' && (*current) <= '9')
        {
            if (has_set_key)
            {
                value = std::stoi(current);
                write(key, value);
                while ((*current) != ';' && (*current) != '\0')
                {
                    ++current;
                }
                has_set_key = false;
            }
            else
            {
                key = std::stoi(current);
                if (is_read)
                {
                    read(key);
                    while ((*current) != ';' && (*current) != '\0')
                    {
                        ++current;
                    }
                    has_set_key = false;
                }
                else
                {
                    has_set_key = true;
                    while ((*current) != ' ')
                    {
                        ++current;
                    }
                }
            }

        }
        ++current;
    }
}

inline void CachingTechnology::read(int32_t key)
{
    if (!_map.contains(key))
    {
        _result.emplace_back(true, false, key, key);
        _list.emplace_front(key, key, true, true);
        _map[key] = _list.begin();
        
        if (_current_count == _c)
        {
            int32_t back_key = _list.back()._key;
            _list.back()._inqueue = false;
            _list.back()._dirty = false;
            _list_outer.push_front(_list.back());
            _list.pop_back();
            _map[back_key] = _list_outer.begin();
            return;
        }
        ++_current_count;
        return;
    }
    list::iterator it = _map[key];
    auto current = *it;
    if (current._inqueue)
    {
        _result.emplace_back(true, true, key, current._value);
        _list.emplace_front(key, current._value, false, true);
        _list.erase(it);
    }
    else
    {
        _result.emplace_back(true, false, key, current._value);
        _list.emplace_front(key, current._value, false, true);
        _list_outer.erase(it);
        if (_current_count == _c)
        {
            int32_t back_key = _list.back()._key;
            _list.back()._inqueue = false;
            _list.back()._dirty = false;
            _list_outer.push_front(_list.back());
            _list.pop_back();
            _map[back_key] = _list_outer.begin();
        }
    }


    _map[key] = _list.begin();
}

inline void CachingTechnology::write(int32_t key, int32_t value)
{
    if (!_map.contains(key))
    {
        _result.emplace_back(false, false, key, value);
        _list.emplace_front(key, value, true, true);
        _map[key] = _list.begin();

        if (_current_count == _c)
        {
            int32_t back_key = _list.back()._key;
            _list.back()._inqueue = false;
            _list.back()._dirty = false;
            _list_outer.push_front(_list.back());
            _list.pop_back();
            _map[back_key] = _list_outer.begin();
            return;
        }
        ++_current_count;
        return;
    }
    list::iterator it = _map[key];
    auto current = *it;
    if (current._inqueue)
    {
        _result.emplace_back(false, true, key, value);
        current._dirty = true;
        _list.emplace_front(key, value, true, true);
        _list.erase(it);
    }
    else
    {
        _result.emplace_back(false, false, key, value);
        _list.emplace_front(key, value, true, true);
        _list_outer.erase(it);
        if (_current_count == _c)
        {
            int32_t back_key = _list.back()._key;
            _list.back()._inqueue = false;
            _list.back()._dirty = false;
            _list_outer.push_front(_list.back());
            _list.pop_back();
            _map[back_key] = _list_outer.begin();
        }
    }
    
    _map[key] = _list.begin();
}
void CachingTechnology::show_result() const
{

    for (const auto& elem : _result)
    {

        if (std::get<_is_read>(elem))
        {
            if (std::get<_is_hit>(elem))
            {
                std::println("READ {}:Hit,value={}", std::get<_key>(elem), std::get<_value>(elem));
            }
            else
            {
                std::println("READ {}:Miss,loaded,value={}", std::get<_key>(elem), std::get<_value>(elem));
            }
        }
        else
        {
            if (std::get<_is_hit>(elem))
            {
                std::println("WRITE {} {}:Hit,updated", std::get<_key>(elem), std::get<_value>(elem));

            }
            else
            {
                std::println("WRITE {} {}:Miss,loaded and updated", std::get<_key>(elem), std::get<_value>(elem));
            }
        }
    }
    std::println();
    if (_c == 0)
    {
        std::println("Cache is empty.");
    }
    else
    {
        std::println("Cachestate(MRUâ€>LRU):");
        for (const auto& it : _list)
        {
            if (!it._inqueue)return;
            std::println("{}:{}", it._key, it._value);
        }
    }
    
>>>>>>> ddeeebdcac797021cd8e29f4914513f13e0b293d
}