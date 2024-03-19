/*
#include <iostream>
#include "safequeue.h"

using namespace std;

template class SafeQueue<int>;

template <typename T>
void SafeQueue<T>::push(const T& value) {
    lock_guard<mutex> lock(m_mutex);

    m_queue.push(value);
    m_cv.notify_one();
}

template <typename T>
void SafeQueue<T>::pop(T& value) {
    unique_lock<mutex> lock(m_mutex);
    m_cv.wait(lock, [this]() { return m_queue.empty() == false; });

    value = m_queue.front();
    m_queue.pop();
}

template <typename T>
bool SafeQueue<T>::empty() {
    lock_guard<mutex> lock(m_mutex);
    
    return m_queue.empty();
}

template <typename T>
size_t SafeQueue<T>::size() {
    lock_guard<mutex> lock(m_mutex);

    return m_queue.size();
}
/**/