#include <jsp.h>
//
// Created by roy on 2019/5/20.
//
#include <jsp.h>
#include <iostream>
#include <string>
#include <sstream>
#include <boost/algorithm/string/trim.hpp>
#include <cgraph.h>
#include <gvc.h>


using namespace std;

JSPManager::JSPManager(int job_count,int machine_count):_job_count(job_count),_machine_count(machine_count) {
    for (int i=0;i<job_count;i++) {
        create_job();
    }
    for (int i=0;i<machine_count;i++) {
        create_machine();
    }
}

JSPManager::JSPManager(string jsp_data_filename){
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

void JSPManager::print_jobs() const{
    for (int i=0;i<job_count();i++) {
        cout<<"Job "<<i<<endl;
        for (int j=0;j<operation_count_in_job(i);j++) {
            const POperation op=get_operation(i,j);
            cout<<"("<<op->job_id()<<","<<op->index_in_job()<<","<<op->machine_id()<<","<<op->duration()<<")";
        }
        cout<<endl;
    }
}
void JSPManager::print_machines() const{
    for (int i=0;i<machine_count();i++) {
        cout<<"Machine "<<i<<endl;
        const Machine& machine=get_machine(i);
        for (POperation op:machine._operations) {
            cout<<"("<<op->job_id()<<","<<op->index_in_job()<<","<<op->machine_id()<<","<<op->duration()<<")";
        }
        cout<<endl;
    }
}
const PJSPGraphNode JSPGraph::START_NODE(new JSPGraphNode{0, nullptr, std::unordered_set<int>{}});
const PJSPGraphNode JSPGraph::END_NODE(new JSPGraphNode{1, nullptr, std::unordered_set<int>{}});

struct PPOperationGreatThan{
    bool operator()(const POperation& a,const POperation& b) const{
        return a->index_in_job()>b->index_in_job();
    }
};

JSPGraph::JSPGraph(const JSPManager& manager):_manager(manager),
        _operation_node_ids(_manager.operation_count()),_machine_operation_orders(_manager.machine_count()){
    _nodes.push_back(START_NODE);
    _nodes.push_back(END_NODE);


    // init adjacent list and reverse adjacent list
    for (int i=0;i<_manager.operation_count()+2;i++) {
        _succeeds_list.push_back(JSPGraphNodeSet{});
        _previous_list.push_back(JSPGraphNodeSet{});
    }

    for (int i=0;i<_manager.job_count();i++) {
        const Job& job = _manager.get_job(i);
        PJSPGraphNode prev_node = START_NODE;
        for (int j=0;j<job.operation_count();j++) {
            const POperation& pop=job.get_operation(j);
            PJSPGraphNode node = create_node(pop);
            add_vertice(prev_node,node);
            _operation_node_ids[pop->id()]=node->id;
            prev_node = node;
        }
        add_vertice(prev_node,END_NODE);
    }

    PPOperationGreatThan great_than{};
    for (int i=0;i<_manager.machine_count();i++) {
        const Machine& machine = _manager.get_machine(i);
        MachineOperationOrder& order = _machine_operation_orders[machine.id()];
        std::vector<POperation> operations(machine.opertions().begin(), machine.opertions().end());
        make_heap(operations.begin(),operations.end(),great_than);
        PJSPGraphNode prev_node = nullptr;
        while(operations.size()>0){
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

#ifdef _DEBUG    
    generate_image("f:/test.png");
#endif    

}

void JSPGraph::generate_image(const char* filename) const {
    GVC_t *gvc;
    Agraph_t *g;
    gvc = gvContext();

    g = agopen("My Graph", Agdirected, NULL);
    char job_str[] = "cluster_";
    char node_str[] = "node_";
    char buf[101];
    vector<Agnode_t *> g_nodes{};
    Agnode_t *g_node;
    g_node = agnode(g, "start", TRUE);
    g_nodes.push_back(g_node);
    g_node = agnode(g, "end", TRUE);
    g_nodes.push_back(g_node);
    for (int i = 0; i < _manager.job_count(); i++) {
        const Job &job = _manager.get_job(i);

        snprintf(buf, 100, "%s%d", job_str, i);
        Agraph_t *subg = agsubg(g, buf, TRUE);
        agattr(subg, AGRAPH, "shape", "box");
        agattr(subg, AGRAPH, "color", "blue");
        snprintf(buf, 100, "job %d", i);
        agattr(subg, AGRAPH, "label", buf);
        for (int j = 0; j < job.operation_count(); j++) {
            POperation pop = job.get_operation(j);
            const PJSPGraphNode &node = _nodes[_operation_node_ids[pop->id()]];
            snprintf(buf, 100, "%s%d(%.2f)", node_str, node->id, node->pop->duration());
            g_node = agnode(subg, buf, TRUE);
            g_nodes.push_back(g_node);
        }
    }

    Agnode_t *g_next_node;
    int count = 0;
    for (int i = 0; i < _nodes.size(); i++) {
        const JSPGraphNodeSet &item = _succeeds_list[i];
        g_node = g_nodes[i];
        for (int next_node_id:item) {
            g_next_node = g_nodes[next_node_id];
            snprintf(buf, 100, "edge_%d", count++);
            agedge(g, g_node, g_next_node, buf, TRUE);
        }
    }

    gvLayout(gvc, g, "dot");
    gvRenderFilename(gvc, g, "png", filename);
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
};




