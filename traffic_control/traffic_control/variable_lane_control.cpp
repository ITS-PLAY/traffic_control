#include "traffic_control.h"

using namespace std;
using namespace std::chrono;

void Node_Variable_Lane_Control::get_Node_Index_Info() {
	entrance_Link_Index.get_Link_Map_Info();
	entrance_Link_Index.get_Link_Index_Info();
	return;
}

void Node_Variable_Lane_Control::implement_Node_Control_Function() {

	map<Turn_Type, int> turn_Type_Nums;
	for (auto it = variable_Lane_State.begin(); it != variable_Lane_State.end(); it++) {
		Turn_Type near_State = Turn_Type(11 + it->second % 2);                                        //相邻的转向
		if (entrance_Link_Index.link_Movements_Index[it->second].movement_Sat_Ratio < turn_Saturation_Limit && entrance_Link_Index.link_Movements_Index[near_State].movement_Sat_Ratio >= turn_Saturation_Limit)
			map_Iterative_Nums[it->first]++;
		else
			map_Iterative_Nums[it->first] = 0;

		int nums_Min = (it->second == Straight) ? 6 : 4;                                              //直行切换左转的最小过渡周期为6，左转切换直行的最小过渡周期为4
		if (map_Iterative_Nums[it->first] < nums_Min)
			continue;
		map<Turn_Type, Movement_Index> temp = entrance_Link_Index.link_Movements_Index;             
		bool flag = modify_Movements_Index(it->first);
		if (flag) {                                                                                     
			it->second = (it->second == Left) ? Straight : Left;
			map_Iterative_Nums[it->first] = 0;
		}                                                                   
		else
			entrance_Link_Index.link_Movements_Index = temp;                                           //回溯转向流量
		turn_Type_Nums[it->second]++;                                                                  //不同车道类型的计数
	}

	if (turn_Type_Nums.size() == 1 || turn_Type_Nums.size() == 0)
		return;
                                                                 
	for (auto it = variable_Lane_State.begin(); it != variable_Lane_State.end(); it++) {               //切换车道
		if (turn_Type_Nums[Left] > 0) {                                                                 
			entrance_Link_Index.lanes[it->first].maneuvers = vector<Maneuver>{ LeftAllowed };
			turn_Type_Nums[Left]--;
		}
		else {
			entrance_Link_Index.lanes[it->first].maneuvers = vector<Maneuver>{ StraightAllowed };
			variable_Lane_State[it->first] = Straight;
		}
	}
	return;
}

bool Node_Variable_Lane_Control::modify_Movements_Index(int laneId) {                                       //判断车道切换后的饱和度差值，是否满足阈值
	Turn_Type current_State = variable_Lane_State[laneId];
	entrance_Link_Index.link_Movements_Index[current_State].movement_Capacity -= entrance_Link_Index.lanes_Index[laneId].capacity_Intersection;
	entrance_Link_Index.link_Movements_Index[current_State].movement_Sat_Ratio = static_cast<double>(entrance_Link_Index.link_Movements_Index[current_State].movement_Volume) / static_cast<double>(entrance_Link_Index.link_Movements_Index[current_State].movement_Capacity);
	entrance_Link_Index.link_Movements_Index[current_State].lanes_Nums--;

	Turn_Type next_State = Turn_Type(11 + current_State % 2);                                              //直行变左转，左转变直行
	int lanes_Num_Double = entrance_Link_Index.link_Movements_Index[next_State].lanes_Nums++;
	entrance_Link_Index.link_Movements_Index[next_State].movement_Capacity *= (static_cast<double>(lanes_Num_Double) + 1.0)/ static_cast<double>(lanes_Num_Double);        
	entrance_Link_Index.link_Movements_Index[next_State].movement_Sat_Ratio = static_cast<double>(entrance_Link_Index.link_Movements_Index[next_State].movement_Volume) / static_cast<double>(entrance_Link_Index.link_Movements_Index[next_State].movement_Capacity);
	
	return ((entrance_Link_Index.link_Movements_Index[next_State].movement_Sat_Ratio >= turn_Saturation_Limit) || abs(entrance_Link_Index.link_Movements_Index[current_State].movement_Sat_Ratio - entrance_Link_Index.link_Movements_Index[next_State].movement_Sat_Ratio) <= turn_Change_Limit);
}

void Node_Variable_Lane_Control::update_Node_Index_Info() {
	entrance_Link_Index.lanes.clear();
	entrance_Link_Index.lanes_Index.clear();
	entrance_Link_Index.link_Movements_Index.clear();
	return;
}

void Node_Variable_Lane_Control::put_Control_Delivery() {
	return;
}

void Node_Variable_Lane_Control::get_Variable_Lane_State_Info() {
	variable_Lane_State.emplace(4, Straight);
	return;
}

