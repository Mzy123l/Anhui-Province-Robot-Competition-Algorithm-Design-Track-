#pragma once
#include <cstddef>
#include <array>

namespace memory_pool
{
    /*
    * memory_pool.hpp
    * @brief 极致性能的单线程内存池
    * @tparam BlockSize 内存池的块大小，单位字节
    */
    template <std::size_t BlockSize>
    class MemoryPool
    {
    public:
        // 此处为加快速度不初始化池，初始值均为垃圾值
        MemoryPool() : _current_pos(_pool.data()) { static_assert(BlockSize > 0, "BlockSize must be greater than 0"); }
        MemoryPool(const MemoryPool&) = delete;
        MemoryPool& operator=(const MemoryPool&) = delete;
        MemoryPool(MemoryPool&&) = delete;
        MemoryPool& operator=(MemoryPool&&) = delete;

        ~MemoryPool() = default;

        template <typename T>
        void* allocate()
        {
            std::uintptr_t ptr = reinterpret_cast<std::uintptr_t>(_current_pos);
            std::size_t padding = (ptr % alignof(T) ? alignof(T) - (ptr % alignof(T)) : 0);
            char* aligned_ptr = reinterpret_cast<char*>(_current_pos + padding);
            char* new_pos = aligned_ptr + sizeof(T);
            if (new_pos > _pool.data() + BlockSize)
            {
                return nullptr; // 内存池已满
            }
            else
            {
                _current_pos = new_pos;
            }
            return static_cast<void*>(aligned_ptr);
        }

        void deallocate(void* ptr)
        {
            // 不回收内存
        }

    private:
        alignas(std::max_align_t) std::array<char, BlockSize> _pool; // 内存池的存储空间
        
        char* _current_pos; // 当前位置

    };
}
