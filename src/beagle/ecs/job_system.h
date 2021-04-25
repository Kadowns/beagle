//
// Created by Ricardo on 4/11/2021.
//

#ifndef BEAGLE_JOB_SYSTEM_H
#define BEAGLE_JOB_SYSTEM_H

#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>
#include <functional>

#include <grapphs/adjacency_list.h>

namespace beagle {

class BaseJob {
public:
    virtual void execute() = 0;
};

class Job : public BaseJob {
public:
    explicit Job(std::function<void()>&& task) : m_task(task) {}
    void execute() override {
        m_task();
    }
private:
    std::function<void()> m_task;
};

class JobSystem {
private:
    class Worker {
    public:
        explicit Worker(JobSystem* manager);
        ~Worker();

        void stop();

    private:
        JobSystem* m_manager;
        size_t m_currentJobIndex;

        std::thread m_thread;
        bool m_stop = false;
    };

    class JobHandle {
    public:
        void run_after(const JobHandle& other) {
            m_manager->add_dependency(m_index, other.m_index);
        }

        void run_before(const JobHandle& other) {
            m_manager->add_dependency(other.m_index, m_index);
        }

    private:
        friend JobSystem;
        friend Worker;

        JobHandle(JobSystem* manager, size_t index, std::shared_ptr<BaseJob>&& job) :
                m_manager(manager), m_index(index), m_job(std::forward<std::shared_ptr<BaseJob>>(job)) {}

        bool is_enqueued() const { return m_enqueued; }
        bool is_executed() const { return m_executed; }

        void set_enqueued(bool value) { m_enqueued = value; }
        void set_executed(bool value) { m_executed = value; }

    private:
        JobSystem* m_manager;
        std::shared_ptr<BaseJob> m_job;
        size_t m_index;
        bool m_enqueued = false;
        bool m_executed = false;
    };

    enum class JobRelation {
        RUN_BEFORE,
        RUN_AFTER
    };

    using JobGraph = gpp::AdjacencyList<JobHandle, JobRelation>;

public:

    JobSystem();
    ~JobSystem();

    template<typename TJob, typename ...Args>
    JobHandle enqueue(Args&& ...args) {
        auto handle = JobHandle(this, m_jobGraph.size(), std::make_shared<TJob>(std::forward<Args>(args)...));
        m_jobGraph.push(handle);
        return handle;
    }

    void execute();

private:
    friend Worker;
    friend JobHandle;
    void add_dependency(size_t jobId, size_t dependencyId);
    bool has_unfinished_job();
    bool has_available_job();
    bool is_job_available(size_t jobId);
    void enqueue_available_jobs();
    void job_finished(size_t jobId);
    size_t next_job();

private:
    JobGraph m_jobGraph;
    std::mutex m_queueMutex;
    std::condition_variable m_awakeWorker;
    std::vector<std::shared_ptr<Worker>> m_workers;
    std::queue<size_t> m_availableJobs;
};

}

#endif //BEAGLE_JOB_SYSTEM_H
