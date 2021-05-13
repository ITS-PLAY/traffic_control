/*
 * 	File  : V2xMsg_UserMap.h
 *  Breif : map消息用户层数据结构
 *  Date  : 2020.4.13
 *  Author: zhuyun/chenkai
 *
 */

#ifndef _V2X_MSG_USER_MAP_H_
#define _V2X_MSG_USER_MAP_H_

#include "V2xMsg_UserCommon.h"

/* speedlimit type */
typedef enum _speed_limit_type {                                                                           //限速类型speedLimits，车道/路段级别
	Hik_speedLimitType_unknown	                                    = 0,
	Hik_speedLimitType_maxSpeedInSchoolZone                      	= 1,
	Hik_speedLimitType_maxSpeedInSchoolZoneWhenChildrenArePresent	= 2,
	Hik_speedLimitType_maxSpeedInConstructionZone	                = 3,
	Hik_speedLimitType_vehicleMinSpeed	                            = 4,
	Hik_speedLimitType_vehicleMaxSpeed	                            = 5,
	Hik_speedLimitType_vehicleNightMaxSpeed	                        = 6,
	Hik_speedLimitType_truckMinSpeed	                            = 7,
	Hik_speedLimitType_truckMaxSpeed	                            = 8,
	Hik_speedLimitType_truckNightMaxSpeed	                        = 9,
	Hik_speedLimitType_vehiclesWithTrailersMinSpeed     	        = 10,
	Hik_speedLimitType_vehiclesWithTrailersMaxSpeed	                = 11,
	Hik_speedLimitType_vehiclesWithTrailersNightMaxSpeed	        = 12
}speed_limit_type_e;


/*
 * @struct connecting_lane_t
 * @brief connecting_lane_t
 */
typedef struct _connecting_lane {                                                                         //与下游车道的连接信息connectingLane，车道级别
	int   laneId;
	
	int  *maneuvers;
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
} connecting_lane_t;

/*
 * @struct connecting_lane_ex_t
 * @brief connecting_lane_ex_t
 */
typedef struct _connecting_lane_ex {                                                                     //与下游车道的连接信息connectingLane，车道级别
	int    			target_section;
	int				target_lane;
	double  		   *connectingLaneWidth;
	int                 connectingLanePointNum;
	point_data_t	   *connectingLanePoints;
	int				   *isolatedConnectingLane;
} connecting_lane_ex_t;

/*
 * @struct connect_data_t
 * @brief connect data
 */
typedef struct _connect_data                                                                           //与下游车道的连接信息，Connection,车道级别
{
    node_reference_id_t remoteIntersection;
	connecting_lane_t  *connectingLane;
	int               *phaseId;
}connect_data_t;

/*
 * @struct connect_data_t
 * @brief connect data
 */
typedef struct _signal_waiting_lane_data                                                                //
{
    double              laneWidth;
	int                 pointNum;
	point_data_t	   *points;
	int                 allowedPhaseIdNum;
	int			   *allowedPhaseIds;
}signal_waiting_lane_data_t;

/*
 * @struct connect_ex_data_t
 * @brief connect extend data
 */
typedef struct _connect_ex_data                                                                         //
{
    node_reference_id_t 		remoteIntersection;
	signal_waiting_lane_data_t *swl;
	int							connectingLaneNum;
	connecting_lane_ex_t  	   *connectingLanes;
	int               		   *phaseId;
	int			   		   *turn_direction;
	#define MANEUVER_MANEUVERSTRAIGHT	0
	#define MANEUVER_MANEUVERLEFTTURN	1
	#define MANEUVER_MANEUVERRIGHTTURN	2
	#define MANEUVER_MANEUVERUTURN	3
}connect_ex_data_t;

/*
 * @struct speedlimit_data_t
 * @brief speedlimit data
 */
typedef struct _speedlimit_data                                                                        //限速类型speedLimits，车道/路段级别
{
    speed_limit_type_e speedLimitType;
	double             speed;
}speedlimit_data_t;

/*
 * @struct movement_info_t
 * @brief movement_info_t
 */
typedef struct _movement_info                                                                         //路段的转向信息movement，路段级别
{
    node_reference_id_t remoteIntersection;
	int               *phaseId;
}movement_info_t;

