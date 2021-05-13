/*
 * 	File  : V2xMsg_UserCommon.h
 *  Breif : v2x消息用户层通用数据结构
 *  Date  : 2020.4.13
 *  Author: zhuyun/chenkai
 *
 */

#ifndef _V2X_MSG_USER_COMMON_H_
#define _V2X_MSG_USER_COMMON_H_

/*
 * @struct point_data_t
 * @brief point data
 */
typedef struct _point_data
{
    double    lat;		//range -90.0000000 to + 90.0000000 degree 	Units of 1/10 micro degree
	double    Long;		//range -180.0000000 to + 180.0000000 degree 	Units of 1/10 micro degree
	double   *elev;		//range -409.5 to + 6143.9 meters,  Units of 10 cm
}point_data_t;

/*
 * @struct vehicle_size_t
 * @brief vehicle_size_t
 */
typedef struct _vehicle_size {
	double  	 width;		//rangge 0 to 10.23 meters, Units 1 cm
	double  	 length;	//rangge 0 to 40.95 meters, Units 1 cm	
	double  	*height;	//rangge 0 to 6.35 meters, Units 5 cm
} vehicle_size_t;

/*
 * @struct description_PR_e
 * @brief description_PR_e
 */
typedef enum _description_PR {
	Hik_description_PR_NOTHING,	
	Hik_description_PR_textString,
	Hik_description_PR_textGB2312
} description_PR_e;

/*
 * @struct test_string_t
 * @brief test_string_t
 */
typedef struct _test_string {
	int              stringLen;			//range 1-512
    unsigned char   *textString;
} test_string_t;

/*
 * @struct test_GB2312_t
 * @brief test_GB2312_t
 */
typedef struct _test_GB2312 {
	int              GB2312Len;			//range 2-521
    unsigned char   *textGB2312;
} test_GB2312_t;

/*
 * @struct description_t
 * @brief description_t
 */
typedef struct _description {
	description_PR_e present;
	union Hik_description_u {
		test_string_t    testString;
		test_GB2312_t	 textGB2312;
	} choice;
} description_t;

/*
 * @struct elevation_confidence_e
 * @brief elevation_confidence_e
 */
typedef enum _elevation_confidence {
	Hik_elevationConfidence_unavailable	= 0,
	Hik_elevationConfidence_elev_500_00	= 1,
	Hik_elevationConfidence_elev_200_00	= 2,
	Hik_elevationConfidence_elev_100_00	= 3,
	Hik_elevationConfidence_elev_050_00	= 4,
	Hik_elevationConfidence_elev_020_00	= 5,
	Hik_elevationConfidence_elev_010_00	= 6,
	Hik_elevationConfidence_elev_005_00	= 7,
	Hik_elevationConfidence_elev_002_00	= 8,
	Hik_elevationConfidence_elev_001_00	= 9,
	Hik_elevationConfidence_elev_000_50	= 10,
	Hik_elevationConfidence_elev_000_20	= 11,
	Hik_elevationConfidence_elev_000_10	= 12,
	Hik_elevationConfidence_elev_000_05	= 13,
	Hik_elevationConfidence_elev_000_02	= 14,
	Hik_elevationConfidence_elev_000_01	= 15
} elevation_confidence_e;


/**
 * 数值描述了 95%置信水平的车辆位置精度。该精度理论上只考虑了当前位置传感器的误差。但是， 当系统能够自动检测错误并修正，相应的精度数值也应该提高
 *
 * */
typedef enum
{
	Hik_PositionConfidence_unavailable	= 0,
	Hik_PositionConfidence_a500m		= 1,
	Hik_PositionConfidence_a200m		= 2,
	Hik_PositionConfidence_a100m		= 3,
	Hik_PositionConfidence_a50m			= 4,
	Hik_PositionConfidence_a20m			= 5,
	Hik_PositionConfidence_a10m			= 6,
	Hik_PositionConfidence_a5m			= 7,
	Hik_PositionConfidence_a2m			= 8,
	Hik_PositionConfidence_a1m			= 9,
	Hik_PositionConfidence_a50cm		= 10,
	Hik_PositionConfidence_a20cm		= 11,
	Hik_PositionConfidence_a10cm		= 12,
	Hik_PositionConfidence_a5cm			= 13,
	Hik_PositionConfidence_a2cm			= 14,
	Hik_PositionConfidence_a1cm			= 15
} Hik_PositionConfidence_e;

