#include "processingElement.h"


ProcessingElement::ProcessingElement():dout1_v(0),dout2_v(0),bout_v(0),loc_reg_v(0),last_t4bind(10), ack_outbuffer12alu(1), ack_outbuffer22alu(1), ack_outbuffer32alu(1) {}
ProcessingElement::~ProcessingElement() {}
void ProcessingElement::ALU(int opcode, int in1, int in2 ,bool for_mux)//ALU作为MUX的时候，要将in3的信号端值作为参数传递给该函数
{
	if (opcode == 8)//MUX
	{
		if (for_mux == 0)
		{
			alu_out = in2;
			alu_out_v = 1;
		}
		else if (for_mux == 1)
		{
			alu_out = in1;
			alu_out_v = 1;
		}
		else
		{
		}
	}
	else
	{
		switch (opcode)
		{
		case 0:
			break;

		case 1://signed add
			alu_out = in1 + in2;
			alu_out_v = 1;
			break;

		case 2://signed mul
			alu_out = in1 * in2;
			alu_out_v = 1;
			break;
		case 3://sub
			alu_out = in1 - in2;
			alu_out_v = 1;
			break;

		case 4://div
			alu_out = in1 / in2;
			alu_out_v = 1;
			break;
		case 5://mod
			alu_out = in1 % in2;
			alu_out_v = 1;
			break;

		case 6: //判断两输入的大小
			alu_out_b = in1 < in2 ? 1 : 0;
			alu_out_b_v = 1;
			break;
		case  7:
			alu_out = in1 == in2 ? 1 : 0;//输出较小的值
			alu_out_v = 1;
			break;
		case 8:

			break;

		case 9://MAC
			alu_out = in1 * in2 + loc_reg;
			alu_out_v = 1;
			break;
		/*case 10:*/


		case 12://leshift
			alu_out = in1 << in2;
			alu_out_v = 1;
			break;
		case 13://rishift
			alu_out = in1 >> in2;
			alu_out_v = 1;
			break;
		case 14:// and
			alu_out = in1 & in2;
			alu_out_v = 1;
			break;
		case 15: //or
			alu_out = in1 | in2;
			alu_out_v = 1;
			break;
		case 16: //cos
			alu_out = cos(in1);
			alu_out_v = 1;
			break;
		case 17: //sin
			alu_out = sin(in1);
			alu_out_v = 1;
			break;

		}
	}
	
	alu_ack();
	

}

bool ProcessingElement::tableBuffer_fifo_full1()
{
	if (tableBuffer_fifo1.size() < intablebuffer_depth)
		return 0;
	else if (tableBuffer_fifo1.size() == intablebuffer_depth)
		return 1;
}

bool ProcessingElement::tableBuffer_fifo_full2()
{
	if (tableBuffer_fifo2.size() < intablebuffer_depth)
		return 0;
	else if (tableBuffer_fifo2.size() == intablebuffer_depth)
		return 1;
}

void ProcessingElement::inbuffer_in(ProcessingElement* pe)
{
	pe->ack2in1port = 0;
	pe->ack2in2port = 0;
	pe->ack2in3port = 0;

	//check flag_reg,3steps to simulate
	int flag_reg1 = pe->config_reg.front()[20];
	int flag_reg2 = pe->config_reg.front()[21];
	int flag_reg3 = pe->config_reg.front()[22];
	if (flag_reg1)//in1port没有悬空
	{
		if (pe->din1_v)//上一个PE发出了request请求
		{
			if (!inBuffer1.isInBufferFull())
			{
				if(inBuffer1.dataIn(pe->din1, pe->din1_v))
					pe->ack2in1port = 1;//数据被接收				
			}
		}
	}

	if (flag_reg2)//in2port没有悬空
	{
		if (pe->din2_v)//上一个PE发出了request请求
		{
			if (!inBuffer2.isInBufferFull())
			{
				if (inBuffer2.dataIn(pe->din2, pe->din2_v))
					pe->ack2in2port = 1;//数据被接收				
			}
		}
	}

	if (flag_reg3)//in3port没有悬空
	{
		if (pe->bin_v)//上一个PE发出了request请求
		{
			pe->inbuffer3 = pe->bin;
			pe->inbuffer3_v = pe->bin_v;
			pe->inbuffer3_tag = pe->bin_tag;
			pe->ack2in3port = 1;

		}
	}
}

