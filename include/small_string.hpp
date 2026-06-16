#pragma once

#include <cstdint>
#include <cstring>
#include <functional>
#include <string_view>
#include <format>
#include <print>

namespace tools
{

    /**
     * @brief 极致优化的 9 字节栈字符串（无动态分配、无容量判断）
     *
     * 布局：char _Buf[9] + uint8_t _Mysize，共 10 字节。
     * _Buf[8] 始终为 '\0'，确保 data() 返回合法的 C 风格字符串。
     * 最大有效长度为 8（_Mysize ∈ [0,8]）。
     * 所有操作直接操作缓冲区，无分支判断。
     */
    class sstring_view;

    class sstring
    {
        char     _Buf[9] = {};   // 缓冲区，_Buf[8] 恒为 '\0'
        uint8_t  _Mysize = 0;    // 当前长度（0~8）

    public:
        sstring() noexcept = default;

        // 从 C 风格字符串构造，最多复制 8 个字符
        inline sstring(const char* s) noexcept
        {
            if (s)
            {
                uint8_t len = 0;
                // 单次循环复制，最多 8 次
                while (len < 8 && s[len])
                {
                    _Buf[len] = s[len];
                    ++len;
                }
                _Mysize = len;
                _Buf[len] = '\0';   // 保证空终止
            }
        }

        sstring(const sstring&) noexcept = default;
        sstring& operator=(const sstring&) noexcept = default;

        inline operator sstring_view() const noexcept;

        inline const char* data() const noexcept { return _Buf; }
        inline uint8_t size() const noexcept { return _Mysize; }
        inline bool empty() const noexcept { return _Mysize == 0; }

        // 比较：先比长度，再 memcmp（编译器会内联为几个 cmp/je）
        friend inline bool operator==(const sstring& a, const sstring& b) noexcept
        {
            return a._Mysize == b._Mysize &&
                std::memcmp(a._Buf, b._Buf, a._Mysize) == 0;
        }
        friend inline bool operator!=(const sstring& a, const sstring& b) noexcept
        {
            return !(a == b);
        }
    };

    /**
     * @brief 字符串视图，不拥有数据
     */
    class sstring_view
    {
        const char* _Data = nullptr;
        uint8_t     _Size = 0;

    public:
        sstring_view() noexcept = default;
        inline sstring_view(const char* data, uint8_t size) noexcept : _Data(data), _Size(size) {}
        inline sstring_view(const sstring& s) noexcept : _Data(s.data()), _Size(s.size()) {}

        inline const char* data() const noexcept { return _Data; }
        inline uint8_t size() const noexcept { return _Size; }
        inline bool empty() const noexcept { return _Size == 0; }

        friend inline bool operator==(const sstring_view& a, const sstring_view& b) noexcept
        {
            return a._Size == b._Size &&
                std::memcmp(a._Data, b._Data, a._Size) == 0;
        }
        friend inline bool operator!=(const sstring_view& a, const sstring_view& b) noexcept
        {
            return !(a == b);
        }
    };

    inline sstring::operator sstring_view() const noexcept
    {
        return sstring_view(_Buf, _Mysize);
    }

} 

// 哈希：复用 std::hash<std::string_view> 的高度优化实现
namespace std 
{
    template<>
    struct hash<tools::sstring_view>
    {
        inline size_t operator()(const tools::sstring_view& sv) const noexcept
        {
            return hash<string_view>()(string_view(sv.data(), sv.size()));
        }
    };

    template<>
    struct hash<tools::sstring>
    {
        inline size_t operator()(const tools::sstring& s) const noexcept
        {
            return hash<string_view>()(string_view(s.data(), s.size()));
        }
    };
}

// 格式化支持
namespace std 
{
    template<>
    struct formatter<tools::sstring>
    {
        constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
        {
            auto it = ctx.begin();
            if (it != ctx.end() && *it != '}')
                throw format_error("unsupported format specifier for sstring");
            return it;
        }
        inline auto format(const tools::sstring& s, format_context& ctx) const -> decltype(ctx.out())
        {
            return format_to(ctx.out(), "{}", string_view(s.data(), s.size()));
        }
    };

    template<>
    struct formatter<tools::sstring_view>
    {
        constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
        {
            auto it = ctx.begin();
            if (it != ctx.end() && *it != '}')
                throw format_error("unsupported format specifier for sstring_view");
            return it;
        }
        inline auto format(const tools::sstring_view& sv, format_context& ctx) const -> decltype(ctx.out())
        {
            return format_to(ctx.out(), "{}", string_view(sv.data(), sv.size()));
        }
    };
}