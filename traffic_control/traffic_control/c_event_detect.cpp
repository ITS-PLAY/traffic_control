#include "event_detect.h"                                                                                                 //��ָͨ��ͳ�ƹ��ܵ�ͷ�ļ�
#include "traffic_control.h"                                                                                              //�����๦�ܵ�ͷ�ļ�

detector_target_info_t*  receive_data() {                                                                           //�Խ���/��ȡһ�����Ŀ��Ϊ��
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
	//map_data_t* mmap_data = nullptr;                                                                                   //�����ͼ����
	//intersection_traffic_flow_info_t* mintersection_index = nullptr;                                                   //���彻ָͨ������

	node_data_t* mnode_data = nullptr;                                                                                   //����ڵĵ�ͼ                                                                                                          
	node_traffic_flow_info_t* mnode_index = nullptr;                                                                     //����ڵĽ�ָͨ��
	Event_detect mevent(1);                                                                                                 //ָ����¼�����ֱ��Ǩ�ƹ���  

	//Node_Variable_Lane_Control node_Variable_Control(0, 1, *mnode_data, *mnode_index);

	while (true) {
		detector_target_info_t* mdetector_target = receive_data();                                                       //���պ��ײ��״�����
		mevent.handle_data(*mdetector_target, *mnode_data, *mnode_index);                                                //�����������Ŷӳ���

		//for (int i = 0; i < variable_Lanes.size(); i++) {                                                              //ÿ�����ڵ���ʼִ�пɱ䳵������,����������ٻ�ȡ������
		//	variable_Lanes[i].update_Node_Index_Info();
		//	variable_Lanes[i].get_Node_Index_Info();
		//	variable_Lanes[i].implement_Node_Control_Function();
		//}
	}
}




