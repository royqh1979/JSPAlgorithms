//
// Taboo Search
//
// Taillard, É. D. (1994). "Parallel Taboo Search Techniques for the Job Shop Scheduling Problem." ORSA Journal on Computing 6(2): 108-117.
// Created by roy on 2019/5/16.
//

#include "jsp.h"
#include <iostream>
using namespace std;

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


    return 0;
}

