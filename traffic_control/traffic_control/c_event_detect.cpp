
#include "event_detect_in_class.h"
struct detector_target_info_t*  receive_data() {
	detector_target_info_t* mdetect = new detector_target_info_t;
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
	return mdetect;

}

#ifndef _cplusplus
#define _cplusplus
#include "c_event_detect.h"
#endif

void c_call_test() {
	Event_detect mevent;

	while (true) {
		detector_target_info_t* mdetector_target = receive_data();
		mevent.handle_data(*mdetector_target);
	}
}


