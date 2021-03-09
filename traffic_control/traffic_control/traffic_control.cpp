#include "traffic_control.h"

void Lane_Map::get_Lane_Map_Info() {
	switch (upstreamId) {
	case 0:
		switch (laneId) {
		case 1: maneuvers = vector<Maneuver>{ RightAllowed }; signal_Controller_Id = 1; phase_Id = 8; break;
		case 2:
		case 3:
		case 4: maneuvers = vector<Maneuver>{ StraightAllowed }; signal_Controller_Id = 1; phase_Id = 8; break;
		case 5: maneuvers = vector<Maneuver>{ LeftAllowed }; signal_Controller_Id = 1; phase_Id = 7; break;
		}
		break;
	case 2:
		switch (laneId) {
		case 1: maneuvers = vector<Maneuver>{ RightAllowed }; signal_Controller_Id = 1; phase_Id = 2; break;
		case 2:
		case 3: maneuvers = vector<Maneuver>{ StraightAllowed }; signal_Controller_Id = 1; phase_Id = 2; break;
		case 4: maneuvers = vector<Maneuver>{ LeftAllowed }; signal_Controller_Id = 1; phase_Id = 1; break;
		}
		break;
	case 4:
		switch (laneId) {
		case 1: maneuvers = vector<Maneuver>{ RightAllowed }; signal_Controller_Id = 1; phase_Id = 4; break;
		case 2:
		case 3:
		case 4: maneuvers = vector<Maneuver>{ StraightAllowed }; signal_Controller_Id = 1; phase_Id = 4; break;
		case 5: maneuvers = vector<Maneuver>{ LeftAllowed }; signal_Controller_Id = 1; phase_Id = 3; break;
		}
		break;
	case 6:
		switch (laneId) {
		case 1: maneuvers = vector<Maneuver>{ RightAllowed }; signal_Controller_Id = 1; phase_Id = 6; break;
		case 2: maneuvers = vector<Maneuver>{ StraightAllowed }; signal_Controller_Id = 1; phase_Id = 6; break;
		case 3: maneuvers = vector<Maneuver>{ LeftAllowed }; signal_Controller_Id = 1; phase_Id = 5; break;
		}
		break;
	}
	return;
}

void Lane_Index::get_Lane_Volume_Info() {
	switch (upstreamId) {
	case 0:
		switch (laneId) {
		case 1: volume_Interval = 20; volume = volume_Interval * 12; capacity_Saturation = 1450; break;
		case 2:
		case 3:
		case 4: volume_Interval = 10; volume = volume_Interval * 12; capacity_Saturation = 1650; break;
		case 5: volume_Interval = 50; volume = volume_Interval * 12; capacity_Saturation = 1550; break;
		}
		break;
	case 2:
		switch (laneId) {
		case 1: volume_Interval = 10; volume = volume_Interval * 12; capacity_Saturation = 1450; break;
		case 2:
		case 3: volume_Interval = 10; volume = volume_Interval * 12; capacity_Saturation = 1650; break;
		case 4: volume_Interval = 10; volume = volume_Interval * 12; capacity_Saturation = 1550; break;
		}
		break;
	case 4:
		switch (laneId) {
		case 1: volume_Interval = 20; volume = volume_Interval * 12; capacity_Saturation = 1450; break;
		case 2:
		case 3:
		case 4: volume_Interval = 15; volume = volume_Interval * 12; capacity_Saturation = 1650; break;
		case 5: volume_Interval = 20; volume = volume_Interval * 12; capacity_Saturation = 1550; break;
		}
		break;
	case 6:
		switch (laneId) {
		case 1: volume_Interval = 10; volume = volume_Interval * 12; capacity_Saturation = 1450; break;
		case 2: volume_Interval = 10; volume = volume_Interval * 12; capacity_Saturation = 1650; break;
		case 3: volume_Interval = 10; volume = volume_Interval * 12; capacity_Saturation = 1550; break;
		}
		break;
	}
	return;
}

