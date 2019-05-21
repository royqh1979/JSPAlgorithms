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
    JSPManagerClass jspManager("e:/abz5.txt");

    for (int i=0;i<jspManager.job_count();i++) {
        cout<<"Job "<<i<<endl;
        for (int j=0;j<jspManager.operation_count_in_job(i);j++) {
            const POperation op= jspManager.get_operation(i,j);
            cout<<"("<<op->id()<<","<<op->job_id()<<","<<op->machine_id()<<","<<op->duration()<<")";
        }
        cout<<endl;
    }


    return 0;
}

