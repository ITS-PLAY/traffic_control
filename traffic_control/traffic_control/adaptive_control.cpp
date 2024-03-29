#include "traffic_control.h"
using namespace std;

void Phase_Index::initial_Phase_Index_Caculation() {
	int num = (int)phase_Lanes.size();
	for (int i = 0; i < num; i++) {
		volume_Interval+= phase_Lanes[i].volume_Interval;                          
		volume += phase_Lanes[i].volume;
		small_Car_Volume += phase_Lanes[i].small_Car_Volume;
		medium_Car_Volume += phase_Lanes[i].medium_Car_Volume;
		large_Car_Volume += phase_Lanes[i].medium_Car_Volume;
		train_Car_Volume += phase_Lanes[i].train_Car_Volume;
		if (phase_Lanes[i].queue_Length > queue_Length)
			queue_Length = phase_Lanes[i].queue_Length;
		if (phase_Lanes[i].queue_Num > queue_Num) {
			queue_Num = phase_Lanes[i].queue_Num;
			queue_Transit = phase_Lanes[i].queue_Transit;
		}	
		time_Headway_Saturation += phase_Lanes[i].time_Headway_Saturation/num;
		capacity_Saturation += phase_Lanes[i].capacity_Saturation;
		capacity_Intersection += phase_Lanes[i].capacity_Intersection;
	}
	volume_Interval /= num; volume /= num;                                                                          //流量取平均值
	switch (queue_Num) {
	    case 0: delay_Vehicles_Start = 0.0; phase_Info.green_Time = 0; break;
	    case 1: delay_Vehicles_Start = 2.0; phase_Info.green_Time = 2; break;
	    case 2: delay_Vehicles_Start = 3.0; phase_Info.green_Time = 3; break;
	    default:
			   delay_Vehicles_Start = 4.0;
		       phase_Info.green_Time = 4; break;
	}
	return;
}

inline void Phase_Index::Pedestrian_Time_Caculation() {

	return;
}

inline void Phase_Index::initial_Demand_Caculation() {             
	initial_Demand = phase_Clearance_Ratio * (1.0F * volume_Interval / Time_Interval * phase_Info.intersection_Signal_Controller.signal_Cycle_Time + queue_Num);       //每一相位的初始需求initial_Demand
	return;
}

double Node_Adaptive_Control::queue_Delay_Value(const int phase_Id) {
	if (phases_Index[phase_Id].queue_Num == 0)
		return 0.0;
	int temp = phases_Index[phase_Id].queue_Num  - phases_Index[phase_Id].capacity_Intersection;
	double ratio = ((phases_Index[phase_Id].queue_Num - phases_Index[phase_Id].queue_Transit) + phases_Index[phase_Id].queue_Transit * car_Delay_Ratio.at("transit")) / phases_Index[phase_Id].queue_Num;                //排队车辆的平均延误换算系数
	if (temp <= 0)
		return ratio * phases_Index[phase_Id].queue_Num  * phases_Index[phase_Id].time_Headway_Saturation;
	else
		return ratio * (phases_Index[phase_Id].capacity_Intersection * phases_Index[phase_Id].time_Headway_Saturation + temp * static_cast<double>(phases_Index[phase_Id].phase_Info.green_Time));
}

