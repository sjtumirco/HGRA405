#pragma once
//this file is a module collection in a single PE
#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include "typedef.h"
#include "define.h"

using namespace std;

//for no tag
class InBuffer {
public:
	int in;
	bool in_v;
	int out;
	bool out_v;
	bool isInBufFull,
		isInDataFinished;

	queue<int> inputBuffer;

public:
	//InBuffer();
	InBuffer(const int inBufDepth = 4);//initialize depth of InBuffer
	~InBuffer();
	void isInBufferFull();//要在dataIn()函数之前运行
	void dataIn();  //bool value show if input data has been stored correctly.
	void dataOut();



private:

	unsigned int bufDepth;
	int inDataTmp;
	int outDataTmp;


};

class OutBuffer
{
public:
	int in;
	int out;
	bool isOutBufFull,
		isDataInFinished;

public:
	//OutBuffer();
	OutBuffer(const int outBufDepth = 8);
	~OutBuffer();
	void isOutBufferFull();//要在dataIn()函数之前运行
	void dataIn();
	void dataOut();


private:
	unsigned int bufDepth;
	queue<int> outputBuffer;
	int inDataTmp;


};


