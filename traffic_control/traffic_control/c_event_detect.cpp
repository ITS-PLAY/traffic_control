
#include "event_detect_in_class.h"
detector_target_info_t receive_data() {

}

#ifndef _cplusplus
#define _cplusplus
#include "c_event_detect.h"
#endif

void c_call_test() {
	Event_detect mevent;

	while (true) {
		detector_target_info_t mdetector_target = receive_data();
		//Vehicleincident_Detection veh_test = Vehicleincident_Detection(mdetector_target.sec, mdetector_target.nsec, mdetector_target.id, mdetector_target.laneId, mdetector_target.label, 1, mdetector_target.radar_px, mdetector_target.radar_py,\
		//	mdetector_target.radar_vx, mdetector_target.radar_vy, mdetector_target.dimensions_x, mdetector_target.dimensions_y);
		mevent.handle_data(mdetector_target);

	}
	

}


