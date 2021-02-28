#include "traffic_control.h"

using namespace std;

inline void Phase_Index::initial_Green_Time_Caculation() {
	switch (queue_Num) {
	    case 0: phase_Info.green_Time = 0; break;
	    case 1: phase_Info.green_Time = 2; break;
	    case 2: phase_Info.green_Time = 3; break;
	    default:
		       phase_Info.green_Time = 4; break;
	}
	return;
}

inline void Phase_Index::initial_Demand_Caculation() {
	intial_Demand = phase_Clearance_Ratio * (1.0F * volume_Interval / Time_Interval) * phase_Info.intersection_Signal_Controller.signal_Cycle_Time + queue_Num;
	return;
}

double Node_Adaptive_Control::queue_Delay_Value(const int phase_Id) {
	int temp = phases_Index[phase_Id].queue_Num  - phases_Index[phase_Id].capacity_Intersection;
	if (temp <= 0)
		return phases_Index[phase_Id].queue_Num * phases_Index[phase_Id].time_Headway_Saturation;
	else
		return phases_Index[phase_Id].capacity_Intersection * phases_Index[phase_Id].time_Headway_Saturation + temp * static_cast<double>(phases_Index[phase_Id].phase_Info.green_Time);
}

void Node_Adaptive_Control::get_Phases_Overlap_Info() {    //�̶������µ���λǶ�����
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

void Node_Adaptive_Control::get_Phases_Sequence_Info() {      //ִ�й̶�������,                                        TODO:���ͬ�׶ε���λ�����Ĭ�ϵ�0��λ
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

void Node_Adaptive_Control::get_Phases_Green_Time(const shared_ptr<Phase_Node>& mphase_Sequence) {                      //�������ݡ���������ʱ���ȣ������̵Ƴ�ʼ��ֵ
	shared_ptr<Phase_Node> phase_Head = mphase_Sequence;
	while (phase_Head) {
		int ring1_phase = phase_Head->phase_Id;                                                       //��1����λ���
		shared_ptr<Stage_Node> stage_Head = phases_Overlap[phase_Head->phase_Id];

		while (stage_Head->next) {
			int ring2_phase = stage_Head->next->phase_Id;                                            //��2����λ���
			double max_volume = max(phases_Index[ring1_phase].intial_Demand, phases_Index[ring2_phase].intial_Demand), min_volume = min(phases_Index[ring1_phase].intial_Demand, phases_Index[ring2_phase].intial_Demand);
			int green_Time = 0;
			if (static_cast<double>(abs(phases_Index[ring1_phase].intial_Demand - phases_Index[ring2_phase].intial_Demand)) / max_volume <= stage_Volume_Diff || min_volume == 0) {
				green_Time = static_cast<int>(max_volume * phases_Index[ring1_phase].time_Headway_Saturation);
				phases_Index[ring1_phase].phase_Info.green_Time += green_Time;                       //��λ1���̵�ʱ��
				phases_Index[ring2_phase].phase_Info.green_Time += green_Time;                       //��λ2���̵�ʱ��
				break;
			}
			green_Time = min_volume * phases_Index[ring1_phase].time_Headway_Saturation;
			phases_Index[ring1_phase].phase_Info.green_Time += green_Time;
			phases_Index[ring2_phase].phase_Info.green_Time += green_Time;

			int ring_Num_Main = (phases_Index[ring1_phase].intial_Demand == min_volume) ? ring2_phase : ring1_phase;
			phases_Index[ring_Num_Main].intial_Demand -= min_volume;                                //������Ҫ��λ��ʣ������
			if (ring1_phase == ring_Num_Main)
				break;                                                                              //��ת��1����һ��λ
			stage_Head = stage_Head->next;
		}
		phase_Head = phase_Head->next;
	}
	return;
}

void Node_Adaptive_Control::phase_Delay_Caculation(const shared_ptr<Phase_Node>& head, int& moment_Of_Cycle, double& total_Delay) {
	if (&head == nullptr)                                                                                                                                                                //TODO:��Ӧ�õĺϷ���
		return;
	total_Delay += phases_Index[head->phase_Id].delay_Vehicles_Start + phases_Index[head->down->phase_Id].delay_Vehicles_Start;                //������������

    phases_Index[head->phase_Id].delay_Queue_Clearance = queue_Delay_Value(head->phase_Id);                                                    //TODO�����ݳ������ͺ�����ϵ������������
    phases_Index[head->down->phase_Id].delay_Queue_Clearance = queue_Delay_Value(head->down->phase_Id); 
	total_Delay += phases_Index[head->phase_Id].delay_Queue_Clearance + phases_Index[head->down->phase_Id].delay_Queue_Clearance;              //�����������

	phases_Index[head->phase_Id].delay_Red_Stop = phases_Index[head->phase_Id].intial_Demand * moment_Of_Cycle;
	phases_Index[head->down->phase_Id].delay_Red_Stop = phases_Index[head->down->phase_Id].intial_Demand * moment_Of_Cycle;
	total_Delay += phases_Index[head->phase_Id].delay_Red_Stop + phases_Index[head->down->phase_Id].delay_Red_Stop;                            //���ͣ������

	moment_Of_Cycle += phases_Index[head->phase_Id].phase_Info.green_Time + phases_Index[head->phase_Id].phase_Info.yellow_Time;
	phase_Delay_Caculation(head->down, moment_Of_Cycle, total_Delay);                                                                          //�ݹ����������λ������
	return;
}

void Node_Adaptive_Control::implement_Node_Control_Function() {
	//���ֲ���

	for (int i = 0; i < phases_Sequence.size(); i++) {
		get_Phases_Green_Time(phases_Sequence[i]);

	}
	
	return;
}

