#pragma once
#include <cstdint>
#include <algorithm>
#include <utility>

namespace Tasks
{
    // E . 智能调度 / Fast charging
       /*
       * 给定三个整数 A，B，C，分别代表三组电芯当前的剩余容量。
         你可以通过重复执行以下两种调度操作，来增加电芯的容量，最终使 A，B，C 三者完全相等：
         1. 并联快充：选择任意两组电芯，同时为它们充电，使它们的容量各增加 1 单位。
         2. 高压直充：选择任意一组电芯，为其进行高压直充，使其容量增加 2 单位。
         可以证明，通过上述操作的组合，总能使得三组电芯的容量相等。
         请编写程序，计算使三组电芯容量全部相等所需的最小调度操作次数。
       */
       /*
       * 【输入格式】
          从标准输入读入数据。
          输入的第一行包含三个正整数 A，B，C。

         【输出格式】
          输出到标准输出。
          输出一个数字，表示所需的最小调度次数

         【数据范围与约定】
          对于 100% 的数据，1 ≤ A, B, C ≤ 106。
       */

       /*
       * 思路: 直接数学方法构造即可
       */
    class FastCharging
    {
    public:
        FastCharging() = default;

        void solve(uint32_t _a, uint32_t _b, uint32_t _c);
        void show_result() const;
        
    private:
        uint32_t _res;
    };
}