#ifndef _EVENT_DETECT_IN_CLASS_H
#define _EVENT_DETECT_IN_CLASS_H

#include "traffic_incident.h"
#include <iostream>

//目标检测结果
struct detector_target_info_t {
    int sec;                     //秒
    int nsec;                    //纳秒
    int id;                      	//目标ID
    int laneId;				 		//获取车辆车道编号
    uint32_t label;               	//目标类别
    int vehicleType;              	//车辆类型
    double radar_px;             	//雷达x轴坐标
    double radar_py;             	//雷达y轴坐标
    double radar_vx;             	//雷达x轴速度
    double radar_vy;             	//雷达y轴速度
    double dimensions_x;         	//目标长度
    double dimensions_y;         	//目标宽度
    int vehicleSpeed;
    double targetLatitude;
    double targetLongitude;
} ;

class Device_Detect{                                                                                  //交叉口事件汇总类
public:
      Device_Detect() {};
      Device_Detect(string json_File_Name){
            //read_Detect_JsonFile(json_File_Name, detect_Config);
            detect_Config_Points = get<0>(detect_Config);
            lanes_Num = get<1>(detect_Config);
            volume_test = Volume_Caculation(lanes_Num,true, detect_Config_Points);
            max_queue_test = Max_Queue_Caculation(lanes_Num, true, speed_Start, speed_End, min_Vehs_Size, map_Lanes_Queue, detect_Config_Points);  
	       
	  }; 

public:
	  Volume_Caculation volume_test;                                                                     //定义流量指标
	  Max_Queue_Caculation max_queue_test;                                                               //定义排队长度指标

      tuple<map<string, vector<vector<Point>>>, int> detect_Config;                                      //配置文件        
      map<string, vector<vector<Point>>> detect_Config_Points;                                           //检测区间
      int lanes_Num;                                                                                     //进口道的车道总数
	  map<int, Vehicleincident_Detection> map_Vehs_Entry, map_Vehs_Stop;                                 //记录entry-stop对中，车辆上一时刻的信息
      map<int, bool> map_Lanes_Queue;                                                                    //记录车道排队的状态
};

class Event_detect{
public:
    Event_detect();
    void handle_data(detector_target_info_t& detector_target_info);

public:
    ~Event_detect();

public:
    tuple<int> intersection_Config;                                                                    //交叉口配置
    map<int, Device_Detect> intersection_Devices_Detect;                                               //根据雷达数，建立对应的事件变量数组
    int eventID = 0;                                                                                   //事件ID编号
    bool flag = false;                                                                                 //时间间隔内，是否已统计
};
#endif