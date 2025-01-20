#ifndef DISCRETE_THREADPOOL_H
#define DISCRETE_THREADPOOL_H

#include <mutex>
#include <thread>
#include <vector>
#include <queue>
#include <functional>

class ThreadPool{
public:

    explicit ThreadPool(unsigned int size) noexcept;
    ThreadPool() noexcept;
    ~ThreadPool() noexcept;
    void submit(const std::function<void()>& job);

private:

    void loop();
    //TODO make this atomic
    bool m_shouldRun;
    std::queue<std::function<void()>> m_jobs;
    std::mutex m_mutex;
    std::vector<std::thread> m_threads;
    std::condition_variable m_conditionVariable;

};

#endif //MRONEILLSCUBES_THREADPOOL_H
