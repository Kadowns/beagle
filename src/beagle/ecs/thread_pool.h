//
// Created by Ricardo on 1/3/2022.
//

#ifndef BEAGLE_THREAD_POOL_H
#define BEAGLE_THREAD_POOL_H

#include <vector>
#include <set>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    typedef std::function<void(size_t executionIndex)> Function;

private:

    struct Work {
        Function func;
        size_t executionIndex;
    };

public:

    void execute(Function&& f, size_t executionCount = 1){
        std::lock_guard<std::mutex> lock(m_workQueueMutex);
        for (int i = 0; i < executionCount; i++){
            m_work
        }
        wait_idle(executionCount);
        for (int i = 0; i < executionCount; i++){
            m_awakeCondition.notify_one();
        }
    }

    size_t size() const{
        return m_threads.size();
    }

    void resize(size_t newSize){
        int count = static_cast<int>(newSize) - static_cast<int>(size());
        if (count < 0){
            m_threads.erase(m_threads.end() - count, m_threads.end());
            return;
        }

        for (int i = 0; i < count; i++){
            m_threads.emplace_back(*this, m_idCounter++);
        }
    }

private:

    class Thread {
    public:

        Thread(ThreadPool& owner, size_t id) : m_owner(owner), m_id(id) {
            m_thread = std::thread([this]{
                while (!m_stop) {
                    auto [work, executionIndex] = m_owner.wait_for_work(m_id);
                    if (work) {
                        work(executionIndex);
                    }
                }
            });
        }

        ~Thread(){
            m_stop = true;
            m_thread.join();
        }

    private:
        ThreadPool& m_owner;
        size_t m_id;
        std::thread m_thread;
        bool m_stop = false;
    };
private:

    std::tuple<Function, size_t> wait_for_work(size_t threadId){
        std::unique_lock<std::mutex> lock(m_mutex);
        m_idleThreads.insert(threadId);
        m_idleCondition.notify_all();
        m_awakeCondition.wait(lock);
        m_idleThreads.erase(threadId);
        return {m_pendingWork, 0};
    }

    void wait_idle(size_t idleCount){
        std::unique_lock<std::mutex> lock(m_mutex);
        m_idleCondition.wait(lock, [this, idleCount]{
            return m_idleThreads.size() >= idleCount;
        });
    }


private:
    friend Thread;
    std::vector<Thread> m_threads;
    std::set<size_t> m_idleThreads;
    std::queue<Work> m_workQueue;
    std::mutex m_mutex;
    std::mutex m_workQueueMutex;
    std::condition_variable m_awakeCondition;
    std::condition_variable m_idleCondition;
    Function m_pendingWork;
    size_t m_idCounter;
};


#endif //BEAGLE_THREAD_POOL_H