/**
 * 数值描述了95%置信水平的时间精度。该精度理论上只考虑了当前计时系统本身传感器的误差。但是，当系统能够自动检测错误并修正，相应的精度数值也应该提高
 * 
 * */
typedef enum
{
	Hik_TimeConfidence_unavailable			 = 0,
	Hik_TimeConfidence_100_seconds  		 = 1,
	Hik_TimeConfidence_50_seconds   		 = 2,
	Hik_TimeConfidence_20_seconds   		 = 3,
	Hik_TimeConfidence_10_seconds   		 = 4,
	Hik_TimeConfidence_2_seconds   			 = 5,
	Hik_TimeConfidence_1_seconds  			 = 6,
	Hik_TimeConfidence_0_5_seconds  		 = 7,
	Hik_TimeConfidence_0_2_seconds  		 = 8,
	Hik_TimeConfidence_0_1_seconds  		 = 9,
	Hik_TimeConfidence_0_05_seconds  		 = 10,
	Hik_TimeConfidence_0_02_seconds  		 = 11,
	Hik_TimeConfidence_0_01_seconds  		 = 12,
	Hik_TimeConfidence_0_005_seconds  		 = 13,
	Hik_TimeConfidence_0_002_seconds  		 = 14,
	Hik_TimeConfidence_0_001_seconds  		 = 15,
	Hik_TimeConfidence_0_0005_seconds  		 = 16,
	Hik_TimeConfidence_0_0002_seconds  		 = 17,
	Hik_TimeConfidence_0_0001_seconds  		 = 18,
	Hik_TimeConfidence_0_00005_seconds  	 = 19,
	Hik_TimeConfidence_0_00002_seconds  	 = 20,
	Hik_TimeConfidence_0_00001_seconds  	 = 21,
	Hik_TimeConfidence_0_000005_seconds  	 = 22,
	Hik_TimeConfidence_0_000002_seconds  	 = 23,
	Hik_TimeConfidence_0_000001_seconds  	 = 24,
	Hik_TimeConfidence_0_0000005_seconds  	 = 25,
	Hik_TimeConfidence_0_0000002_seconds  	 = 26,
	Hik_TimeConfidence_0_0000001_seconds  	 = 27,
	Hik_TimeConfidence_0_00000005_seconds 	 = 28,
	Hik_TimeConfidence_0_00000002_seconds  	 = 29,
	Hik_TimeConfidence_0_00000001_seconds  	 = 30,
	Hik_TimeConfidence_0_000000005_seconds   = 31,
	Hik_TimeConfidence_0_000000002_seconds   = 32,
	Hik_TimeConfidence_0_000000001_seconds   = 33,
	Hik_TimeConfidence_0_0000000005_seconds  = 34,
	Hik_TimeConfidence_0_0000000002_seconds  = 35,
	Hik_TimeConfidence_0_0000000001_seconds  = 36,
	Hik_TimeConfidence_0_00000000005_seconds = 37,
	Hik_TimeConfidence_0_00000000002_seconds = 38,
	Hik_TimeConfidence_0_00000000001_seconds = 39,	
} Hik_TimeConfidence_e;


/*
 * @struct position_confidence_set_t
 * @brief position_confidence_set_t
 * 定义当前实时位置（经纬度和高程）的精度大小，包括水平位置精度和高程精度，由系统自身进行 实时计算和更新。
 */
typedef struct _position_confidence_set {
	Hik_PositionConfidence_e	 	 pos;
	elevation_confidence_e 		*elevation;
} position_confidence_set_t;

/*
 * @struct speed_confidence_e
 * @brief speed_confidence_e
 */
typedef enum _speed_confidence {
	Hik_speedConfidence_unavailable	= 0,
	Hik_speedConfidence_prec100ms	= 1,
	Hik_speedConfidence_prec10ms	= 2,
	Hik_speedConfidence_prec5ms	    = 3,
	Hik_speedConfidence_prec1ms	    = 4,
	Hik_speedConfidence_prec0_1ms	= 5,
	Hik_speedConfidence_prec0_05ms	= 6,
	Hik_speedConfidence_prec0_01ms	= 7
}speed_confidence_e;

/*
 * @struct heading_confidence_e
 * @brief heading_confidence_e
 */
typedef enum _heading_confidence {
	Hik_headingConfidence_unavailable	= 0,
	Hik_headingConfidence_prec10deg	    = 1,
	Hik_headingConfidence_prec05deg   	= 2,
	Hik_headingConfidence_prec01deg	    = 3,
	Hik_headingConfidence_prec0_1deg	= 4,
	Hik_headingConfidence_prec0_05deg	= 5,
	Hik_headingConfidence_prec0_01deg	= 6,
	Hik_headingConfidence_prec0_0125deg	= 7
} heading_confidence_e;

