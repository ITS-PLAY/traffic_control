#include "traffic_control.h"

using namespace std;

int main() {
	Node_Adaptive_Control node_Adaptive_Control;
	traffic_Control_Integration(&node_Adaptive_Control, 1);
	//Node_Variable_Lane_Control node_Variable_Control;
	//traffic_Control_Integration(&node_Variable_Control, 1);

	return 0;
}