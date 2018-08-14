#include <iostream>
#include "store.h"



using namespace std;
using HGRA::Load;
using HGRA::Store;
Store::Store() :data_out_v(0) { se_table_buffer.resize(intablebuffer_depth); }
Store::~Store() {}
void Store::write2men(MemoryInt& memory2,unsigned int& addr_in,int& data_in)
{
	
	/*data_out = data_in;
	data_out_v = data_in_v;*/
	if (data_out_v)
	{
		memory2.write(addr_in, data_in);
	}
	
	//isDataWriteFinished = 1;
}

void Store::addr_in_table()
{
	bool match = 0;
	if (addr_v)//上一个PE发出了request请求
	{
		if (is_addr_buffer_ready(addr_tag))//buffer可以接受数据
		{
			ack2addr_source_node = 1;
			//开始检索哪儿可以存放数据并且记录一下存放数据的bufferID。
			for (vector<SeBuffer>::size_type i = 0; i < se_table_buffer.size(); i++)
			{
				if ((se_table_buffer[i].tag == addr_tag) & se_table_buffer[i].valid2)
				{
					//tag匹配上了，排除了和其他两个端口数据没有存储进去，但是tag被清零造成的tag==0匹配的情况					
					se_table_buffer[i].valid1 = 1;
					se_table_buffer[i].addr = addr;
					se_table_buffer[i].lineOK = lineOK(i);//检查table行是不是准备好了
					match = 1;
					if (se_table_buffer[i].lineOK)	//将准备好的行ID放进链表
					{
						addr_data_in_order.push_back(i);
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
				for (vector<TableLine>::size_type i = 0; i < se_table_buffer.size(); i++)
				{
					if (se_table_buffer[i].tag == 0 && !se_table_buffer[i].valid2 && !se_table_buffer[i].valid1)
					{
						se_table_buffer[i].tag = addr_tag;
						se_table_buffer[i].valid1 = 1;
						se_table_buffer[i].addr = addr;
						break;
					}
					else
						continue;
				}
			}
			
		}
	}
}

void Store::data_in_table()
{
	bool match = 0;
	if (data_in_v)//上一个PE发出了request请求
	{
		if (is_data_buffer_ready(data_in_tag))//buffer可以接受数据
		{
			ack2data_source_node = 1;
			//开始检索哪儿可以存放数据并且记录一下存放数据的bufferID。
			for (vector<SeBuffer>::size_type i = 0; i < se_table_buffer.size(); i++)
			{
				if ((se_table_buffer[i].tag == data_in_tag) & se_table_buffer[i].valid1)
				{
					//tag匹配上了，排除了和其他两个端口数据没有存储进去，但是tag被清零造成的tag==0匹配的情况					
					se_table_buffer[i].valid2 = 1;
					se_table_buffer[i].data = data_in;
					se_table_buffer[i].lineOK = lineOK(i);//检查table行是不是准备好了
					match = 1;
					if (se_table_buffer[i].lineOK)	//将准备好的行ID放进链表
					{
						addr_data_in_order.push_back(i);
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
				for (vector<TableLine>::size_type i = 0; i < se_table_buffer.size(); i++)
				{
					if (se_table_buffer[i].tag == 0 && !se_table_buffer[i].valid2 && !se_table_buffer[i].valid1)
					{
						se_table_buffer[i].tag = data_in_tag;
						se_table_buffer[i].valid2 = 1;
						se_table_buffer[i].data = data_in;
						break;
					}
					else
						continue;
				}
			}
			
		}
	}
}

bool Store::is_addr_buffer_ready(short& addr_tag)
{
	bool interspace = 0; bool same_tag = 0;
	for (vector<SeBuffer>::size_type i = 0; i < se_table_buffer.size(); i++)
	{
		if (se_table_buffer[i].tag == addr_tag & se_table_buffer[i].valid1 == 0)//tag match and valid1 == 0
		{
			interspace = 1;
			break;
		}
		else if (se_table_buffer[i].tag != addr_tag & se_table_buffer[i].valid1 == 0 & se_table_buffer[i].tag == 0)
		{
			interspace = 1;
			break;
		}
		else
		{
			continue;
		}
	}

	for (vector<SeBuffer>::size_type i = 0; i < se_table_buffer.size(); i++)
	{
		if (se_table_buffer[i].tag == addr_tag && se_table_buffer[i].valid1 == 1)
		{
			same_tag = 1;
			break;
		}
		else
		{
			continue;
		}
		
	}

	if (interspace & !same_tag)//buffer中有空位且无同名tag
	{
		 ack2addr_source_node = 1;
	}
	else
	{
		ack2addr_source_node = 0;
	}
	return ack2addr_source_node;
}

bool Store::is_data_buffer_ready(short& data_tag)
{
	bool interspace=1, same_tag=0;
	for (vector<SeBuffer>::size_type i = 0; i < se_table_buffer.size(); i++)
	{
		if (se_table_buffer[i].tag == data_tag & se_table_buffer[i].valid1 == 0)//tag match and valid1 == 0
		{
			interspace = 1;
			break;
		}
		else if (se_table_buffer[i].tag != data_tag & se_table_buffer[i].valid1 == 0 & se_table_buffer[i].tag == 0)
		{
			interspace = 1;
			break;
		}
		else
		{
			continue;
		}
		interspace = 0;
	}

	for (vector<SeBuffer>::size_type i = 0; i < se_table_buffer.size(); i++)
	{
		if (se_table_buffer[i].tag == data_tag && se_table_buffer[i].valid2 == 1)
		{
			same_tag = 1;
			break;
		}
		else
		{
			continue;
		}
		same_tag = 0;
	}

	if (interspace & !same_tag)//buffer中有空位且无同名tag
	{
		ack2data_source_node = 1;
	}
	else
	{
		ack2data_source_node = 0;
	}
	return ack2data_source_node;
}

bool Store::lineOK(unsigned int id)
{
	if (se_table_buffer[id].valid1 & se_table_buffer[id].valid2)
	{
		return 1;
	}
	else
		return 0;

}

void Store::buffer_clear()
{
	int id = addr_data_in_order.front();
	se_table_buffer[id].tag = 0;
	se_table_buffer[id].valid1 = 0;
	se_table_buffer[id].addr = 0;
	se_table_buffer[id].valid2 = 0;
	se_table_buffer[id].data = 0;
	se_table_buffer[id].lineOK = 0;
	addr_data_in_order.pop_front();
}

void Store::dataOut()
{
	if (addr_data_in_order.size())
	{
		int id = addr_data_in_order.front();
		addr_out_tag = se_table_buffer[id].tag;
		addr_out_v = se_table_buffer[id].valid1;
		addr_out = se_table_buffer[id].addr;
		data_out_tag = se_table_buffer[id].tag;
		data_out_v = se_table_buffer[id].valid2;
		data_out = se_table_buffer[id].data;
	}
	
}

void Store::SEcallback(uint addr, unsigned long long clk)
{
	write_success = 1;
}

void Store::callbackACK()
{
	ack_from_mem = 1;
}

//void Store::access(uint ADDR, uint TAG, uint V, DRAMSim::LSUnit* lsp)
//{
//	bool tans_success = lsp->AddTrans(ADDR, TAG, V);
//}