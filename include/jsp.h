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
#include <iostream>

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
    int id() const {return _id; }
    int job_id() const {return _job_id;}
    int machine_id() const { return _machine_id;}
    double duration() const {return _duration;}
    int index_in_job() const {return _index_in_job;}
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

inline bool operator==(const Operation& first, const Operation& second){
    return first.id() == second.id();
}

using POperation = std::shared_ptr<Operation>;
class Job{
private:
    std::vector<POperation> _operations{};
    int _id;
    void add_operation(POperation pop) {
        int index_in_job = _operations.size();
        pop->set_index_in_job(index_in_job);
        _operations.push_back(pop);
    }
    Job(int id): _id(id) {}
public:

    const POperation get_operation(int i) const {
        return _operations[i];
    }

    int operation_count() const {
        return _operations.size();
    }

    int id() const{
        return _id;
    }

    ~Job() {
        for (auto pop:_operations) {
            pop.reset();
        }
        _operations.clear();
    }

    friend class JSPManagerClass;
};

class Machine{
private:
    std::unordered_set<int> _operations{};
    int _id;
    Machine(int id): _id(id){

    }
    void add_operation(const Operation &op) {
        _operations.insert(op.id());
    }
public:
    bool is_operation_in_machine(int id) const {
        return _operations.find(id)!=_operations.end();
    }

    int operation_count() const {
        return _operations.size();
    }


    int id() const{
        return _id;
    }

    ~Machine(){
        _operations.clear();
    }
    friend class JSPManagerClass;
};


class JSPManagerClass {
private:
    int _job_count{0};
    int _machine_count{0};
    std::vector<Job> _jobs{};
    std::vector<Machine> _machines{};
    std::vector<POperation> _operations{};
    void create_job(){
        int id=_jobs.size();
        _jobs.push_back(Job{id});
    }
    void create_machine() {
        int id=_machines.size();
        _machines.push_back(Machine{id});
    }
public:
    JSPManagerClass(int job_count,int machine_count);

    JSPManagerClass(std::string jsp_data_filename);

    const Job& get_job(int id) const{
        return _jobs[id];
    }

    const Machine& get_machine(int id) const {
        return _machines[id];
    }

    POperation add_operation(int job_id, int machine_id, double duration) {
        if (machine_id>=_machine_count) {
            throw std::length_error{"wrong machine number!"};
        }
        int id = _operations.size();
        POperation op(new Operation(id,job_id,machine_id,duration));
        _operations.push_back(op);
        _jobs[job_id].add_operation(op);

        return op;
    }

    const POperation get_operation(int i, int j) const{
        return _jobs[i].get_operation(j);
    }

    int job_count() const {
        return _job_count;
    }

    int operation_count() const {
        return _operations.size();
    }

    int operation_count_in_job(int id) const {
        return get_job(id).operation_count();
    }

    int machine_count() const {
        return _machine_count;
    }

    const POperation get_operation(int id) const {
        return _operations[id];
    }

    ~JSPManagerClass(){
        for (auto pop:_operations) {
            pop.reset();
        }
        _operations.clear();
        _jobs.clear();
        _machines.clear();
    }

};


#endif //JSPALGORITHMS_JSP_H