/*
 * @struct movement_ex_info_t
 * @brief movement_ex_info_t
 */
typedef struct _movement_ex_info                                                                       //路段的转向信息movement，路段级别
{
    node_reference_id_t remoteIntersection;
	int               *phaseId;
	int			   *turn_direction;
	#define MANEUVER_MANEUVERSTRAIGHT	0
	#define MANEUVER_MANEUVERLEFTTURN	1
	#define MANEUVER_MANEUVERRIGHTTURN	2
	#define MANEUVER_MANEUVERUTURN	3
}movement_ex_info_t;

/*
 * @struct lane_type_attribute_e
 * @brief lane_type_attribute_e
 */
typedef enum _lane_type_attribute {                                                                   //车道类型lane_Type，车道级别
	Hik_laneTypeAttributes_PR_NOTHING,	
	Hik_laneTypeAttributes_PR_vehicle,
	Hik_laneTypeAttributes_PR_crosswalk,
	Hik_laneTypeAttributes_PR_bikeLane,
	Hik_laneTypeAttributes_PR_sidewalk,
	Hik_laneTypeAttributes_PR_median,
	Hik_laneTypeAttributes_PR_striping,
	Hik_laneTypeAttributes_PR_trackedVehicle,
	Hik_laneTypeAttributes_PR_parking,
} lane_type_attribute_e;

/*
 * @struct lane_attributes_t
 * @brief lane_attributes_t
 */
