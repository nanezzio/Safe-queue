#include "thread_pool.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

void test_func1() 
{
    std::this_thread::sleep_for(100ms);
    std::cout << "test_func1 executed in thread " << std::this_thread::get_id() << std::endl;
}

void test_func2() 
{
    std::this_thread::sleep_for(200ms);
    std::cout << "test_func2 executed in thread " << std::this_thread::get_id() << std::endl;
}

void test_func_arg(int x)
{
    std::this_thread::sleep_for(50ms);
    std::cout << "test_func_arg(" << x << ") in thread " << std::this_thread::get_id() << std::endl;
}

int main()
{
    std::cout << "Main thread ID: " << std::this_thread::get_id() << std::endl;
    ThreadPool pool;

    auto future_sum = pool.submit([](int a, int b) { return a + b; }, 10, 20);
    std::cout << "Sum result: " << future_sum.get() << std::endl;

    pool.submit(test_func1);
    pool.submit(test_func2);

    for (int i = 0; i < 3; ++i)
    {
        pool.submit(test_func_arg, i);
    }

    for (int i = 0; i < 3; ++i) 
    {
        std::this_thread::sleep_for(1s);
        std::cout << "\nIteration " << i + 1 << ": submitting two tasks" << std::endl;
        pool.submit([]() 
            {
            std::cout << "Task A in thread " << std::this_thread::get_id() << std::endl;
            });
        pool.submit([]() 
            {
            std::cout << "Task B in thread " << std::this_thread::get_id() << std::endl;
            });
    }

    std::this_thread::sleep_for(2s);
    std::cout << "\nMain thread finished." << std::endl;
    return 0;
}