//void ProcessingElement::outBuffer1_out()
//{
//	if (!bp1)
//	{
//		outBuffer1.dataOut();
//		dout1 = outBuffer1.out;
//		dout1_v = 1;
//	}
//	else
//	{
//		dout1_v = 0;
//	}
//	
//}
//void ProcessingElement::outBuffer2_out()
//{
//	
//
//	if (!bp2)
//	{
//		outBuffer2.dataOut();
//		dout2 = outBuffer2.out;
//		dout2_v = 1;
//
//	}
//	else
//		dout2_v = 0;
//}


//void ProcessingElement::outBuffer1In()
//{
//	outBuffer1.in = outbuffer1_in;
//	outBuffer1.dataIn();
//}
//void ProcessingElement::outBuffer2In()
//{
//	outBuffer2.in = outbuffer2_in;
//	outBuffer2.dataIn();
//}

void ProcessingElement::cycle_alu(int opcode)
{
	if (opcode == 2 || opcode == 9)//乘法和乘累加需要两个时钟周期
	{
		cycle += 2;
	}
	else
		cycle++;
}
void ProcessingElement::cycle_entryLogic()
{
	cycle++;
}

void ProcessingElement::cycle_outLogic()
{
	cycle++;
}

void ProcessingElement::alu_ack()
{
	if (ack_outbuffer12alu | ack_outbuffer22alu)
	{
		ack_alu2ib1 = 1; ack_alu2ib2 = 1;
	}
	
}

//buffer
//for tag operate
//InTableBuffer::InTableBuffer() {}
InTableBuffer::InTableBuffer(const int bufDpth) :bp2lastPe_1(0), bp2lastPe_2(0), bp2lastPe_3(0),last_tag_1(16),last_tag_2(16),last_tag_3(16)
{
	InTableBufferEntity.resize(intablebuffer_depth); /*line_ready_order.resize(intablebuffer_depth);*/
}
InTableBuffer::~InTableBuffer() {}

bool InTableBuffer::isDin1BufferReady(short& in1_tag)
{
	//table中是否有同名tag，是否满了，是否在连续发送同一个tag均是使用该函数的返回值表示,即bp2lastPe
	if (in1_tag == last_tag_1)
	{
		bp2lastPe_1 = 0;
		return bp2lastPe_1;
	}
	bool interspace = 0, same_tag = 0;
	for (vector<TableLine>::size_type i = 0; i < InTableBufferEntity.size(); i++)
	{
		if (InTableBufferEntity[i].tag == in1_tag & InTableBufferEntity[i].valid1 == 0)//tag match and valid1 == 0
		{
			interspace = 1;
			break;
		}
		else if (InTableBufferEntity[i].tag != in1_tag & InTableBufferEntity[i].valid1 == 0 & InTableBufferEntity[i].valid2 == 0 & InTableBufferEntity[i].valid3 == 0 & InTableBufferEntity[i].tag == 0)   //?
		{
			interspace = 1;
			break;
		}
		else
		{
			continue;
		}
		//interspace = 0;
	}
	for (vector<TableLine>::size_type i = 0; i < InTableBufferEntity.size(); i++)
	{
		if (InTableBufferEntity[i].tag == in1_tag && InTableBufferEntity[i].valid1 == 1)
		{
			same_tag = 1;
			break;
		}
		else
		{
			continue;
		}
		//same_tag = 0;
	}

	if (interspace & !same_tag)//buffer中有空位且无同名tag
	{
		bp2lastPe_1 = 1;
	}
	else
	{
		bp2lastPe_1 = 0;
	}
	return bp2lastPe_1;
}

