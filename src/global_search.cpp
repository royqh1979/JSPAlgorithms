//
// Taboo Search
//
// Taillard, É. D. (1994). "Parallel Taboo Search Techniques for the Job Shop Scheduling Problem." ORSA Journal on Computing 6(2): 108-117.
// Created by roy on 2019/5/16.
//

#include "jsp.h"
#include <iostream>
using namespace std;

struct StepInfo {
    vector<int> candidate_nodes{};
    vector<int> job_last_finished_index;
    vector<int> machine_last_finished_index;
};

using PStepInfo = shared_ptr<StepInfo>;

void calc_candiate_nodes(JSPSearchGraph& graph,const vector<int> finised_nodes, PStepInfo& stepInfo) {
    if (finised_nodes.empty()) {
        for (int i=0;i<graph.problem().job_count();i++) {
            stepInfo->job_last_finished_index.push_back(-1);
        }
        for (int i=0;i<graph.problem().machine_count();i++) {
            stepInfo->machine_last_finished_index.push_back(-1);
        }
        stepInfo->candidate_nodes.insert(stepInfo->candidate_nodes.end(),graph.start_nodes().begin(),graph.start_nodes().end());
    }else{

    }
}

int main() {
    // optimum 655
    JSPProblem problem("e:/la02.txt");

    problem.print_jobs();
    problem.print_machines();

    JSPSearchGraph graph(problem);

    graph.generate_image("f:/test.png");

    graph.recalculate_earliest_times();
    graph.generate_image("f:/test2.png");

    int short_time=graph.shortest_time();

    vector<int> finished_nodes{};
    vector<PStepInfo> stepInfos{};

    int depth = 0;
    stepInfos.push_back(PStepInfo(new StepInfo{}));
    PStepInfo& stepInfo = stepInfos.back();




    return 0;
}

