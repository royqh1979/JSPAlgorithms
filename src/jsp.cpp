#include <jsp.h>
//
// Created by roy on 2019/5/20.
//
#include <jsp.h>
#include <iostream>
#include <string>
#include <sstream>
#include <boost/algorithm/string/trim.hpp>


using namespace std;

JSPManagerClass::JSPManagerClass(int job_count,int machine_count):_job_count(job_count),_machine_count(machine_count) {
    for (int i=0;i<job_count;i++) {
        create_job();
    }
    for (int i=0;i<machine_count;i++) {
        create_machine();
    }
}

JSPManagerClass::JSPManagerClass(string jsp_data_filename){
    ifstream fs(jsp_data_filename);
    string line;
    while(getline(fs,line)) {
        boost::algorithm::trim(line);
        if (line.find('#')!=0) {
            break;
        }
    }
    istringstream is(line);
    is>>_job_count>>_machine_count;

    for (int i=0;i<_job_count;i++) {
        create_job();
    }
    for (int i=0;i<_machine_count;i++) {
        create_machine();
    }

    for (int i=0;i<_job_count;i++) {
        Job& job=_jobs[i];
        string line1;
        getline(fs,line1);
        istringstream is1(line1);
        while (!is1.eof()) {
            int machine_id;
            double duration;
            is1>>machine_id>>duration;
            add_operation(i, machine_id, duration);
        }
    }

    fs.close();
}