bool InTableBuffer::isDin2BufferReady(short& in2_tag)
{
	if (in2_tag == last_tag_2)
	{
		bp2lastPe_2 = 0;
		return bp2lastPe_2;
	}
	bool interspace = 0, same_tag = 0;
	for (vector<TableLine>::size_type i = 0; i < InTableBufferEntity.size(); i++)
	{
		if (InTableBufferEntity[i].tag == in2_tag & InTableBufferEntity[i].valid2 == 0)//tag match and valid1 == 0
		{
			interspace = 1;
			break;
		}
		else if (InTableBufferEntity[i].tag != in2_tag & InTableBufferEntity[i].valid2 == 0 & InTableBufferEntity[i].valid3 == 0 & InTableBufferEntity[i].valid1 == 0 & InTableBufferEntity[i].tag == 0)
		{
			interspace = 1;
			break;
		}
		else
		{
			continue;
		}
		//interspace = 0;
	}
	for (vector<TableLine>::size_type i = 0; i < InTableBufferEntity.size(); i++)
	{
		if (InTableBufferEntity[i].tag == in2_tag && InTableBufferEntity[i].valid2 == 1)
		{
			same_tag = 1;
			break;
		}
		else
		{
			continue;
		}
		//same_tag = 0;
	}

	if (interspace & !same_tag)//buffer中有空位且无同名tag
	{
		bp2lastPe_2 = 1;
	}
	else
	{
		bp2lastPe_2 = 0;
	}
	return bp2lastPe_2;
}

bool InTableBuffer::isBinBufferReady(short& in3_tag)
{
	if (in3_tag == last_tag_3)
	{
		bp2lastPe_3 = 0;
		return bp2lastPe_3;
	}
	bool interspace = 0, same_tag = 0;
	for (vector<TableLine>::size_type i = 0; i < InTableBufferEntity.size(); i++)
	{
		if (InTableBufferEntity[i].tag == in3_tag & InTableBufferEntity[i].valid3 == 0)//tag match and valid1 == 0
		{
			interspace = 1;
			break;
		}
		else if (InTableBufferEntity[i].tag != in3_tag & InTableBufferEntity[i].valid3 == 0 & InTableBufferEntity[i].valid1 == 0 & InTableBufferEntity[i].valid2 == 0 & InTableBufferEntity[i].tag == 0)
		{
			interspace = 1;
			break;
		}
		else
		{
			continue;
		}
		//interspace = 0;
	}
	for (vector<TableLine>::size_type i = 0; i < InTableBufferEntity.size(); i++)
	{
		if (InTableBufferEntity[i].tag == in3_tag && InTableBufferEntity[i].valid3 == 1)
		{
			same_tag = 1;
			break;
		}
		else
		{
			continue;
		}
		//same_tag = 0;
	}

	if (interspace & !same_tag)//buffer中有空位且无同名tag
	{
		bp2lastPe_3 = 1;
	}
	else
	{
		bp2lastPe_3 = 0;
	}
	return bp2lastPe_3;
}

