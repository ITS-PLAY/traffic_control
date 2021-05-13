#include "traffic_control.h"

void Lane_Map::get_Lane_Map_Info() {
	
	bool stop_Search = false;                                                            //停止查找的标记
	lane_data_t lane_Map_Data;
	for (int i = 0; i <= node_Map_Data.linkNum; i++) {

		if (node_Map_Data.links->upstreamNodeId.nodeid == upstreamId) {                  //查找指定路段
			
			signal_Controller_Id = node_Map_Data.id.nodeid;                             //TODO：方案编号
			phase_Id = *(node_Map_Data.links->movements->phaseId);                      //相位编号

			for (int j = 0; j <= node_Map_Data.links->laneNum; j++) {
				if (node_Map_Data.links->lanes == nullptr) {
					printf("当前交叉口：%d, 上游交叉口：%d, 车道：%d为空\n", nodeId, upstreamId,laneId);
					return;
				}
				if (node_Map_Data.links->lanes->laneId == laneId) {                     //查找指定车道
					lane_Map_Data = *(node_Map_Data.links->lanes);                      //目标车道信息
					stop_Search = true;
					break;
				}
				node_Map_Data.links->lanes++;                                           //下一个车道的指针
			}
		}else if (stop_Search)
			break;
		node_Map_Data.links++;                                                          //下一个路段的指针
	}
	laneWidth = *(lane_Map_Data.laneWidth);
	//laneType = static_cast<Lane_Type>(lane_Map_Data.laneAttributes->laneType);
	//maneuvers.emplace_back(*(lane_Map_Data.maneuvers));
	return;
}

void Lane_Index::get_Lane_Index_Info() {
	
	bool stop_Search = false;
	for (int i = 0; i < node_Index_Data.linkNum; i++) {
		if (node_Index_Data.link_info[i].linkId == upstreamId) {                         //查找指定路段
			for (int j = 0; j < node_Index_Data.link_info[i].laneNum; j++) {
				if (node_Index_Data.link_info[i].lane_info[j].laneId == laneId) {         //查找指定车道
					lane_Index_Data = node_Index_Data.link_info[i].lane_info[j];          //目标车道信息
					stop_Search = true;
					break;
				}
			}
		}else if (stop_Search)
			break;
	}
	get_Lane_Volume_Info();
	get_Lane_Queue_Info();
	capacity_Intersection_Caculation();
	return;
}

void Lane_Index::get_Lane_Volume_Info() {
	volume_Interval = lane_Index_Data.laneVolume;
	capacity_Saturation = lane_Index_Data.laneCapacity;
	return;
}

void Lane_Index::get_Lane_Queue_Info() {
	queue_Length = lane_Index_Data.laneQueueLength;
	queue_Num = lane_Index_Data.laneQueueNum;
	return;
}

void Lane_Index::capacity_Intersection_Caculation() {
	capacity_Intersection = static_cast<int>(capacity_Saturation * (lane_Phase_Info.green_Time + lane_Phase_Info.yellow_Time - delay_Vehicles_Start) / lane_Phase_Info.intersection_Signal_Controller.signal_Cycle_Time);
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

void Link_Map::get_Link_Map_Info() {

	bool stop_Search = false;
	for (int i = 0; i < node_Map_Data.linkNum; i++) {
		if (node_Map_Data.links == nullptr) {
           printf("当前交叉口：%d, 上游交叉口：%d对应的路段为空\n", nodeId, upstreamNodeId_nodeid);
           return;
        }
		if (node_Map_Data.links->upstreamNodeId.nodeid == upstreamNodeId_nodeid) {                          //查找指定路段
			Movement link_Movement;                                                                         //路段的转向
			link_Movement.remoteIntersection_nodeid = node_Map_Data.links->movements->remoteIntersection.nodeid;
			link_Movement.phase_Id = *(node_Map_Data.links->movements->phaseId);
			movements.emplace_back(link_Movement);

			for (int j = 0; j < node_Map_Data.links->laneNum; j++) {
				int lane_Id = node_Map_Data.links->lanes->laneId;
				lanes.emplace(lane_Id, Lane_Map(upstreamNodeId_nodeid, nodeId, lane_Id, node_Map_Data));    //依次加入车道信息
			}
			break;
		}
		node_Map_Data.links++;                                                                              //下一个路段的指针
	}
	return;
}

void Link_Index::get_Link_Index_Info() {
	for (auto it = lanes.begin(); it != lanes.end(); it++) {
		lanes_Index.emplace(it->first, Lane_Index(it->second.upstreamId, it->second.nodeId, it->second.laneId, node_Map_Data, node_Index_Data)); 
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
	nodeId = node_Map_Data.id.nodeid;
	for (int i = 0; i < node_Map_Data.linkNum; i++) {
		int upstreamId = node_Map_Data.links->upstreamNodeId.nodeid;
		links.emplace(upstreamId, Link_Map(upstreamId, nodeId, node_Map_Data));
	}
	return;
}

void Node_Index::get_Node_Index_Info() {
	for (auto it = links.begin(); it != links.end(); it++) {
		entrance_Links_Index.emplace(it->first, Link_Index(it->first, nodeId, node_Map_Data, node_Index_Data));
	}
	return;
}

void Node_Control_Integration::initial_Node_Control() {

}

/*
template<typename T>
vector<T> traffic_Control_Integration(const int nodeId, map_data_t& mmap_data) {
	using second_Clock_Type = time_point<system_clock, seconds>;
	if (typeid(T) == typeid(Node_Variable_Lane_Control)) {                                                                                             //执行可变车道控制
		Node_Map node = Node_Map(nodeId);
		vector<Node_Variable_Lane_Control> variable_Lanes;
		if (variable_Lanes.size() < node.upstream_Nodes.size()) {                                                                                      //每个进口道的初始化
			for (int i = 0; i < node.upstream_Nodes.size(); i++) {
				variable_Lanes.emplace_back(Node_Variable_Lane_Control(node.upstream_Nodes[i], nodeId));                                               //（代码方法）可以通过dynamic_cast
			}
		}
		return variable_Lanes;

		while (true) {
			int cycle_Time = variable_Lanes[0].entrance_Link_Index.lanes_Index[1].lane_Phase_Info.intersection_Signal_Controller.signal_Cycle_Time;
			using second_Clock_Type = time_point<system_clock, seconds>;
			second_Clock_Type current_Time = time_point_cast<seconds>(system_clock::now());
			time_t now_Time = system_clock::to_time_t(current_Time);
			struct tm* now_Time_Norm;
			now_Time_Norm = localtime(&now_Time);
			printf("%d-%d-%d %d:%d:%d \n", 1900 + now_Time_Norm->tm_year, 1 + now_Time_Norm->tm_mon, now_Time_Norm->tm_mday, now_Time_Norm->tm_hour, now_Time_Norm->tm_min, now_Time_Norm->tm_sec);
			if ((current_Time.time_since_epoch().count() % cycle_Time) != 0) {                                                                       //当前时间是否位于周期的结束时刻
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
*/