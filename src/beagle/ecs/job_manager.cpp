//
// Created by Ricardo on 4/11/2021.
//

#include "job_manager.h"

namespace beagle {

JobManager::Worker::Worker(JobManager* manager) : m_manager(manager) {
    m_thread = std::thread([this]{
       while(true) {
           std::unique_lock<std::mutex> lock(m_manager->m_queueMutex);
           m_manager->m_awakeWorker.wait(lock, [this] {
               if (m_stop){
                   return true;
               }
               if (!m_manager->has_available_job()){
                   return false;
               }
               m_currentJobIndex = m_manager->next_job();
               return true;
           });
           lock.unlock();
           if (m_stop) {
               return;
           }
           int64_t time = m_manager->m_jobGraph.vertex(m_currentJobIndex).m_job->timed_execute();
           m_manager->job_finished(m_currentJobIndex, time);
       }
    });
}

JobManager::Worker::~Worker() {
    m_thread.join();
}

void JobManager::Worker::stop() {
    m_stop = true;
}

JobManager::JobManager() {
    m_workers.resize(std::thread::hardware_concurrency());
    for (auto& worker : m_workers){
        worker = std::make_shared<Worker>(this);
    }
}

JobManager::~JobManager() {
    for (auto& worker : m_workers){
        worker->stop();
    }
    m_awakeWorker.notify_all();
}

void JobManager::execute() {

    m_executionTimes.clear();
    for (auto& vertex : m_jobGraph){
        vertex.set_executed(false);
        vertex.set_enqueued(false);
    }
    enqueue_available_jobs();

    m_awakeWorker.notify_all();
    std::unique_lock<std::mutex> lock(m_queueMutex);
    m_awakeWorker.wait(lock, [this]{
        return !has_unfinished_job();
    });
}

void JobManager::enqueue_available_jobs() {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    for (auto& jobVertex : m_jobGraph){
        if (is_job_available(jobVertex.m_index)){
            m_availableJobs.push(jobVertex.m_index);
            jobVertex.set_enqueued(true);
        }
    }
}

bool JobManager::is_job_available(size_t jobId) {
    auto& vertex = m_jobGraph.vertex(jobId);
    if (vertex.is_enqueued()){
        return false;
    }
    for (auto[connectionIndex, connectionRelation] : m_jobGraph.edges_from(vertex.m_index)){
        auto connectedJobVertex = m_jobGraph.vertex(connectionIndex);
        if (connectionRelation == JobRelation::RUN_AFTER && !connectedJobVertex.is_executed()){
            return false;
        }
    }
    return true;
}

void JobManager::add_dependency(size_t jobId, size_t dependencyId) {
    m_jobGraph.connect(dependencyId, jobId, JobRelation::RUN_BEFORE);
    m_jobGraph.connect(jobId, dependencyId, JobRelation::RUN_AFTER);
}

bool JobManager::has_available_job() {
    return !m_availableJobs.empty();
}

bool JobManager::has_unfinished_job() {
    return std::any_of(m_jobGraph.begin(), m_jobGraph.end(), [](const JobVertex& vertex){
        return !vertex.is_executed() && !vertex.is_disposable();
    });
}

void JobManager::job_finished(size_t jobId, int64_t executionTime) {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    auto& vertex = m_jobGraph.vertex(jobId);
    vertex.set_executed(true);
    m_executionTimes.emplace_back(JobProfiling(vertex.m_job->name(), executionTime));
    switch(vertex.m_job->result()){
        case JobResult::SUCCESS:
            enqueue_available_jobs_after(jobId);
            break;
        case JobResult::INTERRUPT:
            interrupt_jobs_after(jobId);
            break;
    }

    if (vertex.is_disposable()){
        vertex.m_job.reset();
        m_jobGraph.erase(jobId);
    }

    lock.unlock();
    m_awakeWorker.notify_all();
}

size_t JobManager::next_job() {
    auto id = m_availableJobs.front();
    m_availableJobs.pop();
    return id;
}

void JobManager::enqueue_available_jobs_after(size_t jobId) {
    for(auto[connectionIndex, connectionRelation] : m_jobGraph.edges_from(jobId)){
        if (connectionRelation != JobRelation::RUN_BEFORE){
            continue;
        }
        auto& connectionVertex = m_jobGraph.vertex(connectionIndex);
        if (is_job_available(connectionVertex.m_index)){
            m_availableJobs.push(connectionIndex);
        }
    }
}

void JobManager::interrupt_jobs_after(size_t jobId) {
    for(auto[connectionIndex, connectionRelation] : m_jobGraph.edges_from(jobId)){
        if (connectionRelation != JobRelation::RUN_BEFORE){
            continue;
        }
        auto& connectionVertex = m_jobGraph.vertex(connectionIndex);
        connectionVertex.set_executed(true);
        interrupt_jobs_after(connectionIndex);
    }
}

}