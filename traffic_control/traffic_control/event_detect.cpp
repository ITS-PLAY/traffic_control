#pragma warning(disable:4996)
#include "event_detect.h"
#include <iostream>

using namespace std::chrono;

Event_detect::Event_detect(int mintersection_Id){
    intersection_Id = mintersection_Id;
    string json_File_Name = "../../jsonFile/intersection_Config.json";                                          //TODO：需要提前指定 交叉口配置文件路径
    read_Intersection_JsonFile(json_File_Name, intersection_Config);
    for (int i = 0; i < get<0>(intersection_Config); i++) {
        string temp = (i < 10) ? "0"+to_string(i):to_string(i);
        json_File_Name="D:\\traffic_control\\jsonFile\\detect_Config_"+ temp + ".json";                                         //TODO：需要提前指定 雷达配置文件路径
        intersection_Devices_Detect.emplace(i, Device_Detect(json_File_Name));
    }
}

Event_detect::~Event_detect(){}

void Event_detect::mapdata_to_mapindex(node_data_t& mnode_data, node_traffic_flow_info_t& mnode_index) {
     mnode_index.nodeId = mnode_data.id.nodeid;                                                                      //添加节点的nodeid
     mnode_index.linkNum = mnode_data.linkNum;                                                                       //添加节点的linkNum
     for (int j = 0; j < mnode_index.linkNum; j++) {
         mnode_index.link_info[j].laneNum = mnode_data.links[j].laneNum;                                             //添加路段的laneNum
         for (int k = 0; k < mnode_index.link_info[j].laneNum; k++) {
             mnode_index.link_info[j].lane_info[k].laneId = mnode_data.links[j].lanes[k].laneId;                     //添加车道的laneId
             //mintersection_index.node_info[i].link_info[j].lane_info[k].isDynamicLane                               //添加是否动态车道                       
             mnode_index.link_info[j].lane_info[k].currentManeuver = *(mnode_data.links[j].lanes[k].maneuvers);        //添加当前车道允许的转向行为

         }
     }

    return;
}

/* 根据毫米波雷达的目标数据，计算流量和排队长度
 * 参数1 detector_target_info_t                              毫米波雷达的目标数据
 * 参数2 map_data_t                                          地图的数据 
 * 参数3 intersection_traffic_flow_info_t                    输出的交通指标数据（如流量、排队长度等）
 */
void Event_detect::handle_data(detector_target_info_t& detector_target_info, node_data_t& mnode_data, node_traffic_flow_info_t& mnode_index) {
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
            it->second.volume_test.caculation_Index();                                                        //计算流量

            it->second.volume_test.update_Vehicles_Info();
        }

        //将流量和排队长度写入交叉口变量mintersection_index                                                                                              
        mapdata_to_mapindex(mnode_data, mnode_index);                                                          //根据mmap_data填写mintersection_index

        int link_num = mnode_data.linkNum;                                                                     //从地图获取交叉口进口道数
        for (int i = 1; i <= link_num; i++) {
            int ladar_id = i;                                                                                  //根据地图进口道的编号匹配雷达编号
            link_data_t mlink_data = mnode_data.links[i];
            for (int j = 0; j < mlink_data.laneNum; i++) {
                int lane_id_ladar = j;                                                                         //根据地图的车道编号匹配雷达检测的车道编号
                mnode_index.link_info[i].lane_info[j].laneVolume = static_cast<int>(intersection_Devices_Detect[ladar_id].volume_test.lanes_Volume[lane_id_ladar]);
                mnode_index.link_info[i].lane_info[j].laneQueueLength = intersection_Devices_Detect[ladar_id].max_queue_test.lanes_Queue_Length[lane_id_ladar];
                mnode_index.link_info[i].lane_info[j].laneQueueNum = intersection_Devices_Detect[ladar_id].max_queue_test.lanes_Queue_Num[lane_id_ladar];
            }
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

        