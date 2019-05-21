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
class Job;

class Operation {
private:
    int _id;
    int _job_id;
    int _machine_id;
    double _duration;
    int _index_in_job=0;
//    int _index_in_machine=0;
    Operation(int id,int job_id,int machine_id, double duration):
        _id(id),_job_id(job_id),_machine_id(machine_id),_duration(duration){
    }
    void set_index_in_job(int index_in_job) {
        _index_in_job = index_in_job;
    }
public:
    const int& id() const {return _id; }
    const int& job_id() const {return _job_id;}
    const int& machine_id() const { return _machine_id;}
    const double& duration() const {return _duration;}
    const int& index_in_job() const {return _index_in_job;}
//    const int& index_in_machine() const {return _index_in_job;}
//    void set_index_in_machine(int index_in_machine) {
//        _index_in_machine = index_in_machine;
//    }
    friend class JSPManagerClass;
    friend class Job;
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
    int addOperation(Operation& op) {
        int index_in_job = _operations.size();
        op.set_index_in_job(index_in_job);
        _operations.push_back(op);
    }
    Job(int id): _id(id) {}
public:

    const Operation& getOperation(int i) const {
        return _operations[i];
    }

    const int& operationCount() const {
        return _operations.size();
    }

    const int& id() const{
        return _id;
    }

    friend class JSPManagerClass;
};

class Machine{
private:
    std::unordered_set<int> _operations{};
    int _id;
    Machine(int id): _id(id){

    }
    void addOperation(const Operation& op) {
        _operations.insert(op.id());
    }
public:
    bool is_operation_in_machine(int id) const {
        return _operations.find(id)!=_operations.end();
    }

    const int& id() const{
        return _id;
    }
    friend class JSPManagerClass;
};


class JSPManagerClass {
private:
    int _job_count{0};
    int _machine_count{0};
    std::vector<Job> _jobs{};
    std::vector<Machine> _machines{};
    std::vector<Operation*> _operations{0};
    void createJob(){
        int id=_jobs.size();
        _jobs.push_back(Job{id});
    }
    void createMachine() {
        int id=_machines.size();
        _machines.push_back(Machine{id});
    }
public:
    JSPManagerClass(int job_count,int machine_count);

    const Job& getJob(int id) const{
        return _jobs[id];
    }

    const Machine& getMachine(int id) const {
        return _machines[id];
    }

    Operation& addOperation(int job_id,int machine_id,double duration) {
        if (machine_id>=_machine_count) {
            throw std::length_error{"wrong machine number!"};
        }
        int id = _operations.size();
        Operation op{id,job_id,machine_id,duration};
        _jobs[job_id].addOperation(op);
        _operations.push_back(&op);
        return op;
    }

    const Operation& getOperation(int i, int j) const{
        return _jobs[i].getOperation(j);
    }

    const

    const Operation& getOperation(int id) const {
        return *(_operations[id]);
    }

};


#endif //JSPALGORITHMS_JSP_H
