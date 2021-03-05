#include "traffic_control.h"

using namespace std;

inline void Phase_Index::initial_Green_Time_Caculation() {
	switch (queue_Num) {
	    case 0: phase_Info.green_Time = 0; break;
	    case 1: phase_Info.green_Time = 2; break;
	    case 2: phase_Info.green_Time = 3; break;
	    default:
		       phase_Info.green_Time = 4; break;
	}
	return;
}

inline void Phase_Index::initial_Demand_Caculation() {             //TODO:是否添加phase_Clearance_Ratio上限为1.0的约束
	initial_Demand = phase_Clearance_Ratio * (1.0F * volume_Interval / Time_Interval * phase_Info.intersection_Signal_Controller.signal_Cycle_Time + queue_Num);
	return;
}

double Node_Adaptive_Control::queue_Delay_Value(const int phase_Id) {
	int temp = phases_Index[phase_Id].queue_Num  - phases_Index[phase_Id].capacity_Intersection;
	if (temp <= 0)
		return phases_Index[phase_Id].queue_Num * phases_Index[phase_Id].time_Headway_Saturation;
	else
		return phases_Index[phase_Id].capacity_Intersection * phases_Index[phase_Id].time_Headway_Saturation + temp * static_cast<double>(phases_Index[phase_Id].phase_Info.green_Time);
}

void Node_Adaptive_Control::get_Phases_Overlap_Info() {    //固定相序下的相位嵌套组合
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

void Node_Adaptive_Control::get_Phases_Sequence_Info() {      //执行固定的相序,                                        TODO:添加同阶段的相位，添加默认的0相位
	shared_ptr<Phase_Node> phase1 (new Phase_Node(1, false));
	shared_ptr<Phase_Node> phase6 (new Phase_Node(6, true));
	phase1->next = phase6;
	shared_ptr<Phase_Node> phase3 (new Phase_Node(3, false));
	phase6->next = phase3;
	shared_ptr<Phase_Node> phase8 (new Phase_Node(8, true));
	phase3->next = phase8;

	phases_Sequence.emplace_back(phase1);
	return;
}

void Node_Adaptive_Control::get_Node_Index_Info() {
	entrance_Links_Index.emplace(0,Link_Index(0, 1));
	entrance_Links_Index.emplace(2,Link_Index(2, 1));
	entrance_Links_Index.emplace(4,Link_Index(4, 1));
	entrance_Links_Index.emplace(6,Link_Index(6, 1));
	return;
}

void Node_Adaptive_Control::initial_Phases_Green_Time(const shared_ptr<Phase_Node>& mphase_Sequence, int& cycle_Time) {                      //给定数据、相序、周期时长等，计算绿灯初始化值
	//TODO:将相序添加到phases_Sequence_Modified容器中
	shared_ptr<Phase_Node> phase_Head = mphase_Sequence;
	while (phase_Head) {
		int ring1_phase = phase_Head->phase_Id;                                                       //环1的相位编号
		shared_ptr<Stage_Node> stage_Head = phases_Overlap[phase_Head->phase_Id];

		while (stage_Head->next) {
			int ring2_phase = stage_Head->next->phase_Id;                                            //环2的相位编号
			double max_volume = max(phases_Index[ring1_phase].initial_Demand, phases_Index[ring2_phase].initial_Demand), min_volume = min(phases_Index[ring1_phase].initial_Demand, phases_Index[ring2_phase].initial_Demand);
			int green_Time = 0;
			if (abs(phases_Index[ring1_phase].initial_Demand - phases_Index[ring2_phase].initial_Demand) / max_volume <= stage_Volume_Diff || min_volume == 0) {
				green_Time = static_cast<int>(max_volume * phases_Index[ring1_phase].time_Headway_Saturation);
				phases_Index[ring1_phase].phase_Info.green_Time += green_Time;                       //环1的当前相位绿灯时长
				phases_Index[ring2_phase].phase_Info.green_Time += green_Time;                       //环2的当前相位绿灯时长
				break;
			}
			green_Time = static_cast<int>(min_volume * phases_Index[ring1_phase].time_Headway_Saturation);
			phases_Index[ring1_phase].phase_Info.green_Time += green_Time;
			phases_Index[ring2_phase].phase_Info.green_Time += green_Time;

			int ring_Num_Main = (phases_Index[ring1_phase].initial_Demand == min_volume) ? ring2_phase : ring1_phase;
			phases_Index[ring_Num_Main].initial_Demand -= min_volume;                                //计算主要相位的剩余需求
			if (ring1_phase == ring_Num_Main)
				break;                                                                               //跳转环1的下一相位
			stage_Head = stage_Head->next;
		}
		phase_Head = phase_Head->next;
	}
	return;
}

void Node_Adaptive_Control::modify_Cycle_Time(shared_ptr<Phase_Node>& mphase_Sequence_Modified, double ratio, const int cycle_Time) {                   
	double ratio_Sum_Increase = 0.0, ratio_Sum_Decrease = 0.0;
	shared_ptr<Phase_Node> phase_Head = mphase_Sequence_Modified;
	while (phase_Head) {
		int ring1_phase = phase_Head->phase_Id;
		int ring2_phase = phase_Head->down->phase_Id;
		if (!phases_Index[ring1_phase].priority_Right || phases_Index[ring1_phase].phase_Info.green_Time * ratio > phases_Index[ring1_phase].green_Time_Pedestrian)        //非优先相位或者非最小绿灯相位
			ratio_Sum_Decrease += phases_Index[ring1_phase].phase_Clearance_Ratio;
		if (!phases_Index[ring2_phase].priority_Right || phases_Index[ring2_phase].phase_Info.green_Time * ratio > phases_Index[ring2_phase].green_Time_Pedestrian)
			ratio_Sum_Decrease += phases_Index[ring2_phase].phase_Clearance_Ratio;
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
			if (!phases_Index[ring1_phase].priority_Right || phases_Index[ring1_phase].phase_Info.green_Time * ratio > phases_Index[ring1_phase].green_Time_Pedestrian)
				phases_Index[ring1_phase].phase_Clearance_Ratio *= (ratio * ratio_Sum_Decrease / ratio_Sum_Increase);
			if (!phases_Index[ring1_phase].priority_Right || phases_Index[ring2_phase].phase_Info.green_Time * ratio > phases_Index[ring2_phase].green_Time_Pedestrian)
				phases_Index[ring2_phase].phase_Clearance_Ratio *= (ratio * ratio_Sum_Decrease / ratio_Sum_Increase);
		}
		phase_Head = phase_Head->next;
	}
	//TODO:先测试是否可以等于指定的周期时长，如果差不多，不执行get_Phases_Green_Time；只修改各相位的phase_Clearance_Ratio，待后续相位时长优化modify_Phase_Green_Time进行统一修改
	//initial_Phases_Green_Time(mphase_Sequence_Modified, const_cast<int&>(cycle_Time));
	return;
}