void Signal_Phase_Info::get_Signal_Phase_Info() {
	switch (phase_Id) {
	case 1: green_Time = 20; yellow_Time = 3; break;
	case 2: green_Time = 34; yellow_Time = 3; break;
	case 3: green_Time = 20; yellow_Time = 3; break;
	case 4: green_Time = 34; yellow_Time = 3; break;
	case 5: green_Time = 20; yellow_Time = 3; break;
	case 6: green_Time = 34; yellow_Time = 3; break;
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
	//data for test
	switch (upstreamNodeId_nodeid) {
	case 0:
		lanes.emplace(5, Lane_Map(0, 1, 5));
		lanes.emplace(4, Lane_Map(0, 1, 4));
		lanes.emplace(3, Lane_Map(0, 1, 3));
		lanes.emplace(2, Lane_Map(0, 1, 2));
		lanes.emplace(1, Lane_Map(0, 1, 1));
		break;
	case 2:
		lanes.emplace(4, Lane_Map(2, 1, 3));
		lanes.emplace(3, Lane_Map(2, 1, 3));
		lanes.emplace(2, Lane_Map(2, 1, 2));
		lanes.emplace(1, Lane_Map(2, 1, 1));
		break;
	case 4:
		lanes.emplace(5, Lane_Map(4, 1, 5));
		lanes.emplace(4, Lane_Map(4, 1, 4));
		lanes.emplace(3, Lane_Map(4, 1, 3));
		lanes.emplace(2, Lane_Map(4, 1, 2));
		lanes.emplace(1, Lane_Map(4, 1, 1));
		break;
	case 6:
		lanes.emplace(3, Lane_Map(6, 1, 3));
		lanes.emplace(2, Lane_Map(6, 1, 2));
		lanes.emplace(1, Lane_Map(6, 1, 1));
		break;
	default: ;
	}
	return;
}

void Link_Index::get_Link_Index_Info() {
	for (auto it = lanes.begin(); it != lanes.end(); it++) {
		lanes_Index.emplace(it->first, Lane_Index(it->second.upstreamId, it->second.nodeId, it->second.laneId));
		vector<Maneuver> temp = it->second.maneuvers;
		if (temp.size() == 1) {                                                                     //ר�ó���
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
		for (int i = 0; i < temp.size(); i++) {                                                     //TODO::��ϳ���

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
	upstream_Nodes.emplace_back(0);        //���νڵ�Ϊ�����򽻲��0,��ǰ�ڵ�IDΪ1
	upstream_Nodes.emplace_back(2);        //���νڵ�Ϊ�����򽻲��2
	upstream_Nodes.emplace_back(4);        //���νڵ�Ϊ�Ϸ��򽻲��4
	upstream_Nodes.emplace_back(6);        //���νڵ�Ϊ�����򽻲��6
	return;
}

void traffic_Control_Integration(Node_Control_Strategy* node_control, int nodeId) {
	using second_Clock_Type = time_point<system_clock, seconds>;
	if (typeid(*node_control) == typeid(Node_Variable_Lane_Control)) {                                                                                  //ִ�пɱ䳵������
		Node_Map node = Node_Map(nodeId);
		vector<Node_Variable_Lane_Control> variable_Lanes;
		if (variable_Lanes.size() < node.upstream_Nodes.size()) {                                                                                      //ÿ�����ڵ��ĳ�ʼ��
			for (int i = 0; i < node.upstream_Nodes.size(); i++) {
				variable_Lanes.emplace_back(Node_Variable_Lane_Control(node.upstream_Nodes[i], nodeId));                                               //�����뷽��������ͨ��dynamic_cast
			}
		}
		while (true) {
			int cycle_Time = variable_Lanes[0].entrance_Link_Index.lanes_Index[1].lane_Phase_Info.intersection_Signal_Controller.signal_Cycle_Time;
			second_Clock_Type current_Time = time_point_cast<seconds>(system_clock::now());
			if ((current_Time.time_since_epoch().count() % cycle_Time) != 0) {                                                                          //��ǰʱ���Ƿ�λ�����ڵĽ���ʱ��
				continue;
			}

			for (int i = 0; i < variable_Lanes.size(); i++) {                                                                                           //ÿ�����ڵ���ʼִ�пɱ䳵������,����������ٻ�ȡ������
				variable_Lanes[i].update_Node_Index_Info();
				variable_Lanes[i].get_Node_Index_Info();
				variable_Lanes[i].implement_Node_Control_Function();
			}
		}
	}

	if (typeid(*node_control) == typeid(Node_Adaptive_Control)) {                                                                                      //ִ������Ӧ����
		Node_Adaptive_Control adaptive_Control = Node_Adaptive_Control(1);
		while (true) {
			second_Clock_Type current_Time = time_point_cast<seconds>(system_clock::now());
			if ((current_Time.time_since_epoch().count() % Time_Interval) == 0) {
				adaptive_Control.update_Node_Index_Info();
				adaptive_Control.get_Node_Index_Info();
				adaptive_Control.implement_Node_Control_Function();
			}
		}
	}

	return;
}