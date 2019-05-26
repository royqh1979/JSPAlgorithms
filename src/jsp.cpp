#include <jsp.h>
//
// Created by roy on 2019/5/20.
//
#include <jsp.h>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <queue>
#include <boost/algorithm/string/trim.hpp>
#include <cgraph.h>
#include <gvc.h>


using namespace std;

JSPProblem::JSPProblem(int job_count,int machine_count):_job_count(job_count),_machine_count(machine_count) {
    for (int i=0;i<job_count;i++) {
        create_job();
    }
    for (int i=0;i<machine_count;i++) {
        create_machine();
    }
}

JSPProblem::JSPProblem(const string& jsp_data_filename){
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

void JSPProblem::print_jobs() const{
    for (int i=0;i<job_count();i++) {
        cout<<"Job "<<i<<endl;
        for (int j=0;j<operation_count_in_job(i);j++) {
            const POperation op=get_operation(i,j);
            cout<<"("<<op->job_id()<<","<<op->index_in_job()<<","<<op->machine_id()<<","<<op->duration()<<")";
        }
        cout<<endl;
    }
}
void JSPProblem::print_machines() const{
    for (int i=0;i<machine_count();i++) {
        cout<<"Machine "<<i<<endl;
        const Machine& machine=get_machine(i);
        for (const POperation& op:machine._operations) {
            cout<<"("<<op->job_id()<<","<<op->index_in_job()<<","<<op->machine_id()<<","<<op->duration()<<")";
        }
        cout<<endl;
    }
}


struct PPOperationGreatThan{
    bool operator()(const POperation& a,const POperation& b) const{
        return a->index_in_job()>b->index_in_job();
    }
};

JSPSearchGraph::JSPSearchGraph(const JSPProblem& problem):_problem(problem),
        _operation_node_ids(_problem.operation_count()),_machine_operation_orders(_problem.machine_count()),
        _start_node(new JSPGraphNode{0, nullptr}),_end_node(new JSPGraphNode{1, nullptr}){
    _nodes.push_back(_start_node);
    _nodes.push_back(_end_node);


    for (int i=0;i<_problem.job_count();i++) {
        const Job& job = _problem.get_job(i);
        PJSPGraphNode prev_node = _start_node;
        for (int j=0;j<job.operation_count();j++) {
            const POperation& pop=job.get_operation(j);
            PJSPGraphNode node = create_node(pop);
            add_vertice(prev_node,node);
            _operation_node_ids[pop->id()]=node->id;
            prev_node = node;
        }
        add_vertice(prev_node,_end_node);
    }

    PPOperationGreatThan great_than{};
    for (int i=0;i<_problem.machine_count();i++) {
        const Machine& machine = _problem.get_machine(i);
        OperationOrdersInMachine& order = _machine_operation_orders[machine.id()];
        std::vector<POperation> operations(machine.opertions().begin(), machine.opertions().end());
        make_heap(operations.begin(),operations.end(),great_than);
        PJSPGraphNode prev_node = nullptr;
        while(!operations.empty()){
            pop_heap(operations.begin(),operations.end(),great_than);
            POperation  op = operations.back();
            int node_id = _operation_node_ids[op->id()];
            operations.pop_back();
            order.push_back(node_id);
            const PJSPGraphNode& node = _nodes[node_id];
            if (prev_node !=nullptr) {
                add_vertice(prev_node,node);
            }
            prev_node = node;
        }
    }

}

void JSPSearchGraph::generate_image(const string& filename) const {
    GVC_t *gvc;
    Agraph_t *g;
    gvc = gvContext();

    g = agopen((char*)"My Graph", Agdirected, nullptr);
    char job_str[] = "cluster_";
    char node_str[] = "node_";
    char buf[101];
    vector<Agnode_t *> g_nodes{};
    Agnode_t *g_node;
    g_node = agnode(g, (char*)"start", TRUE);
    g_nodes.push_back(g_node);
    g_node = agnode(g, (char*)"end", TRUE);
    g_nodes.push_back(g_node);
    for (int i = 0; i < _problem.job_count(); i++) {
        const Job &job = _problem.get_job(i);

        snprintf(buf, 100, "%s%d", job_str, i);
        Agraph_t *subg = agsubg(g, buf, TRUE);
        agattr(subg, AGRAPH, (char*)"shape", (char*)"box");
        agattr(subg, AGRAPH, (char*)"color", (char*)"blue");
        snprintf(buf, 100, "job %d", i);
        agattr(subg, AGRAPH, (char*)"label", buf);
        for (int j = 0; j < job.operation_count(); j++) {
            const POperation& pop = job.get_operation(j);
            const PJSPGraphNode &node = _nodes[_operation_node_ids[pop->id()]];
            snprintf(buf, 100, "%s%d(%.1f ES%.1f EF%.1f)", node_str, node->id, node->pop->duration(),node->earliest_start_time,node->earliest_end_time);
            g_node = agnode(subg, buf, TRUE);
            g_nodes.push_back(g_node);
        }
    }

    Agnode_t *g_next_node;
    int count = 0;
    for (int i = 0; i < _nodes.size(); i++) {
        const unordered_set<int> &successors = _nodes[i]->successors;
        g_node = g_nodes[i];
        for (int next_node_id:successors) {
            g_next_node = g_nodes[next_node_id];
            snprintf(buf, 100, "edge_%d", count++);
            agedge(g, g_node, g_next_node, buf, TRUE);
        }
    }

    gvLayout(gvc, g, "dot");
    gvRenderFilename(gvc, g, "png", filename.c_str());
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
};

void JSPSearchGraph::recalculate_earliest_times(){
    unordered_map<int,int> uncalculated_precedences{};
    queue<int> candidates;
    unordered_set<int> finished;

    for (int id:_start_node->successors) {
        const PJSPGraphNode& node=_nodes[id];

        if (node->precedences.size()==1){
            candidates.push(node->id);
        } else {
            uncalculated_precedences[node->id]=node->precedences.size()-1;
        }
    }
    finished.insert(_start_node->id);

    while (!candidates.empty()) {
        const PJSPGraphNode&  current_node=_nodes[candidates.front()];
        candidates.pop();
        current_node->earliest_start_time = 0;
        for (int id:current_node->precedences) {
            const PJSPGraphNode& precedent = _nodes[id];
            if (precedent->earliest_end_time>current_node->earliest_start_time) {
                current_node->earliest_start_time=precedent->earliest_end_time;
            }
        }
        current_node->earliest_end_time = current_node->earliest_start_time+current_node->duration;
        for (int id:current_node->successors) {
            //the node has calculated, skip it
            if (finished.find(id)!=finished.end()) {
                continue;
            }

            auto it=uncalculated_precedences.find(id);
            int n;
            if (it==uncalculated_precedences.end()) {
                const PJSPGraphNode& successor=_nodes[id];
                n=successor->precedences.size();
            } else {
                n = uncalculated_precedences[id];
            }
            if (n>0) {
                n--;
            }
            if (n==0) {
                uncalculated_precedences.erase(id);
                candidates.push(id);
            } else {
                uncalculated_precedences[id]=n;
            }
        }
        finished.insert(current_node->id);
    }
}