/*
 * @struct steering_wheel_angle_confidence_e
 * @brief steering_wheel_angle_confidence_e
 */
typedef enum _steering_wheel_angle_confidence {
	Hik_steeringWheelAngleConfidence_unavailable	= 0,
	Hik_steeringWheelAngleConfidence_prec2deg	    = 1,
	Hik_steeringWheelAngleConfidence_prec1deg	    = 2,
	Hik_steeringWheelAngleConfidence_prec0_02deg	= 3
} steering_wheel_angle_confidence_e;

/*
 * @struct motion_confidence_set_t
 * @brief motion_confidence_set_t
 */
typedef struct _motion_confidence_set {
	speed_confidence_e	                	*speedCfd;
	heading_confidence_e	            	*headingCfd;
	steering_wheel_angle_confidence_e		*steerCfd;
} motion_confidence_set_t;

/*
 * @struct acceleration_set4way_t
 * @brief acceleration_set4way_t
 * 定义车辆四轴加速度。 
 * Long：纵向加速度。向前加速为正，反向为负。 
 * Lat：横向加速度。向右加速为正，反向为负。 
 * Vert：垂直加速度。沿重力方向向下为正，反向为负。 
 * Yaw：横摆角速度。顺时针旋转为正，反向为负。
 */
typedef struct _acceleration_set4way {
	double      	 Long;//定义车辆加速度。 分辨率为 0.01m/s2，数值 2001 为无效数值[-20,20]
	double      	 lat;//定义车辆加速度。 分辨率为 0.01m/s2，数值 2001 为无效数值[-20,20]
	double      	 vert;//定义 Z 轴方向的加速度大小，Z 轴方向竖直向下，沿着 Z 轴方向为正。 分辨率为 0.02G，G 为重力加速度值 9.80665m/s2。[-2.52, +2.54]
	double      	 yaw;//车辆横摆角速度。指汽车绕垂直轴的偏转，该偏转的大小代表汽车的稳定程度。如果偏转角速度达 到一个阈值，说明汽车发生测滑或者甩尾等危险工况。 顺时针旋转为正，逆时针为负。 数据分辨率为 0.01°/s, [-327.67, 32767]
} acceleration_set4way_t;

typedef enum
{
	Hik_unknownVehicleClass = 0,
	Hik_specialVehicleClass = 1,
	Hik_passenger_Vehicle_TypeUnknown = 10,
	Hik_passenger_Vehicle_TypeOther = 11,
	Hik_lightTruck_Vehicle_TypeUnknown = 20,
	Hik_lightTruck_Vehicle_TypeOther = 21,
	Hik_truck_Vehicle_TypeUnknown = 25,
	Hik_truck_Vehicle_TypeOther = 26,
	Hik_truck_axleCnt2 = 27,
	Hik_truck_axleCnt3 = 28,
	Hik_truck_axleCnt4 = 29,
	Hik_truck_axleCnt4Trailer = 30,
	Hik_truck_axleCnt5Trailer = 31,
	Hik_truck_axleCnt6Trailer = 32,
	Hik_truck_axleCnt5MultiTrailer = 33,
	Hik_truck_axleCnt6MultiTrailer = 34,
	Hik_truck_axleCnt7MultiTrailer = 35,
	Hik_motorcycle_TypeUnknown = 40,
	Hik_motorcycle_TypeOther = 41,
	Hik_motorcycle_Cruiser_Standard = 42,
	Hik_motorcycle_SportUnclad = 43,
	Hik_motorcycle_SportTouring = 44,
	Hik_motorcycle_SuperSport = 45,
	Hik_motorcycle_Touring = 46,
	Hik_motorcycle_Trike = 47,
	Hik_motorcycle_wPassengers = 48,
	Hik_transit_TypeUnknown = 50,
	Hik_transit_TypeOther = 51,
	Hik_transit_BRT = 52,
	Hik_transit_ExpressBus = 53,
	Hik_transit_LocalBus = 54,
	Hik_transit_SchoolBus = 55,
	Hik_transit_FixedGuideway = 56,
	Hik_transit_Paratransit = 57,
	Hik_transit_Paratransit_Ambulance = 58,
	Hik_emergency_TypeUnknown = 60,
	Hik_emergency_TypeOther = 61,
	Hik_emergency_Fire_Light_Vehicle = 62,
	Hik_emergency_Fire_Heavy_Vehicle = 63,
	Hik_emergency_Fire_Paramedic_Vehicle = 64,
	Hik_emergency_Fire_Ambulance_Vehicle = 65,
	Hik_emergency_Police_Light_Vehicle = 66,
	Hik_emergency_Police_Heavy_Vehicle = 67,
	Hik_emergency_Other_Responder = 68,
	Hik_emergency_Other_Ambulance = 69,
	Hik_otherTraveler_TypeUnknown = 80,
	Hik_otherTraveler_TypeOther = 81,
	Hik_otherTraveler_Pedestrian = 82,
	Hik_otherTraveler_Visually_Disabled = 83,
	Hik_otherTraveler_Physically_Disabled = 84,
	Hik_otherTraveler_Bicycle = 85,
	Hik_otherTraveler_Vulnerable_Roadworker = 86,
	Hik_infrastructure_TypeUnknown = 90,
	Hik_infrastructure_Fixed = 91,
	Hik_infrastructure_Movable = 92,
	Hik_equipped_CargoTrailer = 93,
}Hik_vehicle_type_e;

