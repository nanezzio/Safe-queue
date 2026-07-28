#pragma once

#include "safe_queue.h"
#include <vector>
#include <thread>
#include <functional>
#include <future>
#include <atomic>
#include <memory>

class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template<typename Func, typename... Args>
    auto submit(Func&& func, Args&&... args) -> std::future<decltype(func(args...))>
    {
        using ResultType = decltype(func(args...));
        auto task = std::make_shared<std::packaged_task<ResultType()>>(
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
        );
        auto future = task->get_future();
        tasks_.push([task]() { (*task)(); });
        return future;
    }

    void submit(std::function<void()> task) 
    {
        tasks_.push(std::move(task));
    }

private:
    void worker();   

    SafeQueue<std::function<void()>> tasks_;
    std::vector<std::thread> threads_;
    std::atomic<bool> stop_flag_{ false };
};