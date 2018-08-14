#pragma once
#include <vector>
#include <string>


typedef std::vector<std::string>  pe_line;
typedef std::vector<int> pe_line_int;
typedef struct table_line {
	short tag;
	bool valid1;
	int data1;
	bool valid2;
	int data2;
	bool valid3;
	bool data3;
	//bool tag_valid;
	bool line_ok;
}TableLine;

typedef struct tableline4fifo {
	bool valid;
	int data;
}TableLine4Fifo;

typedef struct outbuffer {
	short tag;
	bool valid;
	int data;
}OutBuffer_;


typedef struct lebuffer {
	short tag;
	bool valid;
	unsigned int addr;
	int data;
	bool lineOK;
}LeBuffer;

typedef struct sebuffer{
	short tag;
	bool valid1;
	unsigned int addr;
	bool valid2;
	int data;
	bool lineOK;
}SeBuffer;