typedef enum
{
	Hik_unknownFuel = 0,
	Hik_gasoline = 1,
	Hik_ethanol = 2,
	Hik_diesel = 3,
	Hik_electric = 4,
	Hik_hybrid = 5,
	Hik_hydrogen = 6,
	Hik_natGasLiquid = 7,
	Hik_natGasComp = 8,
	Hik_propane = 9,
}Hik_FuelType_e;
/*
 * @struct vehicle_classification_t
 * @brief vehicle_classification_t
 */
typedef struct _vehicle_classification {
	Hik_vehicle_type_e   classification;
	Hik_FuelType_e     	*fuelType;  
} vehicle_classification_t;

/*
 * @struct node_reference_id_t
 * @brief node_reference_id_t
 * 节点 ID 是由一个全局唯一的地区 ID 和一个地区内部唯一的节点 ID 组成。
 */
typedef struct _node_reference_id {
	int   	    *region;
	int     	 nodeid;
} node_reference_id_t;


/*
 * @struct date_time_t
 * @brief date_time_t
 */
typedef struct _date_time {
	int	*year;
	int	*month;
	int	*day;
	int	*hour;
	int	*minute;
	int	*second;
	int	*offset;
} date_time_t;


/*
 * @struct basic_safety_message_t
 * @brief bsm data
 */
typedef struct _positional_accuracy {
	double	 semiMajor;
	double	 semiMinor;
	double	 orientation;
} positional_accuracy_t;



/*
 * @struct reference_path_t
 * @brief reference_path_t
 */
typedef struct _reference_path {
    int              pathPointNum;
	point_data_t    *pathPoints;
	double	         pathRadius;//表示绝对值半径大小。 分辨率为 10cm。[0, 6553.5]
} reference_path_t;


/*
 * @struct reference_link_t
 * @brief reference_link_t
 * 定义关联路段，用路段的上下游节点 ID 表示。 路段的上下游节点唯一确定一条路段。 可包含具体的关联车道。如不指定车道，则默认为该路段所有车道。
 */
typedef struct _reference_link {
	node_reference_id_t  upstreamNodeId;
	node_reference_id_t	 downstreamNodeId;

	int   	            *referenceLanes;
	#define REFERENCELANES_RESERVED	   0x8000
	#define REFERENCELANES_LANE1	   0x4000
	#define REFERENCELANES_LANE2	   0x2000
	#define REFERENCELANES_LANE3	   0x1000
	#define REFERENCELANES_LANE4	   0x800
	#define REFERENCELANES_LANE5	   0x400
	#define REFERENCELANES_LANE6	   0x200
	#define REFERENCELANES_LANE7	   0x100
	#define REFERENCELANES_LANE8	   0x80
	#define REFERENCELANES_LANE9	   0x40
	#define REFERENCELANES_LANE10	   0x20
	#define REFERENCELANES_LANE11	   0x10
	#define REFERENCELANES_LANE12	   0x08
	#define REFERENCELANES_LANE13	   0x04
	#define REFERENCELANES_LANE14	   0x02
	#define REFERENCELANES_LANE15	   0x01
} reference_link_t;

