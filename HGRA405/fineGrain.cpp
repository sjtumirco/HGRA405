#include "fineGrain.h"


//LoopBegin�ڵ�
LoopBegin::LoopBegin() :out_v(0) {}
LoopBegin::~LoopBegin() {}

void LoopBegin::lbegin()
{
	if (!begin_in | !feedback_in)
	{
		out_v = begin_in ^ feedback_in;
		out = (~begin_in)&feedback_in;
	}
	else
		cout << "Error!�����벻��ͬʱΪ1" << endl;
}

//Loop End�ڵ�
LoopEnd::LoopEnd():in1_v(0),in1(0),in2(0), out2back(0), out2end(0) {  }
LoopEnd::~LoopEnd() {}
void LoopEnd::lend()
{
	if (in1_v == 0)//��ִ���κβ���
	{
		if (in2 == 0)
		{
			out2back = 0;
			out2end = 0;
		}
		else if (in2 == 1)
		{
			//cout << "loop end�ڵ����������ģʽ�Ǳ���ֹ��" << endl;
		}		
	}
	else if (in1_v == 1 && in1 == 1)
	{
		if (in2 == 0)
		{
			out2back = 0;
			out2end = 0;
		}
		else if (in2 == 1)
		{
			out2back = 1;
			out2end = 0;
			in1_v = 0;
		}
	}
	else if (in1_v == 1 && in1 == 0)
	{
		if (in2 == 0)
		{
			out2back = 0;
			out2end = 0;
		}
		else if (in2 = 1)
		{
			out2back = 0;
			out2end = 1;
		}
	}	
}


//Join�ڵ�, (pathJoin) 
Join::Join() { inputCollect.resize(joininportNums);
}
Join::~Join() {}

void Join::join()//�����������֮ǰһ��Ҫȷ��inputCollect������������������µ�����
{
	 vector<bool>::size_type size_in = inputCollect.size();
	bool out_init = inputCollect[0] & inputCollect[1];
	for (unsigned int i = 2; i < size_in; i++)
	{
		out_init = out_init & inputCollect[i];		
	}
	out = out_init;
	ismergeFinished = 1;

}
//breakBp�ڵ�
JoinBp::JoinBp() {}
JoinBp::~JoinBp() {}
void JoinBp::join()
{
	auto size_in = inputCollect.size();
	bool out_init = inputCollect[0] & inputCollect[1];
	for (unsigned int i = 2; i < size_in; i++)
	{
		out_init = out_init & inputCollect[i];
	}
	out = out_init;
}

//Switch�ڵ�,(switch point)
Switch::Switch():in_v(0),in(0),out1(0),out2(0) { }
Switch::~Switch() {}
void Switch::path_select()
{
	if (in_v)
	{
		if (in)
		{
			//cond==1,chose out2
			out1 = 0;
			out2 = 1;
		}
		else if (!in)
		{
			//cond==0,chose out1
			out1 = 1;
			out2 = 0;
		}
	}
	
}

void Switch::path_select_tag()
{
	if (in_v)
	{
		if (in)
		{
			//cond==1,chose out2
			out1 = 0;
			out2_tag = in_tag;
			out3_tag = in_tag;
			out3_v = 1;
			out3 = in;
			out2 = 1;
		}
		else if (!in)
		{
			//cond==0,chose out1
			out1 = 1;
			out1_tag = in_tag;
			out3_tag = in_tag;
			out3_v = 1;
			out3 = in;
			out2 = 0;
		}
	}
	

}

//FineGrained4,(2bit-mergeOR)
//Switch::Switch():in_v(0),out1(0),in(0),out2(0) {}
//Switch::~Switch() {}
//void Switch::path_select()
//{
//	if (in1_v)
//	{
//		if (in2)//in1,in2ͬʱΪ�ߵ�ƽ
//		{
//			//cout << "ERROR!����ͬʱ����" << endl;
//			out_v = 0;
//		}
//		else if (!in2)
//		{
//			out = 0;
//			out_v = 1;
//		}
//		else
//		{
//		}
//	}
//	else if (!in1_v)
//	{
//		if (in2)
//		{
//			out_v = 1;
//			out = 1;
//		}
//		else if (!in2)
//		{
//			out_v = 0;
//		}
//		else
//		{
//		}
//	}
//	else
//	{
//	}
//}
		



//LoopEnds�ڵ�
LoopEnds::LoopEnds():compare_in_v(0),break_in_v(0) {  }
LoopEnds::~LoopEnds() {}
void LoopEnds::loopends()
{
	if (!compare_in_v & break_in_v)
	{
		//cout << "this kind of situation is not allowed!" << endl;
	}
	
	if (!compare_in_v)
	{
		if (!break_in_v)
		{
			out2lbegin = 0;
			out2lend = 0;
		}
	}
	else if (compare_in_v & compare_in)
	{
		if (!break_in_v)
		{
			out2lbegin = 0;
			out2lend = 0;
		}
		else if (break_in_v & break_in)
		{
			out2lbegin = 1;
			out2lend = 0;
		}
		else if (break_in_v & !break_in)
		{
			out2lbegin = 0;
			out2lend = 1;
		}
		else
		{

		}
	}
	else if (compare_in_v & !compare_in)
	{
		if (!break_in_v)
		{
			out2lbegin = 0;
			out2lend = 0;
		}
		else if (break_in_v & break_in)
		{
			out2lbegin = 0;
			out2lend = 1;
		}
		else if (break_in_v & !break_in)
		{
			out2lbegin = 0;
			out2lend = 1;
		}
		else
		{

		}
	}
	else
	{

	}
}



//Break�ڵ�
Break::Break():from_break_path(0),from_join_path(0) { }
Break::~Break() {}
void Break::mergeBreak()
{
	if (from_break_path_v & !from_break_path)
	{
		if (!from_join_path)
		{
			out_h = 0;
			out_l = 0;
		}
		else
		{
			out_h = 1;
			out_l = 1;
		}
	}
	else if (from_break_path_v & from_break_path)
	{
		if (!from_join_path)
		{
			out_h = 1;
			out_l = 0;
		}
		else
		{
			//cout << "two path can't be activated at simultaneously!" << endl;
		}
	}
	else
		cout << "two path can't be activated at simultaneously!" << endl;

}

