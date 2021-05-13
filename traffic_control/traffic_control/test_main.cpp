#include "traffic_control.h"

using namespace std;

int main() {
	Node_Control_Integration node_Control(1);

	while (true) {
		second_Clock_Type current_Time = time_point_cast<seconds>(system_clock::now());
		//if ((current_Time.time_since_epoch().count() % Time_Interval) == 0) {
		node_Control.node_Adaptive_Control.get_Node_Index_Info();
		node_Control.node_Adaptive_Control.implement_Node_Control_Function();
		node_Control.node_Adaptive_Control.update_Node_Index_Info();
		//}
	}

	return 0;
}