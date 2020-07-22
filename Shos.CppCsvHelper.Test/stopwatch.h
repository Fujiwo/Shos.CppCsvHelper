#pragma once
#include <iostream>
#include <chrono>
#include <ctime>
#include <functional>

class stopwatch
{
    const std::chrono::time_point<std::chrono::system_clock> start; // 計測開始時刻

public:
    std::function<void(double)> output = show_result;
    //void (*output)(double elapsed) = show_result;

    stopwatch() : start(std::chrono::system_clock::now())
    {}

    virtual ~stopwatch()
    {
        auto end = std::chrono::system_clock::now();  // 計測終了時刻
        double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
        output(elapsed);
    }

private:
    static void show_result(double elapsed)
    {
        std::cout << "(" << elapsed << "s.)" << std::endl;
    }
};
