#pragma once

#include <chrono>
#include <print>

namespace timer 
{

    // 时间单位枚举，用于编译期选择
    enum class TimeUnit
    {
        Seconds,
        Milliseconds,
        Microseconds,
        Nanoseconds
    };

    // 计时器类模板，默认以毫秒为单位
    template<TimeUnit Unit = TimeUnit::Milliseconds>
    class Timer
    {
    public:
        // 默认构造函数
        Timer() : running_(false) {}

        // 开始计时
        void start()
        {
            start_time_ = std::chrono::high_resolution_clock::now();
            running_ = true;
        }

        // 停止计时
        void stop()
        {
            if (running_)
            {
                stop_time_ = std::chrono::high_resolution_clock::now();
                running_ = false;
            }
        }


        void show_time() const
        {
            double value = elapsed();
            const char* unit = nullptr;

            if constexpr (Unit == TimeUnit::Seconds)
                unit = "s";
            else if constexpr (Unit == TimeUnit::Milliseconds)
                unit = "ms";
            else if constexpr (Unit == TimeUnit::Microseconds)
                unit = "us";
            else if constexpr (Unit == TimeUnit::Nanoseconds)
                unit = "ns";

            std::print("耗时: {} {}\n", value, unit);
        }



        // 默认析构函数
        ~Timer() = default;

    private:
        // 查看经过时间，单位为模板指定的 Unit
        double elapsed() const
        {
            auto end = running_ ? std::chrono::high_resolution_clock::now() : stop_time_;
            auto duration = end - start_time_;

            // 编译期选择单位
            if constexpr (Unit == TimeUnit::Seconds)
            {
                return std::chrono::duration<double>(duration).count();
            }
            else if constexpr (Unit == TimeUnit::Milliseconds)
            {
                return std::chrono::duration<double, std::milli>(duration).count();
            }
            else if constexpr (Unit == TimeUnit::Microseconds)
            {
                return std::chrono::duration<double, std::micro>(duration).count();
            }
            else if constexpr (Unit == TimeUnit::Nanoseconds)
            {
                return std::chrono::duration<double, std::nano>(duration).count();
            }
            else
            {
                static_assert(Unit == TimeUnit::Seconds, "Unsupported time unit");
            }
        }
        bool running_;
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
        std::chrono::time_point<std::chrono::high_resolution_clock> stop_time_;
    };

} // namespace timer