//定义加速度置信度
typedef enum
{
	HIK_AccConfidence_unavailable	= 0,
	HIK_AccConfidence_prec100deg	= 1,
	HIK_AccConfidence_prec10deg	= 2,
	HIK_AccConfidence_prec5deg	= 3,
	HIK_AccConfidence_prec1deg	= 4,
	HIK_AccConfidence_prec0_1deg	= 5,
	HIK_AccConfidence_prec0_05deg	= 6,
	HIK_AccConfidence_prec0_01deg	= 7

} HIK_AccConfidence_e;

//定义角速度置信度
typedef enum
{
	HIK_AngularVConfidence_unavailable	= 0,
	HIK_AngularVConfidence_prec100deg	= 1,
	HIK_AngularVConfidence_prec10deg	= 2,
	HIK_AngularVConfidence_prec5deg	= 3,
	HIK_AngularVConfidence_prec1deg	= 4,
	HIK_AngularVConfidence_prec0_1deg	= 5,
	HIK_AngularVConfidence_prec0_05deg	= 6,
	HIK_AngularVConfidence_prec0_01deg	= 7

} HIK_AngularVConfidence_e;

/**
 * 定义四轴加速度置信度
 */
typedef struct
{
	int lonAccConfidence;   //HIK_AccConfidence_e
	int latAccConfidence;   //HIK_AccConfidence_e
	int vertAccConfidence;  //HIK_AccConfidence_e
	int yawRateCon;		 //HIK_AngularVConfidence_e

}accSet4Way_confidence_t;



/**
 * pathPlanningPoint
 *定义路径规划中的位置点信息。 包括地图中的位置、相对经纬度和相对高程、建议速度、航向角、四轴加速度、预计到达时间以及 相关参数的置信度水平。
 * */
typedef struct
{
	reference_link_t 			*posInMap;//OPTIONAL
	point_data_t				pos;
	position_confidence_set_t 	*posAccuracy;
	double 						*speed;
	int 						*speedCfd;//speed_confidence_e
	double	 					*heading;
	int							*headingCfd;//heading_confidence_e
	acceleration_set4way_t 		*accelSet;
	accSet4Way_confidence_t 	*acc4WayConfidence; //四轴加速度置信度
	double 						*estimatedTime;//分辨率为 10 毫秒
	double 						*timeConfidence;//定义置信度。 分辨率为 0.005

}path_point_t;


//定义路径规划信息
typedef struct
{
	int 				path_planning_num;
	path_point_t 		*path_planning;//定义路径规划中的位置点信息。包括地图中的位置、相对经纬度和相对高程、建议速度、航向角、四轴加速度、预计到达时间以及相关参数的置信度水平。

}path_planning_list_t;


typedef struct
{
	int 	behavior;
	#define DRIVEBEHAVIOR_GOSTRAIGHTFORWARD	        	0x2000//直行
	#define DRIVEBEHAVIOR_LANECHANGINGTOLEFT	        0x1000//向左变更车道
	#define DRIVEBEHAVIOR_LANECHANGINGTORIGHT	        0x800//向右变更车道
	#define DRIVEBEHAVIOR_RAMPIN	                 	0x400//匝道进入
	#define DRIVEBEHAVIOR_RAMPOUT	         			0x200//匝道驶出
	#define DRIVEBEHAVIOR_INTERSECTIONSTRAIGHTTHROUGH	0x100//直行通过交叉路口
	#define DRIVEBEHAVIOR_INTERSECTIONTURNLEFT	        0x80//左转通过交叉路口
	#define DRIVEBEHAVIOR_INTERSECTIONTURNRIGHT	        0x40//右转通过交叉路口
	#define DRIVEBEHAVIOR_INTERSECTIONUTURN	            0x20//掉头通过交叉路口
	#define DRIVEBEHAVIOR_STOP_AND_GO	                0x10//即停即走
	#define DRIVEBEHAVIOR_STOP	                        0x08//停止
	#define DRIVEBEHAVIOR_SLOW_DOWN                     0x04//减速慢行
	#define DRIVEBEHAVIOR_SPEED_UP                     	0x02//加速行驶
	#define DRIVEBEHAVIOR_PARKING                       0x01//泊车

}drive_behavior_t;


typedef struct
{
	int 	coordinationInfo;
	#define COORDINATIONINFO_COOPERATIVELANECHANGING	    0x80
	#define COORDINATIONINFO_COOPERATIVEVEHMERGING	        0x40
	#define COORDINATIONINFO_LANECHANGINGATINTERSECTION	    0x20
	#define COORDINATIONINFO_NO_SIGNALINTERSECTIONPASSING	0x10
	#define COORDINATIONINFO_DYNAMICLANEMANAGEMENT	        0x08
	#define COORDINATIONINFO_LANERESERVATION				0x04
	#define COORDINATIONINFO_LANERESTRICTION	        	0x02
	#define COORDINATIONINFO_SIGNALPRIORITY	        		0x01

}coordination_info_t;



