﻿#pragma once
#include <iostream>
#include <math.h>
#include <vector>
#include <map>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;

using Maneuver = enum { StraightAllowed = 0, LeftAllowed, RightAllowed, UTurnAllowed, LeftTurnOnRedAllowed, RightTurnOnRedAllowed, LaneChangeAllowed, 
                        NoStoppingAllowed, YieldAllwaysRequired, GoWithHalt, Caution, Reserved };
using Turn_Type = enum { Straight = 11, Left = 12, Right = 13, StraightLeft = 21, StraightRight = 22, LeftRight = 23, All = 24, UTurn = 31 };
using Lane_Type = enum {};
using Light_State = enum {};
using Intersection_Status_Object = enum {};

static const double turn_Saturation_Limit = 0.8;               //饱和流率的阈值
static const double turn_Change_Limit = 0.4;                   //车道切换后的饱和流率差值的阈值

struct Point {
	double latitude;
	double longitude;
	double elevation;
};
struct Speed_Limit {
	int speedLimit_Type;
	double speed;
};
struct Connection {                        //车道下游
	int remoteIntersection_region;
	int remoteIntersection_nodeid;
	int connectingLane_laneId;            //辅助信号
	vector<Maneuver> connectingLane_maneuver;
};

class Movement {
public:
	Movement() {};
public:
	int remoteIntersection_region;
	int remoteIntersection_nodeid;
	int phase_Id;                      //主信号
};

class Movement_Index :public Movement {
public:
	Movement_Index() {};
public:
	int movement_Volume = 0;
	int movement_Capacity = 0;
	double movement_Sat_Ratio = 0.0;
	int lanes_Nums = 0;
};

/*-----------------------------------------------------------------------------------*/
class Phase_State {                                         //车道的SPAT配时类
public:
	Phase_State() {};
public:
	int phase_Id;
	Light_State light;
	int start_Time;
	int min_End_Time;
	int max_End_Time;
	int likely_End_Time;
};

class Intersection_State {                                   //信号控制器的SPAT状态类
public:
	Intersection_State() {};
public:
	int intersection_Id;
	Intersection_Status_Object status;
	int Minute_Of_Year;
	long long time_Stamp = time_point_cast<seconds>(system_clock::now()).time_since_epoch().count();
public:
	map<int,Phase_State> phases;
};

//双环配时类，相当于SPAT的整理版本
class Signal_Controller_Info {                                  //信号方案参数类
public:
	Signal_Controller_Info() {};
	Signal_Controller_Info(int msignal_Controller_ID):signal_Controller_ID(msignal_Controller_ID){
		get_Signal_Controller_Info();
	};
	void get_Signal_Controller_Info();
public:
	int signal_Controller_ID;
	int signal_Cycle_Time;
	int signal_Offset_Time;
};

class Signal_Phase_Info {                                       //信号相位参数类
public:
	Signal_Phase_Info() {};
	Signal_Phase_Info(int msignal_Controller_Id, int mphase_Id):phase_Id(mphase_Id) {
		intersection_Signal_Controller = Signal_Controller_Info(msignal_Controller_Id);
		get_Signal_Phase_Info();
	};
	void get_Signal_Phase_Info();
public:
	int phase_Id;
	int green_Time;
	int yellow_Time;
	int all_Red_Time;
public:
	Signal_Controller_Info intersection_Signal_Controller;
};
/*-----------------------------------------------------------------------------------------------*/

class Lane_Map {                                            //车道属性类，包含静态属性
public:
	Lane_Map() {};
	Lane_Map(int mupstreamId, int mnodeId, int mlaneId):upstreamId(mupstreamId), nodeId(mnodeId),laneId(mlaneId){
		get_Lane_Map_Info();
	};
	void get_Lane_Map_Info();
public:
	int upstreamId;
	int nodeId;
	int laneId;
	double laneWidth;
	Lane_Type laneType;
	vector<Maneuver> maneuvers;
	vector<Connection> connectsTo;
public:
	int signal_Controller_Id;
	int phase_Id;
};

