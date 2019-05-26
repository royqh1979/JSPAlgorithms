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
    JSPManager jspManager("e:/abz5.txt");

   // jspManager.print_jobs();
   // jspManager.print_machines();

    JSPGraph graph(jspManager);
    return 0;
}

