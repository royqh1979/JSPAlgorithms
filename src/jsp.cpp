#include <jsp.h>
//
// Created by roy on 2019/5/20.
//
#include <jsp.h>
#include <iostream>
#include <string>


using namespace std;

JSPManagerClass::JSPManagerClass(int job_count,int machine_count):_job_count(job_count),_machine_count(machine_count) {
    for (int i=0;i<job_count;i++) {
        createJob();
    }
    for (int i=0;i<machine_count;i++) {
        createMachine();
    }
}
OperationManagerClass OperationManager;
JobManagerClass JobManager;



