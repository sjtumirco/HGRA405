#include <iostream>
#include <string>

#include "typedef.h"

#include "graph.h"

//#include "stringConvert.h"

using namespace std;

Graph::Graph(int V)
{
	this->V = V;
	adj = new list<int>[V];

	indegree = new int[V];  // 入度全部初始化为0
	for (int i = 0; i<V; ++i)
		indegree[i] = 0;
}

Graph::~Graph()
{
	delete[] adj;
	delete[] indegree;
}

void Graph::addEdge(int v, int w)  //v -> w，v 是顶点
{
	adj[v].push_back(w);
	++indegree[w];
}

bool Graph::topological_sort()
{
	for (int i = 0; i<V; ++i)
		if (indegree[i] == 0)
			q.push(i);         // 将所有入度为0的顶点入队

	int count = 0;             // 计数，记录当前已经输出的顶点数 
	
	while (!q.empty())
	{
		int v = q.front();      // 从队列中取出一个顶点
		q1.push_back(v);
		q.pop();

		cout << v << " ";      // 输出该顶点
		++count;
		// 将所有v指向的顶点的入度减1，并将入度减为0的顶点入栈
		list<int>::iterator beg = adj[v].begin();
		for (; beg != adj[v].end(); ++beg)
			if (!(--indegree[*beg]))
				q.push(*beg);   // 若入度为0，则入栈
	}

	if (count < V)
		return false;           // 没有输出全部顶点，有向图中有回路
	else
		return true;            // 拓扑排序成功
}



vector<pe_line_int> Graph::get_config_sorted(vector<pe_line_int> vec_pe_config)
{
	for (unsigned int it = 0; it<q1.size(); it++)
	{
		for (unsigned int j = 0; j < vec_pe_config.size(); j++)
		{
			if (vec_pe_config[j][0] == q1[it])
				pe_config_int_sorted.push_back(vec_pe_config[j]);

		}
	}
	return pe_config_int_sorted;
}

vector<int> Graph::returnRlt()
{
	return q1;
}


int Graph::peNum2index(int peNum)
{
	for (unsigned int i = 0; i <q1.size(); i++)
	{
		if (q1[i] == peNum)
			return i;
		else
			continue;
	}
}