class Lane_Index :public Lane_Map {                          //车道动态类，包含动态属性
public:
	Lane_Index() {};
	Lane_Index(int mupstreamId, int mnodeId, int mlaneId):Lane_Map(mupstreamId,mnodeId,mlaneId) {
		get_Lane_Volume_Info();
		lane_Phase_Info = Signal_Phase_Info(signal_Controller_Id, phase_Id);
		capacity_Intersection_Caculation();
		get_Lane_Queue_Info();
		lane_Delay_Caculation();
	};
	void get_Lane_Volume_Info();
	void get_Lane_Queue_Info() {};
	void lane_Delay_Caculation() {};
	void capacity_Intersection_Caculation();
public:
	int volume_Interval = 0;
	int volume = 0;
	int small_Car_Volume = 0;
	int medium_Car_Volume = 0;
	int large_Car_Volume = 0;
	int train_Car_Volume = 0;
	int transit_Car_Volume = 0;
	double queue_Length = 0.0;
	int queue_Num = 0;
	int capacity_Saturation = 0;
	int capacity_Intersection = 0;
	double delay_Vehicles_Start = 0.0;
	double delay_Red_Stop = 0.0;
	double delay_Queue_Clearance = 0.0;
public:
	Signal_Phase_Info lane_Phase_Info;
};

class Link_Map {                                             //路段属性类，包含静态属性
public:
	Link_Map() {};
	Link_Map(int mupstreamId, int mnodeId):upstreamNodeId_nodeid(mupstreamId),nodeId(mnodeId){
		get_Link_Map_Info();
	};
	void get_Link_Map_Info();
public:
	string name;
	int nodeId;
	int upstreamNodeId_nodeid;
	int upstreamNodeId_region;
	vector<Speed_Limit> speed_Limits;
	vector<Point> points;
	vector<Movement> movements;
public:
	map<int,Lane_Map> lanes;
};

class Link_Index :public Link_Map {                         //路段动态类，包含转向流量
public:
	Link_Index() {};
	Link_Index(int mupstreamId, int mnodeId):Link_Map(mupstreamId,mnodeId) {
		get_Link_Index_Info();
	};
	void get_Link_Index_Info();
	void movements_Index_Caculation();
public:
	map<Turn_Type,Movement_Index> link_Movements_Index;
	map<int,Lane_Index> lanes_Index;
};

class Node_Map {
public:
	Node_Map() {};
	Node_Map(int mnodeId):nodeId(mnodeId) {
		get_Node_Map_Info();
	};
	void get_Node_Map_Info();
public:
	int nodeId;
	string node_Name;
	vector<int> upstream_Nodes;
};
class Node_Index :public Node_Map{
public:
	Node_Index() {};
	Node_Index(int mnodeId) :Node_Map(mnodeId) {}
public:
	map<int,Link_Index> entrance_Links_Index;
};

/*--------------------------------------------------------------------------------------------------------*/
class Node_Control_Strategy {                                                         //控制抽象类
public:
	virtual void get_Node_Index_Info() = 0;
	virtual void implement_Node_Control_Function() = 0;
	virtual void put_Control_Delivery() = 0;
	virtual void update_Node_Index_Info() = 0;
};

class Node_Variable_Lane_Control:public Node_Control_Strategy,public Link_Index{      //可变导向车道的控制类
public:
	Node_Variable_Lane_Control() {};
	Node_Variable_Lane_Control(int mupstreamId,int mnodeId){
		entrance_Link_Index = Link_Index(mupstreamId, mnodeId);
		get_Variable_Lane_State_Info();
	};
public:
	virtual void get_Node_Index_Info();
	virtual void implement_Node_Control_Function();
	virtual void put_Control_Delivery();
	virtual void update_Node_Index_Info();
	bool modify_Movements_Index(int laneId);
	void get_Variable_Lane_State_Info();
public:
	map<int, Turn_Type> variable_Lane_State;                        //已渠化的可变车道当前状态
	map<int, int> map_Iterative_Nums;                               //每个车道，周期迭代的次数
	Link_Index entrance_Link_Index;
};

void traffic_Control_Integration(Node_Control_Strategy *node_control, int nodeId);
/*--------------------------------------------------------------------------------------------------------*/

struct Phase_Node {                                             //相序嵌套的邻接表类
	int phase_Id;
	Phase_Node *next;
};

//决策树的节点类

//决策树模型类-控制类

//交通控制算法
//1)相序嵌套矩阵，由邻接表建立

//2)当前相位下，车道流量和车道排队车辆的更新

//3)相位时长初始化
//车道流量、车道排队长度，数据源为毫米波雷达（已由MEC处理，暂存MySQL），车道控制信息按照SPAT获取

//4)周期时长调整

//5)延误计算
//车道流量、车道排队长度、车道通行能力已有

//6)相位时长优化


