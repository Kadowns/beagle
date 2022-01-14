//
// Created by Ricardo on 12/24/2021.
//

#include "job_graph.h"

using namespace beagle;

void Job::precede(Job other) {
    m_owner.precede(*this, other);
}

void Job::succeed(Job other) {
    m_owner.succeed(*this, other);
}

JobGraph::JobRelationView<JobGraph::JobRelation::AFTER> Job::predecessors() {
    return m_owner.predecessors(*this);
}

JobGraph::JobRelationView<JobGraph::JobRelation::BEFORE> Job::successors() {
    return m_owner.successors(*this);
}

void JobGraph::precede(Job a, Job b) {
    m_graph.connect(a.m_index, b.m_index, JobRelation::BEFORE);
    m_graph.connect(b.m_index, a.m_index, JobRelation::AFTER);
}

void JobGraph::succeed(Job a, Job b) {
    precede(b, a);
}

JobPredecessors JobGraph::predecessors(const Job &job) {
    return JobPredecessors(*this, m_graph.edges_from(job.m_index));
}

JobSuccessors JobGraph::successors(const Job &job) {
    return JobSuccessors(*this, m_graph.edges_from(job.m_index));
}