bool InTableBuffer::line_ok(vector<TableLine>::size_type i,ProcessingElement* pe)
{
	if (!flag_reg1 & !flag_reg2 & !flag_reg3)//000
	{
		return 0;
	}
	else if (!flag_reg1 & !flag_reg2 & flag_reg3)//001
	{
		if (InTableBufferEntity[i].valid3)
		{
			return 1;
		}
		else
			return 0;
	}
	else if (!flag_reg1 & flag_reg2 & !flag_reg3)//010
	{
		if (InTableBufferEntity[i].valid2)
		{
			return 1;
		}
		else
			return 0;
	}
	else if (!flag_reg1 & flag_reg2 & flag_reg3)//011
	{
		if (pe->config_reg.front()[2] == 8)//MUX操作
		{
			if (InTableBufferEntity[i].valid3 & !InTableBufferEntity[i].data3)// valid3=1;data3=0,选择in2
			{
				if (InTableBufferEntity[i].valid2)
				{
					return 1;
				}
				else
					return 0;
			}			
			else
				return 0;
		}
		else
		{
			if (InTableBufferEntity[i].valid2 & InTableBufferEntity[i].valid3)
			{
				return 1;
			}
			else
				return 0;
		}
		
	}
	else if (flag_reg1 & !flag_reg2 & !flag_reg3)//100
	{
		if (InTableBufferEntity[i].valid1)
			return 1;
		else
			return 0;
	}
	else if (flag_reg1 & !flag_reg2 & flag_reg3)//101
	{
		if (pe->config_reg.front()[2] == 8)//MUX操作
		{
			
			if (InTableBufferEntity[i].valid3 & InTableBufferEntity[i].data3)//valid3=1;data3=1,选择in1
			{
				if (InTableBufferEntity[i].valid1)
				{
					return 1;
				}
				else
					return 0;
			}
			else
				return 0;
		}
		else
		{
			if (InTableBufferEntity[i].valid1 & InTableBufferEntity[i].valid3)
				return 1;
			else
				return 0;
		}
		
	}
	else if (flag_reg1 & flag_reg2 & !flag_reg3)//110
	{
		if (InTableBufferEntity[i].valid1 & InTableBufferEntity[i].valid2)
			return 1;
		else
			return 0;
	}
	else if (flag_reg1 & flag_reg2 & flag_reg3)//111
	{
		if (pe->config_reg.front()[2] == 8)//MUX操作
		{

			if (InTableBufferEntity[i].valid3 & !InTableBufferEntity[i].data3)//valid3=1;data3=0,选择in2
			{
				if (InTableBufferEntity[i].valid2)
				{
					return 1;
				}
				else
					return 0;
			}
			else if (InTableBufferEntity[i].valid3 & InTableBufferEntity[i].data3)//valid3=1;data3=1,选择in1
			{
				if (InTableBufferEntity[i].valid1)
					return 1;
				else
					return 0;
			}
			else
				return 0;
		}
		else
		{
			if (InTableBufferEntity[i].valid1 & InTableBufferEntity[i].valid2 & InTableBufferEntity[i].valid3)
				return 1;
			else
				return 0;
		}
		
	}
}

