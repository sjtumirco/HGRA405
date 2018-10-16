#include "load.h"

using namespace HGRA;
//using DRAMSim::LSUnit;

Load::Load() :data_out_v(0),ack2addrgen(0),load_success(0),
ack(0),ack_from_data_demand_side(0){ le_table_buffer.resize(intablebuffer_depth);
}
Load::~Load() {}
void Load::addrInBuffer()
{
	pair<uint32, int> dataPair;
	if (addr_in_v)
	{
		dataPair.first = addr_in;
		//dataPair.second = addr_in_v;
		addr_out = addr_in;
		addr_out_v = 1;
	}
	else
		std::cout << "input addr is not ready!" << std::endl;
}

void Load::dataInBuffer()
{
	if (data_in_v)
	{
		dataPair.second = data_in;
		data_in_v = 0;
		datapair_v = 1;
	}
	
}

void Load::dataOutput()
{
	data_out = dataPair.second;
	data_out_v = 1;
}

void Load::reset()
{
	if (isDataOutFinished)
		data_out_v = 0;
}

void Load::addrInTableBuffer()
{
	ack2addrgen = 0;	//Ĭ�����ڿ�ʼʱack��Ϊ�͵�ƽ
	uint32 cnt = 0;		//��cnt��������Ǳ�ռ�ݵ�״̬
	for (int i = 0; i < le_table_buffer.size(); i++)
	{
		if (!le_table_buffer[i].valid)
		{
			le_table_buffer[i].tag = addr_in_tag;
			le_table_buffer[i].valid = 1;
			le_table_buffer[i].addr = addr_in;
			ack2addrgen = 1;
			le_addr_order.push_back(i);
			break;
		}
		else
			cnt++;
	}
	if (cnt == le_table_buffer.size())
	{
		//table full
		ack2addrgen = 0;
	}
}

void Load::addr_out_buffer()
{
	if (!le_addr_order.empty())
	{
		//�����ݴ�buffer���ó�����׼���ý���L/S Unit
		int le_buffer_id_loading = le_addr_order.front();
		if (le_table_buffer[le_buffer_id_loading].valid)
		{
			addr_out_tag = le_table_buffer[le_buffer_id_loading].tag;
			addr_out_v = le_table_buffer[le_buffer_id_loading].valid;
			addr_out = le_table_buffer[le_buffer_id_loading].addr;
			is_addr_out_buffer = 1;
			//le_addr_order.pop_front();
		}
		else
			is_addr_out_buffer = 0;
	}
	
	
}

void Load::data_in_buffer()
{
	if (data_in_v && !le_addr_order.empty())
	{
		le_table_buffer[le_addr_order.front()].data = data_in;
		le_lineOK_order.push_back(le_addr_order.front());
		data_in_v = 0;
		addr_out_v = 0;
		le_addr_order.pop_front();
	}
}
void Load::data_output_tag()
{
	//��table���ó�һ�������͵�����˿���
	data_out_tag = le_table_buffer[le_lineOK_order.front()].tag;
	data_out_v = 1;
	data_out = le_table_buffer[le_lineOK_order.front()].data;	
}

void Load::buffer_clear()
{
	int id = le_lineOK_order.front();
	le_table_buffer[id].addr = 0;
	le_table_buffer[id].data = 0;
	le_table_buffer[id].tag = 0;
	le_table_buffer[id].valid = 0;
	//data_out_v = 0;
	le_lineOK_order.pop_front();
}


void Load::LEcallback(uint addr, unsigned long long clk)
{
	load_success = 1;
}

void Load::callbackACK()
{
	ack = 1;//LSuint ȡ���ɹ�
}
//void Load::access(uint ADDR, uint TAG, uint V, DRAMSim::LSUnit* lsp)
//{
//	//uint V�Ĳ������ݵ�ʱ����Ҫ��������ǿ��ת��  bool->unsigned int
//	bool trans_succss;
//	trans_succss = lsp->AddTrans(ADDR, TAG, V);
//}