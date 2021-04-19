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

class JobSystem {
private:
    class Worker;
public:
    class Job {
    public:
        explicit Job(std::function<void()>&& task) :
                m_task(std::move(task)) {}

    private:
        friend Worker;
        friend JobSystem;

        void execute();
    private:
        std::function<void()> m_task;
    };
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

    struct JobVertex {
        std::shared_ptr<Job> job;
        size_t index;
        bool enqueued = false;
        bool executed = false;
    };

    enum class JobRelation {
        DEPENDENCY_OF,
        DEPENDS_ON
    };

    using JobGraph = gpp::AdjacencyList<JobVertex, JobRelation>;

public:

    JobSystem();
    ~JobSystem();

    size_t add_job(std::function<void()>&& task);
    void add_dependency(size_t jobId, size_t dependencyId);
    void execute();

private:
    friend Worker;
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
