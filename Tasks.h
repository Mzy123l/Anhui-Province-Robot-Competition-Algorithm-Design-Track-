#pragma once
#include "include/brake_distance.h"
#include "include/timer.hpp"
#include <iostream>

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
    };
}