double Node_Adaptive_Control::red_Stop_Delay_Value(const int phase_Id, const int moment_Of_Cycle) {
	double ratio = 1.0;
	int msignal_Cycle_Time = phases_Index[phase_Id].phase_Info.intersection_Signal_Controller.signal_Cycle_Time;                                       //周期时长
	int green_Time_Effctive = phases_Index[phase_Id].phase_Info.green_Time + phases_Index[phase_Id].phase_Info.yellow_Time;                            //有效绿灯时长
	if (phases_Index[phase_Id].volume_Interval > 0)
		ratio = ((phases_Index[phase_Id].volume_Interval - phases_Index[phase_Id].transit_Car_Volume * car_Volume_Ratio.at("transit")) + phases_Index[phase_Id].transit_Car_Volume * car_Volume_Ratio.at("transit") * car_Delay_Ratio.at("transit")) / phases_Index[phase_Id].volume_Interval;
	double actual_Queue_Num = phases_Index[phase_Id].initial_Demand / phases_Index[phase_Id].phase_Clearance_Ratio - (static_cast<double>(phases_Index[phase_Id].phase_Info.green_Time) - phases_Index[phase_Id].delay_Vehicles_Start) / phases_Index[phase_Id].time_Headway_Saturation;
	printf("phase_Id: %d ,green_time: %d, actual_demand_ratio: %f, actual_queue_num: %f  \n", phase_Id, phases_Index[phase_Id].phase_Info.green_Time, phases_Index[phase_Id].initial_Demand / phases_Index[phase_Id].phase_Clearance_Ratio, actual_Queue_Num);
	
	double wait_Vehs_Ratio = 0.5;                                                                                                                    //考虑车辆的均匀到达，红灯等待延误的系数设为0.5   
	double wait_Before_Green_Ratio = 1.0 * moment_Of_Cycle / msignal_Cycle_Time, 
		wait_After_Green_Ratio = 1.0 * (msignal_Cycle_Time - moment_Of_Cycle - green_Time_Effctive) / msignal_Cycle_Time;                            //绿灯前/后的等待车辆比例(当考虑绿波车队形式时，调整对应的比例)
	return ratio * Time_Interval / msignal_Cycle_Time *(wait_Vehs_Ratio * phases_Index[phase_Id].initial_Demand / phases_Index[phase_Id].phase_Clearance_Ratio * wait_Before_Green_Ratio * moment_Of_Cycle +
		wait_Vehs_Ratio * phases_Index[phase_Id].initial_Demand / phases_Index[phase_Id].phase_Clearance_Ratio * wait_After_Green_Ratio * (msignal_Cycle_Time - green_Time_Effctive) + 
		max(0.0, actual_Queue_Num) * (msignal_Cycle_Time - green_Time_Effctive));
}

void Node_Adaptive_Control::get_Phases_Overlap_Info() {             //固定或者可变相序下的相位嵌套组合
	shared_ptr<Stage_Node> phase1 (new Stage_Node(1));
	phase1->next = make_shared<Stage_Node>(Stage_Node(5));            //嵌套相位只有1个时
	//phase1->next = make_shared<Stage_Node>(Stage_Node(5));          //嵌套相位有两个时
	//phase1->next->next = make_shared<Stage_Node>(Stage_Node(2));    
	phases_Overlap.emplace(1, phase1);

	shared_ptr<Stage_Node> phase6 (new Stage_Node(6));
	phase6->next = make_shared<Stage_Node>(Stage_Node(2));            //嵌套相位只有1个时
	//phase6->next = make_shared<Stage_Node>(Stage_Node(5));          //嵌套相位有两个时
	//phase6->next->next = make_shared<Stage_Node>(Stage_Node(2));
	phases_Overlap.emplace(6, phase6);

	shared_ptr<Stage_Node> phase3 (new Stage_Node(3));                
	phase3->next = make_shared<Stage_Node>(Stage_Node(7));            //嵌套相位只有1个时
	//phase3->next = make_shared<Stage_Node>(Stage_Node(7));          //嵌套相位有两个时
	//phase3->next->next = make_shared<Stage_Node>(Stage_Node(4));
	phases_Overlap.emplace(3, phase3);

	shared_ptr<Stage_Node> phase8 (new Stage_Node(8));
	phase8->next = make_shared<Stage_Node>(Stage_Node(4));            //嵌套相位只有1个时
	//phase8->next = make_shared<Stage_Node>(Stage_Node(7));          //嵌套相位有两个时
	//phase8->next->next = make_shared<Stage_Node>(Stage_Node(4));
	phases_Overlap.emplace(8, phase8);
	return;
}

void Node_Adaptive_Control::get_Phases_Sequence_Info() {            //执行固定或者可变的相序下的相位屏障（屏障位置通过环1的相位确定）                                        
	shared_ptr<Phase_Node> phase0(new Phase_Node(0, false));        //默认的0相位

	shared_ptr<Phase_Node> phase1 (new Phase_Node(1, true));        //嵌套相位只有1个时，每个相位都是屏障相位
	//shared_ptr<Phase_Node> phase1(new Phase_Node(1, false));      //嵌套相位有2个时，且同一进口左转和直行分开放行下，只设置后放行的相位为屏障相位
	shared_ptr<Phase_Node> phase6 (new Phase_Node(6, true));        
	phase1->next = phase6;

	shared_ptr<Phase_Node> phase3 (new Phase_Node(3, true));       //嵌套相位只有1个时，每个相位都是屏障相位
	//shared_ptr<Phase_Node> phase3 (new Phase_Node(3, false));    //嵌套相位有2个时，且同一进口左转和直行分开放行下，只设置后放行的相位为屏障相位
	phase6->next = phase3;

	shared_ptr<Phase_Node> phase8 (new Phase_Node(8, true));
	phase3->next = phase8;

	phases_Sequence.emplace_back(phase1);

	for (int i = 0; i < phases_Sequence.size(); i++) {
		phases_Sequence_Modified.emplace_back(make_shared<Phase_Node>(0, false));
	}
	return;
}

