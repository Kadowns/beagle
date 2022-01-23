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

        JobResult execute() {
            return m_job();
        }

    private:
        std::function<JobResult()> m_job;
    };

public:

    //For now, there is only one type of one way relation
    enum JobRelation {
        PRECEDE,
    };

    typedef typename Graph<JobVertex, JobRelation>::GraphIterator GraphIterator;
    typedef typename Graph<JobVertex, JobRelation>::EdgesFromView EdgesFromView;
    typedef typename Graph<JobVertex, JobRelation>::EdgesToView EdgesToView;

    class JobIterator : public std::iterator<std::input_iterator_tag, size_t> {
    public:

        bool operator==(const JobIterator& rhs) const { return m_it == rhs.m_it; }

        bool operator!=(const JobIterator& rhs) const { return m_it != rhs.m_it; }

        JobIterator& operator++();

        Job operator*();

    private:
        JobIterator(JobGraph& owner, GraphIterator it) :
                m_owner(owner),
                m_it(it){

        }

        void next();

    private:
        friend class JobGraph;

        JobGraph& m_owner;
        GraphIterator m_it;
    };

    template<typename ViewType>
    class JobConnectionIterator : public std::iterator<std::input_iterator_tag, size_t> {
    public:

        bool operator==(const JobConnectionIterator<ViewType>& rhs) const { return m_it == rhs.m_it; }

        bool operator!=(const JobConnectionIterator<ViewType>& rhs) const { return m_it != rhs.m_it; }

        JobConnectionIterator<ViewType>& operator++();

        Job operator*();

    private:
        typedef typename ViewType::Iterator ViewIterator;

        JobConnectionIterator(const JobGraph& owner, const ViewType& view, ViewIterator it) :
            m_owner(owner),
            m_view(view),
            m_it(it){

        }

        void next();

    private:
        friend class JobConnectionView;

        const JobGraph& m_owner;
        const ViewType& m_view;
        ViewIterator m_it;
    };

    template<typename ViewType>
    class JobConnectionView {
    public:

        size_t size() const { return m_view.size(); }

        JobConnectionIterator<ViewType> begin() { return JobConnectionIterator<ViewType>(m_owner, m_view, m_view.begin()); }
        JobConnectionIterator<ViewType> end() { return JobConnectionIterator<ViewType>(m_owner, m_view, m_view.end()); }

        JobConnectionIterator<ViewType> begin() const { return JobConnectionIterator<ViewType>(m_owner, m_view, m_view.begin()); }
        JobConnectionIterator<ViewType> end() const { return JobConnectionIterator<ViewType>(m_owner, m_view, m_view.end()); }

    private:
        friend JobGraph;

        JobConnectionView(const JobGraph& owner, const ViewType& view) : m_owner(owner), m_view(view) {}

    private:
        const JobGraph& m_owner;
        ViewType m_view;
    };

    typedef JobConnectionView<EdgesToView> JobPredecessors;
    typedef JobConnectionView<EdgesFromView> JobSuccessors;

    template<typename F>
    Job emplace(F&& callable);

    template<typename ...F>
    std::tuple<F...> emplace(F&&... callables);

    inline void precede(Job a, Job b);

    inline void succeed(Job a, Job b);

    JobIterator begin() { return JobIterator(*this, m_graph.begin()); }
    JobIterator end() { return JobIterator(*this, m_graph.end()); }

    JobPredecessors predecessors(const Job& job);

    JobSuccessors successors(const Job& job);

    JobPredecessors predecessors(const Job& job) const;

    JobSuccessors successors(const Job& job) const;

    Job at(size_t index);

    size_t size() const;

    JobResult execute(const Job& job);

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

    JobPredecessors predecessors() const;

    JobSuccessors successors() const;

    JobResult execute();

    size_t id() const { return m_index; }

private:
    friend class JobGraph;
    JobGraph& m_owner;
    size_t m_index;
};

template<typename ViewType>
Job JobGraph::JobConnectionIterator<ViewType>::operator*() {
    return Job(m_owner, m_it->first);
}

template<typename ViewType>
void JobGraph::JobConnectionIterator<ViewType>::next() {
    if(m_it != m_view.end()){
        m_it++;
    }
}

template<typename ViewType>
JobGraph::JobConnectionIterator<ViewType>& JobGraph::JobConnectionIterator<ViewType>::operator++() {
    next();
    return *this;
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
