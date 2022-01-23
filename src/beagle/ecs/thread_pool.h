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
#include <future>
#include <memory>

namespace beagle {

class ThreadPool {
public:

    class Work {
    public:

        explicit Work(std::function<void()> work) :
                m_work(std::move(work)) {}

        bool is_complete() const {
            return m_complete;
        }

        void wait() {
            std::unique_lock<std::mutex> lock(m_workMutex);
            m_workFinished.wait(lock, [this]() {
                return is_complete();
            });
        }

        void operator()() {
            m_work();
            m_complete = true;
            m_workFinished.notify_all();
        }

    private:
        std::function<void()> m_work;
        std::mutex m_workMutex;
        std::condition_variable m_workFinished;
        bool m_complete = false;
    };

    class WorkResult {
    public:

        WorkResult() {}

        WorkResult(std::shared_ptr<Work> work) {
            push(std::move(work));
        }

        void push(std::shared_ptr<Work> work) {
            m_workDependencies.emplace_back(std::move(work));
        }

        void clear() {
            m_workDependencies.clear();
        }

        void wait() {
            for (auto &work : m_workDependencies) {
                work->wait();
            }
        }

    private:
        std::vector<std::shared_ptr<Work>> m_workDependencies;
    };

public:

    explicit ThreadPool(size_t size) {
        for (int i = 0; i < size; i++) {
            m_threads.emplace_back([this] {
                while (!m_stop) {
                    auto work = wait_for_work();
                    if (work) {
                        work->operator()();
                    }
                }
            });
        }
    }

    ~ThreadPool() {
        m_stop = true;
        m_awakeCondition.notify_all();
    }

    template<typename F>
    WorkResult execute(F &&f) {
        std::lock_guard<std::mutex> lock(m_workQueueMutex);
        m_workQueue.push(std::make_shared<Work>(f));
        WorkResult result(m_workQueue.back());
        m_awakeCondition.notify_one();
        return result;
    }

    template<typename F>
    WorkResult execute_for(size_t count, F &&f) {
        std::lock_guard<std::mutex> lock(m_workQueueMutex);
        WorkResult result;
        for (int i = 0; i < count; i++) {
            m_workQueue.push(std::make_shared<Work>([i, f] {
                f(i);
            }));
            result.push(m_workQueue.back());
            m_awakeCondition.notify_one();
        }

        return result;
    }

    size_t size() const {
        return m_threads.size();
    }

private:

    std::shared_ptr<Work> wait_for_work() {
        std::unique_lock<std::mutex> lock(m_workQueueMutex);

        m_awakeCondition.wait(lock, [this] {
            return !m_workQueue.empty() || m_stop;
        });

        if (m_stop) {
            return nullptr;
        }

        auto work = m_workQueue.front();
        m_workQueue.pop();
        return work;
    }

private:
    std::vector<std::thread> m_threads;
    std::queue<std::shared_ptr<Work>> m_workQueue;
    std::mutex m_workQueueMutex;
    std::condition_variable m_awakeCondition;
    bool m_stop = false;
};

}

#endif //BEAGLE_THREAD_POOL_H