typedef enum
{
	HIK_PersonalDeviceUserType_unavailable	= 0,
	HIK_PersonalDeviceUserType_aPEDESTRIAN	= 1,
	HIK_PersonalDeviceUserType_aPEDALCYCLIST	= 2,
	HIK_PersonalDeviceUserType_aROADWORKER	= 3,
	HIK_PersonalDeviceUserType_anANIMAL	= 4
	/*
	 * Enumeration is extensible
	 */
} HIK_PersonalDeviceUserType_e;


typedef enum
{
	HIK_NumberOfParticipantsInCluster_unavailable	= 0,
	HIK_NumberOfParticipantsInCluster_small	= 1,
	HIK_NumberOfParticipantsInCluster_medium	= 2,
	HIK_NumberOfParticipantsInCluster_large	= 3
	/*
	 * Enumeration is extensible
	 */
} HIK_NumberOfParticipantsInCluster_e;


typedef enum
{
	HIK_Attachment_unavailable	= 0,
	HIK_Attachment_stroller	= 1,
	HIK_Attachment_bicycleTrailer	= 2,
	HIK_Attachment_cart	= 3,
	HIK_Attachment_wheelchair	= 4,
	HIK_Attachment_otherWalkAssistAttachments	= 5,
	HIK_Attachment_pet	= 6
	/*
	 * Enumeration is extensible
	 */
} HIK_Attachment_e;


typedef enum
{
	HIK_PropelledInformation_PR_NOTHING,	/* No components present */
	HIK_PropelledInformation_PR_human,
	HIK_PropelledInformation_PR_animal,
	HIK_PropelledInformation_PR_motor
	/* Extensions may appear below */

} PropelledInformation_PR_e;



typedef enum
{
	HIK_HumanPropelledType_unavailable	= 0,
	HIK_HumanPropelledType_otherTypes	= 1,
	HIK_HumanPropelledType_onFoot	= 2,
	HIK_HumanPropelledType_skateboard	= 3,
	HIK_HumanPropelledType_pushOrKickScooter	= 4,
	HIK_HumanPropelledType_wheelchair	= 5
	/*
	 * Enumeration is extensible
	 */
} HIK_HumanPropelledType_e;


typedef enum
{
	HIK_AnimalPropelledType_unavailable	= 0,
	HIK_AnimalPropelledType_otherTypes	= 1,
	HIK_AnimalPropelledType_animalMounted	= 2,
	HIK_AnimalPropelledType_animalDrawnCarriage	= 3
	/*
	 * Enumeration is extensible
	 */
} HIK_AnimalPropelledType_e;


typedef enum
{
	HIK_MotorizedPropelledType_unavailable	= 0,
	HIK_MotorizedPropelledType_otherTypes	= 1,
	HIK_MotorizedPropelledType_wheelChair	= 2,
	HIK_MotorizedPropelledType_bicycle	= 3,
	HIK_MotorizedPropelledType_scooter	= 4,
	HIK_MotorizedPropelledType_selfBalancingDevice	= 5
	/*
	 * Enumeration is extensible
	 */
} HIK_MotorizedPropelledType_e;


typedef struct
{
	PropelledInformation_PR_e present;
	union propelled_information_u {
		int	 human;			//HIK_HumanPropelledType_e
		int	 animal;		//HIK_AnimalPropelledType_e
		int	 motor;			//HIK_MotorizedPropelledType_e
		/*
		 * This type is extensible,
		 * possible extensions are below.
		 */
	} choice;

}propelled_information_t;