void Node_Adaptive_Control::get_Node_Index_Info() {
	node_Index.entrance_Links_Index.emplace(0,Link_Index(0, 1));
	node_Index.entrance_Links_Index.emplace(2,Link_Index(2, 1));
	node_Index.entrance_Links_Index.emplace(4,Link_Index(4, 1));
	node_Index.entrance_Links_Index.emplace(6,Link_Index(6, 1));
	get_Phases_Index_Info();                                                                                                  //更新交叉口所有相位的信息phases_Index
	return;
}

void Node_Adaptive_Control::get_Phases_Index_Info() {
	for (auto it_link = node_Index.entrance_Links_Index.begin(); it_link != node_Index.entrance_Links_Index.end(); it_link++) {    
		map<int, Lane_Index> lanes_temp = it_link->second.lanes_Index;
		for (auto it_lane = lanes_temp.begin(); it_lane != lanes_temp.end(); it_lane++) {
			phases_Index[it_lane->second.phase_Id].phase_Lanes.emplace_back(it_lane->second);                                 //将车道的指标、相位信息分别存入相位的车道集合和相位变量中
			phases_Index[it_lane->second.phase_Id].phase_Info = it_lane->second.lane_Phase_Info;
			phases_Index[it_lane->second.phase_Id].phase_Id = it_lane->second.phase_Id;
		}
	}
	for (auto it_phase = phases_Index.begin(); it_phase != phases_Index.end(); it_phase++) {
		it_phase->second.initial_Phase_Index_Caculation();
		it_phase->second.initial_Demand_Caculation();
		it_phase->second.Pedestrian_Time_Caculation();
	}
	return;
}

void Node_Adaptive_Control::adjust_Cycle_Time(const int cycle_Time) {
	for (auto& it_phase : phases_Index) {
		it_phase.second.phase_Info.intersection_Signal_Controller.signal_Cycle_Time = cycle_Time;
	}
	return;
}

void Node_Adaptive_Control::initial_Phases_Green_Time(const shared_ptr<Phase_Node>& mphase_Sequence, shared_ptr<Phase_Node>& mphase_Sequence_Modified, int& cycle_Time) {                      //给定数据、相序、周期时长等，计算绿灯初始化值
	shared_ptr<Phase_Node> phase_Head = mphase_Sequence;
	shared_ptr<Phase_Node> mphase_Modified_Temp = mphase_Sequence_Modified;
	while (phase_Head) {
		int ring1_phase = phase_Head->phase_Id;                                                                               //环1的相位编号
		shared_ptr<Stage_Node> stage_Head = phases_Overlap[ring1_phase];

		while (stage_Head->next) {
			int ring2_phase = stage_Head->next->phase_Id;                                                                    //环2的相位编号
			double max_volume = max(phases_Index[ring1_phase].initial_Demand, phases_Index[ring2_phase].initial_Demand), min_volume = min(phases_Index[ring1_phase].initial_Demand, phases_Index[ring2_phase].initial_Demand);
			stage_Head = stage_Head->next;
			if (min_volume == 0.0 && !(phase_Head->barrier_Flag && !stage_Head->next))                                       //双环相位除屏障相位外，如果最小流量为零，则按流量搭接下一个有效的相位
				continue;                                                                                                    

			int green_Time = 0;
			if (abs(phases_Index[ring1_phase].initial_Demand - phases_Index[ring2_phase].initial_Demand) / max(max_volume,1.0) <= stage_Volume_Diff || (phase_Head->barrier_Flag && !stage_Head->next)) {        //嵌套相位流量均衡，或者双环都处于屏障相位
				green_Time = floor(max(max_volume,0.01) * phases_Index[ring1_phase].time_Headway_Saturation);                 //max(,0.01)是避免绿灯为零的异常
				phases_Index[ring1_phase].initial_Demand = max(phases_Index[ring1_phase].initial_Demand - max_volume,0.0);    //计算各相位的剩余需求
				phases_Index[ring2_phase].initial_Demand = max(phases_Index[ring1_phase].initial_Demand - max_volume, 0.0);
			}else {
				green_Time = floor(min_volume * phases_Index[ring1_phase].time_Headway_Saturation);
				phases_Index[ring1_phase].initial_Demand -= min_volume;                                                       //计算各相位的剩余需求
				phases_Index[ring2_phase].initial_Demand -= min_volume;
			}
			phases_Index[ring1_phase].phase_Info.green_Time += green_Time;                                                    //环1的当前相位绿灯时长
			phases_Index[ring2_phase].phase_Info.green_Time += green_Time;                                                    //环2的当前相位绿灯时长
			mphase_Sequence_Modified->next = make_shared<Phase_Node>(ring1_phase);                                            //将相序添加到phases_Sequence_Modified中
			mphase_Sequence_Modified->next->down = make_shared<Phase_Node>(ring2_phase);
			mphase_Sequence_Modified = mphase_Sequence_Modified->next;
			cycle_Time += green_Time + phases_Index[ring1_phase].phase_Info.yellow_Time + phases_Index[ring1_phase].phase_Info.all_Red_Time;
		}
		phase_Head = phase_Head->next;
	}
	mphase_Sequence_Modified = mphase_Modified_Temp->next;
	return;
}

