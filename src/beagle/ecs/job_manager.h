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
#include <eagle/memory/pointer.h>

#include <beagle/utils/graph.h>

namespace beagle {


enum class JobResult {
    SUCCESS = 0,
    INTERRUPT = 1
};

class BaseJob {
public:
    explicit BaseJob(const std::string& name) : m_name(name) {}
    virtual ~BaseJob() = default;

    uint64_t timed_execute() {
        m_timer.start();
        m_result = execute();
        m_timer.stop();
        return m_timer.time_nano();
    }

    virtual JobResult execute() = 0;

    JobResult result() const { return m_result; }
    const std::string& name() { return m_name; }

private:
    eagle::Timer m_timer;
    std::string m_name;
    JobResult m_result;
};

class Job : public BaseJob {
public:
    explicit Job(std::function<JobResult()>&& task, const std::string& name = "Lambda") : BaseJob(name), m_task(std::move(task)) {}
    JobResult execute() override {
        return m_task();
    }
private:
    std::function<JobResult()> m_task;
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
private:
    class JobVertex {
    public:
        JobVertex() = default;

    private:
        friend JobManager;
        friend Worker;

        JobVertex(JobManager* manager, eagle::StrongPointer<BaseJob>&& job) :
                m_manager(manager), m_job(std::forward<eagle::StrongPointer<BaseJob>>(job)) {}

        bool is_enqueued() const { return m_enqueued; }
        bool is_executed() const { return m_executed; }
        bool is_disposable() const { return m_disposable; }

        void set_enqueued(bool value) { m_enqueued = value; }
        void set_executed(bool value) { m_executed = value; }

    private:
        JobManager* m_manager = nullptr;
        eagle::StrongPointer<BaseJob> m_job;
        size_t m_index = 0;
        bool m_enqueued = false;
        bool m_executed = false;
        bool m_disposable = false;
    };

    enum class JobRelation {
        RUN_BEFORE,
        RUN_AFTER
    };

    using JobGraph = Graph<JobVertex, JobRelation>;

    struct JobProfiling {
        JobProfiling(const std::string& name, int64_t durationNano) :
        name(name), durationNano(durationNano) {}

        const std::string& name;
        int64_t durationNano;
    };
public:

    class JobHandle {
    public:
        JobHandle() = default;
        JobHandle(size_t index, JobManager* manager) : m_index(index), m_manager(manager) {}

        JobVertex& operator*(){
            return m_manager->m_jobGraph.vertex(m_index);
        }

        JobVertex& operator->(){
            return this->operator*();
        }

        JobVertex& operator*() const {
            return m_manager->m_jobGraph.vertex(m_index);
        }

        JobVertex& operator->() const {
            return this->operator*();
        }

        void run_after(const JobHandle& other){
            m_manager->add_dependency(m_index, other.m_index);
        }

        void run_before(const JobHandle& other){
            m_manager->add_dependency(other.m_index, m_index);
        }

    private:
        size_t m_index = 0;
        JobManager* m_manager = nullptr;
    };

public:

    JobManager();
    ~JobManager();

    template<typename TJob, typename ...Args>
    JobHandle enqueue(Args&& ...args) {
        size_t index = m_jobGraph.insert(JobVertex(this, eagle::make_strong<TJob>(std::forward<Args>(args)...)));
        auto& handle = m_jobGraph.vertex(index);
        handle.m_index = index;
        return {index, this};
    }

    template<typename TJob, typename ...Args>
    JobHandle dispose(Args&& ...args) {
        auto handle = enqueue<TJob, Args...>(std::forward<Args>(args)...);
        handle->m_disposable = true;
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
    void enqueue_available_jobs_after(size_t jobId);
    void interrupt_jobs_after(size_t jobId);
    void job_finished(size_t jobId, int64_t executionTime);
    size_t next_job();

private:
    JobGraph m_jobGraph;
    std::mutex m_queueMutex;
    std::condition_variable m_awakeWorker;
    std::vector<eagle::StrongPointer<Worker>> m_workers;
    std::queue<size_t> m_availableJobs;
    std::vector<JobProfiling> m_executionTimes;
};

}

#endif //BEAGLE_JOB_SYSTEM_H
