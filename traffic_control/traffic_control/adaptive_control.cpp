#include "traffic_control.h"

using namespace std;

void Node_Adaptive_Control::get_Phases_Overlap_Info() {    //固定相序下的相位嵌套组合
	shared_ptr<Stage_Node> phase1 (new Stage_Node(1));
	phase1->next = make_shared<Stage_Node>(Stage_Node(5));
	//phase1->next->next = make_shared<Stage_Node>(Stage_Node(2));
	phases_Overlap.emplace(1, phase1);

	shared_ptr<Stage_Node> phase6 (new Stage_Node(6));
	phase6->next = make_shared<Stage_Node>(Stage_Node(2));
	phases_Overlap.emplace(6, phase6);

	shared_ptr<Stage_Node> phase3 (new Stage_Node(3));
	phase3->next = make_shared<Stage_Node>(Stage_Node(7));
	phases_Overlap.emplace(3, phase3);

	shared_ptr<Stage_Node> phase8 (new Stage_Node(8));
	phase8->next = make_shared<Stage_Node>(Stage_Node(4));
	phases_Overlap.emplace(8, phase8);

	return;
}

void Node_Adaptive_Control::get_Phases_Sequence_Info() {      //执行固定的相序
	shared_ptr<Phase_Node> phase1 (new Phase_Node(1, false));
	shared_ptr<Phase_Node> phase6 (new Phase_Node(6, true));
	phase1->next = phase6;
	shared_ptr<Phase_Node> phase3 (new Phase_Node(3, false));
	phase6->next = phase3;
	shared_ptr<Phase_Node> phase8 (new Phase_Node(8, true));
	phase3->next = phase8;

	phases_Sequence.emplace_back(phase1);
	return;
}

void Node_Adaptive_Control::get_Node_Index_Info() {

}

void Node_Adaptive_Control::get_Phases_Green_Time() {                      //给定数据、相序、周期时长等，计算绿灯初始化值
    
	for (int i = 0; i < phases_Sequence.size(); i++) {
		shared_ptr<Phase_Node> phase_Head = phases_Sequence[i];
		while (phase_Head) {
			int ring1_phase = phase_Head->phase_Id;                                     //环1的相位编号
			shared_ptr<Stage_Node> stage_Head = phases_Overlap[phase_Head->phase_Id];
			while (stage_Head->next) {
				int ring2_phase = stage_Head->next->phase_Id;

				//TODO1: 如何通过Lane_Index，计算Phase_Index的指标
				//TODO2: 绿灯初始化代码混乱，嵌套语句过多，需重新完善
				double max_volume = max(phases_Index[ring1_phase].volume_Interval * phases_Index[ring1_phase].phase_Clearance_Ratio, phases_Index[ring2_phase].volume_Interval * phases_Index[ring2_phase].phase_Clearance_Ratio);
				double min_volume = min(phases_Index[ring1_phase].volume_Interval * phases_Index[ring1_phase].phase_Clearance_Ratio, phases_Index[ring2_phase].volume_Interval * phases_Index[ring2_phase].phase_Clearance_Ratio);
				if (static_cast<double>(abs(phases_Index[ring1_phase].volume_Interval - phases_Index[ring2_phase].volume_Interval)) / static_cast<double>(max_volume) <= stage_Volume_Diff || min_volume == 0) {
					int green_Time = max_volume / time_Interval * optimal_Cycle_Time / phases_Index[ring1_phase].time_Headway_Saturation + phases_Index[ring1_phase].queue_Num * phases_Index[ring1_phase].phase_Clearance_Ratio * phases_Index[ring1_phase].time_Headway_Saturation;
				}else {
                    int green_Time = min_volume / time_Interval * optimal_Cycle_Time / phases_Index[ring1_phase].time_Headway_Saturation + phases_Index[ring1_phase].queue_Num * phases_Index[ring1_phase].phase_Clearance_Ratio * phases_Index[ring1_phase].time_Headway_Saturation;
				    //TODO3: 计算某一相位剩余的流量

				}
				stage_Head = stage_Head->next;
			}
			phase_Head = phase_Head->next;
		}
		
	}

	return;
}

void Node_Adaptive_Control::phase_Delay_Caculation() {

}

void Node_Adaptive_Control::implement_Node_Control_Function() {

}

