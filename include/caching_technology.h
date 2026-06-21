#pragma once
#include "allocator.hpp"
#include <unordered_map>
#include <list>
#include <functional>
#include <vector>

namespace Tasks
{
    // 缓存技术 / Caching technology
    /*
    * 【输入格式】
    从标准输入读入数据。
    输入共两行。第一行一个非负整数 C，表示缓存容量（C = 0 表示缓存无法存放任何键值对）。
    第二行一个完整字符串 S，表示操作序列。操作之间用分号 ; 分隔。
    每个操作格式为 READ key 或 WRITE key value，其中 key 与 value 均为 32 位带符号整数。
    字符串中允许出现任意数量的空白字符（空格等），它们可能出现在操作首尾、分号前后、以及单词之间。
    你需要正确解析出所有操作并按顺序执行。

    【输出格式】
    输出到标准输出。
    对每个解析出的操作，按执行顺序输出一行：
    1. 读命中：READ key:Hit,value=v
    2. 读缺失：READ key:Miss,loaded,value=v
    3. 写命中：WRITE key value:Hit,updated
    4. 写缺失：WRITE key value:Miss,loaded and updated
    其中 key 与 value 为操作中的参数，v 为读操作实际读到的值。
    当所有操作处理完后，先输出一个空行，再额外输出缓存最终状态：
    • 若缓存非空，输出一行 Cachestate(MRU‐>LRU):。接着按 MRU 到 LRU 顺序，
    每行输出一个键值对，格式 key:value。
    • 若缓存为空，输出 Cache is empty.。

    【数据范围与约定】
    对于 10% 的数据，C = 1。
    对于全部数据，0 ≤ C ≤ 1000，−2
    31 ≤ key, value ≤ 2
    31−1。操作串长度 |S| ≤ 2×105。
    */

    /*
    * 思路: 队列 + unordered_map <key , iterator>
    */
    class CachingTechnology
    {
    public:
        CachingTechnology() :_pool(), _alloc(_pool), _current_count(0),
            _map(allocator::PoolAllocator<std::pair<const int32_t, list::iterator>, _block_size>(_alloc)),
            _list_outer(allocator::PoolAllocator<Node, _block_size>(_alloc)),
            _list(allocator::PoolAllocator<Node, _block_size>(_alloc)),
            _result(allocator::PoolAllocator<tuple, _block_size>(_alloc)){}
        
        void solve(char*, uint16_t C);
        void show_result() const;

    private:
        inline void read(int32_t key);
        inline void write(int32_t key, int32_t value);
        
    private:
        struct Node
        {
            Node(int32_t key, int32_t val, bool dirty, bool inqueue) : _key(key), _value(val), _dirty(dirty), _inqueue(inqueue) {}
            int32_t _key;
            int32_t _value;
            bool _dirty = false;
            bool _inqueue = false;
        };

        uint16_t _c;
        uint16_t _current_count;
        static constexpr std::size_t _block_size = 80000000;
        memory_pool::MemoryPool<_block_size> _pool;
        allocator::PoolAllocator<int32_t, _block_size> _alloc;

        using list = std::list <Node, allocator::PoolAllocator<Node, _block_size>>;

        // 节点顺序
        list _list;
        // 键值到节点的映射
        std::unordered_map <int32_t, list::iterator, std::hash<int32_t>, std::equal_to<int32_t>,
            allocator::PoolAllocator<std::pair<const int32_t, list::iterator>, _block_size>> _map; 
        list _list_outer;
        static constexpr uint8_t _is_read = 0;
        static constexpr uint8_t _is_hit = 1;
        static constexpr uint8_t _key = 2;
        static constexpr uint8_t _value = 3;
        using tuple = std::tuple<bool, bool, int32_t, int32_t>;
        // 是否为读 | 是否命中 | Key | Value
        std::vector<tuple, allocator::PoolAllocator<tuple, _block_size>>  _result;
    };
}