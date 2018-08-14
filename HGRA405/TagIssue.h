#pragma once
/*
TagIssue模块是发送tag的模块，功能：在每个时钟周期发送一个tag值到输出端口，
如果输出端口的值没有被拿走（valid值有没有被清零），那么该模块不发射数据到
输出端口，知道上一次发射的值被拿走
*/
#include <iostream>
#include <vector>
typedef unsigned int uint32_t;
class TagIssue
{
public:
	TagIssue();
	~TagIssue();
	//output
	bool out_v;
	uint32_t out;
	uint32_t out_t;//虚拟接口
	std::vector<int> tag_container;
	uint32_t pointor;

	//func	
	void tag_issue();
private:
	void tag_container_init();

};