void Node_Adaptive_Control::update_Phase_Index_Info() {
	for (auto& it : phases_Index) {
		it.second.initial_Demand_Caculation();
	}
	return;
}

void Node_Adaptive_Control::phase_Delay_Caculation(const shared_ptr<Phase_Node>& head, int& moment_Of_Cycle, double& total_Delay) {
	if (&head == nullptr)                                                                                                                                                                //TODO:空应用的合法性
		return;
	total_Delay += phases_Index[head->phase_Id].delay_Vehicles_Start + phases_Index[head->down->phase_Id].delay_Vehicles_Start;                //车辆启动延误

    phases_Index[head->phase_Id].delay_Queue_Clearance = queue_Delay_Value(head->phase_Id);                                                    //TODO：根据车辆类型和延误系数，计算延误
    phases_Index[head->down->phase_Id].delay_Queue_Clearance = queue_Delay_Value(head->down->phase_Id); 
	total_Delay += phases_Index[head->phase_Id].delay_Queue_Clearance + phases_Index[head->down->phase_Id].delay_Queue_Clearance;              //车辆清空延误

	phases_Index[head->phase_Id].delay_Red_Stop = phases_Index[head->phase_Id].initial_Demand * moment_Of_Cycle;
	phases_Index[head->down->phase_Id].delay_Red_Stop = phases_Index[head->down->phase_Id].initial_Demand * moment_Of_Cycle;
	total_Delay += phases_Index[head->phase_Id].delay_Red_Stop + phases_Index[head->down->phase_Id].delay_Red_Stop;                            //红灯停车延误

	moment_Of_Cycle += phases_Index[head->phase_Id].phase_Info.green_Time + phases_Index[head->phase_Id].phase_Info.yellow_Time;
	phase_Delay_Caculation(head->down, moment_Of_Cycle, total_Delay);                                                                          
	phase_Delay_Caculation(head->next, moment_Of_Cycle, total_Delay);                                                                          //递归计算所有相位的延误
	return;
}

