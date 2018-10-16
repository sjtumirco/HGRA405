#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include "typedef.h"
#include "define.h"
#include "store.h"
//#include "LSUnit.h"

using std::vector;
using std::string;
typedef unsigned int uint;
//namespace DRAMSim 
//{
//	class LSUnit;
//}
typedef unsigned int uint32;
//class LSUnit;
namespace HGRA
{
	class Load
	{
	public:
		unsigned int addr_in, addr_out;
		bool addr_in_v, addr_out_v, data_out_v, data_in_v;
		bool isDataOutFinished = 0;
		short addr_in_tag, addr_out_tag, data_out_tag;
		int data_in, data_out;
		bool ack2addrgen, ack_from_data_demand_side;
		bool is_addr_out_buffer;
		uint ack;//ack_from_lsunit
		bool load_success;

		queue<vector<int>> config_reg;
		vector<LeBuffer> le_table_buffer;		//buffer for tag operate
		pair<uint32, int> dataPair;				//buffer for no tag operate
		//queue<pair<uint32, int>> addrfifo;	//深度没有设置，具体硬件中有冲突
											
		list<unsigned int> le_addr_order;
		list<unsigned int> le_lineOK_order;
	private:
		//vector<pair<unsigned int, int>> tableBuffer;


	public:
		Load();
		//Load() { tableBuffer.resize(2); }
		~Load();
		void addrInBuffer();
		void dataInBuffer();		//no tag operate	
		void dataOutput();

		void addrInTableBuffer();	//addrIn for tag operate
		void addr_out_buffer();
		void data_in_buffer();
		void data_output_tag();
		void reset();
		void buffer_clear();	//与之相连的PE的ack线有效的时候调用该函数

		//INTERFACE WITH LSUNIT
		void LEcallback(uint addr, unsigned long long clk);
		void callbackACK();
		//void access(uint ADDR, uint TAG, uint V, DRAMSim::LSUnit* lsp); access函数在main过程中实现

	};
}