void InTableBuffer::dataIn(ProcessingElement* pe)
{
	//ack clear before op
	pe->ack2in1port = 0;
	pe->ack2in1port = 0;
	pe->ack2in1port = 0;

	//check flag_reg,3steps to simulate
	flag_reg1 = pe->config_reg.front()[20];
	flag_reg2 = pe->config_reg.front()[21];
	flag_reg3 = pe->config_reg.front()[22];
	if (flag_reg1)//in1port没有悬空
	{
		bool match = 0;
		if (pe->din1_v)//上一个PE发出了request请求
		{
			bool ready1 = isDin1BufferReady(pe->din1_tag);
			pe->ack2in1port = pe->inTableBuffer.bp2lastPe_1;
			if (ready1)//buffer可以接受数据
			{
				pe->ack2in1port = 1;
				//开始检索哪儿可以存放数据并且记录一下存放数据的bufferID。
				for (vector<TableLine>::size_type i = 0; i < InTableBufferEntity.size(); i++)
				{
					if ((InTableBufferEntity[i].tag == pe->din1_tag) & ((InTableBufferEntity[i].valid2) | (InTableBufferEntity[i].valid3)))
					{
						//tag匹配上了，排除了其他两个端口数据没有存储进去和tag被清零造成的tag==0匹配的情况					
						InTableBufferEntity[i].valid1 = 1;
						InTableBufferEntity[i].data1 = pe->din1;
						last_tag_1 = pe->din1_tag;
						InTableBufferEntity[i].line_ok = line_ok(i,pe);//检查table行是不是准备好了
						match = 1;
						if (InTableBufferEntity[i].line_ok)	//将准备好的行ID放进链表
						{
							line_ready_order.push_back(i);
						}
						break;
					}
					else
					{
						continue;
					}
				}
				if (!match)
				{
					//table遍历一遍没有匹配的tag,将tag和数据插入表中空位
					for (vector<TableLine>::size_type i = 0; i < InTableBufferEntity.size(); i++)
					{
						if (InTableBufferEntity[i].tag == 0 && ((InTableBufferEntity[i].valid2) | (InTableBufferEntity[i].valid3) | (InTableBufferEntity[i].valid1)) == 0)
						{
							InTableBufferEntity[i].tag = pe->din1_tag;
							InTableBufferEntity[i].valid1 = 1;
							InTableBufferEntity[i].data1 = pe->din1;
							InTableBufferEntity[i].line_ok = line_ok(i, pe);//检查table行是不是准备好了
							last_tag_1 = pe->din1_tag;
							if (InTableBufferEntity[i].line_ok)	//将准备好的行ID放进链表
							{
								line_ready_order.push_back(i);
							}
							break;
						}
					}
				}
				
				//清除上一个PE端口上的数据
			
			}
		}
	}

	if (flag_reg2)//in2port没有悬空
	{
		bool match = 0;
		if (pe->din2_v)//连接该端口的上一个PE发出了request请求
		{
			bool ready2 = isDin2BufferReady(pe->din2_tag);
			pe->ack2in2port = pe->inTableBuffer.bp2lastPe_2;
			if (ready2)//buffer可以接受数据
			{
				pe->ack2in2port = 1;
				//开始检索哪儿可以存放数据并且记录一下存放数据的bufferID。
				for (vector<TableLine>::size_type i = 0; i < InTableBufferEntity.size(); i++)
				{
					if ((InTableBufferEntity[i].tag == pe->din2_tag) & ((InTableBufferEntity[i].valid1) | (InTableBufferEntity[i].valid3)))
					{
						//tag匹配上了，排除了和其他两个端口数据没有存储进去，但是tag被清零造成的tag==0匹配的情况					
						InTableBufferEntity[i].valid2 = 1;
						InTableBufferEntity[i].data2 = pe->din2;
						last_tag_2 = pe->din2_tag;
						InTableBufferEntity[i].line_ok = line_ok(i,pe);//检查table行是不是准备好了
						match = 1;
						if (InTableBufferEntity[i].line_ok)	//将准备好的行ID放进链表
						{
							line_ready_order.push_back(i);
						}
						break;
					}
					else
					{
						continue;
					}
				}
				if (!match)
				{
					//table遍历一遍没有匹配的tag,将tag和数据插入表中空位
					for (vector<TableLine>::size_type i = 0; i < InTableBufferEntity.size(); i++)
					{
						if (InTableBufferEntity[i].tag == 0 && ((InTableBufferEntity[i].valid1) | (InTableBufferEntity[i].valid3) | (InTableBufferEntity[i].valid2)) == 0)
						{
							InTableBufferEntity[i].tag = pe->din2_tag;
							InTableBufferEntity[i].valid2 = 1;
							InTableBufferEntity[i].data2 = pe->din2;
							InTableBufferEntity[i].line_ok = line_ok(i, pe);//检查table行是不是准备好了
							last_tag_2 = pe->din2_tag;
							break;
							//这种情况下不用检测行是否OK，因为din1是第一个进入到表格中的数据
						}
					}
				}
				
			}
		}
	}

	if (flag_reg3)//in3port没有悬空
	{
		bool match = 0;
		if (pe->bin_v)//连接该端口的上一个PE发出了request请求
		{
			bool ready3 = isBinBufferReady(pe->bin_tag);
			pe->ack2in3port = pe->inTableBuffer.bp2lastPe_3;
			if (ready3)//buffer可以接受数据
			{
				pe->ack2in3port = 1;
				//开始检索哪儿可以存放数据并且记录一下存放数据的bufferID。
				for (vector<TableLine>::size_type i = 0; i < InTableBufferEntity.size(); i++)
				{
					if ((InTableBufferEntity[i].tag == pe->bin_tag) & ((InTableBufferEntity[i].valid1) | (InTableBufferEntity[i].valid2)))
					{
						//tag匹配上了，排除了和其他两个端口数据没有存储进去，但是tag被清零造成的tag==0匹配的情况					
						InTableBufferEntity[i].valid3 = 1;
						InTableBufferEntity[i].data3 = pe->bin;
						last_tag_3 = pe->bin_tag;
						InTableBufferEntity[i].line_ok = line_ok(i,pe);//检查table行是不是准备好了
						match = 1;
						if (InTableBufferEntity[i].line_ok)	//将准备好的行ID放进链表
						{
							line_ready_order.push_back(i);
						}
						break;
					}
					else
					{
						continue;
					}
				}
				if (!match)
				{
					//table遍历一遍没有匹配的tag,将tag和数据插入表中空位
					for (vector<TableLine>::size_type i = 0; i < InTableBufferEntity.size(); i++)
					{
						if (InTableBufferEntity[i].tag == 0 && ((InTableBufferEntity[i].valid1) | (InTableBufferEntity[i].valid2) | (InTableBufferEntity[i].valid3) == 0))
						{
							InTableBufferEntity[i].tag = pe->bin_tag;
							InTableBufferEntity[i].valid3 = 1;
							InTableBufferEntity[i].data3 = pe->bin;
							InTableBufferEntity[i].line_ok = line_ok(i, pe);//检查table行是不是准备好了
							last_tag_3 = pe->bin_tag;
							break;
							//这种情况下不用检测行是否OK，因为din1是第一个进入到表格中的数据
						}
					}
				}
				
			}
		}
	}
}

