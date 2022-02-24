//
// Created by Ricardo on 12/24/2021.
//

#ifndef BEAGLE_JOB_GRAPH_H
#define BEAGLE_JOB_GRAPH_H

#include <beagle/utils/graph.h>
#include <eagle/memory/pointer.h>
#include <functional>
#include <string>
#include <memory>

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

        void name(const std::string& name) {
            m_name = name;
        }

        const std::string& name() const{
            return m_name;
        }

    private:
        std::function<JobResult()> m_job;
        std::string m_name;
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
    class JobConnectionView;

    template<typename ViewType>
    class JobConnectionIterator : public std::iterator<std::input_iterator_tag, size_t> {
    public:

        bool operator==(const JobConnectionIterator<ViewType>& rhs) const { return m_it == rhs.m_it; }

        bool operator!=(const JobConnectionIterator<ViewType>& rhs) const { return m_it != rhs.m_it; }

        JobConnectionIterator<ViewType>& operator++();

        Job operator*();

    private:
        typedef typename ViewType::Iterator ViewIterator;

        JobConnectionIterator(JobGraph& owner, ViewType& view, ViewIterator it) :
            m_owner(owner),
            m_view(view),
            m_it(it){

        }

        void next();

    private:
        friend class JobConnectionView<ViewType>;

        JobGraph& m_owner;
        ViewType& m_view;
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

        JobConnectionView(JobGraph& owner, const ViewType& view) : m_owner(owner), m_view(view) {}

    private:
        JobGraph& m_owner;
        ViewType m_view;
    };

    typedef JobConnectionView<EdgesToView> JobPredecessors;
    typedef JobConnectionView<EdgesFromView> JobSuccessors;

    template<typename F, typename ...Args>
    Job emplace(Args&&... args);

    inline void precede(Job a, Job b);

    inline void succeed(Job a, Job b);

    JobIterator begin() { return JobIterator(*this, m_graph.begin()); }
    JobIterator end() { return JobIterator(*this, m_graph.end()); }

    JobPredecessors predecessors(const Job& job);

    JobSuccessors successors(const Job& job);

    Job at(size_t index);

    void name(const Job& job, const std::string& name);

    const std::string& name(const Job& job) const;

    size_t size() const;

    JobResult execute(const Job& job);

private:

    Graph<JobVertex, JobRelation> m_graph;
};

typedef JobGraph::JobPredecessors JobPredecessors;
typedef JobGraph::JobSuccessors JobSuccessors;

class Job {
public:
    Job() : m_owner(nullptr), m_index(~0UL) {}

    explicit Job(JobGraph* owner, size_t index) : m_owner(owner), m_index(index) {}

    void precede(Job other);

    void succeed(Job other);

    Job& name(const std::string& name);

    const std::string& name() const;

    JobPredecessors predecessors();

    JobSuccessors successors();

    JobPredecessors predecessors() const;

    JobSuccessors successors() const;

    JobResult execute();

    size_t id() const { return m_index; }

private:
    friend class JobGraph;
    JobGraph* m_owner;
    size_t m_index;
};

class CallableJob {
public:

    explicit CallableJob(std::function<JobResult()> callable) : m_callable(std::move(callable)) {}

    JobResult operator()(){
        return m_callable();
    }

private:
    std::function<JobResult()> m_callable;
};

template<typename ViewType>
Job JobGraph::JobConnectionIterator<ViewType>::operator*() {
    auto pair = *m_it;
    return Job(&m_owner, pair.first);
}

template<typename ViewType>
void JobGraph::JobConnectionIterator<ViewType>::next() {
    if(m_it != m_view.end()){
        ++m_it;
    }
}

template<typename ViewType>
JobGraph::JobConnectionIterator<ViewType>& JobGraph::JobConnectionIterator<ViewType>::operator++() {
    next();
    return *this;
}

template<typename F, typename ...Args>
Job JobGraph::emplace(Args&&... args) {
    return Job(this, m_graph.insert(JobVertex([wrapper = std::make_shared<F>(std::forward<Args>(args)...)]{
        return (*wrapper)();
    })));
}

}

#endif //BEAGLE_JOB_GRAPH_H
