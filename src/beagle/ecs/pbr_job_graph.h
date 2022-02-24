//
// Created by Ricardo on 1/23/2022.
//

#ifndef BEAGLE_PBRJOBGRAPH_H
#define BEAGLE_PBRJOBGRAPH_H

#include <beagle/engine.h>
#include <beagle/ecs/job_graph.h>

namespace beagle {

struct PBRJobGraph {
    explicit PBRJobGraph(Engine* engine);

    JobGraph graph;

    Job updateTransformJob;

    Job beginRenderJob;
    Job endRenderJob;

    Job skyboxUpdateVertexUboJob;
    Job skyboxRenderJob;

    Job meshUpdateVertexUboJob;
    Job meshUpdateInstanceBufferJob;
    Job meshUpdateFragmentUboJob;
    Job meshRenderJob;

    Job cameraUpdatePerspectiveProjectionJob;
    Job cameraRenderJob;
};

}

#endif //BEAGLE_PBRJOBGRAPH_H
