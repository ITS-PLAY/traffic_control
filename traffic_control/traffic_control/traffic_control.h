#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;
//初始化
enum class Maneuver {};
enum class Lane_Type {};
enum class Light_State {};
enum class Intersection_Status_Object {};

struct Point {
	double latitude;
	double longitude;
	double elevation;
};
struct Speed_Limit {
	int speedLimit_Type;
	double speed;
};
struct Connection {
	int remoteIntersection_region;
	int remoteIntersection_nodeid;
	int connectingLane_laneId;           //辅助信号
	vector<Maneuver> connectingLane_maneuver;
};

class Movement {
public:
	Movement() {};
public:
	int remoteIntersection_region;
	int remoteIntersection_nodeid;
	int phaseId;                      //主信号
};

class Movement_Flow :public Movement {
public:
	Movement_Flow() {};
public:
	double movement_Volume = 0.0;
	double movement_Capacity = 0.0;
	double movement_Sat_Ratio = 0.0;
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
	// time_Stamp;

public:
	vector<Phase_State> phases;
};

//双环配时类，相当于SPAT的整理版本
class Signal_Controller_Info {                                  //信号方案参数类
public:
	Signal_Controller_Info() {};

public:
	int signal_Controller_ID;
	int signal_Cycle_Time;
	int signal_Offset_Time;

};

class Signal_Phase_Info {                                       //信号相位参数类
public:
	Signal_Phase_Info() {};
public:
	int phase_ID;
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
public:
	int laneId;
	double laneWidth;
	Lane_Type laneType;
	vector<Maneuver> maneuvers;
	vector<Connection> connectsTo;
};

class Lane_Index :public Lane_Map {                          //车道动态类，包含动态属性
public:
	Lane_Index() {};
	void get_Lane_Volume_Info();
	void get_Lane_Queue_Info();
	void lane_Delay_Caculation();

public:
	int volume_Interval;
	int volume;
	int small_Car_Volume;
	int medium_Car_Volume;
	int large_Car_Volume;
	int train_Car_Volume;
	int transit_Car_Volume;
	double queue_Length;
	int queue_Num;
	int capacity_Saturation;
	int capacity_Intersection;
	double delay_Vehicles_Start;
	double delay_Red_Stop;
	double delay_Queue_Clearance;

private:
	Signal_Phase_Info lane_Phase_Info;
};

class Link_Map {                                             //路段类
public:
	Link_Map() {};
public:
	string name;
	int upstreamNodeId_nodeid;
	int upstreamNodeId_region;
	vector<Speed_Limit> speed_Limits;
	vector<Point> points;
	vector<Movement> movements;
public:
	vector<Lane_Map> lanes;
};

class Link_Index :public Link_Map {                         //路段动态类，包含转向流量
public:
	Link_Index() {};

public:
	vector<Movement_Flow> movements_Flow;
	vector<Lane_Index> entrance_Link_Index;
};

class Variable_Lane_Control {                                  //可变导向的功能类
public:
	Variable_Lane_Control(int node_Id) {};
public:
	void lane_Movement_Judge();
	static int iterative_Nums;         //周期迭代的次数

public:
	Link_Index entrance_Link_Index;
};


struct Phase_Node {                                             //相序嵌套的邻接表类
	int phase_Id;
	Phase_Node *next;
};

//决策树的节点类

//决策树模型类-控制类



//可变导向车道方法
//车道流量，数据源为毫米波雷达（已由MEC处理，暂存MySQL）


//车道通行能力计算，控制数据按照SPAT获取，路网数据按照MAP获取（已由RSU处理）


//转向的饱和度，取转向车道的最大饱和度，由车道流量和通行能力计算

//切换的判断



//交通控制算法
//1)相序嵌套矩阵，由邻接表建立

//2)当前相位下，车道流量和车道排队车辆的更新

//3)相位时长初始化
//车道流量、车道排队长度，数据源为毫米波雷达（已由MEC处理，暂存MySQL），车道控制信息按照SPAT获取

//4)周期时长调整

//5)延误计算
//车道流量、车道排队长度、车道通行能力已有

//6)相位时长优化