void Node_Adaptive_Control::modify_Clearance_Ratio(shared_ptr<Phase_Node>& mphase_Sequence_Modified, const double ratio) {
	double ratio_Sum_Increase = 0.0, ratio_Sum_Decrease = 0.0;
	shared_ptr<Phase_Node> phase_Head = mphase_Sequence_Modified;
	vector<int> phases_ID;                                                                                                                                                     //存储双环的相位编号
	while (phase_Head) {
		int ring1_phase = phase_Head->phase_Id;                                                                                                                                //环1的相位编号
		phases_ID.emplace_back(ring1_phase);
		if (phase_Head->down) {
			int ring2_phase = phase_Head->down->phase_Id;                                                                                                                      //环2的相位编号
			phases_ID.emplace_back(ring2_phase);
		}
		for (auto mphase_ID : phases_ID) {
			if (!phases_Index[mphase_ID].priority_Right) {                                                                                                                      //非优先相位
				if (phases_Index[mphase_ID].phase_Info.green_Time * ratio < phases_Index[mphase_ID].green_Time_Pedestrian) {
					phases_Index[mphase_ID].phase_Clearance_Ratio *= phases_Index[mphase_ID].green_Time_Pedestrian / (phases_Index[mphase_ID].phase_Info.green_Time * ratio);   //当小于最小绿灯相位，调高清空比例，保证后续大于等于最小绿灯
				}
				ratio_Sum_Decrease += phases_Index[mphase_ID].phase_Clearance_Ratio;
			}
			else {
				if (phases_Index[mphase_ID].phase_Info.green_Time < phases_Index[mphase_ID].green_Time_Pedestrian)
					phases_Index[mphase_ID].phase_Clearance_Ratio *= phases_Index[mphase_ID].green_Time_Pedestrian / phases_Index[mphase_ID].phase_Info.green_Time;
			}
			ratio_Sum_Increase += phases_Index[mphase_ID].phase_Clearance_Ratio;
		}
		phase_Head = phase_Head->next;
		phases_ID.clear();
	}

	phase_Head = mphase_Sequence_Modified;
	while (phase_Head) {
		int ring1_phase = phase_Head->phase_Id;
		int ring2_phase = phase_Head->down->phase_Id;
		if (ratio > 1.0) {
			phases_Index[ring1_phase].phase_Clearance_Ratio *= ratio;
			phases_Index[ring2_phase].phase_Clearance_Ratio *= ratio;
		}else {
			if (!phases_Index[ring1_phase].priority_Right)
				phases_Index[ring1_phase].phase_Clearance_Ratio *= (ratio * ratio_Sum_Decrease / ratio_Sum_Increase);
			if (!phases_Index[ring1_phase].priority_Right)
				phases_Index[ring2_phase].phase_Clearance_Ratio *= (ratio * ratio_Sum_Decrease / ratio_Sum_Increase);
		}
		phase_Head = phase_Head->next;
	}
	return;
}

