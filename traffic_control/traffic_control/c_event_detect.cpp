#include "event_detect.h"                                                                                                 //交通指标统计功能的头文件
#include "traffic_control.h"                                                                                              //控制类功能的头文件

detector_target_info_t*  receive_data() {                                                                           //以接收/获取一个检测目标为例
	detector_target_info_t* mdetect = (detector_target_info_t*) malloc(sizeof(detector_target_info_t));
	mdetect->sec = 1;
	mdetect->nsec = 1;
	mdetect->id = 1;
	mdetect->laneId = 12;
	mdetect->label = 5;
	mdetect->vehicleType = 1;
	mdetect->radar_px = -1.7;
	mdetect->radar_py = 205.0;
	mdetect->radar_vx = 0.0; 
	mdetect->radar_vy = 3.0;
	mdetect->dimensions_x = 1.8;
	mdetect->dimensions_y = 4;
	mdetect->source_id = 1;
	return mdetect;
}

#ifndef _cplusplus
#define _cplusplus
#include "c_event_detect.h"
#endif

void c_call_function() {
	//map_data_t* mmap_data = nullptr;                                                                                   //整体地图数据
	//intersection_traffic_flow_info_t* mintersection_index = nullptr;                                                   //整体交通指标数据

	node_data_t* mnode_data = nullptr;                                                                                   //交叉口的地图                                                                                                          
	node_traffic_flow_info_t* mnode_index = nullptr;                                                                     //交叉口的交通指标
	Event_detect mevent(1);                                                                                                 //指标和事件代码直接迁移过来  

	//Node_Variable_Lane_Control node_Variable_Control(0, 1, *mnode_data, *mnode_index);

	while (true) {
		detector_target_info_t* mdetector_target = receive_data();                                                       //接收毫米波雷达数据
		mevent.handle_data(*mdetector_target, *mnode_data, *mnode_index);                                                //计算流量和排队长度

		//for (int i = 0; i < variable_Lanes.size(); i++) {                                                              //每个进口道开始执行可变车道功能,先清空数据再获取新数据
		//	variable_Lanes[i].update_Node_Index_Info();
		//	variable_Lanes[i].get_Node_Index_Info();
		//	variable_Lanes[i].implement_Node_Control_Function();
		//}
	}
}




