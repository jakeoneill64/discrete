//
// Created by Jake M O'Neill on 26/09/2022.
//

#include "threadpool.h"
#include <thread>

ThreadPool::ThreadPool(unsigned int size) noexcept
: m_threads{size}, m_shouldRun{true}
{
    for(int x = 0; x < size; x++){
        m_threads[x] = std::thread([&]{loop();});
    }
}

ThreadPool::ThreadPool() noexcept
: ThreadPool(std::thread::hardware_concurrency())
{}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_shouldRun = false;
    }
    m_conditionVariable.notify_all();
    for (std::thread& active_thread : m_threads) {
        active_thread.join();
    }
    m_threads.clear();
}

void ThreadPool::submit(const std::function<void()>& job) {
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_jobs.push(job);
    }
    m_conditionVariable.notify_one();
}

void ThreadPool::loop() {
    while(true){
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_conditionVariable.wait(lock, [this] {
                return !m_jobs.empty() || !m_shouldRun;
            });
            if (!m_shouldRun) {
                return;
            }
            job = m_jobs.front();
            m_jobs.pop();
        }
        job();
    }
}

