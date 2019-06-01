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

class JSPProblem;
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
    int _duration;
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
    Operation(int id,int job_id,int machine_id, int duration):
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
    int duration() const {return _duration;}
    /**
     * Get the operation's index in the job
     * @return
     */
    int index_in_job() const {return _index_in_job;}
//    const int& index_in_machine() const {return _index_in_job;}
//    void set_index_in_machine(int index_in_machine) {
//        _index_in_machine = index_in_machine;
//    }
    friend class JSPProblem;
    friend class Job;
};

/**
 * Hash function object for the Operation class
 * Used in hashset/hashmap
 */
struct Operation_hash{
    size_t operator()(const Operation& op) const {
        return std::hash<int>()(op.id());
    }
};

inline bool operator==(const Operation& first, const Operation& second){
    return first.id() == second.id();
}

/**
 * Pointer to the Operaction class
 */
using POperation = std::shared_ptr<Operation>;

/**
 * Hash function object for the POperation class
 * Used in hashset/hashmap
 */
struct POperation_hash{
    size_t operator()(const POperation& op) const {
        return std::hash<int>()(op->id());
    }
};

inline bool operator==(const POperation& first, const POperation& second){
    return first->id() == second->id();
}

using POperationSet = std::unordered_set<POperation, POperation_hash>;

/**
 * The Job class
 * A job is composed of several operations.
 */
class Job{
private:
    std::vector<POperation> _operations{};
    int _id;
    /**
     * Add an operation to the job and make it the last operation of the job.
     * @param pop
     */
    void add_operation(POperation& pop) {
        int index_in_job = _operations.size();
        pop->set_index_in_job(index_in_job);
        _operations.push_back(pop);
    }
    /**
     * The constructor.
     *
     * @param id the id of the job.
     */
    explicit Job(int id): _id(id) {}
public:

    /**
     * Get a pointer to the operation with the specifed index_in_job in the job.
     *
     * The operation is the (index_in_job+1)th operation in the job.
     * So if the operation is the 5th in the job, its index_in_job should be 4.
     *
     * @param index_in_job the index_in_job value of the operation
     * @return pointer to the specified operation
     */
    const POperation& get_operation(int index_in_job) const {
        return _operations[index_in_job];
    }

    /**
     * Get count of the operations in the job.
     *
     * @return
     */
    int operation_count() const {
        return _operations.size();
    }

    /**
     * Get the id of the job.
     *
     * @return
     */
    int id() const{
        return _id;
    }

    friend class JSPProblem;
};

/**
 * The Machine class
 */
class Machine{
private:
    POperationSet  _operations{};
    int _id;
    /**
     * The constructor
     *
     * @param id the id of the machine.
     */
    explicit Machine(int id): _id(id){  }
    /**
     * Add a operation to the machine, which means that the operation must be processed on this machine.
     * @param op
     */
    void add_operation(const POperation &op) {
        _operations.insert(op);
    }
public:
    /**
     * Get count of the operations.
     *
     * @return
     */
    int operation_count() const {
        return _operations.size();
    }

    /**
     * Get all operations must be proccessed on this machine.
     *
     * @return
     */
    const POperationSet& opertions() const {
        return _operations;
    }

    /**
     * Get id of the machine
     * @return
     */
    int id() const{
        return _id;
    }


    friend class JSPProblem;
};


/**
 * The JSP problem class
 * Each jsp problem is corresponding to one JspProblem object.
 * It contains all the informations of the problem, such as jobs, operations and machines.
 */
class JSPProblem {
private:
    int _job_count{0};
    int _machine_count{0};
    std::vector<Job> _jobs{};
    std::vector<Machine> _machines{};
    std::vector<POperation> _operations{};

    /**
     * Create a new job
     *
     * @return the new job object
     */
    Job& create_job(){
        int id=_jobs.size();
        _jobs.push_back(Job{id});
        return _jobs.back();
    }

    /**
     * Create a new machine
     * @return the new machine object
     */
    Machine& create_machine() {
        int id=_machines.size();
        _machines.push_back(Machine{id});
        return _machines.back();
    }
public:
    /**
     * The constructor
     *
     * @param job_count total counts of jobs
     * @param machine_count total counts of machines
     */
    JSPProblem(int job_count,int machine_count);

    /**
     * Read the data file and construct the corresponding jsp problem
     *
     * @param jsp_data_filename the data file name
     */
    explicit JSPProblem(const std::string& jsp_data_filename);

    /**
     * Get the job with the specified id
     *
     * @param id the jobs'id
     * @return the job
     */
    const Job& get_job(int id) const{
        return _jobs[id];
    }

    /**
     * Get the machine with the specified id
     * @param id the machine's id
     * @return the machine
     */
    const Machine& get_machine(int id) const {
        return _machines[id];
    }

