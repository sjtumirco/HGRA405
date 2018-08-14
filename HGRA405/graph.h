
#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <queue>
#include "typedef.h"

//对图的排序分为以下几个步骤：
//1.提取配置文件中的图关系，初始化图的领接表（领接表是存储该顶点出度个数的链表）
//2.完善每一个顶点自身的信息（包括顶点个数，入度数）
//3.搜索每一个顶点的入度个数，为0的时候就将该顶点放置到排序队列中去，并且删除该顶点和该顶点对应的领接表中该顶点的所有出度
//4.继续搜索下一个顶点


using namespace std;


class Graph
{
private:
	int V;             // 顶点个数
	list<int> *adj;    // 邻接表
	queue<int> q;      
	vector<int> q1;
	int* indegree;     // 记录每个顶点的入度
	vector<pe_line_int> pe_config_int_sorted;
	
	
public:
	Graph(int V);                   
	~Graph();  
	//vector<pair<int, int>> return_tmp;//存储边的关系
	void addEdge(int v, int w);
	bool topological_sort();
	vector<int> returnRlt();
	int peNum2index(int);
	vector<pe_line_int> get_config_sorted(vector<pe_line_int> vec_pe_config);
	vector<pair<int, int>> edgeConstruct(vector<vector<int>> vec_config_parsed);

};