typedef struct
{
	int useState;
	#define	PERSONALDEVICEUSAGESTATE_UNAVAILABLE		0x100
	#define	PERSONALDEVICEUSAGESTATE_OTHER				0x80
	#define	PERSONALDEVICEUSAGESTATE_IDLE				0x40
	#define	PERSONALDEVICEUSAGESTATE_LISTENINGTOAUDIO	0x20
	#define	PERSONALDEVICEUSAGESTATE_TYPING				0x10
	#define	PERSONALDEVICEUSAGESTATE_CALLING			0x08
	#define	PERSONALDEVICEUSAGESTATE_PLAYINGGAMES		0x04
	#define	PERSONALDEVICEUSAGESTATE_READING			0x02
	#define	PERSONALDEVICEUSAGESTATE_VIEWING			0x01

	int assistType;
	#define	PERSONALASSISTIVE_UNAVAILABLE				0x20
	#define	PERSONALASSISTIVE_OTHERTYPE					0x10
	#define	PERSONALASSISTIVE_VISION					0x08
	#define	PERSONALASSISTIVE_HEARING					0x04
	#define	PERSONALASSISTIVE_MOVEMENT					0x02
	#define	PERSONALASSISTIVE_COGNITION					0x01

}personalExtensions_t;


typedef enum
{
	HIK_RoadWorkerType_unavailable	= 0,
	HIK_RoadWorkerType_trafficPolice	= 1,
	HIK_RoadWorkerType_constructionPersonnel	= 2,
	HIK_RoadWorkerType_policeOfficers	= 3,
	HIK_RoadWorkerType_trafficControlPersons	= 4,
	HIK_RoadWorkerType_railroadCrossingGuards	= 5,
	HIK_RoadWorkerType_emergencyOrganizationPersonnel	= 6
	/*
	 * Enumeration is extensible
	 */
} HIK_RoadWorkerType_e;


typedef enum
{
	HIK_PersonalCrossing_unavailable	= 0,
	HIK_PersonalCrossing_request	= 1,
	HIK_PersonalCrossing_crossing	= 2,
	HIK_PersonalCrossing_finish	= 3
	/*
	 * Enumeration is extensible
	 */
} HIK_PersonalCrossing_e;


typedef struct
{
	int workerType; 	//HIK_RoadWorkerType_e

	int activityType;
	#define	ROADWORKERACTIVITYTYPE_UNAVAILABLE				0x20
	#define	ROADWORKERACTIVITYTYPE_WORKINGONROAD			0x10
	#define	ROADWORKERACTIVITYTYPE_SETTINGUPCLOSURES		0x08
	#define	ROADWORKERACTIVITYTYPE_RESPONDINGTOEVENTS		0x04
	#define	ROADWORKERACTIVITYTYPE_DIRECTINGTRAFFIC			0x02
	#define	ROADWORKERACTIVITYTYPE_OTHERACTIVITIES			0x01

}roadWorkerExtensions_t;


typedef struct
{
	int personalCrossing;	//HIK_PersonalCrossing_e


}personalRequest_t;


//非机动车数据，psm，ssm用到
typedef struct
{
	int 					basicType; 						//HIK_PersonalDeviceUserType_e
	int 					*clusterSize;					//HIK_NumberOfParticipantsInCluster_e
	propelled_information_t *propulsion;
	int 					*attachment; 					//HIK_Attachment_e
	personalExtensions_t 	*personalExt;
	roadWorkerExtensions_t  *roadWorkerExt;
	personalRequest_t		*personalReq;

}non_motorData_t;


/*
 * @struct position_confidence_e
 * @brief position_confidence_e
 */
typedef enum _position_confidence {
	Hik_positionConfidence_unavailable	= 0,
	Hik_positionConfidence_a500m	    = 1,
	Hik_positionConfidence_a200m	    = 2,
	Hik_positionConfidence_a100m	    = 3,
	Hik_positionConfidence_a50m	        = 4,
	Hik_positionConfidence_a20m	        = 5,
	Hik_positionConfidence_a10m   	    = 6,
	Hik_positionConfidence_a5m	        = 7,
	Hik_positionConfidence_a2m	        = 8,
	Hik_positionConfidence_a1m	        = 9,
	Hik_positionConfidence_a50cm	    = 10,
	Hik_positionConfidence_a20cm	    = 11,
	Hik_positionConfidence_a10cm	    = 12,
	Hik_positionConfidence_a5cm	        = 13,
	Hik_positionConfidence_a2cm	        = 14,
	Hik_positionConfidence_a1cm	        = 15
} position_confidence_e;

/*
 * @struct transmission_state_e
 * @brief transmission_state_e
 */
typedef enum _transmission_state {
	Hik_transmissionState_neutral	    = 0,
	Hik_transmissionState_park	        = 1,
	Hik_transmissionState_forwardGears	= 2,
	Hik_transmissionState_reverseGears	= 3,
	Hik_transmissionState_reserved1	    = 4,
	Hik_transmissionState_reserved2	    = 5,
	Hik_transmissionState_reserved3	    = 6,
	Hik_transmissionState_unavailable	= 7
} transmission_state_e;

