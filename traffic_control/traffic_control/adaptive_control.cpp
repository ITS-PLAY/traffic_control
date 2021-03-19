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
	volume_Interval /= num; volume /= num;                                              //流量取平均值
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
	initial_Demand = max(phase_Clearance_Ratio * (1.0F * volume_Interval / Time_Interval * phase_Info.intersection_Signal_Controller.signal_Cycle_Time + queue_Num), 1.0);       //每一相位的initial_Demand至少设置一辆车
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
	int green_Time_Effctive = phases_Index[phase_Id].phase_Info.green_Time + phases_Index[phase_Id].phase_Info.yellow_Time;                          //有效绿灯时长
	if (phases_Index[phase_Id].queue_Num > 0)
		ratio = ((phases_Index[phase_Id].queue_Num - phases_Index[phase_Id].queue_Transit) + phases_Index[phase_Id].queue_Transit * car_Delay_Ratio.at("transit")) / phases_Index[phase_Id].queue_Num;
	printf("phase_Id: %d , queue_num: %f \n", phase_Id, phases_Index[phase_Id].initial_Demand * phases_Index[phase_Id].time_Headway_Saturation - phases_Index[phase_Id].phase_Info.green_Time);
	return ratio * (phases_Index[phase_Id].initial_Demand * moment_Of_Cycle +
		max(0.0, phases_Index[phase_Id].initial_Demand * phases_Index[phase_Id].time_Headway_Saturation - phases_Index[phase_Id].phase_Info.green_Time) * (phases_Index[phase_Id].phase_Info.intersection_Signal_Controller.signal_Cycle_Time - green_Time_Effctive));
}

void Node_Adaptive_Control::get_Phases_Overlap_Info() {             //固定相序下的相位嵌套组合
	shared_ptr<Stage_Node> phase1 (new Stage_Node(1));
	phase1->next = make_shared<Stage_Node>(Stage_Node(5));
	//phase1->next->next = make_shared<Stage_Node>(Stage_Node(2));
	phases_Overlap.emplace(1, phase1);

	shared_ptr<Stage_Node> phase6 (new Stage_Node(6));
	phase6->next = make_shared<Stage_Node>(Stage_Node(2));
	phases_Overlap.emplace(6, phase6);

	shared_ptr<Stage_Node> phase3 (new Stage_Node(3));
	phase3->next = make_shared<Stage_Node>(Stage_Node(7));
	phases_Overlap.emplace(3, phase3);

	shared_ptr<Stage_Node> phase8 (new Stage_Node(8));
	phase8->next = make_shared<Stage_Node>(Stage_Node(4));
	phases_Overlap.emplace(8, phase8);
	return;
}

