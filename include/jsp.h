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

class JSPManager;
class Job;



/**
 * The Operation Class
 * An operation is the minimal work unit used in JSSP problems.
 * A Job is composed of some operations.
 * Each operation must be processed on the specified machine.
 */
class Operation {
private:
    int _id;
    int _job_id;
    int _machine_id;
    double _duration;
    int _index_in_job=0;
//    int _index_in_machine=0;
    /**
     * The consturctor
     *
     * @param id  the id of the operation.It must be unique in one JSP problem.
     * @param job_id  the id of the job to which the operation belongs.
     * @param machine_id the id of the machine that processed the operation.
     * @param duration the processing time of the operation
     */
    Operation(int id,int job_id,int machine_id, double duration):
        _id(id),_job_id(job_id),_machine_id(machine_id),_duration(duration){
    }

    /**
     * Set the operation's index in the job
     *
     * The operation is the (index_in_job+1)th operation in the job.
     * So if the operation is the 5th in the job, its index_in_job should be 4.
     *
     * @param index_in_job the operation's index in the job
     */
    void set_index_in_job(int index_in_job) {
        _index_in_job = index_in_job;
    }
public:
    /**
     * Get the operations'id
     * @return
     */
    int id() const {return _id; }
    /**
     * Get the id of the job to which the operation belongs.
     * @return
     */
    int job_id() const {return _job_id;}
    /**
     * Get the id of the machine that processed the operation.
     * @return
     */
    int machine_id() const { return _machine_id;}
    /**
     * Get the processing time of the operation
     * @return
     */
    double duration() const {return _duration;}
    /**
     * Get the operation's index in the job
     * @return
     */
    int index_in_job() const {return _index_in_job;}
//    const int& index_in_machine() const {return _index_in_job;}
//    void set_index_in_machine(int index_in_machine) {
//        _index_in_machine = index_in_machine;
//    }
    friend class JSPManager;
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

struct POperation_hash{
    size_t operator()(const POperation& op) const {
        return std::hash<int>()(op->id());
    }
};

inline bool operator==(const POperation& first, const POperation& second){
    return first->id() == second->id();
}

using POperationSet = std::unordered_set<POperation, POperation_hash>;

class Job{
private:
    std::vector<POperation> _operations{};
    int _id;
    void add_operation(POperation& pop) {
        int index_in_job = _operations.size();
        pop->set_index_in_job(index_in_job);
        _operations.push_back(pop);
    }
    Job(int id): _id(id) {}
public:

    const POperation& get_operation(int i) const {
        return _operations[i];
    }

    int operation_count() const {
        return _operations.size();
    }

    int id() const{
        return _id;
    }

    friend class JSPManager;
};

class Machine{
private:
    POperationSet  _operations{};
    int _id;
    Machine(int id): _id(id){

    }
    void add_operation(const POperation &op) {
        _operations.insert(op);
    }
public:

    int operation_count() const {
        return _operations.size();
    }

    const POperationSet& opertions() const {
        return _operations;
    }

    int id() const{
        return _id;
    }


    friend class JSPManager;
};


class JSPManager {
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
    JSPManager(int job_count,int machine_count);

    JSPManager(std::string jsp_data_filename);

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
        _machines[machine_id].add_operation(op);

        return op;
    }

    const POperation& get_operation(int i, int j) const{
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

    const POperation& get_operation(int id) const {
        return _operations[id];
    }

    void print_jobs() const;
    void print_machines() const;
};

struct JSPGraphNode {
    int id;
    POperation pop;
    std::unordered_set<int> precedents;
};

using PJSPGraphNode = std::shared_ptr<JSPGraphNode>;

using JSPGraphNodeSet = std::unordered_set<int>;

using MachineOperationOrder = std::vector<int>;

class JSPGraph{
private:
    const JSPManager& _manager;
    std::vector<PJSPGraphNode> _nodes{};

    std::vector<JSPGraphNodeSet> _succeeds_list{}; // remember each vertice's succeedings ( record prev ->next)
    std::vector<JSPGraphNodeSet> _previous_list{};  // remember each vertice's previous ( record next->prev)
    std::vector<MachineOperationOrder> _machine_operation_orders{};

    std::vector<int> _operation_node_ids; // aux list to remember operation's corresponding node id

    PJSPGraphNode create_node(const POperation& pop) {
        int id = _nodes.size();
        PJSPGraphNode node(new JSPGraphNode());
        node->id = id;
        node->pop = pop;
        node->precedents = std::unordered_set<int>{};
        _nodes.push_back(node);
        return node;
    }
public:
    const static PJSPGraphNode START_NODE;
    const static PJSPGraphNode END_NODE;

    JSPGraph(const JSPManager& manager);

    const JSPManager& manager() const {
        return _manager;
    }

    void add_vertice(int prev_id, int next_id) {
        _succeeds_list[prev_id].insert(next_id);
        _previous_list[next_id].insert(prev_id);
    }

    void add_vertice(const PJSPGraphNode& prev, const PJSPGraphNode& next) {
        add_vertice(prev->id,next->id);
    }

    /**
     * Draw the graph to the specified image file
     *
     * @param filename the file to drawn
     */
    void generate_image(const char* filename) const;

    /**
     * Draw the graph to the specified image file
     *
     * @param filename the file to drawn
     */
    void generate_image(const std::string& filename) const {
        generate_image(filename.c_str());
    }

};


/**
 * todo:
 * 找最长路径
 *
 */
#endif //JSPALGORITHMS_JSP_H