void Node_Adaptive_Control::update_Phase_Index_Info() {
	for (auto& it : phases_Index) {
		it.second.initial_Demand_Caculation();
	}
	return;
}

bool Node_Adaptive_Control::update_Optimal_Phases(const shared_ptr<Phase_Node>& mphase_Sequence, map<int, Phase_Index>& local_Phases_Index, double& local_Min_Delay, double& cycle_Min_Delay) {
	double total_Delay = 0.0;
	int moment_Of_Cycle = 0;
	phase_Delay_Caculation(mphase_Sequence, moment_Of_Cycle, total_Delay);
	adjust_Cycle_Time(moment_Of_Cycle/2);                                                                                                  //调整周期时长的值等于各相位绿灯时长的累加
	printf("delay: %f\n", total_Delay);
	printf("\n");
	if (total_Delay < local_Min_Delay) {
		local_Min_Delay = total_Delay;
		local_Phases_Index = phases_Index;
	}
	cycle_Min_Delay = (total_Delay < cycle_Min_Delay) ? total_Delay : cycle_Min_Delay;
	if (total_Delay < min_Delay) {
		optimal_Phase_Sequence = mphase_Sequence;
		optimal_Phase_Scheme = phases_Index;
		optimal_Cycle_Time = (*optimal_Phase_Scheme.begin()).second.phase_Info.intersection_Signal_Controller.signal_Cycle_Time;
		min_Delay = total_Delay;
		return true;
	}
	return false;
}

void Node_Adaptive_Control::phase_Delay_Caculation(const shared_ptr<Phase_Node> head, int& moment_Of_Cycle, double& total_Delay) { 
	if (head == nullptr)                                                                                                                       
		return;
	int lane_Num = phases_Index[head->phase_Id].phase_Lanes.size();
	total_Delay += phases_Index[head->phase_Id].delay_Vehicles_Start;                                                                //车辆启动延误(以平均车道计算，不考虑车道数）

    phases_Index[head->phase_Id].delay_Queue_Clearance =  queue_Delay_Value(head->phase_Id);                                                      
	total_Delay += phases_Index[head->phase_Id].delay_Queue_Clearance;                                                               //车辆清空延误,根据车辆类型和延误系数，计算延误(以平均车道计算，不考虑车道数）

	phases_Index[head->phase_Id].delay_Red_Stop = red_Stop_Delay_Value(head->phase_Id, moment_Of_Cycle);
	total_Delay += phases_Index[head->phase_Id].delay_Red_Stop;                                                                      //红灯停车延误(以平均车道计算，不考虑车道数）

	phase_Delay_Caculation(head->down, moment_Of_Cycle, total_Delay);       
	phase_Delay_Caculation(head->next, moment_Of_Cycle, total_Delay);                                                                //递归计算所有相位的延误
	moment_Of_Cycle += phases_Index[head->phase_Id].phase_Info.green_Time + phases_Index[head->phase_Id].phase_Info.yellow_Time + phases_Index[head->phase_Id].phase_Info.all_Red_Time;
	return;
}

