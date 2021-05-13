
/*
 * 	File  : dayII_scene_internal.h
 *  Breif : dayII场景预警信息
 *  Date  : 2021.3.1
 *  Author: wq
 *
 */

#ifndef LIBALG_INCLUDE_DAYII_SCENE_INTERNAL_H_
#define LIBALG_INCLUDE_DAYII_SCENE_INTERNAL_H_


//#include "dayII_scene_msgCenter.h"
#include "V2xMsg_UserCommon.h"


#define MAX_CLC_VEHICLE_NUM 32
#define MAX_TARGET_NUM 64
#define MAX_NODE_NUM 8
#define MAX_LINK_NUM 8
#define MAX_LANE_NUM 8

/*
typedef struct
{
	clc_scene_t			clc_scene;
	int 				update_flag;//1-update, 0-not update
	struct event_base*  clc_base;
	pthread_rwlock_t 	clc_scene_lock;

}clc_scene_ctx_t;
*/

typedef struct
{
	unsigned char           id[8];
	point_data_t            pos;		//车辆当前位置坐标
	//i_and_r_data_t			intAndReq;	//车辆具体意图与请求
	double                  speed;
	double                  heading;
	vehicle_size_t          size;
}scene_combine_msg_t;

typedef struct
{
	int num;
	scene_combine_msg_t scene_msg[MAX_CLC_VEHICLE_NUM];
}scene_combine_msg_list_t;

typedef struct
{
	char   			targetType;					//目标类型,0--未知；1--行人；2--机动车；3--非机动车
	char   			vehicleType;				//目标是机动车的时候子类型, 0--未知；1--大型车；2--中型车；3--小型车
	float  			longitude;					//GPS经度，6位精度
	float  			latitude;					//GPS纬度，6位精度
	float  			speed;						//目标速度--纵向速度
	float           heading;                	//目标方向角

}scene_sensor_target_info_t;


typedef struct
{
	unsigned int 				num;
	scene_sensor_target_info_t 	scene_sensor_target_info[MAX_TARGET_NUM];

}scene_sensor_target_info_list_t;


typedef struct
{
	double                  dist;
	double                  horizonDist;
	double                  heading_hv_rv;
	double                  heading_rv_hv;
	double                  angle_hv_rv;
	double                  angle_hvrv_hv;
	double                  angle_rvhv_hv;
	//v2v_position_relation_e position;

}p2p_relation_t;


typedef struct
{
	float  			longitude;
	float  			latitude;
	float  			speed;
	float           heading;

}scene_point_data_t;



///////////////gspa//////////////////

typedef struct
{
	//gspa_scene_t		gspa_scene;
	int 				update_flag;//1-update, 0-not update
	struct event_base*  gspa_base;
	//pthread_rwlock_t 	gspa_scene_lock;

}gspa_scene_ctx_t;


typedef struct
{
	int isMatch;			//是否匹配成功，1--success, 0--failed
	int pamList_idx;		//匹配上的pamList index
	int pamNodeList_idx;	//匹配上的pamNodeList index
	int nodeId;				//匹配上的node节点id
	int nodeAttributes;		//匹配上的node节点属性，0--others, 1--entrance, 2--exit

}pam_node_match_result_t;


typedef struct
{
	int node_num;
	int edge_num;
	int edges[MAX_NODE_NUM][MAX_NODE_NUM];
	int node_id_list[MAX_NODE_NUM];
	point_data_t node_pos_list[MAX_NODE_NUM];

}pam_node_graph_t;


typedef struct
{
	int dist;
	int actual_node_num;
	int node_id_list[MAX_NODE_NUM];

}pam_parking_guide_result_t;


///////////////dlm//////////////////
/*
typedef struct
{
	dlm_scene_t			dlm_scene;
	int 				update_flag;//1-update, 0-not update
	struct event_base*  dlm_base;
	//pthread_rwlock_t 	dlm_scene_lock;

}dlm_scene_ctx_t;
*/

//目标检测结果
typedef struct
{
	double sec;                     //秒
	double nsec;                    //纳秒
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
	int source_id;

}detector_target_info_t;


