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

JSPSearchGraph::JSPSearchGraph(const JSPProblem& problem):_problem(&problem),
        _machine_operation_orders(_problem->machine_count()) {

    for (int i = 0; i < _problem->job_count(); i++) {
        const Job &job = _problem->get_job(i);
        for (int j = 0; j < job.operation_count(); j++) {
            const POperation &pop = job.get_operation(j);
            PJSPGraphNode node = create_node(pop);
            if (j == 0) {
                _start_nodes.insert(node->id);
            } else if (j == job.operation_count() - 1) {
                _end_nodes.insert(node->id);
            }
        }
    }
}

void JSPSearchGraph::clear_machine_operation_orders() {
    for (OperationOrdersInMachine& order:_machine_operation_orders) {
        order.clear();
    }

    for (PJSPGraphNode& node:_nodes) {
        node->index_in_machine=-1;
    }
}

void JSPSearchGraph::generate_greedy_solution() {
    PPOperationGreatThan great_than{};
    for (int i=0;i<_problem->machine_count();i++) {
        const Machine& machine = _problem->get_machine(i);
        OperationOrdersInMachine& order = _machine_operation_orders[machine.id()];
        std::vector<POperation> operations(machine.opertions().begin(), machine.opertions().end());
        make_heap(operations.begin(),operations.end(),great_than);
        while(!operations.empty()){
            pop_heap(operations.begin(),operations.end(),great_than);
            POperation  op = operations.back();
            int node_id = op->id();
            PJSPGraphNode& node = _nodes[node_id];
            set_node_as_next_in_machine_orders(node);
            operations.pop_back();
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
    Agnode_t *g_node, *g_start_node, *g_end_node;
    g_start_node = agnode(g, (char*)"start", TRUE);
    g_end_node = agnode(g, (char*)"end", TRUE);
    for (int i = 0; i < _problem->job_count(); i++) {
        const Job &job = _problem->get_job(i);

        snprintf(buf, 100, "%s%d", job_str, i);
        Agraph_t *subg = agsubg(g, buf, TRUE);
        agattr(subg, AGRAPH, (char*)"shape", (char*)"box");
        agattr(subg, AGRAPH, (char*)"color", (char*)"blue");
        snprintf(buf, 100, "job %d", i);
        agattr(subg, AGRAPH, (char*)"label", buf);
        for (int j = 0; j < job.operation_count(); j++) {
            const POperation& pop = job.get_operation(j);
            const PJSPGraphNode &node = _nodes[pop->id()];
            snprintf(buf, 100, "%s%d(%d ES%d EF%d)", node_str, node->id, node->duration,node->earliest_start_time,node->earliest_end_time);
            g_node = agnode(subg, buf, TRUE);
            g_nodes.push_back(g_node);
        }
    }



    Agnode_t *g_next_node;
    int j_count,m_count = 0;
    for (int id:_start_nodes){
        g_node = g_nodes[id];
        snprintf(buf, 100, "j_edge_%d", j_count++);
        agedge(g, g_start_node, g_node, buf, TRUE);
    }
    for (int i = 0; i < _nodes.size(); i++) {
        g_node = g_nodes[i];
        int next_id = get_next_in_job(_nodes[i]);
        if (next_id==-1) {
            g_next_node = g_end_node;
        } else if (next_id>=0) {
            g_next_node = g_nodes[next_id];
        } else{
            throw runtime_error("error next in job node!");
        }
        snprintf(buf, 100, "j_edge_%d", j_count++);
        agedge(g, g_node, g_next_node, buf, TRUE);

        next_id = get_next_in_machine(_nodes[i]);
        if (next_id==-1) {
            g_next_node = g_end_node;
        } else if (next_id>=0) {
            g_next_node = g_nodes[next_id];
        } else{
            char buf[100];
            sprintf(buf,"error next in machine node (id: %d, %d,%d,%d,%d)!",_nodes[i]->id,_nodes[i]->job_id,_nodes[i]->index_in_job,
                    _nodes[i]->machine_id,_nodes[i]->index_in_machine);
            throw runtime_error(buf);
        }
        snprintf(buf, 100, "m_edge_%d", m_count++);
        agedge(g, g_node, g_next_node, buf, TRUE);
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

    for (int id:_start_nodes) {
        const PJSPGraphNode& node=_nodes[id];

        if (get_prev_in_machine(node)==-1){
            candidates.push(node->id);
        } else {
            uncalculated_precedences[node->id]=1;
        }
    }

    while (!candidates.empty()) {
        PJSPGraphNode&  current_node=_nodes[candidates.front()];
        candidates.pop();
        calc_node_earlist_times(current_node);
        int next_in_job_id = get_next_in_job(current_node);
        int next_in_machine_id = get_next_in_machine(current_node);

        for (int i=0;i<2;i++) {
            int id;
            if (i==1) {
                id = next_in_job_id;
            } else {
                id = next_in_machine_id;
            }
            if (id<0) {
                continue;
            }
            //the node has calculated, skip it
            if (finished.find(id)!=finished.end()) {
                continue;
            }
            auto it=uncalculated_precedences.find(id);
            int n;
            if (it==uncalculated_precedences.end()) {
                const PJSPGraphNode& successor=_nodes[id];
                n=get_precedence_count(successor);
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

    last_node_time = 0;
    for (int id:_end_nodes) {
        const PJSPGraphNode& node = _nodes[id];
        if (node->earliest_end_time>last_node_time) {
            last_node_time = node->earliest_end_time;
        }
    }
}

void JSPSearchGraph::calc_node_earlist_times(PJSPGraphNode &node) {
    int prev_in_job_id = get_prev_in_job(node);
    int prev_in_machine_id = get_prev_in_machine(node);
//    cout<<"****"<<prev_in_job_id<<","<<prev_in_machine_id<<endl;
    if (prev_in_job_id == -1 && prev_in_machine_id == -1) {
        node->earliest_start_time = 0;
    } else if (prev_in_job_id == -1) {
        const PJSPGraphNode &precedent_in_machine = _nodes[prev_in_machine_id];
        node->earliest_start_time = precedent_in_machine->earliest_end_time;
    } else if (prev_in_machine_id == -1) {
        const PJSPGraphNode &precedent_in_job = _nodes[prev_in_job_id];
        node->earliest_start_time = precedent_in_job->earliest_end_time;
    } else {
        const PJSPGraphNode &precedent_in_job = _nodes[prev_in_job_id];
        const PJSPGraphNode &precedent_in_machine = _nodes[prev_in_machine_id];
        if (precedent_in_job->earliest_end_time > precedent_in_machine->earliest_start_time) {
            node->earliest_start_time = precedent_in_job->earliest_end_time;
        } else {
            node->earliest_start_time = precedent_in_machine->earliest_end_time;
        }
    }
    node->earliest_end_time = node->earliest_start_time+node->duration;
}




