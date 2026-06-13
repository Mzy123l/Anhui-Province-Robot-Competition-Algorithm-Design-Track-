#pragma once
#include "include/brake_distance.h"
#include "include/timer.hpp"
#include <iostream>
#include "include/taskb.h"

namespace Tasks
{
    class Tasks
    {
    public:
        Tasks() = default;

        void work(char choice)
        {
            switch (choice)
            {
            case 'A':
                taskA();
                break;
            case 'B': 
                taskB();
                break;

            }
        }
    private:
        void taskA()
        {
            timer::Timer<timer::TimeUnit::Microseconds> timer;

            uint32_t n;
            std::cin >> n;

            timer.start();

            BrakeDistance bd;

            bd.solve(n);

            timer.stop();

            bd.show_result();

            timer.show_time();
        }

        void taskB()
        {
            timer::Timer<timer::TimeUnit::Microseconds> timer;
            int32_t C;
            std::string opsStr;
            std::cin >> C;
            std::cin.ignore();                 // 忽略换行
            std::getline(std::cin, opsStr);

            timer.start();
            TaskB tb;
            tb.solve(C, opsStr);
            timer.stop();
            tb.show_result();
            timer.show_time();
        }
    };
}