typedef struct _lane_attributes                                                                     //车道属性laneAttributes,车道级别
{
    int                  *shareWith;
	#define LANESHARING_OVERLAPPINGLANEDESCRIPTIONPROVIDED     0x80
	#define LANESHARING_MULTIPLELANESTREATEDASONELANE	       0x40
	#define LANESHARING_OTHERNONMOTORIZEDTRAFFICTYPES	       0x20
	#define LANESHARING_INDIVIDUALMOTORIZEDVEHICLETRAFFIC      0x10
	#define LANESHARING_BUSVEHICLETRAFFIC	                   0x08
	#define LANESHARING_TAXIVEHICLETRAFFIC	                   0x04
	#define LANESHARING_PEDESTRIANSTRAFFIC	                   0x02
	#define LANESHARING_CYCLISTVEHICLETRAFFIC	               0x01
	#define LANESHARING_TRACKEDVEHICLETRAFFIC	               0x8000
	#define LANESHARING_PEDESTRIANTRAFFIC	                   0x4000

	lane_type_attribute_e laneType;
	union _attributes
	{
	    int	 vehicle;
		#define LANEATTRIBUTES_VEHICLE_ISVEHICLEREVOCABLELANE	          0x80
		#define LANEATTRIBUTES_VEHICLE_ISVEHICLEFLYOVERLANE	              0x40
		#define LANEATTRIBUTES_VEHICLE_HOVLANEUSEONLY	                  0x20
		#define LANEATTRIBUTES_VEHICLE_RESTRICTEDTOBUSUSE	              0x10
		#define LANEATTRIBUTES_VEHICLE_RESTRICTEDTOTAXIUSE	              0x08
		#define LANEATTRIBUTES_VEHICLE_RESTRICTEDFROMPUBLICUSE            0x04
		#define LANEATTRIBUTES_VEHICLE_HASIRBEACONCOVERAGE	              0x02
		#define LANEATTRIBUTES_VEHICLE_PERMISSIONONREQUEST	              0x01 
		
		int	 crosswalk;
		#define LANEATTRIBUTES_CROSSWALK_CROSSWALKREVOCABLELANE	          0x80
		#define LANEATTRIBUTES_CROSSWALK_BICYLEUSEALLOWED	              0x40
		#define LANEATTRIBUTES_CROSSWALK_ISXWALKFLYOVERLANE	              0x20
		#define LANEATTRIBUTES_CROSSWALK_FIXEDCYCLETIME	                  0x10
		#define LANEATTRIBUTES_CROSSWALK_BIDIRECTIONALCYCLETIMES	      0x08
		#define LANEATTRIBUTES_CROSSWALK_HASPUSHTOWALKBUTTON	          0x04
		#define LANEATTRIBUTES_CROSSWALK_AUDIOSUPPORT	                  0x02
		#define LANEATTRIBUTES_CROSSWALK_RFSIGNALREQUESTPRESENT	          0x01
		#define LANEATTRIBUTES_CROSSWALK_UNSIGNALIZEDSEGMENTSPRESENT      0x8000
	
		int	 bikeLane;
		#define LANEATTRIBUTES_BIKE_BIKEREVOCABLELANE	                  0x80
		#define LANEATTRIBUTES_BIKE_PEDESTRIANUSEALLOWED	              0x40
		#define LANEATTRIBUTES_BIKE_ISBIKEFLYOVERLANE	                  0x20
		#define LANEATTRIBUTES_BIKE_FIXEDCYCLETIME	                      0x10
		#define LANEATTRIBUTES_BIKE_BIDIRECTIONALCYCLETIMES	              0x08
		#define LANEATTRIBUTES_BIKE_ISOLATEDBYBARRIER	                  0x04
		#define LANEATTRIBUTES_BIKE_UNSIGNALIZEDSEGMENTSPRESENT           0x02
		
		int	 sidewalk;
		#define LANEATTRIBUTES_SIDEWALK_SIDEWALK_REVOCABLELANE            0x80
		#define LANEATTRIBUTES_SIDEWALK_BICYLEUSEALLOWED	              0x40
		#define LANEATTRIBUTES_SIDEWALK_ISSIDEWALKFLYOVERLANE             0x20
		#define LANEATTRIBUTES_SIDEWALK_WALKBIKES	                      0x10
	
		int	 median;
		#define Hik_LaneAttributes_Barrier_median_RevocableLane               0x80
		#define Hik_LaneAttributes_Barrier_median	                          0x40
		#define Hik_LaneAttributes_Barrier_whiteLineHashing	                  0x20
		#define Hik_LaneAttributes_Barrier_stripedLines	                      0x10
		#define Hik_LaneAttributes_Barrier_doubleStripedLines	              0x08
		#define Hik_LaneAttributes_Barrier_trafficCones	                      0x04
		#define Hik_LaneAttributes_Barrier_constructionBarrier	              0x02
		#define Hik_LaneAttributes_Barrier_trafficChannels	                  0x01
		#define Hik_LaneAttributes_Barrier_lowCurbs	                          0x8000
		#define Hik_LaneAttributes_Barrier_highCurbs	                      0x4000
		
		int	 striping;
		#define    LANEATTRIBUTES_STRIPING_STRIPETOCONNECTINGLANESREVOCABLELANE	  0x80
		#define    LANEATTRIBUTES_STRIPING_STRIPEDRAWONLEFT                       0x40
		#define    LANEATTRIBUTES_STRIPING_STRIPEDRAWONRIGHT                      0x20
		#define    LANEATTRIBUTES_STRIPING_STRIPETOCONNECTINGLANESLEFT            0x10
		#define    LANEATTRIBUTES_STRIPING_STRIPETOCONNECTINGLANESRIGHT           0x08
		#define    LANEATTRIBUTES_STRIPING_STRIPETOCONNECTINGLANESAHEAD           0x04
	
		int	 trackedVehicle;
		#define    LANEATTRIBUTES_TRACKEDVEHICLE_SPEC_REVOCABLELANE	              0x80
	    #define    LANEATTRIBUTES_TRACKEDVEHICLE_SPEC_COMMUTERRAILROADTRACK       0x40
	    #define    LANEATTRIBUTES_TRACKEDVEHICLE_SPEC_LIGHTRAILROADTRACK          0x20
	    #define    LANEATTRIBUTES_TRACKEDVEHICLE_SPEC_HEAVYRAILROADTRACK          0x10
	    #define    LANEATTRIBUTES_TRACKEDVEHICLE_SPEC_OTHERRAILTYPE               0x08
	
		int	 parking;
		#define    LANEATTRIBUTES_PARKING_PARKINGREVOCABLELANE	                  0x80
		#define    LANEATTRIBUTES_PARKING_PARALLELPARKINGINUSE                    0x40
		#define    LANEATTRIBUTES_PARKING_HEADINPARKINGINUSE                      0x20
		#define    LANEATTRIBUTES_PARKING_DONOTPARKZONE                           0x10
		#define    LANEATTRIBUTES_PARKING_PARKINGFORBUSUSE                        0x08
		#define    LANEATTRIBUTES_PARKING_PARKINGFORTAXIUSE                       0x04
		#define    LANEATTRIBUTES_PARKING_NOPUBLICPARKINGUSE	                  0x02
	}choice;
}lane_attributes_t;