Tree_Stage_Node* Node_Adaptive_Control::build_Tree(Tree_Stage_Node* head, const shared_ptr<Phase_Node>& mphase_Sequence, const shared_ptr<Stage_Node>& mstage_Overlap) {                     //除基本参数和初始化的清空比例外，相位参数一切归零，建立决策树
	//TODO:添加mphase_Sequence_Modified的逻辑，用于存储完整的相序
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
	if (abs(temp / max_volume) <= stage_Volume_Diff) {
		int green_time = static_cast<int>(max_volume * phases_Index[ring1_phase].time_Headway_Saturation);
		head->ring1_Phase_Info.green_Time = green_time;
		head->ring2_Phase_Info.green_Time = green_time;
	}else {
		int green_time = static_cast<int>(min_volume * phases_Index[ring1_phase].time_Headway_Saturation);
		head->ring1_Phase_Info.green_Time = green_time;
		head->ring2_Phase_Info.green_Time = green_time;
	}

	if (temp/max_volume > stage_Volume_Diff) 
		head->left_Tree = build_Tree(head->left_Tree, mphase_Sequence, mstage_Overlap->next);
	if (mphase_Sequence->next) 
		head->right_Tree = build_Tree(head->right_Tree, mphase_Sequence->next, phases_Overlap[mphase_Sequence->next->phase_Id]->next);
	return head;
}

