#pragma warning(disable:4996)
#include "event_detect_in_class.h"
#include <iostream>

using namespace std::chrono;

Event_detect::Event_detect()
{
    string json_File_Name = "/home/pc/zhangnanyue/catkin_hikai_vr_fusion/src/hikai_vr_fusion/event_detect/cfg/intersection_Config.json";
    //read_Intersection_JsonFile(json_File_Name, intersection_Config);

    for (int i = 0; i < get<0>(intersection_Config); i++) {
        string temp = (i < 10) ? "0"+to_string(i):to_string(i);
        json_File_Name="/home/pc/zhangnanyue/catkin_hikai_vr_fusion/src/hikai_vr_fusion/event_detect/cfg/detect_Config_"+ temp + ".json";
        intersection_Devices_Detect.emplace(i, Device_Detect(json_File_Name));
    }
    //sub=nh.subscribe(sub_topic,10,&Event_detect::recieve_data,this);// /rfans_driver/rfans_points /rslidar_points    
    //pub = nh.advertise<event_msgs::EventObjectsArray>("/events", 10);
    
}

Event_detect::~Event_detect(){}


void Event_detect::handle_data(detector_target_info_t& detector_target_info){
    int object_event_source = 4;
    int device_source = -1;

    second_Clock_Type current_Time = time_point_cast<seconds>(system_clock::now());
    time_t now_Time = system_clock::to_time_t(current_Time);
    struct tm* now_Time_Norm;
    now_Time_Norm = localtime(&now_Time);
    string time_String = to_string(1900 + now_Time_Norm->tm_year) + "-" + to_string(1 + now_Time_Norm->tm_mon) + "-" + to_string(now_Time_Norm->tm_mday) + " " + to_string(now_Time_Norm->tm_hour) + ":" + to_string(now_Time_Norm->tm_min) + ":" + to_string(now_Time_Norm->tm_sec);

    double time_stamp = 0.0;
    double time_stamp_nsec = 0.0;

    for (int i = 0; i < 1; i++)
    {
        int sec = detector_target_info.sec;
        int nsec = detector_target_info.nsec;
        int id = detector_target_info.id;
        int lane_id = detector_target_info.laneId;
        uint32_t label = 5;
        double object_px = detector_target_info.radar_px;
        double object_py = detector_target_info.radar_py;
        double object_vx = detector_target_info.radar_vx;
        double object_vy = detector_target_info.radar_vy;
        double length = detector_target_info.dimensions_x;
        double width = detector_target_info.dimensions_y;
        double lat = detector_target_info.targetLatitude;
        double lon = detector_target_info.targetLongitude;
        int source = 1;

        Vehicleincident_Detection veh_test = Vehicleincident_Detection(sec, nsec, id, lane_id, label, 1, object_px, object_py, object_vx, object_vy, length, width);//初始化每个车辆的信息
        veh_test = Location_In_Zone("detect_zone", veh_test, intersection_Devices_Detect[source].detect_Config_Points).update_Veh();                //区域检测
        time_stamp = sec;
        time_stamp_nsec = veh_test.timestamp;

        if (veh_test.lane_Num <= (10 + intersection_Devices_Detect[source].lanes_Num)) {
            veh_test = Location_Cross_Line("entry_line", intersection_Devices_Detect[source].map_Vehs_Entry, veh_test, intersection_Devices_Detect[source].detect_Config_Points, time_stamp, window_Interval).update_Veh();  //驶入区域检测
            veh_test = Location_Cross_Line("stop_line", intersection_Devices_Detect[source].map_Vehs_Stop, veh_test, intersection_Devices_Detect[source].detect_Config_Points, time_stamp, window_Interval).update_Veh();   //驶出区域检测
           
            intersection_Devices_Detect[source].volume_test.get_Vehicles_Info(veh_test);                      //采集车辆，用于计算流量
            intersection_Devices_Detect[source].max_queue_test.get_Vehicles_Info(veh_test);                   //采集车辆，用于计算速度、密度、车头间距和排队长度
        }
    }
    for (auto it = intersection_Devices_Detect.begin(); it != intersection_Devices_Detect.end(); it++) {
        it->second.max_queue_test.Headway_Density_Caculation::caculation_Index();                             //计算车头间距、密度
        it->second.max_queue_test.caculation_Index();                                                         //计算排队长度
    }

    if (fmod(time_stamp, time_Interval) == 0.0 && !flag) {  
        double sum_Volume = 0.0;
        for (auto it = intersection_Devices_Detect.begin(); it != intersection_Devices_Detect.end(); it++) {
            it->second.volume_test.caculation_Index();                                                               //计算流量
         
            it->second.volume_test.update_Vehicles_Info();
        }
        flag = true;
    }

    for (auto it = intersection_Devices_Detect.begin(); it != intersection_Devices_Detect.end(); it++) {
        it->second.max_queue_test.Headway_Density_Caculation::update_Vehicles_Info();
        it->second.max_queue_test.update_Vehicles_Info();
    }

    if (fmod(time_stamp, time_Interval) != 0.0)
        flag = false;
    return;
}

        