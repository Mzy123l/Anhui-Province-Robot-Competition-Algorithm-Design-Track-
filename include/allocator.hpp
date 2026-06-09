#pragma once
#include "memory_pool.hpp"

namespace allocator
{
    /*
    * @brief 基于内存池的分配器
    * @tparam T 分配器分配的类型
    */
    template <typename T, std::size_t BlockSize>
    class PoolAllocator
    {
    private:
        using MemoryPool = memory_pool::MemoryPool<BlockSize>;
    public:
        // 标准类型定义
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        // 可以安全复制、移动、交换
        using propagate_on_container_copy_assignment = std::true_type;
        using propagate_on_container_move_assignment = std::true_type;
        using propagate_on_container_swap = std::true_type;

        // 总是相等（如果所有分配器共享同一内存池）
        // 如果每个分配器有独立内存池，则设置为false_type
        using is_always_equal = std::true_type;
        // 标准rebind模板
        template <typename U>
        struct rebind
        {
            using other = PoolAllocator<U, BlockSize>;
        };
        // 构造函数
        explicit PoolAllocator(MemoryPool& pool) noexcept
            : pool_(&pool){}

        // 重新绑定的构造函数
        template<typename U>
        PoolAllocator(const PoolAllocator<U, BlockSize>& other) noexcept
            : pool_(other.get_pool())
        {}

        // 分配内存
        T* allocate()
        {

            // 从内存池分配
            void* ptr = pool_->allocate<T>();
            if (!ptr)
            {
                return nullptr;
            }

            return static_cast<T*>(ptr);
        }
        // 释放内存
        void deallocate(T* ptr, std::size_t n) noexcept 
        {
            // 不释放
        }

        // 构造对象
        template<typename U, typename... Args>
        void construct(U* ptr, Args&&... args)
        {
            ::new(static_cast<void*>(ptr)) U(std::forward<Args>(args)...);
        }

        // 销毁对象
        template<typename U>
        void destroy(U* ptr)
        {
            ptr->~U();
        }

        // 比较操作符
        template<typename U>
        bool operator==(const PoolAllocator<U, BlockSize>& other) const noexcept
        {
            return pool_ == other.get_pool();
        }

        template<typename U>
        bool operator!=(const PoolAllocator<U, BlockSize>& other) const noexcept
        {
            return !(*this == other);
        }
    private:
        MemoryPool* pool_;

        // 允许不同类型的分配器互相访问私有成员
        template<typename U, std::size_t BlockSize>
        friend class PoolAllocator;
    };
}