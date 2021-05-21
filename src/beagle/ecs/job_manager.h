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

#include <eagle/timer.h>

#include <grapphs/adjacency_list.h>

namespace beagle {

class BaseJob {
public:
    explicit BaseJob(const std::string& name) : m_name(name) {}

    uint64_t timed_execute() {
        m_timer.start();
        execute();
        m_timer.stop();
        return m_timer.time_nano();
    }
    virtual void execute() = 0;

    const std::string& name() { return m_name; }

private:
    eagle::Timer m_timer;
    std::string m_name;
};

class Job : public BaseJob {
public:
    explicit Job(std::function<void()>&& task, const std::string& name = "Lambda") : BaseJob(name), m_task(task) {}
    void execute() override {
        m_task();
    }
private:
    std::function<void()> m_task;
};

class JobManager {
private:
    class Worker {
    public:
        explicit Worker(JobManager* manager);
        ~Worker();

        void stop();

    private:
        JobManager* m_manager;
        size_t m_currentJobIndex;

        std::thread m_thread;
        bool m_stop = false;
    };
public:
    class JobHandle {
    public:
        JobHandle() = default;
        void run_after(const JobHandle& other) {
            m_manager->add_dependency(m_index, other.m_index);
        }

        void run_before(const JobHandle& other) {
            m_manager->add_dependency(other.m_index, m_index);
        }

    private:
        friend JobManager;
        friend Worker;

        JobHandle(JobManager* manager, size_t index, std::shared_ptr<BaseJob>&& job) :
                m_manager(manager), m_index(index), m_job(std::forward<std::shared_ptr<BaseJob>>(job)) {}

        bool is_enqueued() const { return m_enqueued; }
        bool is_executed() const { return m_executed; }

        void set_enqueued(bool value) { m_enqueued = value; }
        void set_executed(bool value) { m_executed = value; }

    private:
        JobManager* m_manager = nullptr;
        std::shared_ptr<BaseJob> m_job;
        size_t m_index = 0;
        bool m_enqueued = false;
        bool m_executed = false;
    };
private:

    enum class JobRelation {
        RUN_BEFORE,
        RUN_AFTER
    };

    using JobGraph = gpp::AdjacencyList<JobHandle, JobRelation>;

    struct JobProfiling {
        JobProfiling(const std::string& name, int64_t durationNano) :
        name(name), durationNano(durationNano) {}

        const std::string& name;
        int64_t durationNano;
    };

public:

    JobManager();
    ~JobManager();

    template<typename TJob, typename ...Args>
    JobHandle enqueue(Args&& ...args) {
        auto handle = JobHandle(this, m_jobGraph.size(), std::make_shared<TJob>(std::forward<Args>(args)...));
        m_jobGraph.push(handle);
        return handle;
    }

    void execute();
    const std::vector<JobProfiling>& job_profiling() const { return m_executionTimes; }

private:
    friend Worker;
    friend JobHandle;
    void add_dependency(size_t jobId, size_t dependencyId);
    bool has_unfinished_job();
    bool has_available_job();
    bool is_job_available(size_t jobId);
    void enqueue_available_jobs();
    void job_finished(size_t jobId, int64_t executionTime);
    size_t next_job();

private:
    JobGraph m_jobGraph;
    std::mutex m_queueMutex;
    std::condition_variable m_awakeWorker;
    std::vector<std::shared_ptr<Worker>> m_workers;
    std::queue<size_t> m_availableJobs;
    std::vector<JobProfiling> m_executionTimes;
};

}

#endif //BEAGLE_JOB_SYSTEM_H
