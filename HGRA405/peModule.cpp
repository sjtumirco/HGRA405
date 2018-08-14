#include "peModule.h"
//for no tag operate
InBuffer::InBuffer(const int bufDpth) { this->bufDepth = bufDepth; }
InBuffer::~InBuffer() {}
void InBuffer::isInBufferFull()
{
	if (inputBuffer.size() < inbuffer_depth)
	{
		isInBufFull = 0;
	}

	else
	{
		isInBufFull = 1;
	}

}

void InBuffer::dataIn()
{
	if (!isInBufFull)
	{
		if (in_v)
		{
			inDataTmp = in;
			inputBuffer.push(inDataTmp);
			isInDataFinished = 1;
		}
		else
			isInDataFinished = 0;

	}

	else
		isInDataFinished = 0;

}

void InBuffer::dataOut()
{
	if (!inputBuffer.empty())
	{
		outDataTmp = inputBuffer.front();
		out = outDataTmp;
		inputBuffer.pop();
	}
}

//definition of OutBuffer

//OutBuffer::OutBuffer() {}
OutBuffer::OutBuffer(const int bufDepth) { this->bufDepth = bufDepth; }
OutBuffer::~OutBuffer() {}

void OutBuffer::isOutBufferFull()
{
	if (outputBuffer.size() < outbuffer_depth)
	{
		isOutBufFull = 0;

	}
	else
	{
		isOutBufFull = 1;

	}
}


void OutBuffer::dataIn()
{
	if (!isOutBufFull)
	{
		inDataTmp = in;
		outputBuffer.push(inDataTmp);
		isDataInFinished = 1;
	}
	else
	{
		//std::cout << "OutBuffer if full1" << std::endl;
		isDataInFinished = 0;
	}
}

void OutBuffer::dataOut()
{
	if (!outputBuffer.empty())
	{
		out = outputBuffer.front();
		outputBuffer.pop();
	}


}


