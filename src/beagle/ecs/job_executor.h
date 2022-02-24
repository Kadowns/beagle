//
// Created by Ricardo on 14/01/22.
//

#ifndef BEAGLE_JOB_EXECUTOR_H
#define BEAGLE_JOB_EXECUTOR_H

#include <beagle/ecs/thread_pool.h>
#include <beagle/ecs/job_graph.h>
#include <eagle/log.h>

namespace beagle {

class JobExecutor {
public:
    class JobExecution {
    public:

        explicit JobExecution(JobGraph& graph) : m_graph(graph) {
            enqueue_available_jobs();
        }

        Job next(){
            std::lock_guard<std::mutex> lock(m_queueMutex);
            auto id = m_availableJobs.front();
            m_availableJobs.pop();
            return m_graph.at(id);
        }

        void complete(Job job, JobResult result){
            std::lock_guard<std::mutex> lock(m_queueMutex);
            switch(result){
                case JobResult::SUCCESS:
                    success(job);
                    break;
                case JobResult::INTERRUPT:
                    interrupt(job);
                    break;
            }
        }

        bool has_available_job() const {
            return !m_availableJobs.empty();
        }

        bool is_complete(const Job& job){
            return m_completedJobs.find(job.id()) != m_completedJobs.end();
        }

        bool is_complete(){
            return m_graph.size() == m_completedJobs.size();
        }

        std::mutex& mutex() { return m_queueMutex; }

    private:
        void enqueue_available_jobs(){
            for(auto job : m_graph){
                if (is_available(job)){
                    m_availableJobs.push(job.id());
                }
            }
        }

        void interrupt(const Job& job){
            m_completedJobs.insert(job.id());
            for (auto successor : job.successors()){
                interrupt(successor);
            }
        }

        void success(const Job& job){
            m_completedJobs.insert(job.id());
            for (auto successor : job.successors()){
                if (is_available(successor)){
                    auto id = successor.id();
                    m_availableJobs.push(id);
                }
            }
        }

        bool is_available(const Job& job){
            for (auto predecessor : job.predecessors()){
                if (!is_complete(predecessor)){
                    return false;
                }
            }
            return true;
        }

    private:
        JobGraph& m_graph;
        std::queue<size_t> m_availableJobs;
        std::set<size_t> m_completedJobs;
        std::mutex m_queueMutex;
    };

public:
    explicit JobExecutor(ThreadPool& threadPool) : m_threadPool(threadPool) {}


    void execute(JobGraph& graph){

        JobExecution execution(graph);
        std::condition_variable jobCompleted;
        std::unique_lock<std::mutex> lock(execution.mutex());
        while (!execution.is_complete()){

            jobCompleted.wait(lock, [&execution]{
                return execution.has_available_job() || execution.is_complete();
            });

            if (execution.is_complete()){
                break;
            }
            lock.unlock();
            auto job = execution.next();
            EG_TRACE("beagle", "Executing: {0}", job.name());
            m_threadPool.execute([&execution, &jobCompleted, job]() mutable {
                auto result = job.execute();
                execution.complete(job, result);
                jobCompleted.notify_all();
            });
            lock.lock();
        }
    }

private:
    ThreadPool& m_threadPool;
};

}

#endif //BEAGLE_JOB_EXECUTOR_H