/*
 * @struct link_data_t
 * @brief link data
 */
typedef struct _lane_data                                                                                //车道总体信息Lane，车道级别
{
	int       	        laneId;
	double             *laneWidth;
	lane_attributes_t  *laneAttributes;
	
	int 	           *maneuvers;
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
	
	int                 connectNum;
	connect_data_t     *connectsTo;
	int                 speedLimitNum;
	speedlimit_data_t  *speedlimits;
	int                 pointNum;
	point_data_t	   *points;
} lane_data_t;

/*
 * @struct st_point_t
 * @brief st point data
 */
typedef struct _st_point                                                                                //坐标点point，基础结构体
{
    double	 s_axis;
	double	 t_axis;
}st_point_t;

/*
 * @struct link_ex_data_t
 * @brief link extend data
 */
typedef struct _lane_ex_data                                                                              //车道总体信息Lane，车道级别
{
	int       	        laneRefID;
	double             *laneWidth;
	lane_attributes_t  *laneAttributes;
	
	int 	           *maneuvers;
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
	
	int                 connectExNum;
	connect_ex_data_t  *connectsToEx;
	int                 speedLimitNum;
	speedlimit_data_t  *speedlimits;
	int                 STPointNum;
	st_point_t	   	   *st_points;
} lane_ex_data_t;

/*
 * @struct section_data_t
 * @brief section_data_t
 */
typedef struct _section_data                                                                           //车道总体信息Lane，车道级别
{
    int 				secId;
	int                	laneExNum;
	lane_ex_data_t	   *lanesEx;
}section_data_t;

/*
 * @struct link_data_t
 * @brief link data
 */
typedef struct _link_data                                                                             //路段总体信息Link，路段级别
{
	char       			name[64];
	node_reference_id_t upstreamNodeId;
	double  	        linkWidth;
	int                	speedLimitNum;
	speedlimit_data_t  *speedlimits;
	int                	pointNum;
	point_data_t       *points;
	int                	movementNum;
	movement_info_t    *movements;
	int                	laneNum;
	lane_data_t	       *lanes;
}link_data_t;

/*
 * @struct link_ex_data_t
 * @brief link extend data
 */
typedef struct _link_ex_data                                                                          //路段总体信息Link，路段级别 
{
	char       			name[64];
	node_reference_id_t upstreamNodeId;
	double  	       *linkWidth;
	int                	speedLimitNum;
	speedlimit_data_t  *speedlimits;
	int                	pointNum;
	point_data_t       *refLine;
	int                 movementExNum;
	movement_ex_info_t *movementsEx;
	int					sectionNum;
	section_data_t     *sections;
}link_ex_data_t;

/*
 * @struct polygon_data_t
 * @brief polygon_data_t
 */
typedef struct _polygon_data                                                                       //线段的点集合，基础结构体
{
	int 				pointNum;
	point_data_t	   *points;
}polygon_data_t;

/*
 * @struct prohibited_zone_t
 * @brief prohibited zone info
 */
typedef struct _prohibited_zone                                                                  //禁停区域，节点级别
{
	polygon_data_t     *centralCircleProhibitedZone;
	int 				non_motorVehiclePolygonNum;
	polygon_data_t	   *non_motorVehicleProhibitedZones;
	int 				gridLineMarkingPolygonNum;
	polygon_data_t	   *gridLineMarkingProhibitedZones;
}prohibited_zone_t;

/*
 * @struct node_data_t
 * @brief node data
 */
typedef struct _node_data                                                                       //节点总体信息，节点级别                             
{
	char      		    name[64];
	node_reference_id_t id;
	point_data_t        refPos;
	int                 linkNum;
	link_data_t	       *links;
	int					linkExNum;
	link_ex_data_t	   *linksEx;
	prohibited_zone_t  *prohibitedzone;
}node_data_t;

/*
 * @struct map_data_t
 * @brief map data
 */
typedef struct _map_data                                                                      //地图总数据包
{
    int          msgCnt;
	int          *moy;
    int          nodeNum;
	node_data_t	 *nodes;
}map_data_t;

#endif

