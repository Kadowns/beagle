//
// Created by Ricardo on 12/24/2021.
//

#include <stdexcept>
#include "job_graph.h"

using namespace beagle;

void Job::precede(Job other) {
    m_owner->precede(*this, other);
}

void Job::succeed(Job other) {
    m_owner->succeed(*this, other);
}

JobPredecessors Job::predecessors() {
    return m_owner->predecessors(*this);
}

JobSuccessors Job::successors() {
    return m_owner->successors(*this);
}

JobPredecessors Job::predecessors() const {
    return m_owner->predecessors(*this);
}

JobSuccessors Job::successors() const {
    return m_owner->successors(*this);
}

JobResult Job::execute(){
    return m_owner->execute(*this);
}

Job& Job::name(const std::string& name) {
    m_owner->name(*this, name);
    return *this;
}

const std::string& Job::name() const {
    return m_owner->name(*this);
}

Job JobGraph::JobIterator::operator*() {
    return Job(&m_owner, m_it.index());
}

void JobGraph::JobIterator::next() {
    if (m_it != m_owner.m_graph.end()){
        ++m_it;
    }
}

JobGraph::JobIterator &JobGraph::JobIterator::operator++() {
    next();
    return *this;
}

void JobGraph::precede(Job a, Job b) {
    m_graph.connect(a.m_index, b.m_index, JobRelation::PRECEDE);
}

void JobGraph::succeed(Job a, Job b) {
    precede(b, a);
}

JobPredecessors JobGraph::predecessors(const Job &job) {
    return JobPredecessors(*this, m_graph.edges_to(job.m_index));
}

JobSuccessors JobGraph::successors(const Job &job) {
    return JobSuccessors(*this, m_graph.edges_from(job.m_index));
}

Job JobGraph::at(size_t index) {
    return Job(this, index);
}

size_t JobGraph::size() const {
    return m_graph.size();
}

JobResult JobGraph::execute(const Job& job) {
    return m_graph.vertex(job.m_index).execute();
}

void JobGraph::name(const Job& job, const std::string& name) {
    m_graph.vertex(job.m_index).name(name);
}

const std::string& JobGraph::name(const Job& job) const {
    return m_graph.vertex(job.m_index).name();
}