Tree_Stage_Node* Node_Adaptive_Control::build_Tree(Tree_Stage_Node* head, const shared_ptr<Phase_Node>& mphase_Sequence, const shared_ptr<Stage_Node>& mstage_Overlap) {                     //除基本参数和初始化的清空比例外，相位参数一切归零，建立决策树
	if (!mphase_Sequence || !mstage_Overlap) 
		return nullptr;
	int ring1_phase = mphase_Sequence->phase_Id;
	int ring2_phase = mstage_Overlap->phase_Id;
	if (head == nullptr) 
		head = new Tree_Stage_Node(ring1_phase, ring2_phase);

	double max_volume = max(phases_Index[ring1_phase].initial_Demand, phases_Index[ring2_phase].initial_Demand);
	double min_volume = min(phases_Index[ring1_phase].initial_Demand, phases_Index[ring2_phase].initial_Demand);
	double temp = phases_Index[ring1_phase].initial_Demand - phases_Index[ring2_phase].initial_Demand;
	if (min_volume == 0.0 && max_volume == 0.0)
	   return nullptr;

	int green_Time = 0;
	if (abs(temp / max(max_volume,1.0)) <= stage_Volume_Diff || (mphase_Sequence->barrier_Flag && !mstage_Overlap->next)) {           //max(,0.01)是避免绿灯为零的异常 
		green_Time = floor(max(max_volume, 0.01) * phases_Index[ring1_phase].time_Headway_Saturation);
		phases_Index[ring1_phase].initial_Demand = max(phases_Index[ring1_phase].initial_Demand - max_volume, 0.0);                   //计算各相位的剩余需求
		phases_Index[ring2_phase].initial_Demand = max(phases_Index[ring1_phase].initial_Demand - max_volume, 0.0);
	}else {
		green_Time = floor(min_volume * phases_Index[ring1_phase].time_Headway_Saturation);
		phases_Index[ring1_phase].initial_Demand -= min_volume;
		phases_Index[ring2_phase].initial_Demand -= min_volume;
	}
	head->ring1_Phase_Info.green_Time = green_Time;
	head->ring2_Phase_Info.green_Time = green_Time;

	if (abs(temp/max(max_volume,1.0)) > stage_Volume_Diff && mstage_Overlap->next)
		head->left_Tree = build_Tree(head->left_Tree, mphase_Sequence, mstage_Overlap->next);
	if (mphase_Sequence->next) 
		head->right_Tree = build_Tree(head->right_Tree, mphase_Sequence->next, phases_Overlap[mphase_Sequence->next->phase_Id]->next);
	return head;
}

void Node_Adaptive_Control::tree_Phase_Sequence(Tree_Stage_Node* head, vector<Tree_Stage_Node*>& tree_Node, vector<vector<Tree_Stage_Node*>>& tree_Phases_Set) {
	if (!head)
		return;
	tree_Node.emplace_back(head);
	if (!head->left_Tree && !head->right_Tree) {
		tree_Phases_Set.emplace_back(tree_Node);
		return;
	}
	
	tree_Phase_Sequence(head->left_Tree, tree_Node, tree_Phases_Set);
	tree_Phase_Sequence(head->right_Tree, tree_Node, tree_Phases_Set);
	tree_Node.pop_back();
	return;
}

vector<shared_ptr<Phase_Node>> Node_Adaptive_Control::to_Phase_Node(const vector<vector<Tree_Stage_Node*>>& tree_Phases_Set) {
	vector<shared_ptr<Phase_Node>> result;
	for (int i = 0; i < tree_Phases_Set.size(); i++) {
		shared_ptr<Phase_Node> blank_Node = make_shared<Phase_Node>();                                                       //哨兵节点
		shared_ptr<Phase_Node> temp = blank_Node;
		for (int j = 0; j < tree_Phases_Set[i].size(); j++) {
			if (tree_Phases_Set[i][j]->ring1_Phase_Info.green_Time == 0)
				continue;
			temp->next = make_shared<Phase_Node>(tree_Phases_Set[i][j]->ring1_Phase_Id);
			temp = temp->next;
			temp->phase_Info = tree_Phases_Set[i][j]->ring1_Phase_Info;
			temp->down = make_shared<Phase_Node>(tree_Phases_Set[i][j]->ring2_Phase_Id);
			temp->down->phase_Info = tree_Phases_Set[i][j]->ring2_Phase_Info;
		}
		result.emplace_back(blank_Node->next);
	}
	return result;
}

