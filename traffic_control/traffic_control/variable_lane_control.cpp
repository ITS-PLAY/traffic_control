﻿#include "traffic_control.h"

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
	entrance_Link_Index.lanes_Index.clear();
	entrance_Link_Index.link_Movements_Index.clear();
	return;
}

void Node_Variable_Lane_Control::put_Control_Delivery() {
	return;
}

void Node_Variable_Lane_Control::get_Variable_Lane_State_Info() {
	variable_Lane_State.emplace(4, Straight);
}

void Lane_Map::get_Lane_Map_Info() {
	switch (laneId) {
	     case 1: maneuvers = vector<Maneuver>{ RightAllowed }; signal_Controller_Id = 1; phase_Id = 8; break;
         case 2: 
		 case 3: 
	     case 4: maneuvers = vector<Maneuver>{ StraightAllowed }; signal_Controller_Id = 1; phase_Id = 8; break;
	     case 5: maneuvers = vector<Maneuver>{ LeftAllowed }; signal_Controller_Id = 1; phase_Id = 7; break;
	}
	return;
}

void Lane_Index::get_Lane_Volume_Info() {
	switch (laneId) {
	     case 1: volume_Interval = 20; volume = volume_Interval * 12; capacity_Saturation = 1450; break;
		 case 2: 
		 case 3: 
		 case 4: volume_Interval = 10; volume = volume_Interval * 12; capacity_Saturation = 1650; break;
		 case 5: volume_Interval = 50; volume = volume_Interval * 12; capacity_Saturation = 1550; break;
	}
	return;
}

void Signal_Phase_Info::get_Signal_Phase_Info() {
	switch (phase_Id) {
	     case 7: green_Time = 20; yellow_Time = 3; break;
	     case 8: green_Time = 34; yellow_Time = 3; break;
	}
	return;
}

void Signal_Controller_Info::get_Signal_Controller_Info() {
	signal_Cycle_Time = 120;
	return;
}

void Lane_Index::capacity_Intersection_Caculation() {
	capacity_Intersection = static_cast<int>(capacity_Saturation * (lane_Phase_Info.green_Time + lane_Phase_Info.yellow_Time - delay_Vehicles_Start) / lane_Phase_Info.intersection_Signal_Controller.signal_Cycle_Time);
	return;
}

void Link_Map::get_Link_Map_Info() {
	lanes.emplace(5, Lane_Map(0, 1, 5));
	lanes.emplace(4, Lane_Map(0, 1, 4));
	lanes.emplace(3, Lane_Map(0, 1, 3));
	lanes.emplace(2, Lane_Map(0, 1, 2));
	lanes.emplace(1, Lane_Map(0, 1, 1));
	return;
}

void Link_Index::get_Link_Index_Info() {
	for (auto it = lanes.begin(); it != lanes.end(); it++) {
		lanes_Index.emplace(it->first, Lane_Index(it->second.upstreamId, it->second.nodeId, it->second.laneId));
		vector<Maneuver> temp = it->second.maneuvers;
		if (temp.size() == 1) {                                                                     //专用车道
			switch (temp[0]) {
			     case StraightAllowed: link_Movements_Index.emplace(Straight, Movement_Index()); 
					                   link_Movements_Index[Straight].movement_Volume += lanes_Index[it->first].volume;
					                   link_Movements_Index[Straight].movement_Capacity += lanes_Index[it->first].capacity_Intersection;
									   link_Movements_Index[Straight].lanes_Nums++;
					                   break;                   
			     case LeftAllowed:link_Movements_Index.emplace(Left, Movement_Index());
					              link_Movements_Index[Left].movement_Volume += lanes_Index[it->first].volume;
					              link_Movements_Index[Left].movement_Capacity += lanes_Index[it->first].capacity_Intersection;
								  link_Movements_Index[Left].lanes_Nums++;
					              break;
			}
			continue;
		}
		for (int i = 0; i < temp.size(); i++) {                                                     //TODO::混合车道
			
		}
	}
	movements_Index_Caculation();
	return;
}
void Link_Index::movements_Index_Caculation() {
	for (auto it = link_Movements_Index.begin(); it != link_Movements_Index.end(); it++) {
		it->second.movement_Sat_Ratio = static_cast<double>(it->second.movement_Volume) / static_cast<double>(it->second.movement_Capacity);
	}
	return;
}

void Node_Map::get_Node_Map_Info() {
	upstream_Nodes.emplace_back(0);        //上游节点为北方向交叉口0,当前节点ID为1
	return;
}

void traffic_Control_Integration(Node_Control_Strategy *node_control,int nodeId) {
	if (typeid(*node_control) == typeid(Node_Variable_Lane_Control)) {                                                                                  //执行可变车道控制
		Node_Map node = Node_Map(nodeId);
		vector<Node_Variable_Lane_Control> variable_Lanes;
		if (variable_Lanes.size() < node.upstream_Nodes.size()) {                                                                                      //每个进口道的初始化
			for (int i = 0; i < node.upstream_Nodes.size(); i++) {
				variable_Lanes.emplace_back(Node_Variable_Lane_Control(node.upstream_Nodes[i], nodeId));                                               //（代码方法）可以通过dynamic_cast
			}
		}
		while (true) {
			int cycle_Time = variable_Lanes[0].entrance_Link_Index.lanes_Index[1].lane_Phase_Info.intersection_Signal_Controller.signal_Cycle_Time;    
			using second_Clock_Type = time_point<system_clock, seconds>;
			second_Clock_Type current_Time = time_point_cast<seconds>(system_clock::now());
			if ((current_Time.time_since_epoch().count() % cycle_Time) != 0) {                                                                          //当前时间是否位于周期的结束时刻
				continue;
			}

			for (int i = 0; i < variable_Lanes.size(); i++) {                                                                                           //每个进口道开始执行可变车道功能,先清空数据再获取新数据
				variable_Lanes[i].update_Node_Index_Info();                                                     
				variable_Lanes[i].get_Node_Index_Info();
				variable_Lanes[i].implement_Node_Control_Function();        
			}
		}
	}

	return;
}