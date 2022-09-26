//
// Created by Jake M O'Neill on 25/09/2022.
//

#ifndef MRONEILLSCUBES_THREADPOOL_H
#define MRONEILLSCUBES_THREADPOOL_H

#include <mutex>
#include <thread>
#include <vector>
#include <queue>

class ThreadPool{
public:
    explicit ThreadPool(unsigned int size);
    ThreadPool();
    ~ThreadPool();
    void submit(const std::function<void()>& job);
private:
    void loop();
    bool m_shouldRun;
    std::queue<std::function<void()>> m_jobs;
    std::mutex m_mutex;
    std::vector<std::thread> m_threads;
    std::condition_variable m_conditionVariable;
};

#endif //MRONEILLSCUBES_THREADPOOL_H