/*
 * @struct FullPositionVector_t
 * @brief FullPositionVector_t
 */
typedef struct HikFullPositionVector {
	date_time_t 	           *utcTime;
	point_data_t	            pos;
	double      	           *heading;
	int					   *transmission;		//see transmission_state_e
	double              	   *speed;
	positional_accuracy_t	   *posAccuracy;
	position_confidence_set_t  *posConficence;		//see position_confidence_e
	int				       *timeConfidence;
	motion_confidence_set_t    *motionCfd;
} HikFullPositionVector_t;

/*
 * @struct path_history_point_t
 * @brief path_history_point_t
 */
typedef struct _path_history_point {
	point_data_t	             pos;
	int        	             timeOffset;
	double       	            *speed;
	position_confidence_set_t	*posAccuracy;
	double 	                    *coarseHeading;
} path_history_point_t;

/*
 * @struct path_history_t
 * @brief path_history_t
 */
typedef struct _path_history {
	HikFullPositionVector_t	*initialPosition;

	int            	        *currGNSSstatus;
	#define GNSSSTATUS_UNAVAILABLE	               0x80
	#define GNSSSTATUS_ISHEALTHY	               0x40
	#define GNSSSTATUS_ISMONITORED	               0x20
	#define GNSSSTATUS_BASESTATIONTYPE	           0x10
	#define GNSSSTATUS_APDOPOFUNDER5	           0x08
	#define GNSSSTATUS_INVIEWOFUNDER5	           0x04
	#define GNSSSTATUS_LOCALCORRECTIONSPRESENT	   0x02
	#define GNSSSTATUS_NETWORKCORRECTIONSPRESENT   0x01

	int                      pathHistoryPointNum;
	path_history_point_t	*pathHistoryPoints;

} path_history_t;


//定义车身的灯光状态
typedef struct
{
	int  lights;
	#define EXTERIORLIGHTS_LOWBEAMHEADLIGHTSON	      0x100
	#define EXTERIORLIGHTS_HIGHBEAMHEADLIGHTSON	      0x80
	#define EXTERIORLIGHTS_LEFTTURNSIGNALON	          0x40
	#define EXTERIORLIGHTS_RIGHTTURNSIGNALON	      0x20
	#define EXTERIORLIGHTS_HAZARDSIGNALON	          0x10
	#define EXTERIORLIGHTS_AUTOMATICLIGHTCONTROLON	  0x08
	#define EXTERIORLIGHTS_DAYTIMERUNNINGLIGHTSON	  0x04
	#define EXTERIORLIGHTS_FOGLIGHTON	              0x02
	#define EXTERIORLIGHTS_PARKINGLIGHTSON	          0x01

}exteriorLights_t;



/*
 * @struct participant_data_t
 * @brief participant data
 */
typedef struct _participant_data
{
    int				      ptcType;
	int	                  ptcId;
	int		              source;
	unsigned char             id[8];
	int                      secMark;
	point_data_t              pos;
	position_confidence_set_t posConfidence;
	transmission_state_e     *transmission;
	double                    speed;
	double                    heading;
	double                   *steeringWheelAngle;
	motion_confidence_set_t  *motionCfd;
	acceleration_set4way_t   *accelSet;
	vehicle_size_t            size;
	vehicle_classification_t *vehicleClass;

}participant_data_t;


typedef enum
{
	Hik_eventSource_unknown	        = 0,
	Hik_eventSource_police	        = 1,
	Hik_eventSource_government	    = 2,
	Hik_eventSource_meteorological	= 3,
	Hik_eventSource_internet	    = 4,
	Hik_eventSource_detection	    = 5
} event_source_e;


/*
 * @struct rsi_time_details_t
 * @brief rsi_time_details_t
 */
typedef struct
{
	int	*startTime;
	int	*endTime;
	int	*endTimeConfidence;

} rsi_time_details_t;


/*
 * @struct rte_data_t
 * @brief rte_data_t
 */
typedef struct _rte_data
{
	int	              rteId;
	int    	          eventType;
	event_source_e	      eventSource;
	point_data_t	     *eventPos;
	double	             *eventRadius;
	description_t    	 *description;
	rsi_time_details_t 	 *timeDetails;
	unsigned char        *priority;
	int                   referencePathNum;
	reference_path_t	 *referencePaths;
	int                   referenceLinkNum;
	reference_link_t     *referenceLinks;
	int 	             *eventConfidence;

} rte_data_t;



#endif

