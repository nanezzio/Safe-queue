#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

template<typename T>
class SafeQueue 
{
public:
    SafeQueue() = default;
    ~SafeQueue() = default;

    SafeQueue(const SafeQueue&) = delete;
    SafeQueue& operator=(const SafeQueue&) = delete;

    void push(T value)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(value));
        }
        cond_var_.notify_one();
    }

    bool pop(T& value, const std::atomic<bool>& stop_flag) 
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_var_.wait(lock, [&] 
            {
            return !queue_.empty() || stop_flag.load();
            });
        if (stop_flag.load() && queue_.empty()) 
        {
            return false;
        }
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return false;
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    void wake_all() 
    {
        cond_var_.notify_all();
    }

    size_t size() const 
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_var_;
};