vector<pair<int, int>> Graph::edgeConstruct(vector<vector<int>> vec_config_parsed)
{
	vector<pair<int, int>> return_tmp;
	pair<int, int> edge_tmp;
	unsigned int index;
	for (unsigned int i = 0; i < vec_config_parsed.size(); i++)
	{
		vec_config_parsed[i].push_back(i);
	}
	for (unsigned int i = 0; i < vec_config_parsed.size(); i++)	
	{
		//当前正在处理的是PE
		if (vec_config_parsed[i][0] == 8)
		{
			

			//in1_from
			//in1 from LE
			if (vec_config_parsed[i][5] == 1)
			{
				index = vec_config_parsed[i][3];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 9)//found le
					{
						if (vec_config_parsed[j][1] == index)//found le index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			//in1 from PE
			else if (vec_config_parsed[i][5] == 2)
			{
				index = vec_config_parsed[i][3];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 8)//found pe
					{
						if (vec_config_parsed[j][1] == index)//found pe index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}

			//in1 from fg1
			else if (vec_config_parsed[i][5] == 3)
			{
				index = vec_config_parsed[i][3];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 1)//found fg1
					{
						if (vec_config_parsed[j][1] == index)//found fg1 index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			//in1 from fg2
			else if (vec_config_parsed[i][5] == 4)
			{
				index = vec_config_parsed[i][3];
				if (vec_config_parsed[i][4] == 1)//from port2,将来自于port1的反馈边去除
				{
					for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
					{
						if (vec_config_parsed[j][0] == 2)//found fg2
						{
							if (vec_config_parsed[j][1] == index)//found fg2 index
							{
								edge_tmp.first = j;
								edge_tmp.second = i;
								return_tmp.push_back(edge_tmp);
							}
						}
					}
				}

			}
			//in1 from fg3
			else if (vec_config_parsed[i][5] == 5)
			{
				index = vec_config_parsed[i][3];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 3)//found fg3
					{
						if (vec_config_parsed[j][1] == index)//found fg3 index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			//in1 from fg4
			else if (vec_config_parsed[i][5] == 6)
			{
				index = vec_config_parsed[i][3];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 4)//found fg4
					{
						if (vec_config_parsed[j][1] == index)//found fg4 index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			//in1 from fg5
			else if (vec_config_parsed[i][5] == 7)
			{
			}
			//in1 from fg6
			else if (vec_config_parsed[i][5] == 8)
			{
			}
			//in1 from fg7
			else if (vec_config_parsed[i][5] == 9)
			{
			}

			//in2 from
			//in2 from LE
			if (vec_config_parsed[i][8] == 1)
			{
				index = vec_config_parsed[i][6];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 9)//found le
					{
						if (vec_config_parsed[j][1] == index)//found le index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			//in2 from PE
			if (vec_config_parsed[i][8] == 2)
			{
				index = vec_config_parsed[i][6];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 8)//found pe
					{
						if (vec_config_parsed[j][1] == index)//found pe index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			//in2 from fg1
			if (vec_config_parsed[i][8] == 3)
			{
				index = vec_config_parsed[i][6];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 1)//found fg1
					{
						if (vec_config_parsed[j][1] == index)//found fg1 index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			//in2 from fg2
			if (vec_config_parsed[i][8] == 4)
			{
				index = vec_config_parsed[i][6];
				if (vec_config_parsed[i][7] == 1)//from port2,将来自于port1的反馈边去除
				{
					for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
					{
						if (vec_config_parsed[j][0] == 2)//found fg2
						{
							if (vec_config_parsed[j][1] == index)//found fg2 index
							{
								edge_tmp.first = j;
								edge_tmp.second = i;
								return_tmp.push_back(edge_tmp);
							}
						}
					}
				}

			}
			//in2 from fg3
			if (vec_config_parsed[i][8] == 5)
			{
				index = vec_config_parsed[i][6];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 3)//found fg3
					{
						if (vec_config_parsed[j][1] == index)//found fg3 index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			//in2 from fg4
			if (vec_config_parsed[i][8] == 6)
			{
				index = vec_config_parsed[i][6];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 4)//found fg4
					{
						if (vec_config_parsed[j][1] == index)//found fg4 index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			//in2 from fg5
			if (vec_config_parsed[i][8] == 7)
			{

			}

			//in3 from
			//in3 from LE
			if (vec_config_parsed[i][11] == 1)
			{
				index = vec_config_parsed[i][9];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 9)//found le
					{
						if (vec_config_parsed[j][1] == index)//found le index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			//in3 from PE
			if (vec_config_parsed[i][11] == 2)
			{
				index = vec_config_parsed[i][9];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 8)//found pe
					{
						if (vec_config_parsed[j][1] == index)//found pe index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			//in3 from fg1
			if (vec_config_parsed[i][11] == 3)
			{
				index = vec_config_parsed[i][9];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 1)//found fg1
					{
						if (vec_config_parsed[j][1] == index)//found fg1 index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			//in3 from fg2
			if (vec_config_parsed[i][11] == 4)
			{
				index = vec_config_parsed[i][9];
				if (vec_config_parsed[i][10] == 1)//from port2,将来自于port1的反馈边去除
				{
					for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
					{
						if (vec_config_parsed[j][0] == 2)//found fg2
						{
							if (vec_config_parsed[j][1] == index)//found fg2 index
							{
								edge_tmp.first = j;
								edge_tmp.second = i;
								return_tmp.push_back(edge_tmp);
							}
						}
					}
				}
			}
			//in3 from fg3
			if (vec_config_parsed[i][11] == 5)
			{
				index = vec_config_parsed[i][9];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 3)//found fg3
					{
						if (vec_config_parsed[j][1] == index)//found fg3 index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			//in3 from fg4
			if (vec_config_parsed[i][11] == 6)
			{
				index = vec_config_parsed[i][9];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 4)//found fg4
					{
						if (vec_config_parsed[j][1] == index)//found fg4 index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			//in3 from fg5
			if (vec_config_parsed[i][11] == 7)
			{

			}
		}
		//当前正在处理的是LE
		else if (vec_config_parsed[i][0] == 9)
		{
			//LE输入来源只能是PE
			index = vec_config_parsed[i][2];
			for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
			{
				if (vec_config_parsed[j][0] == 8)//found pe
				{
					if (vec_config_parsed[j][1] == index)//found pe index
					{
						edge_tmp.first = j;
						edge_tmp.second = i;
						return_tmp.push_back(edge_tmp);
					}
				}
			}
		}
		//当前正在处理的是SE
		else if (vec_config_parsed[i][0] == 0)
		{
			//SE输入来源只能是PE
			//addr from
			if (vec_config_parsed[i][4])
			{
				index = vec_config_parsed[i][2];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 8)//found pe
					{
						if (vec_config_parsed[j][1] == index)//found pe index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}

			//data from
			if (vec_config_parsed[i][7])
			{
				index = vec_config_parsed[i][5];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 8)//found pe
					{
						if (vec_config_parsed[j][1] == index)//found pe index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			

		}
		//当前正在处理的是FG1
		else if (vec_config_parsed[i][0] == 1)
		{
			//只有两个输入，一个是来自begin，一个来自fg2的反馈输出，都需要作为入度的边
		}
		//当前正在处理的是FG2
		else if (vec_config_parsed[i][0] == 2)
		{
			//in1 from
			if (vec_config_parsed[i][4] == 1)//in1 from PE
			{
				index = vec_config_parsed[i][2];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 8)// found PE
					{
						if (vec_config_parsed[j][1] == index)//found PE index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			else if (vec_config_parsed[i][4] == 2)//in1 from fg1
			{
			}
			else if (vec_config_parsed[i][4] == 3)//in1 from fg2
			{
				index = vec_config_parsed[i][2];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 2)//found fg2
					{
						if (vec_config_parsed[j][1] == index)//found fg2 index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			//in2 from
			if (vec_config_parsed[i][7] == 1)//in2 from PE
			{
				index = vec_config_parsed[i][5];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 8)//found PE
					{
						if (vec_config_parsed[j][1] == index)//found PE index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			else if (vec_config_parsed[i][7] == 2)//in2 from fg1
			{
			}
			else if (vec_config_parsed[i][7] == 3)//in2 from fg2
			{
				index = vec_config_parsed[i][5];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 2)//found fg2
					{
						if (vec_config_parsed[j][1] == index)//found fg2 index
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			
		}
		//当前正在处理的是FG3
		else if (vec_config_parsed[i][0] == 3)
		{
			//fg3的输入都是来自于PE，且它的输入端口的个数是不定的
		}
		//当前正在处理的是FG4
		else if (vec_config_parsed[i][0] == 4)
		{
			//in1 from
			if (vec_config_parsed[i][4] == 1)//in1 from PE
			{
				index = vec_config_parsed[i][2];
				for (unsigned int j = 0; j < vec_config_parsed.size(); j++)
				{
					if (vec_config_parsed[j][0] == 8)//found PE
					{
						if (vec_config_parsed[j][1] == index)
						{
							edge_tmp.first = j;
							edge_tmp.second = i;
							return_tmp.push_back(edge_tmp);
						}
					}
				}
			}
			else if (vec_config_parsed[i][4] == 2)//in1 from fg1
			{
				//
			}
			else if (vec_config_parsed[i][4] == 3)//in1 from fg2
			{
				//来源于fg2的反馈输入的边忽略
			}
			else if (vec_config_parsed[i][4] == 4)//in1 from fg3
			{
			}
		}
		else if (vec_config_parsed[i][0] == 5)//正在处理的是fg5
		{
		}

		
	}
	return return_tmp;
}




//Config2graph::Config2graph(int v)  //构造函数
//{
//	this->v = v;
//	li = new list<int>[v];
//	indegree = new int[v];
//	for (int i = 0; i < v; i++)
//	{
//		indegree[i] = 0;
//
//	}
//}
//Config2graph::~Config2graph()	//析构函数
//{
//	delete[] li;
//	delete[]indegree;
//}
//
//void Config2graph::addEdge(int v, int w)	//v-->w 的边
//{
//	li[v].push_back(w);
//	indegree[w]++;
//}
//
//
//bool Config2graph::topoSort()
//{
//	for (int i = 0; i < v; i++)
//	{
//		if (indegree[i] == 0)
//			q.push(i);
//	}
//
//	int count = 0;				//记录当前被处理过的顶点数目
//	while (!q.empty())
//	{
//		int u = q.front();
//		q1.push_back(u);
//		q.pop();
//		count++;
//		cout << "当前被处理的顶点是： " << u << "..." << endl;
//		//将所有u指向的顶点的入度减去1，并将入度为0的顶点入栈
//		list<int>::iterator it = li[u].begin();
//		for (; it != li[u].end(); it++)
//		{
//			indegree[*it]--;
//			if (!(indegree[*it]))
//				q.push(*it);
//		}
//		if (count < v)
//			return false;
//		else
//			return true;
//		
//
//	}
//
//}