void Node_Adaptive_Control::get_Phases_Sequence_Info() {            //执行固定的相序,                                        
	shared_ptr<Phase_Node> phase0(new Phase_Node(0, false));        //默认的0相位

	shared_ptr<Phase_Node> phase1 (new Phase_Node(1, false));
	shared_ptr<Phase_Node> phase6 (new Phase_Node(6, true));
	shared_ptr<Phase_Node> phase5(new Phase_Node(5, false));
	phase1->next = phase6;
	phase1->down = phase5;

	shared_ptr<Phase_Node> phase3 (new Phase_Node(3, false));
	shared_ptr<Phase_Node> phase2(new Phase_Node(2, true));
	phase6->next = phase3;
	phase6->down = phase2;

	shared_ptr<Phase_Node> phase8 (new Phase_Node(8, true));
	shared_ptr<Phase_Node> phase7(new Phase_Node(7, false));
	phase3->next = phase8;
	phase3->down = phase7;

	shared_ptr<Phase_Node> phase4(new Phase_Node(4, false));
	phase8->down = phase4;

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
	get_Phases_Index_Info();                                                                                    //更新交叉口所有相位的信息phases_Index
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

void Node_Adaptive_Control::initial_Phases_Green_Time(const shared_ptr<Phase_Node>& mphase_Sequence, shared_ptr<Phase_Node>& mphase_Sequence_Modified, int& cycle_Time) {                      //给定数据、相序、周期时长等，计算绿灯初始化值
	shared_ptr<Phase_Node> phase_Head = mphase_Sequence;
	shared_ptr<Phase_Node> mphase_Modified_Temp = mphase_Sequence_Modified;
	while (phase_Head) {
		int ring1_phase = phase_Head->phase_Id;                                                       //环1的相位编号
		shared_ptr<Stage_Node> stage_Head = phases_Overlap[phase_Head->phase_Id];

		while (stage_Head->next) {
			int ring2_phase = stage_Head->next->phase_Id;                                            //环2的相位编号
			double max_volume = max(phases_Index[ring1_phase].initial_Demand, phases_Index[ring2_phase].initial_Demand), min_volume = min(phases_Index[ring1_phase].initial_Demand, phases_Index[ring2_phase].initial_Demand);
			int green_Time = 0;
			if (abs(phases_Index[ring1_phase].initial_Demand - phases_Index[ring2_phase].initial_Demand) / max_volume <= stage_Volume_Diff || min_volume == 0 || !stage_Head->next->next) {
				green_Time = static_cast<int>(max_volume * phases_Index[ring1_phase].time_Headway_Saturation);
				phases_Index[ring1_phase].phase_Info.green_Time += green_Time;                       //环1的当前相位绿灯时长
				phases_Index[ring2_phase].phase_Info.green_Time += green_Time;                       //环2的当前相位绿灯时长
				mphase_Sequence_Modified->next = make_shared<Phase_Node>(ring1_phase);                     //将相序添加到phases_Sequence_Modified中
				mphase_Sequence_Modified->next->down = make_shared<Phase_Node>(ring2_phase);
				mphase_Sequence_Modified = mphase_Sequence_Modified->next;
				cycle_Time += green_Time + phases_Index[ring1_phase].phase_Info.yellow_Time + phases_Index[ring1_phase].phase_Info.all_Red_Time;
				break;
			}
			green_Time = static_cast<int>(min_volume * phases_Index[ring1_phase].time_Headway_Saturation);
			phases_Index[ring1_phase].phase_Info.green_Time += green_Time;
			phases_Index[ring2_phase].phase_Info.green_Time += green_Time;
			mphase_Sequence_Modified = make_shared<Phase_Node>(ring1_phase);
			mphase_Sequence_Modified->next = make_shared<Phase_Node>(ring2_phase);
			mphase_Sequence_Modified = mphase_Sequence_Modified->next;
			cycle_Time += green_Time + phases_Index[ring1_phase].phase_Info.yellow_Time + phases_Index[ring1_phase].phase_Info.all_Red_Time;

			int ring_Num_Main = (phases_Index[ring1_phase].initial_Demand == min_volume) ? ring1_phase : ring2_phase;
			phases_Index[ring_Num_Main].initial_Demand -= min_volume;                                //计算主要相位的剩余需求
			if (ring1_phase == ring_Num_Main)
				break;                                                                               //跳转环1的下一相位
			stage_Head = stage_Head->next;
		}
		phase_Head = phase_Head->next;
	}
	mphase_Sequence_Modified = mphase_Modified_Temp->next;
	return;
}

void Node_Adaptive_Control::modify_Cycle_Time(shared_ptr<Phase_Node>& mphase_Sequence_Modified, const double ratio, const int cycle_Time) {                   
	double ratio_Sum_Increase = 0.0, ratio_Sum_Decrease = 0.0;
	shared_ptr<Phase_Node> phase_Head = mphase_Sequence_Modified;
	while (phase_Head) {
		int ring1_phase = phase_Head->phase_Id;
		int ring2_phase = phase_Head->down->phase_Id;
		if (!phases_Index[ring1_phase].priority_Right) {
			if (phases_Index[ring1_phase].phase_Info.green_Time * ratio < phases_Index[ring1_phase].green_Time_Pedestrian) {                                                     //非优先相位
				phases_Index[ring1_phase].phase_Clearance_Ratio *= phases_Index[ring1_phase].green_Time_Pedestrian / (phases_Index[ring1_phase].phase_Info.green_Time * ratio);   //当小于最小绿灯相位，调高清空比例，保证后续大于等于最小绿灯
			}
			ratio_Sum_Decrease += phases_Index[ring1_phase].phase_Clearance_Ratio;
		}
		if (!phases_Index[ring2_phase].priority_Right) {
			if (phases_Index[ring2_phase].phase_Info.green_Time * ratio < phases_Index[ring2_phase].green_Time_Pedestrian) {        
				phases_Index[ring2_phase].phase_Clearance_Ratio *= phases_Index[ring2_phase].green_Time_Pedestrian / (phases_Index[ring2_phase].phase_Info.green_Time * ratio);
			}
			ratio_Sum_Decrease += phases_Index[ring2_phase].phase_Clearance_Ratio;
		}	
		ratio_Sum_Increase += phases_Index[ring1_phase].phase_Clearance_Ratio + phases_Index[ring2_phase].phase_Clearance_Ratio;
		phase_Head = phase_Head->next;
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

bool Node_Adaptive_Control::update_Optimal_Phases(const shared_ptr<Phase_Node>& mphase_Sequence, double& local_Min_Delay) {
	double total_Delay = 0.0;
	int moment_Of_Cycle = 0;
	phase_Delay_Caculation(mphase_Sequence, moment_Of_Cycle, total_Delay);
	local_Min_Delay = (total_Delay < local_Min_Delay) ? total_Delay : local_Min_Delay;
	if (total_Delay < min_Delay) {
		optimal_Phase_Sequence = mphase_Sequence;
		optimal_Phase_Scheme = phases_Index;
		min_Delay = total_Delay;
		return true;
	}
	return false;
}

void Node_Adaptive_Control::phase_Delay_Caculation(const shared_ptr<Phase_Node> head, int& moment_Of_Cycle, double& total_Delay) { 
	if (head == nullptr)                                                                                                                       
		return;
	int lane_Num = phases_Index[head->phase_Id].phase_Lanes.size();
	total_Delay += phases_Index[head->phase_Id].delay_Vehicles_Start * lane_Num;                                                                //车辆启动延误

    phases_Index[head->phase_Id].delay_Queue_Clearance =  queue_Delay_Value(head->phase_Id);                                                      
	total_Delay += phases_Index[head->phase_Id].delay_Queue_Clearance * lane_Num;                                                               //车辆清空延误,根据车辆类型和延误系数，计算延误

	phases_Index[head->phase_Id].delay_Red_Stop = red_Stop_Delay_Value(head->phase_Id, moment_Of_Cycle);
	total_Delay += phases_Index[head->phase_Id].delay_Red_Stop * lane_Num;                                                                      //红灯停车延误

	phase_Delay_Caculation(head->down, moment_Of_Cycle, total_Delay);       
	phase_Delay_Caculation(head->next, moment_Of_Cycle, total_Delay);                                                                           //递归计算所有相位的延误
	moment_Of_Cycle += phases_Index[head->phase_Id].phase_Info.green_Time + phases_Index[head->phase_Id].phase_Info.yellow_Time;
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
	phases_Index[ring1_phase].initial_Demand -= min_volume;
	phases_Index[ring2_phase].initial_Demand -= min_volume;
	if (abs(temp / max_volume) <= stage_Volume_Diff || min_volume == 0 || !mstage_Overlap->next) {
		int green_time = static_cast<int>(max_volume * phases_Index[ring1_phase].time_Headway_Saturation);
		head->ring1_Phase_Info.green_Time = green_time;
		head->ring2_Phase_Info.green_Time = green_time;
	}else {
		int green_time = static_cast<int>(min_volume * phases_Index[ring1_phase].time_Headway_Saturation);
		head->ring1_Phase_Info.green_Time = green_time;
		head->ring2_Phase_Info.green_Time = green_time;
	}

	if (abs(temp/max_volume) > stage_Volume_Diff && mstage_Overlap->next)
		head->left_Tree = build_Tree(head->left_Tree, mphase_Sequence, mstage_Overlap->next);
	if (mphase_Sequence->next) 
		head->right_Tree = build_Tree(head->right_Tree, mphase_Sequence->next, phases_Overlap[mphase_Sequence->next->phase_Id]->next);
	return head;
}

void Node_Adaptive_Control::tree_Phase_Sequence(Tree_Stage_Node* head, vector<Tree_Stage_Node*>& tree_Node, vector<vector<Tree_Stage_Node*>>& tree_Phases_Set) {
	if (!head)
		return;
	if (!head->right_Tree) {
		tree_Node.emplace_back(head);
		if (head->left_Tree) {
			tree_Node.emplace_back(head->left_Tree);
		}
		tree_Phases_Set.emplace_back(tree_Node);
		return;
	}
	tree_Node.emplace_back(head);
	vector<Tree_Stage_Node*> temp = tree_Node;
	tree_Phase_Sequence(head->left_Tree, tree_Node, tree_Phases_Set);
	tree_Node = temp;
	tree_Phase_Sequence(head->right_Tree, tree_Node, tree_Phases_Set);
	return;
}

vector<shared_ptr<Phase_Node>> Node_Adaptive_Control::to_Phase_Node(const vector<vector<Tree_Stage_Node*>>& tree_Phases_Set) {
	vector<shared_ptr<Phase_Node>> result;
	for (int i = 0; i < tree_Phases_Set.size(); i++) {
		shared_ptr<Phase_Node> blank_Node = make_shared<Phase_Node>();                                          //哨兵节点
		shared_ptr<Phase_Node> temp = blank_Node;
		for (int j = 0; j < tree_Phases_Set[i].size(); j++) {
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

void Node_Adaptive_Control::modify_Phase_Green_Time(Tree_Stage_Node* head, double& local_Min_Delay) {
	vector<vector<Tree_Stage_Node*>> tree_Phases_Set;                                                            
	vector<Tree_Stage_Node*> tree_Node;
	tree_Phase_Sequence(head, tree_Node, tree_Phases_Set);
	vector<shared_ptr<Phase_Node>> phases_Set = to_Phase_Node(tree_Phases_Set);
	map<int, Phase_Index> mphases_Index = copy_Phases_Index();
	update_Phase_Index_Info();                                                                                    //重新计算相位的流量需求
	
	bool flag = false;
	for (int i = 0; i < phases_Set.size(); i++) {
		update_Optimal_Phases(phases_Set[i], local_Min_Delay);                                                        //保存延误最小的相序和相位方案

		shared_ptr<Phase_Node> temp_Head = phases_Set[i];
		while (temp_Head->next) {                                                                                 //迭代相序中的相位，排除最后一个相序阶段
			bool flag_delta = false;                                                                              
			int min_Green = static_cast<int>(min(phases_Index[temp_Head->phase_Id].green_Time_Pedestrian, phases_Index[temp_Head->down->phase_Id].green_Time_Pedestrian)) + delta_Green;
			while (!flag_delta && phases_Index[temp_Head->phase_Id].phase_Info.green_Time >= min_Green && !(phases_Index[temp_Head->phase_Id].priority_Right || phases_Index[temp_Head->down->phase_Id].priority_Right)) {                       
				phases_Index[temp_Head->phase_Id].phase_Info.green_Time -= delta_Green;                          //不断减少当前相位的绿灯
				phases_Index[temp_Head->down->phase_Id].phase_Info.green_Time -= delta_Green;

				shared_ptr<Phase_Node> backtrack_Head = temp_Head->next;                                         //试探性回溯的方式，将delta_Green分别加到后续的相位中
				while (backtrack_Head) {
					phases_Index[backtrack_Head->phase_Id].phase_Info.green_Time += delta_Green;
					phases_Index[backtrack_Head->down->phase_Id].phase_Info.green_Time += delta_Green;

					flag = update_Optimal_Phases(phases_Set[i], local_Min_Delay);
					if (!flag) {                                                                                 //贪婪方式跳出当前，执行下个相位的绿灯调整
						flag_delta = true;
						break;
					}	
					backtrack_Head = backtrack_Head->next;
					if (backtrack_Head) {
						phases_Index[backtrack_Head->phase_Id].phase_Info.green_Time -= delta_Green;
						phases_Index[backtrack_Head->down->phase_Id].phase_Info.green_Time -= delta_Green;
					}
				}
			}
			temp_Head = temp_Head->next;
		}
		phases_Index = mphases_Index;
	}
	return;
}

double Node_Adaptive_Control::cycle_Delay_Caculation(const int cycle_Time) {
	double local_Min_Delay = FLT_MAX;
	for (int i = 0; i < phases_Sequence.size(); i++) {
		Tree_Stage_Node* temp_Head;
		int cycle_Time_Initial = 0;
		map<int, Phase_Index> phases_Index_Temp = copy_Phases_Index();                                                                 //回溯phases_Index

		for (auto it = phases_Index.begin(); it != phases_Index.end(); it++) {                                                         //将cycle_Time写入Phase_Index
			it->second.phase_Info.intersection_Signal_Controller.signal_Cycle_Time = cycle_Time;
			it->second.initial_Demand_Caculation();
		}
		initial_Phases_Green_Time(phases_Sequence[i], phases_Sequence_Modified[i], cycle_Time_Initial);                                

		while (abs(cycle_Time_Initial - cycle_Time) >= 2) {
			modify_Cycle_Time(phases_Sequence_Modified[i], cycle_Time * 1.0 / cycle_Time_Initial, cycle_Time);                         //初始化相位的清空比例
			update_Phase_Index_Info();
			for (auto it = phases_Index.begin(); it != phases_Index.end(); it++) {
				it->second.phase_Info.green_Time = phases_Index_Temp[it->first].phase_Info.green_Time;
			}
			cycle_Time_Initial = 0;
			initial_Phases_Green_Time(phases_Sequence[i], phases_Sequence_Modified[i], cycle_Time_Initial);
		}
		
		temp_Head = build_Tree(nullptr, phases_Sequence[i], phases_Overlap[phases_Sequence[i]->phase_Id]->next);                        //决策树的建立

		modify_Phase_Green_Time(temp_Head, local_Min_Delay);                                                                           //相位时长优化

		phases_Index = phases_Index_Temp;
	}
	return local_Min_Delay;
}

void Node_Adaptive_Control::implement_Node_Control_Function() {                                                             //TODO:相序中屏障的使用，环周期和屏障时刻的一致相等
	if (cycle_Time_Lower == cycle_Time_Upper) {
		min_Delay = cycle_Delay_Caculation(cycle_Time_Lower);  
		return;
	}
	int optimal_Lower = cycle_Time_Lower, optimal_Upper = cycle_Time_Lower + (cycle_Time_Upper - cycle_Time_Lower) / 2;
	int suboptimal_Lower = optimal_Upper + 1, suboptimal_Upper = cycle_Time_Upper;                          //次优区间，减少陷入局部最优的几率
	double total_Delay = 0.0;
	while ((optimal_Upper - optimal_Lower) >= 2 ) {
		int optimal_Mid = optimal_Lower + (optimal_Upper - optimal_Lower) / 2;
		int suboptimal_Mid = suboptimal_Lower + (suboptimal_Upper - suboptimal_Lower) / 2;
		double sub_total_Delay = cycle_Delay_Caculation(suboptimal_Mid);
		if (sub_total_Delay < total_Delay) {                                     //交换搜索区间
			swap(optimal_Lower, suboptimal_Lower);
			swap(optimal_Upper, suboptimal_Upper);
			swap(optimal_Mid, suboptimal_Mid);
			total_Delay = sub_total_Delay;
		}else
		    total_Delay = cycle_Delay_Caculation(optimal_Mid);

		if (cycle_Delay_Caculation(optimal_Lower) < cycle_Delay_Caculation(optimal_Upper))             //二分搜索区间，沿下降最快的区间搜索
			optimal_Upper = optimal_Mid - 1;
		else 
			optimal_Lower = optimal_Mid + 1;
		
		if (cycle_Delay_Caculation(suboptimal_Lower) < cycle_Delay_Caculation(suboptimal_Upper))       //同步更新次优区间
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

Node_Adaptive_Control::~Node_Adaptive_Control()
{
	delete_Tree_Node(optimal_Head);
}

