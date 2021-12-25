//
// Created by Ricardo on 12/24/2021.
//

#ifndef BEAGLE_JOB_GRAPH_H
#define BEAGLE_JOB_GRAPH_H

#include <beagle/utils/graph.h>

namespace beagle {

enum class JobResult {
    SUCCESS = 0,
    INTERRUPT = 1
};

class BaseJob {
public:
    virtual JobResult execute() = 0;
private:

};

class JobGraph {
private:

    class JobVertex {

    };

    enum JobRelation {

    };

public:

private:

    Graph<JobVertex, JobRelation> m_graph;


};

}

#endif //BEAGLE_JOB_GRAPH_H
