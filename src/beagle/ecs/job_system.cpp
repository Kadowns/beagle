//
// Created by Ricardo on 4/11/2021.
//

#include "job_system.h"

namespace beagle {

void JobSystem::Job::execute() {
    m_task();
}

JobSystem::Worker::Worker(JobSystem* manager) : m_manager(manager) {
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
           m_manager->m_jobGraph.vertex(m_currentJobIndex)->job->execute();
           m_manager->job_finished(m_currentJobIndex);
       }
    });
}

JobSystem::Worker::~Worker() {
    m_thread.join();
}

void JobSystem::Worker::stop() {
    m_stop = true;
}

JobSystem::JobSystem() {
    m_workers.resize(std::thread::hardware_concurrency());
    for (auto& worker : m_workers){
        worker = std::make_shared<Worker>(this);
    }
}

JobSystem::~JobSystem() {
    for (auto& worker : m_workers){
        worker->stop();
    }
    m_awakeWorker.notify_all();
}

size_t JobSystem::add_job(std::function<void()>&& task) {
    JobVertex vertex = {std::make_shared<Job>(std::move(task)), m_jobGraph.size()};
    m_jobGraph.push(vertex);
    return vertex.index;
}

void JobSystem::execute() {

    for (auto vertex : m_jobGraph){
        vertex->executed = false;
        vertex->enqueued = false;
    }
    enqueue_available_jobs();

    m_awakeWorker.notify_all();
    std::unique_lock<std::mutex> lock(m_queueMutex);
    m_awakeWorker.wait(lock, [this]{
        return !has_unfinished_job();
    });
}

void JobSystem::enqueue_available_jobs() {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    for (auto jobVertex : m_jobGraph){
        if (is_job_available(jobVertex->index)){
            m_availableJobs.push(jobVertex->index);
            jobVertex->enqueued = true;
        }
    }
}

bool JobSystem::is_job_available(size_t jobId) {
    auto vertex = m_jobGraph.vertex(jobId);
    if (vertex->enqueued){
        return false;
    }
    for (auto[connectionIndex, connectionRelation] : m_jobGraph.edges_from(vertex->index)){
        auto connectedJobVertex = m_jobGraph.vertex(connectionIndex);
        if (connectionRelation == JobRelation::DEPENDS_ON && !connectedJobVertex->executed){
            return false;
        }
    }
    return true;
}

void JobSystem::add_dependency(size_t jobId, size_t dependencyId) {
    m_jobGraph.connect(dependencyId, jobId, JobRelation::DEPENDENCY_OF);
    m_jobGraph.connect(jobId, dependencyId, JobRelation::DEPENDS_ON);
}

bool JobSystem::has_available_job() {
    return !m_availableJobs.empty();
}

bool JobSystem::has_unfinished_job() {
    return std::any_of(m_jobGraph.begin(), m_jobGraph.end(), [](JobVertex* vertex){return !vertex->executed;});
}

void JobSystem::job_finished(size_t jobId) {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    m_jobGraph.vertex(jobId)->executed = true;
    for(auto[connectionIndex, connectionRelation] : m_jobGraph.edges_from(jobId)){
        if (connectionRelation != JobRelation::DEPENDENCY_OF){
            continue;
        }
        auto connectionVertex = m_jobGraph.vertex(connectionIndex);
        if (is_job_available(connectionVertex->index)){
            m_availableJobs.push(connectionIndex);
        }
    }
    lock.unlock();
    m_awakeWorker.notify_all();
}

size_t JobSystem::next_job() {
    auto id = m_availableJobs.front();
    m_availableJobs.pop();
    return id;
}

}