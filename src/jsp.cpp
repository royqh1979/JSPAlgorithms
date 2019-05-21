//
// Created by roy on 2019/5/20.
//
#include <jsp.h>
#include <iostream>
#include <string>


using namespace std;

JobManagerClass::JobManagerClass(ifstream dataFileStream) {
    string line;
    while( getline(dataFileStream,line)) {
        if (line.find("#") != 0) {
            break;
        }
    }
    if (dataFileStream.eof()) {
        return;
    }
    dataFileStream>>

}
OperationManagerClass OperationManager;
JobManagerClass JobManager;