void InTableBuffer::dataOut(ProcessingElement* pe)
{
	list<unsigned int>::iterator i;
	for (i = line_ready_order.begin(); i != line_ready_order.end(); i++)
	{
		if (InTableBufferEntity[*i].line_ok)
		{
			//不去检测是不是每一个端口都是非悬空的状态，即便inbuffer中是无效的数据也拿出来，只不过不使用
			pe->inbuffer1_out = InTableBufferEntity[*i].data1;
			pe->inbuffer1_out_v = InTableBufferEntity[*i].valid1;
			pe->inbuffer1_out_tag = InTableBufferEntity[*i].tag;
			pe->inbuffer2_out = InTableBufferEntity[*i].data2;
			pe->inbuffer2_out_v = InTableBufferEntity[*i].valid2;
			pe->inbuffer2_out_tag = InTableBufferEntity[*i].tag;
			pe->inbuffer3_out = InTableBufferEntity[*i].data3;
			pe->inbuffer3_out_v = InTableBufferEntity[*i].valid3;
			pe->inbuffer3_out_tag = InTableBufferEntity[*i].tag;
			break;
		}
	}
}


void InTableBuffer::buffer_clear()
{
	if (!line_ready_order.empty())
	{
		int id = line_ready_order.front();
		InTableBufferEntity[id].tag = 0;
		InTableBufferEntity[id].valid1 = 0;
		InTableBufferEntity[id].data1 = 0;
		InTableBufferEntity[id].valid2 = 0;
		InTableBufferEntity[id].data2 = 0;
		InTableBufferEntity[id].valid3 = 0;
		InTableBufferEntity[id].data3 = 0;
		InTableBufferEntity[id].line_ok = 0;
		line_ready_order.pop_front();
	}

	
}
//definition of OutBuffer

//OutBuffer::OutBuffer() {}
OutTableBuffer::OutTableBuffer(const int bufDepth) { this->bufDepth = bufDepth; }
OutTableBuffer::~OutTableBuffer() {}

bool OutTableBuffer::isOutBufferFull()
{
	if (outBuffer.size() < outtablebuffer_depth)
	{
		isOutBufFull = 0;
		return isOutBufFull;
	}
	else
	{
		isOutBufFull = 1;
		return isOutBufFull;
	}
}