void Node_Adaptive_Control::modify_Phase_Green_Time(Tree_Stage_Node* head, double& cycle_Min_Delay) {
	vector<vector<Tree_Stage_Node*>> tree_Phases_Set;                                                            
	vector<Tree_Stage_Node*> tree_Node;
	tree_Phase_Sequence(head, tree_Node, tree_Phases_Set);
	vector<shared_ptr<Phase_Node>> phases_Set = to_Phase_Node(tree_Phases_Set);
	update_Phase_Index_Info();                                                                                                //重新计算相位的流量需求
	map<int, Phase_Index> mphases_Index = copy_Phases_Index();
	
	bool flag = false;
	for (int i = 0; i < phases_Set.size(); i++) {                                  
		shared_ptr<Phase_Node> temp_Tail = phases_Set[i];                                                                     //记录链表尾部，用于创建循环链表
		int phase_Sequence_Nums = 1;                                                                                          //记录相位阶段的个数                                                                                           //记录信号周期长度
		while (temp_Tail->next) {
			temp_Tail = temp_Tail->next;
			phase_Sequence_Nums++;
		}

		shared_ptr<Phase_Node> temp_Head = phases_Set[i];
		while (temp_Head) {                                                                                                    //迭代相序中的相位
			map<int, Phase_Index> local_Phases_Index;                                                                          //局部最优的相位方案
			double local_Min_Delay = FLT_MAX;
			int min_Green = static_cast<int>(min(phases_Index[temp_Head->phase_Id].green_Time_Pedestrian, phases_Index[temp_Head->down->phase_Id].green_Time_Pedestrian)) + delta_Green;
			while (phases_Index[temp_Head->phase_Id].phase_Info.green_Time >= min_Green && !(phases_Index[temp_Head->phase_Id].priority_Right || phases_Index[temp_Head->down->phase_Id].priority_Right)) {                       
				phases_Index[temp_Head->phase_Id].phase_Info.green_Time -= delta_Green;                                        //不断减少当前相位的绿灯
				phases_Index[temp_Head->down->phase_Id].phase_Info.green_Time -= delta_Green;

				temp_Tail->next = phases_Set[i];
				shared_ptr<Phase_Node> backtrack_Head = temp_Head->next;                                                       //试探性回溯的方式，将delta_Green分别加到后续阶段的相位中
				int invalid_Nums = 0;                                                                                          //delta_Green依次增加到后续相位后，delay > min_Delay的个数
				while (backtrack_Head && (backtrack_Head->phase_Id != temp_Head->phase_Id && backtrack_Head->down->phase_Id != temp_Head->down->phase_Id)) {     //执行循环链表的每个相位
					phases_Index[backtrack_Head->phase_Id].phase_Info.green_Time += delta_Green;
					phases_Index[backtrack_Head->down->phase_Id].phase_Info.green_Time += delta_Green;

					temp_Tail->next = nullptr;                                                                                  //将循环链表重置为单向链表
					flag = update_Optimal_Phases(phases_Set[i], local_Phases_Index, local_Min_Delay, cycle_Min_Delay);          //保存延误最小的相序和相位方案
					if (!flag) {                                                                                 
						invalid_Nums++;
					}	
					temp_Tail->next = phases_Set[i];

					phases_Index[backtrack_Head->phase_Id].phase_Info.green_Time -= delta_Green;
					phases_Index[backtrack_Head->down->phase_Id].phase_Info.green_Time -= delta_Green;
					backtrack_Head = backtrack_Head->next;
				}
				phases_Index = local_Phases_Index;                                                                              //在局部最优的相位方案基础上，按步长减少当前相位的绿灯
				temp_Tail->next = nullptr;
				if (invalid_Nums == phase_Sequence_Nums - 1)                                                                    //贪婪方式跳出当前，执行下个相位阶段的绿灯调整
					break;
			}
			phases_Index = mphases_Index;
			temp_Head = temp_Head->next;
			if (temp_Head == phases_Set[i])
				break;
		}	

		if (cycle_Min_Delay == FLT_MAX) {                                                                                       //所有相位以最小绿执行时，直接得出相位参数
			map<int, Phase_Index> local_Phases_Index;                                                                  
			double local_Min_Delay = FLT_MAX;
		    update_Optimal_Phases(phases_Set[i], local_Phases_Index, local_Min_Delay, cycle_Min_Delay);                         //保存延误最小的相序和相位方案
		}
	}
	return;
}

