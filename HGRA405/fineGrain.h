#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include "define.h"

using namespace std;
class LoopBegin			//loop begin节点
{	
	
public:
	LoopBegin();
	~LoopBegin();
	//input
	bool begin_in, feedback_in;
	/*bool begin_in_v;*/
	//output
	bool out_v,out;
	//bool out_v;
	queue<vector<int>> config_reg;
	void lbegin();	
};



class LoopEnd			//loop end节点
{
public:
	LoopEnd();
	~LoopEnd();
	//input
	bool in1, in1_v;		//from PE's judgemental result
	bool in2;				//from DFG
	//output
	bool out2back;				// to re-start node
	bool out2end;				// to end node 	
	queue<vector<int>> config_reg;
	//function
	void lend();	
};

class Join			//join节点
{
public:
	Join();
	~Join();
	vector<bool> inputCollect;  //input port is not certain,use vector for flexiable 
	bool out;
	queue<vector<int>> config_reg;
	void join();

public:
	bool ismergeFinished; //表示一次汇合操作是否已经完成

};

class Shit			//shit节点
{
public:
	Shit();
	~Shit();
	vector<bool> inputCollect;  //input port is not certain,use vector for flexiable 
	bool out;
	queue<vector<int>> config_reg;
	void shit();

public:
	bool ismergeFinished; //表示一次汇合操作是否已经完成

};
class JoinBp {
public:
	JoinBp();
	~JoinBp();
	vector<bool> inputCollect;
	bool out;
	queue<vector<int>> config_reg;
	void join();


};

class Switch				//switch节点
{
public:
	Switch();
	~Switch();
	bool in,in_v;
	short in_tag;
	bool out1;  //path A
	bool out2; //path B
	short out1_tag;
	short out2_tag;
	short out3_tag;	//path C
	bool out3_v;
	bool out3;
	queue<vector<int>> config_reg;
	void path_select();
	void path_select_tag();

};

class LoopEnds //in1--2b,in2--2b,in1 from i<N,in2 from break point
								//break节点
{
public:
	LoopEnds();
	~LoopEnds();
	//input
	bool compare_in, compare_in_v;
	bool break_in, break_in_v;
	//outut
	bool out2lbegin, out2lend;
	queue<vector<int>> config_reg;
	void loopends();
	
};



//Break 节点
class Break 
{
public:
	Break();
	~Break();
	bool from_break_path, from_break_path_v,from_join_path;
	bool out_h, out_l;
	queue<vector<int>> config_reg;
	void mergeBreak();
};

