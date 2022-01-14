//
// Created by Ricardo on 12/24/2021.
//

#ifndef BEAGLE_JOB_GRAPH_H
#define BEAGLE_JOB_GRAPH_H

#include <beagle/utils/graph.h>
#include <eagle/memory/pointer.h>

namespace beagle {

enum class JobResult {
    SUCCESS = 0,
    INTERRUPT = 1
};


class Job;


class JobGraph {
private:

    class JobVertex {
    public:
        explicit JobVertex(std::function<JobResult()> job) : m_job(std::move(job)) {}

    private:
        std::function<JobResult()> m_job;
    };

public:

    enum JobRelation {
        BEFORE,
        AFTER
    };

    typedef typename Graph<JobVertex, JobRelation>::GraphIterator Iterator;
    typedef typename Graph<JobVertex, JobRelation>::EdgesFromView EdgesView;

    template<JobRelation R>
    class JobRelationIterator : public std::iterator<std::input_iterator_tag, size_t> {
    public:

        JobRelationIterator& operator++(){
            m_it++;
            next();
            return *this;
        }

        Job operator*();

    private:
        JobRelationIterator(JobGraph& owner, EdgesView& view, EdgesView::ConstIterator it) :
            m_owner(owner),
            m_view(view),
            m_it(it){

        }

        void next(){
            while(m_it != m_view.end() && m_it->second != R){
                m_it++;
            }
        }

    private:
        template<JobRelation R>
        friend class JobRelationView;

        JobGraph& m_owner;
        EdgesView& m_view;
        EdgesView::ConstIterator m_it;
    };

    template<JobRelation R>
    class JobRelationView {
    public:

        JobRelationIterator<R> begin() { return JobRelationIterator<R>(m_owner, m_view, m_view.begin()); }
        JobRelationIterator<R> end() { return JobRelationIterator<R>(m_owner, m_view, m_view.end()); }

        JobRelationIterator<R> begin() const { return JobRelationIterator<R>(m_owner, m_view, m_view.begin()); }
        JobRelationIterator<R> end() const { return JobRelationIterator<R>(m_owner, m_view, m_view.end()); }

    private:
        friend JobGraph;

        JobRelationView(JobGraph& owner, const EdgesView& view) : m_owner(owner), m_view(view) {}

    private:
        JobGraph& m_owner;
        EdgesView m_view;
    };

    typedef JobRelationView<JobRelation::AFTER> JobPredecessors;
    typedef JobRelationView<JobRelation::BEFORE> JobSuccessors;

    template<typename F>
    Job emplace(F&& callable);

    template<typename ...F>
    std::tuple<F...> emplace(F&&... callables);

    inline void precede(Job a, Job b);

    inline void succeed(Job a, Job b);

    Iterator begin() { return m_graph.begin(); }
    Iterator end() { return m_graph.end(); }

    Iterator begin() const { return m_graph.begin(); }
    Iterator end() const { return m_graph.end(); }

    JobPredecessors predecessors(const Job& job);

    JobSuccessors successors(const Job& job);

private:

    Graph<JobVertex, JobRelation> m_graph;
};

typedef JobGraph::JobRelation JobRelation;
typedef JobGraph::JobPredecessors JobPredecessors;
typedef JobGraph::JobSuccessors JobSuccessors;

class Job {
public:
    explicit Job(JobGraph& owner, size_t index) : m_owner(owner), m_index(index) {}

    void precede(Job other);

    void succeed(Job other);

    JobPredecessors predecessors();

    JobSuccessors successors();

private:
    friend class JobGraph;
    JobGraph& m_owner;
    size_t m_index;
};

template<JobGraph::JobRelation R>
Job JobGraph::JobRelationIterator<R>::operator*() {
    return Job(m_owner, m_it->first);
}

template<typename F>
Job JobGraph::emplace(F&& callable) {
    return Job(*this, m_graph.insert(JobVertex(callable)));
}

template<typename... F>
std::tuple<F...> JobGraph::emplace(F&&... callables) {
    return std::make_tuple(emplace<F>(callables)...);
}

}

#endif //BEAGLE_JOB_GRAPH_H
