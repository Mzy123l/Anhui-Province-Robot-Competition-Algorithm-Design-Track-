#pragma once
#include "include/brake_distance.h"
#include "include/fast_charging.h"
#include "include/energy_classification.h"
#include "include/delivery_plan.h"
#include "include/caching_technology.h"
#include "include/gear_calibration.h"
#include "include/independent_service.h"
#include "include/keep_health.h"
#include "include/line_optimization.h"
#include "include/journey_endurance.h"
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
            case 'F': taskF(); break;
            case 'G': taskG(); break;
            case 'H': taskH(); break;
            case 'I': taskI(); break;
            case 'J': taskJ(); break;
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
        {
            constexpr int16_t line = 50; // 实际测试 m = 50 时速度接近
            timer::Timer<timer::TimeUnit::Microseconds> timer;
            int16_t target[4];
            int16_t m;
            for (uint8_t i = 0; i < static_cast<uint8_t>(4); ++i)
            {
                std::cin >> target[i];
            }
            std::cin >> m;
            if (m < line)
            {
                GearCalibrationWithSmallElems gc;
                gc.input(target, m);
                timer.start();
                gc.solve();
                timer.stop();
                gc.show_result();
                timer.show_time();
            }
            else
            {
                GearCalibration gc;
                gc.input(target, m);
                timer.start();
                gc.solve();
                timer.stop();
                gc.show_result();
                timer.show_time();
            }
        }
        void taskG()
        {
            timer::Timer<timer::TimeUnit::Microseconds> timer;
            IndependentService is;
            is.input();
            timer.start();
            is.solve();
            timer.stop();
            is.show_result();
            timer.show_time();
        }
        void taskH()
        {
            timer::Timer<timer::TimeUnit::Microseconds> timer;
            JourneyEndurance je;
            je.input();
            timer.start();
            je.solve();
            timer.stop();
            je.show_result();
            timer.show_time();
        }
        void taskI()
        {
            timer::Timer<timer::TimeUnit::Microseconds> timer;
            uint32_t Q, K;
            std::cin >> Q >> K;
            KeepHealth kh(Q, K);
            kh.input();
            timer.start();
            kh.solve();
            timer.stop();
            kh.show_result();
            timer.show_time();

        }
        void taskJ()
        {
            timer::Timer<timer::TimeUnit::Microseconds> timer;
            LineOptimization lo;
            lo.input();
            timer.start();
            lo.solve();
            timer.stop();
            lo.show_result();
            timer.show_time();
        }
    };
}