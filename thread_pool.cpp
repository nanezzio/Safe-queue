#include "thread_pool.h"
#include <iostream>

ThreadPool::ThreadPool() 
{
    unsigned int n = std::thread::hardware_concurrency();
    if (n == 0) n = 2;   
    threads_.reserve(n);
    for (unsigned int i = 0; i < n; ++i) 
    {
        threads_.emplace_back(&ThreadPool::worker, this);
    }
}

ThreadPool::~ThreadPool()
{
    stop_flag_.store(true);
    tasks_.wake_all();   
    for (auto& t : threads_) 
    {
        if (t.joinable()) t.join();
    }
}

void ThreadPool::worker() 
{
    while (true) 
    {
        std::function<void()> task;
        if (!tasks_.pop(task, stop_flag_)) 
        {
            break;
        }
        task();
    }
}