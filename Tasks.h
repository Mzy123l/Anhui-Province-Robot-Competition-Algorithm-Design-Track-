#pragma once
#include "include/brake_distance.h"
#include "include/fast_charging.h"
#include "include/energy_classification.h"
#include "include/delivery_plan.h"
#include "include/caching_technology.h"

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
            case 'A': taskA(); break;
            case 'B': taskB(); break;
            case 'C': taskC(); break;
            case 'D': taskD(); break;
            case 'E': taskE(); break;

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
            uint32_t c;
            std::cin >> c;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            char src[200000];
            
            std::cin.getline(src, 200000);

            timer.start();
            CachingTechnology ct;
            ct.solve(src, c);
            timer.stop();
            ct.show_result();
            timer.show_time();
        }
        void taskC()
        {
            /*constexpr uint32_t _line = 65535;*/
            timer::Timer<timer::TimeUnit::Microseconds> timer;
            uint32_t n, k;

            std::cin >> n >> k;
          /*  if (n < _line)
            {*/
                DeliveryPlanWithSmallElems dp;
                dp.input(n);
                timer.start();
                dp.solve();
                timer.stop();
                dp.show_result();
                timer.show_time();
            /*}*/
           /* else
            {
                DeliveryPlan dp;
                dp.input(n);
                timer.start();
                dp.solve();
                timer.stop();
                dp.show_result();
                timer.show_time();
            }*/
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
        void taskF()
        {}
        void taskG()
        {}
        void taskH()
        {}
        void taskI()
        {}
        void taskJ()
        {}
    };
}