    /**
     * Add an operation
     *
     * @param job_id id of the job to which the operation belongs
     * @param machine_id id of the machine that the operation is processed on
     * @param duration the processing time of the operation
     * @return
     */
    POperation add_operation(int job_id, int machine_id, int duration) {
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

    /**
     * Get the pointer to the specified operation
     *
     * @param job_id id of the job to which the operation belongs
     * @param index_in_job the operation's index in the job (@see Job::get_operation)
     * @return the pointer of the specified operation
     */
    const POperation& get_operation(int job_id, int index_in_job) const{
        return _jobs[job_id].get_operation(index_in_job);
    }

    /**
     * Get total count of the jobs
     *
     * @return
     */
    int job_count() const {
        return _job_count;
    }

    /**
     * Get total count of the operations
     *
     * @return
     */
    int operation_count() const {
        return _operations.size();
    }

    /**
     * get count of the operations in the specified job
     *
     * @param job_id id of the job to which the operations belong
     * @return
     */
    int operation_count_in_job(int job_id) const {
        return get_job(job_id).operation_count();
    }

    /**
     * Get total count of the machines.
     *
     * @return
     */
    int machine_count() const {
        return _machine_count;
    }

    /**
     * Get pointer to the operation
     * @param id id of the operation
     * @return
     */
    const POperation& get_operation(int id) const {
        return _operations[id];
    }

    /**
     * Print jobs info to stdin
     */
    void print_jobs() const;

    /**
     * Print machines info to stdin
     */
    void print_machines() const;
};

/**
 * Node of the JSP search graph
 */
struct JSPGraphNode {
    int id;
    int job_id;
    int machine_id;
    int index_in_job;
    int index_in_machine;
    int duration;
    int earliest_start_time=0;
    int earliest_end_time=0;
    int latest_start_time=0;
    int latest_end_time=0;
};

using PJSPGraphNode = std::shared_ptr<JSPGraphNode>;


using OperationOrdersInMachine = std::vector<int>;



/**
 * JSP Search Graph
 */
class JSPSearchGraph{
private:
    const JSPProblem* _problem;
    std::vector<PJSPGraphNode> _nodes{};
    std::unordered_set<int> _start_nodes{};
    std::unordered_set<int> _end_nodes{};
    int last_node_time = 0;

    std::vector<OperationOrdersInMachine> _machine_operation_orders{};
    /**
     * Create a new operation node
     *
     * @param pop pointer to the node's corresponding operation
     * @return the created node
     */
    PJSPGraphNode create_node(const POperation& pop) {
        int id = _nodes.size();
        PJSPGraphNode node(new JSPGraphNode{id,pop->job_id(),pop->machine_id(),pop->index_in_job(),-1,pop->duration()});
#ifdef _DEBUG
        if (node->id!=pop->id()) {
            throw std::runtime_error("Operation id is not equal to graph node id!");
        }
#endif
        _nodes.push_back(node);
        return node;
    }
public:

    /**
     * Construct the search graph
     *
     * @param problem the JSP problem of the search graph
     */
    explicit JSPSearchGraph(const JSPProblem& problem);

    /**
     * Get the JSP problem
     * @return
     */
    const JSPProblem& problem() const {
        return *(_problem);
    }

    /**
     * Get the precedant job operation node id
     *
     * @param node
     * @return
     */
    int get_prev_in_job(const PJSPGraphNode& node) const {
        if (node->index_in_job==0) {
            return -1;
        }
        return problem().get_job(node->job_id).get_operation(node->index_in_job-1)->id();
    }

    int get_prev_in_machine(const PJSPGraphNode& node) const {
        if (node->index_in_machine==-1) {
            return -2;
        }
        if (node->index_in_machine==0) {
            return -1;
        }
        return _machine_operation_orders[node->machine_id][node->index_in_machine-1];
    }

    int get_next_in_job(const PJSPGraphNode& node) const {
        if (node->index_in_job == problem().get_job(node->job_id).operation_count()-1) {
            return -1;
        }
        return problem().get_job(node->job_id).get_operation(node->index_in_job+1)->id();
    }

    int get_next_in_machine(const PJSPGraphNode& node) const {
        if (node->index_in_machine==-1) {
            return -2;
        }
        if (node->index_in_machine==problem().get_machine(node->machine_id).operation_count()-1) {
            return -1;
        }
        return _machine_operation_orders[node->machine_id][node->index_in_machine+1];
    }

    int get_precedence_count(const PJSPGraphNode& node) const {
        if (node->index_in_machine==0) {
            return (node->index_in_job == 0)?0:1;
        } else{
            return (node->index_in_job == 0)?1:2;
        }
    }

    int get_successor_count(const PJSPGraphNode& node) const {
        if (node->index_in_machine==problem().get_machine(node->machine_id).operation_count()-1) {
            return (node->index_in_job == problem().get_job(node->job_id).operation_count()-1)?0:1;
        } else{
            return (node->index_in_job == problem().get_job(node->job_id).operation_count()-1)?1:2;
        }
    }


    /**
     * 将operation结点设置为machine活动关系队列中的下一个
     * @param node
     */
    void set_node_as_next_in_machine_orders(PJSPGraphNode &node) {
        node->index_in_machine = _machine_operation_orders[node->machine_id].size();
        _machine_operation_orders[node->machine_id].push_back(node->id);
    }

    /**
     * 从machine活动关系队列中移除最后一个operation结点
     */
    void remove_last_node_in_machine_orders(int machine_id) {
        int last_node_id = _machine_operation_orders[machine_id].back();
        PJSPGraphNode& node = _nodes[last_node_id];
        node->index_in_machine=-1;
        _machine_operation_orders[machine_id].pop_back();
    }

    /**
     *     清空machine活动关系队列
     * @return
     */
    void clear_machine_operation_orders();

    const std::unordered_set<int>& start_nodes() const {
        return _start_nodes;
    }

    /**
     * generate a solution using greedy algorithms
     */
    void generate_greedy_solution();

    /**
     * Draw the graph to the specified image file
     *
     * @param filename the file to drawn
     */
    void generate_image(const std::string& filename) const;

    /**
     * Recalcuate the start/finish time of nodes
     */
    void recalculate_earliest_times();

    double shortest_time() {
        return last_node_time;
    }

    PJSPGraphNode& get_node(int node_id) {
        return _nodes[node_id];
    }

    void calc_node_earlist_times(PJSPGraphNode &node) ;
};


/**
 * todo:
 * 找最长路径
 *
 */
#endif //JSPALGORITHMS_JSP_H
