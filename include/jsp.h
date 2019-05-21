//
// Created by roy on 2019/5/20.
//

#ifndef JSPALGORITHMS_JSP_H
#define JSPALGORITHMS_JSP_H

#include <vector>
#include <unordered_set>
#include <map>
#include <memory>
#include <algorithm>
#include <fstream>

class JSPManagerClass;

class Operation {
private:
    int _id;
    int _job_id;
    int _machine_id;
    double _duration;
    Operation(int id,int job_id,int machine_id,double duration):
        _id(id),_job_id(job_id),_machine_id(machine_id),_duration(duration){}
public:
    const int& id() const {return _id; }
    const int& job_id() const {return _job_id;}
    const int& machine_id() const { return _machine_id;}
    const double& duration() const {return _duration;}
    friend class JSPManagerClass;
};

struct Operation_hash{
    size_t operator()(const Operation& op) const {
        return std::hash<int>()(op.id());
    }
};

bool operator==(const Operation& first, const Operation& second){
    return first.id() == second.id();
}

class Job{
private:
    std::vector<Operation> _operations{};
    int _id;
    int addOperation(Operation op) {
        _operations.push_back(op);
    }

public:
    Job(int id): _id(id) {}

    const Operation& getOperation(int i) const {
        return _operations[i];
    }

    const int& getOperationPosition(Operation op) const {
        auto pos = std::find(_operations.cbegin(),_operations.cend(),op);
        if (pos == _operations.cend()){
            return -1;
        } else {
            return pos -_operations.cbegin();
        }
    }

    const int& operationCount() const {
        return _operations.size();
    }

    const int id() const{
        return _id;
    }

    friend class JSPManagerClass;
};


class JSPManagerClass {
private:
    int _job_count{0};
    int _machine_count{0};
    std::vector<Job> _jobs{};
    int _operation_counts{0};
public:
    JSPManagerClass(int job_count,int machine_count):_job_count(job_count),_machine_count(machine_count) {}

    Job& addJob(){
        int id=_jobs.size();
        _jobs.push_back(Job{id});
        return _jobs.back();
    }

    Job& getJob(int id) {
        return _jobs[id-1];
    }

    Operation& addOperation(int job_id,int machine_id,double duration) {
        int id = _operation_counts++;
        Operation op{id,job_id,machine_id,duration};
        getJob(job_id).addOperation(op);
        return op;
    }

    Job& getOperation(int job_id, int id){

    }




};

/**
 * Operation管理器
 */
class OperationManagerClass {
private:
    std::vector<Operation> _operations{};

public:
    /**
     * 创建一个新的operation
     * @param job_id job的id
     * @param machine_id machine的id
     * @param time 操作时间
     * @return operation的id
     */
    const Operation& addOperation(int job_id, int machine_id, float time) {
        int id = _operations.size();
        _operations.push_back(Operation{id,job_id,machine_id,time});
        return _operations.back();
    }

    int count() const {
        return _operations.size();
    }

    const Operation& getOperation(int id) const{
        return _operations[id];
    }
};



extern OperationManagerClass OperationManager;


class JobManagerClass{
private:
    std::vector<Job> _jobs{};
public:
    JobManagerClass(){}

    JobManagerClass(std::ifstream dataFileStream);

    Job& addJob() {
        int id=_jobs.size();
        _jobs.push_back(Job(id));
        return _jobs.back();
    }

    Job& getJob(int id) {
        return _jobs[id];
    }
};



extern JobManagerClass JobManager;



#endif //JSPALGORITHMS_JSP_H
