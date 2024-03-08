#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class SafeQueue {
private:
    mutable std::mutex m_mutex;
    std::queue<T> m_queue;
    std::condition_variable m_cv;
public:
    void push(const T& value) {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_queue.push(value);
        m_cv.notify_one();
    }

    void pop(T& value)  {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this]() { return m_queue.empty() == false; });

        value = m_queue.front();
        m_queue.pop();
    }
    void top(T& value)  {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this]() { return m_queue.empty() == false; });

        value = m_queue.front();
    }
    bool empty()  {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        return m_queue.empty();
    }
    size_t size() {
        std::lock_guard<std::mutex> lock(m_mutex);

        return m_queue.size();
    }
};

