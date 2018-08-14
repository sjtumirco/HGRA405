#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include "memoryInt.h"
#include "define.h"
#include "typedef.h"
#include "load.h"
//#include "LSUnit.h"

typedef unsigned int uint;
//namespace DRAMSim {
//	class LSUnit;
//}
namespace HGRA
{
	class Store
	{
		//for no tag operate
	public:
		unsigned int addr;
		bool  addr_v;
		short addr_tag;
		int data_in;
		bool data_in_v;
		short data_in_tag;
		int data_out;
		bool data_out_v;
		short data_out_tag;
		unsigned int addr_out;
		bool addr_out_v;
		short addr_out_tag;

		bool se_extra_out_for_end;
		bool se_extra_out_for_end_v;
		bool ack_from_mem = 0;
		bool ack2addr_source_node;
		bool ack2data_source_node;
		queue<vector<int>> config_reg;
		vector<SeBuffer> se_table_buffer;
		list<unsigned> addr_data_in_order;
		//for dramsim
		bool write_success = 0;


	public:
		Store();
		~Store();
		void write2men(MemoryInt& memory2,unsigned int& addr_in,int& data_in);
		//void reset();

		//for tag op
		void addr_in_table();
		void data_in_table();
		void dataOut();
		bool is_addr_buffer_ready(short& addr_tag);
		bool is_data_buffer_ready(short& data_tag);
		bool lineOK(unsigned int id);
		void buffer_clear();
		//INTERFACE WITH LSUINT
		void SEcallback(uint addr, unsigned long long clk);
		void callbackACK();
		//void access(uint ADDR, uint TAG, uint V, DRAMSim::LSUnit* lsp); access函数在main过程中实现


	};
}