void OutTableBuffer::dataIn()
{
	isOutBufferFull();
	if (!isOutBufFull)
	{
		inDataTmp.data = in;
		inDataTmp.tag = in_tag;
		inDataTmp.valid = in_v;
		outBuffer.push(inDataTmp);
		isDataInFinished = 1;
	}
	else
	{
		//std::cout << "OutBuffer if full1" << std::endl;
		isDataInFinished = 0;
	}
}

void OutTableBuffer::dataOut()
{

	if (!outBuffer.empty())
	{
		outDataTmp = outBuffer.front();
		out = outDataTmp.data;
		out_tag = outDataTmp.tag;
		out_v = outDataTmp.valid;
	}
}

void OutTableBuffer::buffer_clear()
{
	if (!outBuffer.empty())
	{
		outBuffer.pop();
	}	
}

void ProcessingElement::valid_clear(CLOCK port_idx)
{
	if (port_idx == 0)//port1
	{
		dout1_v = 0;
	}
	else if (port_idx == 1)//port2
	{
		dout2_v = 0;
	}
	else if (port_idx == 2)//port3
	{
		bout_v = 0;
	}
}


//for no tag inBuffer and outBuffer类的定义
//for no tag operate
InBuffer::InBuffer(const int bufDpth) { this->bufDepth = bufDepth; }
InBuffer::~InBuffer() {}
bool InBuffer::isInBufferFull()
{
	if (inputBuffer.size() < inbuffer_depth)
	{
		return 0;//no full
	}
	else
	{
		return 1;//full
	}
}

bool InBuffer::dataIn(int in, bool in_v)    //由于该函数没有PE指针的信息且被动态时的datain复用，将拉bp的代码写在unitSim中
{
	if (!isInBufferFull())
	{
		if (in_v)
		{
			OutBuffer_no_tag buffer_line_tmp;
			buffer_line_tmp.data = in;
			buffer_line_tmp.valid = in_v;
			inputBuffer.push(buffer_line_tmp);
			return 1;//入数成功
		}
	}
	else
		return 0;
}

bool InBuffer::dataOut(int& out, bool& out_v)  //并没有被使用
{
	if (inputBuffer.size() != 0)
	{
		OutBuffer_no_tag outDataTmp;
		outDataTmp = inputBuffer.front();
		out = outDataTmp.data;
		out_v = outDataTmp.valid;
		inputBuffer.pop();
		return 1;
	}
	else
		return 0;
}

void InBuffer::resize_(const int size)
{
	//
}

//definition of OutBuffer

//OutBuffer::OutBuffer() {}
OutBuffer::OutBuffer(const int bufDepth) { this->bufDepth = bufDepth; }
OutBuffer::~OutBuffer() {}

bool OutBuffer::isOutBufferFull()
{
	if (outputBuffer.size() < outbuffer_depth)
	{
		return 0;//没有满
	}
	else
	{
		return 1;//满了
	}
}


bool OutBuffer::dataIn(int in, bool in_v)
{
	if (!isOutBufferFull())
	{
		OutBuffer_no_tag inDataTmp;
		inDataTmp.data = in;
		inDataTmp.valid = in_v;
		outputBuffer.push(inDataTmp);
		return 1;
	}
	else
	{
		//std::cout << "OutBuffer if full1" << std::endl;
		return 0;
	}
}

bool OutBuffer::dataOut(int& out, bool& out_v)
{
	//负责出数和清buffer
	if (outputBuffer.size() != 0)
	{
		OutBuffer_no_tag outDataTmp;
		outDataTmp = outputBuffer.front();
		out = outDataTmp.data;
		out_v = outDataTmp.valid;
		outputBuffer.pop();
		return 1;
	}
	else
		return 0;


}

void OutBuffer::buffer_clear()
{
	if (!outputBuffer.empty())
	{
		outputBuffer.pop();
	}
}