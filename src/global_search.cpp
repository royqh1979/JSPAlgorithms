/**
 * 全局搜索
 *
 * 使用A*算法进行全局搜索
 */

#include "jsp.h"
#include <cstring>
#include <cstdlib>
#include <iostream>
using namespace std;

int main() {
    // optimum 655
    JSPProblem problem("e:/la02.txt");

    problem.print_jobs();
    problem.print_machines();


    JSPSearchGraph graph(problem);

    graph.generate_greedy_solution();
    graph.generate_image("f:/test.png");

    //使用贪心法求得初始解，作为搜索上界
    graph.recalculate_earliest_times();
    graph.generate_image("f:/test2.png");

    int known_shortest_time = graph.shortest_time();

//    known_shortest_time = 1000;

    cout<<known_shortest_time<<endl;
    graph.clear_machine_operation_orders();

    cout<<"start!"<<endl;
    int max_depth = problem.operation_count();
    int last_operation_in_jobs[max_depth]; //每个job中最后一个operation的序号
    int last_finished_in_jobs[max_depth]; // 每个job中已经完成的最后一个operation在job中的序号
    int step_jobs[max_depth]; //每一步选择执行哪个job中的第一个一个未执行operation
    int current_finish_times[max_depth]; //每一步之后当前图的完成时间
    int remain_machine_times[problem.machine_count()]; //每个machine剩余任务的总完成时间
    int remain_job_times[problem.job_count()]; //每个job剩余任务的总完成时间
    int operation_machines[max_depth]; // 每一步处理的operation所用的机器
    int operation_machines_composite[max_depth][problem.machine_count()]; //每一步处理的operation的机器组合
    int count=0;

    memset(remain_machine_times,0,sizeof(remain_machine_times));
    memset(remain_job_times,0,sizeof(remain_job_times));
    for (int i=0;i<problem.job_count();i++) {
        last_operation_in_jobs[i]=problem.operation_count_in_job(i)-1;
        last_finished_in_jobs[i]=-1;
        for (int j=0;j<problem.operation_count_in_job(i);j++) {
            const POperation &op=problem.get_operation(i,j);
            remain_job_times[i]+=op->duration();
            remain_machine_times[op->machine_id()]+=op->duration();
        }
    }

    int depth = 0;
    step_jobs[depth]=-1;
    memset(operation_machines_composite[0],0,sizeof(int)*problem.machine_count());
    while (depth >= 0) {
        int j = (++step_jobs[depth]);
//        if (depth<=5) {
//            cout<<depth<<":";
//            for (int i = 0; i <= depth; i++) {
//                cout << step_jobs[i]<<",";
//            }
//            cout<<endl;
//        }
        if (j>=problem.job_count()) {
//            if (depth<=5)
//                cout<<"back"<<endl;
            //回溯
            depth--;
            if (depth<0)
                break;
            int j=step_jobs[depth];
            const POperation&  op = problem.get_operation(j,last_finished_in_jobs[j]);
            PJSPGraphNode& node = graph.get_node(op->id());
            graph.remove_last_node_in_machine_orders(node->machine_id);
            remain_job_times[node->job_id]+=node->duration;
            remain_machine_times[node->machine_id]+=node->duration;
            last_finished_in_jobs[j]--;
        } else {
//            if (depth<=5)
//                cout<<last_finished_in_jobs[j]<<"--"<<last_operation_in_jobs[j]<<endl;
            if (last_finished_in_jobs[j]>=last_operation_in_jobs[j]) {
                //该job中的operation已全部执行，因此不可行
                continue;
            }
            //计算活动时间
            const POperation&  op = problem.get_operation(j,last_finished_in_jobs[j]+1);
            PJSPGraphNode& node = graph.get_node(op->id());
            if (depth>0 && (operation_machines_composite[depth][node->machine_id]==0) && (node->machine_id<operation_machines[depth-1])) {
                //与本活动组合相同的组合之前已经尝试过，跳过
                continue;
            }
            graph.set_node_as_next_in_machine_orders(node);
            remain_job_times[node->job_id]-=node->duration;
            remain_machine_times[node->machine_id]-=node->duration;
            graph.calc_node_earlist_times(node);
            //判断是否需要剪枝
//            if (depth<=5)
//                cout<<node->earliest_start_time<<"++"<<node->earliest_end_time<<"++"<<known_shortest_time<<endl;
            int max_last_time = remain_job_times[node->job_id]>remain_machine_times[node->machine_id]?remain_job_times[node->job_id]:remain_machine_times[node->machine_id];
            if (node->earliest_end_time+max_last_time>=known_shortest_time) {
                //剪枝
//                cout<<problem.operation_count()<<"cut:"<<depth<<","<<node->earliest_end_time+max_last_time<<endl;
                graph.remove_last_node_in_machine_orders(node->machine_id);
                remain_job_times[node->job_id]+=node->duration;
                remain_machine_times[node->machine_id]+=node->duration;
                continue;
            }
            //记录时间
            if (depth==0) {
                current_finish_times[depth]=node->earliest_end_time;
            }else {
                int finish_time = current_finish_times[depth-1];
                current_finish_times[depth]=finish_time>node->earliest_end_time?finish_time:node->earliest_end_time;
            }
            if (depth==problem.operation_count()-1) {
                //找到一个新的解
                //判断是否需要更新最优解
                if (current_finish_times[depth]<known_shortest_time) {
                    known_shortest_time = current_finish_times[depth];
                    cout<<"found one!"<<current_finish_times[depth]<<endl;
                    count++;
                    char buf[100];
                    sprintf(buf,"f:/s-%04d.png",known_shortest_time);
                    graph.generate_image(buf);
                }
                graph.remove_last_node_in_machine_orders(node->machine_id);
                remain_job_times[node->job_id]+=node->duration;
                remain_machine_times[node->machine_id]+=node->duration;
                //回溯
                depth--;
                int j=step_jobs[depth];
                const POperation&  op = problem.get_operation(j,last_finished_in_jobs[j]);
                PJSPGraphNode& node = graph.get_node(op->id());
                graph.remove_last_node_in_machine_orders(node->machine_id);
                remain_job_times[node->job_id]+=node->duration;
                remain_machine_times[node->machine_id]+=node->duration;
                last_finished_in_jobs[j]--;
            } else {
                //继续下一步
                operation_machines[depth]=node->machine_id;
                if (depth>0) {
                    if (operation_machines_composite[depth - 1][depth] == 0) {
                        memcpy(operation_machines_composite[depth], operation_machines_composite[depth-1],sizeof(int)*problem.machine_count());
                    } else {
                        memset(operation_machines_composite[depth], 0,sizeof(int)*problem.machine_count());
                        for (int i=depth-1;i>=0;i++) {
                            if (operation_machines[i]==node->machine_id) {
                                break;
                            }
                            operation_machines_composite[depth][i]=1;
                        }
                    }
                } else {
                    memset(operation_machines_composite[depth], 0,sizeof(int)*problem.machine_count());
                }
                operation_machines_composite[depth][node->machine_id] = 1;
                last_finished_in_jobs[j]++;
                depth++;
                step_jobs[depth]=-1;
            }
        }
    }

    return 0;
}