void Node_Adaptive_Control::tree_Phase_Sequence(Tree_Stage_Node* head, vector<Tree_Stage_Node*>& tree_Node, vector<vector<Tree_Stage_Node*>>& tree_Phases_Set) {
	if (!head->right_Tree) {
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

void Node_Adaptive_Control::modify_Phase_Green_Time(Tree_Stage_Node* head, double& total_Delay) {                     
	vector<vector<Tree_Stage_Node*>> tree_Phases_Set;                                                            //TODO:如何保存类中phases_Index的值
	vector<Tree_Stage_Node*> tree_Node;
	tree_Phase_Sequence(head, tree_Node, tree_Phases_Set);
	vector<shared_ptr<Phase_Node>> phases_Set = to_Phase_Node(tree_Phases_Set);

	int moment_Of_Cycle = 0;
	for (int i = 0; i < phases_Set.size(); i++) {
		shared_ptr<Phase_Node> temp_Head = phases_Set[i];
		while (temp_Head) {                                                                                      //迭代相序中的相位
			int min_Green = static_cast<int>(min(phases_Index[temp_Head->phase_Id].green_Time_Pedestrian, phases_Index[temp_Head->down->phase_Id].green_Time_Pedestrian));
			while (phases_Index[temp_Head->phase_Id].phase_Info.green_Time >= min_Green && !(phases_Index[temp_Head->phase_Id].priority_Right || phases_Index[temp_Head->down->phase_Id].priority_Right)) {                       
				phases_Index[temp_Head->phase_Id].phase_Info.green_Time -= delta_Green;                          //不断减少当前相位的绿灯
				phases_Index[temp_Head->down->phase_Id].phase_Info.green_Time -= delta_Green;

				//试探性回溯的方式，将delta_Green分别加到后续的相位中
				shared_ptr<Phase_Node> backtrack_Head = temp_Head->next;
				while (backtrack_Head) {
					phases_Index[backtrack_Head->phase_Id].phase_Info.green_Time += delta_Green;
					phases_Index[backtrack_Head->next->phase_Id].phase_Info.green_Time += delta_Green;

					phase_Delay_Caculation(phases_Set[i], moment_Of_Cycle, total_Delay);                         //TODO:phase_Delay_Caculation可设置链表头和链表尾，计算某一段相序的延误

					phases_Index[backtrack_Head->phase_Id].phase_Info.green_Time -= delta_Green;
					phases_Index[backtrack_Head->next->phase_Id].phase_Info.green_Time -= delta_Green;
					backtrack_Head = backtrack_Head->next;
				}
			}
			temp_Head = temp_Head->next;
		}
	}
	return;
}

double Node_Adaptive_Control::cycle_Delay_Caculation(const int cycle_Time) {
	double total_Delay = 0.0;
	for (int i = 0; i < phases_Sequence.size(); i++) {
		//TODO:将cycle_Time写入Phase_Index
		Tree_Stage_Node* temp_Head;
		int cycle_Time_Initial = 0;
		initial_Phases_Green_Time(phases_Sequence[i], cycle_Time_Initial);

		while (abs(cycle_Time_Initial - cycle_Time) >= 2)
		    modify_Cycle_Time(phases_Sequence_Modified[i], static_cast<double>(cycle_Time / cycle_Time_Initial), cycle_Time);

		int moment_Of_Cycle = 0;
		phase_Delay_Caculation(phases_Sequence_Modified[i], moment_Of_Cycle, total_Delay);
		//决策树的建立
		temp_Head = build_Tree(nullptr, phases_Sequence[i], phases_Overlap[phases_Sequence[i]->phase_Id]->next);
		//相位时长优化
		total_Delay = 0.0;
		modify_Phase_Green_Time(temp_Head,total_Delay);

	}
	return total_Delay;
}

void Node_Adaptive_Control::implement_Node_Control_Function() {                //TODO:每一相位的initial_Demand至少设置一辆车
	if (cycle_Time_Lower == cycle_Time_Upper) {
		min_Delay = cycle_Delay_Caculation(cycle_Time_Lower);
		optimal_Cycle_Time = cycle_Time_Lower;
		return;
	}
	int optimal_Lower = cycle_Time_Lower, optimal_Upper = cycle_Time_Lower + (cycle_Time_Upper - cycle_Time_Lower) / 2;
	int suboptimal_Lower = optimal_Upper, suboptimal_Upper = cycle_Time_Upper;
	double total_Delay = 0.0;
	int optimal_Cycle = 0;
	while ((optimal_Upper - optimal_Lower) >= 2 ) {
		int optimal_Mid = optimal_Lower + (optimal_Upper - optimal_Lower) / 2;
		int suboptimal_Mid = suboptimal_Lower + (suboptimal_Upper - suboptimal_Lower) / 2;
		if (cycle_Delay_Caculation(suboptimal_Mid) < total_Delay) {                                     //交换搜索区间
			swap(optimal_Lower, suboptimal_Lower);
			swap(optimal_Upper, suboptimal_Upper);
			swap(optimal_Mid, suboptimal_Mid);
		}
		total_Delay = cycle_Delay_Caculation(optimal_Mid);
		optimal_Cycle = optimal_Mid;

		if (cycle_Delay_Caculation(optimal_Lower) < cycle_Delay_Caculation(optimal_Upper)) {            //二分搜索区间，沿下降最快的区间搜索
			optimal_Upper = optimal_Mid;
		}else {
			optimal_Lower = optimal_Mid;
		}

		if (cycle_Delay_Caculation(suboptimal_Lower) < cycle_Delay_Caculation(suboptimal_Upper)) {
			suboptimal_Upper = suboptimal_Mid;
		}else {
			suboptimal_Lower = suboptimal_Mid;
		}
	}
	if (total_Delay < min_Delay) {
		min_Delay = total_Delay;
		optimal_Cycle_Time = optimal_Cycle;
	}
	return;
}

void Node_Adaptive_Control::put_Control_Delivery() {
	
	return;
}
void Node_Adaptive_Control::update_Node_Index_Info() {

	return;
}

Node_Adaptive_Control::~Node_Adaptive_Control()
{
	delete optimal_Head; optimal_Head = nullptr;
	delete optimal_Phase_Sequence; optimal_Phase_Sequence = nullptr;
}