typedef struct
{
	unsigned int 			num;
	detector_target_info_t 	scene_sensor_target_info[MAX_TARGET_NUM];
}scene_dlm_sensor_target_info_list_t;


//车道级交通流参数
typedef struct
{
//	int nodeId;
//	int linkId;
	int laneId;
	int isDynamicLane;		//是否是动态车道， 1--是， 0--不是
	int currentManeuver;	//当前车道允许的转向行为
	int laneVolume;			//车道流量
	int laneQueueNum;		//排队长度车辆数量
	float laneQueueLength;	//车道排队长度
	int laneCapacity;		//车道通行能力
	float laneSaturation;	//车道饱和度

}lane_traffic_flow_info_t;


//typedef struct
//{
//	int num;
//	lane_traffic_flow_info_t lane_traffic_flow_info[MAX_LANE_NUM];
//
//}lane_traffic_flow_info_list_t;

typedef struct
{
	int 	laneNum;
	int     linkId;
	lane_traffic_flow_info_t lane_info[MAX_LANE_NUM];
	int 	isExistDynamicLane;						//是否存在动态车道， 1--存在， 0--不存在
	float 	leftSaturation;
	float 	straightSaturation;
	int 	leftSaturationContinousNum; 			//左转饱和度连续满足条件的次数
	int 	straightSaturationContinousNum; 		//直行饱和度连续满足条件的次数

}link_traffic_flow_info_t;

typedef struct
{
	int nodeId;
	int linkNum;
	link_traffic_flow_info_t link_info[MAX_LINK_NUM];

}node_traffic_flow_info_t;

typedef struct
{
	int nodeNum;
	node_traffic_flow_info_t node_info[MAX_NODE_NUM];

}intersection_traffic_flow_info_t;



//dlm算法结果
typedef struct
{
	int nodeId;
	int linkIndex;
	int laneIndex;
	int laneId;
	int maneuvers;
	#define ALLOWEDMANEUVERS_MANEUVERSTRAIGHTALLOWED	        	 0x80
	#define ALLOWEDMANEUVERS_MANEUVERLEFTALLOWED	                 0x40
	#define ALLOWEDMANEUVERS_MANEUVERRIGHTALLOWED	                 0x20
	#define ALLOWEDMANEUVERS_MANEUVERUTURNALLOWED	                 0x10
	#define ALLOWEDMANEUVERS_MANEUVERLEFTTURNONREDALLOWED	         0x08
	#define ALLOWEDMANEUVERS_MANEUVERRIGHTTURNONREDALLOWED	         0x04
	#define ALLOWEDMANEUVERS_MANEUVERLANECHANGEALLOWED	             0x02
	#define ALLOWEDMANEUVERS_MANEUVERNOSTOPPINGALLOWED	        	 0x01
	#define ALLOWEDMANEUVERS_YIELDALLWAYSREQUIRED	            	 0x8000
	#define ALLOWEDMANEUVERS_GOWITHHALT	                        	 0x4000
	#define ALLOWEDMANEUVERS_CAUTION	                             0x2000
	#define ALLOWEDMANEUVERS_RESERVED1                               0x1000

}dlm_lane_maneuvers_info_t;


typedef struct
{
	int num;
	dlm_lane_maneuvers_info_t dlm_lane_maneuvers_info[MAX_LANE_NUM];

}dlm_lane_maneuvers_info_list_t;

/*
int cv2x_alg_scene_clc_write(clc_scene_t *clc_scene);
void cv2x_alg_scene_clc_param_init(void);
int cv2x_alg_scene_clc_init(void);

int cv2x_alg_scene_gspa_write(gspa_scene_t *gspa_scene);
void cv2x_alg_scene_gspa_param_init(void);
int cv2x_alg_scene_gspa_init(void);

int cv2x_alg_scene_dlm_write(dlm_scene_t *dlm_scene);
void cv2x_alg_scene_dlm_param_init(void);
int cv2x_alg_scene_dlm_init(void);

int cv2x_alg_scene_traffic_flow_statistics_init(void);
*/

#endif /* LIBALG_INCLUDE_DAYII_SCENE_INTERNAL_H_ */
