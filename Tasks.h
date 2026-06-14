#pragma once
#include "include/brake_distance.h"
#include "include/fast_charging.h"
#include "include/energy_classification.h"


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
            case 'D':
                taskD();
                break;
            case 'E':
                taskE();
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
        void taskD()
        {
            timer::Timer<timer::TimeUnit::Microseconds> timer;

            EnergyClassification ec;
   
            ec.input();

            timer.start();

            ec.solve();

            timer.stop();

            ec.show_result();

            timer.show_time();
        }
        void taskE()
        {
            timer::Timer<timer::TimeUnit::Nanoseconds> timer;
            uint32_t a, b, c;
            std::cin >> a >> b >> c;
            timer.start();
            FastCharging fc;
            fc.solve(a, b, c);
            timer.stop();
            fc.show_result();
            timer.show_time();
        }
    };
}