double Node_Adaptive_Control::cycle_Delay_Caculation(const int cycle_Time) {
	double local_Min_Delay = FLT_MAX;
	printf("cycle_Time: %d \n", cycle_Time);
	for (int i = 0; i < phases_Sequence.size(); i++) {
		Tree_Stage_Node* temp_Head;
		int cycle_Time_Initial = 0;
		map<int, Phase_Index> phases_Index_Temp = copy_Phases_Index();                                                          //回溯phases_Index

		for (auto it = phases_Index.begin(); it != phases_Index.end(); it++) {                                                         
			it->second.phase_Info.intersection_Signal_Controller.signal_Cycle_Time = cycle_Time;
			it->second.initial_Demand_Caculation();
		}
		initial_Phases_Green_Time(phases_Sequence[i], phases_Sequence_Modified[i], cycle_Time_Initial);                                

		while (abs(cycle_Time_Initial - cycle_Time) >= 2) {
			modify_Clearance_Ratio(phases_Sequence_Modified[i], cycle_Time * 1.0 / cycle_Time_Initial);                         //初始化相位的清空比例
			update_Phase_Index_Info();
			for (auto it = phases_Index.begin(); it != phases_Index.end(); it++) {
				it->second.phase_Info.green_Time = phases_Index_Temp[it->first].phase_Info.green_Time;
			}
			cycle_Time_Initial = 0;
			initial_Phases_Green_Time(phases_Sequence[i], phases_Sequence_Modified[i], cycle_Time_Initial);
		}
		
		update_Phase_Index_Info();                                                                                              //根据清空比例，调整各相位的初始需求
		
		temp_Head = build_Tree(nullptr, phases_Sequence[i], phases_Overlap[phases_Sequence[i]->phase_Id]->next);                //决策树的建立

		modify_Phase_Green_Time(temp_Head, local_Min_Delay);                                                                    //相位时长优化

		phases_Index = phases_Index_Temp;
	}
	printf("minimum delay: %f \n", local_Min_Delay);
	return local_Min_Delay;
}

void Node_Adaptive_Control::implement_Node_Control_Function() {                                                                       
	if (abs(cycle_Time_Lower - cycle_Time_Upper) < 2) {
		min_Delay = cycle_Delay_Caculation(cycle_Time_Upper);  
		return;
	}
	int optimal_Lower = cycle_Time_Lower, optimal_Upper = cycle_Time_Lower + (cycle_Time_Upper - cycle_Time_Lower) / 2;
	int suboptimal_Lower = optimal_Upper + 1, suboptimal_Upper = cycle_Time_Upper;                                                 //次优区间，减少陷入局部最优的几率
	double total_Delay = 0.0;
	while ((optimal_Upper - optimal_Lower) >= 2 ) {
		int optimal_Mid = optimal_Lower + (optimal_Upper - optimal_Lower) / 2;
		int suboptimal_Mid = suboptimal_Lower + (suboptimal_Upper - suboptimal_Lower) / 2;
		double sub_total_Delay = cycle_Delay_Caculation(suboptimal_Mid);
		if (sub_total_Delay < total_Delay) {                                                                                       //交换搜索区间
			swap(optimal_Lower, suboptimal_Lower);
			swap(optimal_Upper, suboptimal_Upper);
			swap(optimal_Mid, suboptimal_Mid);
			total_Delay = sub_total_Delay;
		}else
		    total_Delay = cycle_Delay_Caculation(optimal_Mid);

		if (cycle_Delay_Caculation(optimal_Lower) < cycle_Delay_Caculation(optimal_Upper))                                        //二分搜索区间，沿下降最快的区间搜索
			optimal_Upper = optimal_Mid - 1;
		else 
			optimal_Lower = optimal_Mid + 1;
		
		if (cycle_Delay_Caculation(suboptimal_Lower) < cycle_Delay_Caculation(suboptimal_Upper))                                  //同步更新次优区间
			suboptimal_Upper = suboptimal_Mid - 1;
		else 
			suboptimal_Lower = suboptimal_Mid + 1;
	}

	return;
}

void Node_Adaptive_Control::put_Control_Delivery() {
	
	return;
}

void Node_Adaptive_Control::update_Node_Index_Info() {
	for (auto it = node_Index.entrance_Links_Index.begin(); it != node_Index.entrance_Links_Index.end(); it++) {                      //暂时清空所有的结果，后续可优化变量的动态分配规则
		it->second.lanes.clear();
		it->second.lanes_Index.clear();
		it->second.link_Movements_Index.clear();
		it->second.get_Link_Index_Info();
	}              
	optimal_Phase_Scheme.clear();
	phases_Index.clear();
	min_Delay = FLT_MAX;

	delete_Tree_Node(optimal_Head);
	return;
}

void Node_Adaptive_Control::delete_Tree_Node(Tree_Stage_Node* root) {
	if (!root)
		return;
	delete_Tree_Node(root->left_Tree);
	delete_Tree_Node(root->right_Tree);
	delete root; root = nullptr;
	return;
}


