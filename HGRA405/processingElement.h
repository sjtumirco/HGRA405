#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include "peModule.h"
#include <list>
#include "typedef.h"
#include "define.h"

typedef unsigned int CLOCK;



using namespace std;

class ProcessingElement;//类前向申明
class InTableBuffer {

public:
	int in1, in2, in3;
	bool in1_v, in2_v, in3_v;
	short in1_t, in2_t, in3_t;
	int out1, out2, out3;
	bool out1_v, out2_v, out3_v;
	short out1_t, out2_t, out3_t;

	bool bp2lastPe_1, bp2lastPe_2, bp2lastPe_3;

	vector<TableLine> InTableBufferEntity;
	//queue<int> inputBuffer;
	list<unsigned int> line_ready_order;

public:
	//InBuffer();
	InTableBuffer(const int inBufDepth = 4);//initialize depth of InBuffer
	~InTableBuffer();
	//void isInBufferFull();//要在dataIn()函数之前运行
	void dataIn(ProcessingElement* pe);  //bool value show if input data has been stored correctly.
	void dataOut(ProcessingElement* pe);
	void buffer_clear();
private:
	bool isDin1BufferReady(short& in1_tag);
	bool isDin2BufferReady(short& in2_tag);
	bool isBinBufferReady(short& in3_tag);
	bool line_ok(vector<TableLine>::size_type i,ProcessingElement* pe);
	//indicate which input port is aviable
	bool flag_reg1, flag_reg2, flag_reg3;
	//same tag cannot entry one PE
	short last_tag_1, last_tag_2, last_tag_3;
	//记录tag进入的顺序
	list<short> tag_list;
	unsigned int bufDepth;
	int inDataTmp;
	int outDataTmp;


};

class OutTableBuffer
{
public:
	int in;
	short in_tag;
	bool in_v;
	int out;
	short out_tag;
	bool out_v;
	bool isOutBufFull,
		isDataInFinished;
	queue<OutBuffer_> outBuffer;

public:
	//OutBuffer();
	OutTableBuffer(const int outBufDepth = 4);
	~OutTableBuffer();
	bool isOutBufferFull();
	void dataIn();
	void dataOut();
	void buffer_clear();


private:
	unsigned int bufDepth;
	
	OutBuffer_ inDataTmp;
	OutBuffer_ outDataTmp;


};


class ProcessingElement {

public:
	//input port
	int din1;
	int din2;
	bool bin;
	bool din1_v, din2_v, bin_v;
	short din1_tag, din2_tag, bin_tag;
	//output port
	int dout1;
	int dout2;
	bool bout;
	bool dout1_v, dout2_v, bout_v;
	short dout1_tag, dout2_tag, bout_tag;
	//intermiddle variables
	bool bp1, bp2;					//reprensent outputBuffer1 and outputBuffer2 backpressure signal
	/*bool outbuffer3, outbuffer3_v;*/
	short outbuffer3_tag;
	//ack 信号
	bool ack2in1port, ack2in2port, ack2in3port;
	int last_t4bind;

	//
	int cycle;



	
	//input and output buffer declaration
	InBuffer inBuffer1, inBuffer2;
	OutBuffer outBuffer1, outBuffer2;
	InTableBuffer inTableBuffer;
	OutTableBuffer outTableBuffer1, outTableBuffer2;
	//for tag match mode
	queue<TableLine4Fifo> tableBuffer_fifo1;
	queue<TableLine4Fifo> tableBuffer_fifo2;
	//inbuffer for no tag
	int inbuffer1, inbuffer2;
	bool inbuffer1_v, inbuffer2_v;
	bool inbuffer3, inbuffer3_v;
	//outbuffer for no tag
	int outbuffer1, outbuffer2;
	bool outbuffer1_v, outbuffer2_v;
	bool outbuffer3, outbuffer3_v;
	//local reg declaration
	int loc_reg;
	short loc_reg_tag;
	bool loc_reg_v;
	queue<vector<int>> config_reg; //配置寄存器，深度可以配置
	//inableBuffer输出信号申明
	int inbuffer1_out, inbuffer2_out,inbuffer3_out;
	short inbuffer1_out_tag, inbuffer2_out_tag, inbuffer3_out_tag;
	bool inbuffer1_out_v, inbuffer2_out_v, inbuffer3_out_v;
	//alu输入输出信号申明
	int alu_out, alu_out_b;
	bool alu_out_v, alu_out_b_v;
	int alu_in1, alu_in2;
	bool alu_in1_v, alu_in2_v;
	bool ack_alu2ib1, ack_alu2ib2;
	//outbuffer输入输出信号申明
	int outbuffer1_in, outbuffer2_in;
	short outbuffer1_in_tag, outbuffer2_in_tag;
	bool outbuffer1_in_v, outbuffer2_in_v, outbuffer3_in_v;
	int outbuffer1_out, outbuffer2_out;
	bool ack_outbuffer12alu, ack_outbuffer22alu;
	
	
	
	


public:
	ProcessingElement();
	~ProcessingElement();
	void ALU(int opcode, int in1, int in2, bool for_mux);

	//inBuffer入数,出数
	void peInportCtr1();
	void peInportCtr2();
	void inBufferOutCtr1();
	void inBufferOutCtr2();
	//outBuffer入数，出数
	void outBuffer1In();
	void outBuffer2In();
	void peExportCtr1();
	void peExportCtr2();
	//出口数据有效位清零
	void valid_clear(CLOCK port_idx);
	//clock
	void cycle_alu(int opcode);
	void cycle_entryLogic();
	void cycle_outLogic();
	bool tableBuffer_fifo_full1();
	bool tableBuffer_fifo_full2();

private:
	void alu_ack();
	

};



