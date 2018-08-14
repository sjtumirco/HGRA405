//作用是将符号化的配置文件解析成数字化的配置信息存到文件中去，备用
//分为3个部分，分为PE配置解析、SE\LE配置解析、细粒度部分及TA配置解析
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include "define.h"
typedef unsigned int uint;
using namespace std;
class ConfigParse
{private:
	pair<int,int> pefield1(string& str);		//PE字段1解析[pe_index]
	int pefield2(string& str);					//PE字段2解析[opcode]
	vector<int> pefield3_4_5(string& str);		//PE字段那3,4,5解析[in1_from+in1_port+in1_flag]																							
	int pefield6(string& str);					//PE字段6解析[lr_from]
	int pefield7(string& str);					//PE字段7解析[alu_in1_from]
	int pefield8(string& str);					//PE字段8解析[alu_in2_from]
	int pefield9(string& str);					//PE字段9解析[ob1_from]
	int pefield10(string& str);					//PE字段10解析[ob2_from]
	int pefield11(string& str);					//PE字段11解析[end_node]
	int pefield12(string& str);					//PE字段12解析[pemode]
	bool pefield13(string& str);				//PE字段13解析[tagMode]
	bool pefield14_15_16(string& str);			//PE字段14,15,16解析
	int pefield17(string& str);					//PE字段17解析
	int pefield18(string& str);
	//le
	pair<int,int> lefield1(string& str);		//LE字段1解析[le_index]
	vector<int> lefield2(string& str);			//LE字段2解析[in_from+in_port]
	bool lefield3(string& str);					//LE字段3解析[tagMode]
	//se
	pair<int,int> sefield1(string& str);		//SE字段1解析[category+index]
	vector<int> sefield2(string& str);			//SE字段2解析[se_addr_from]
	vector<int> sefield3(string& str);			//SE字段3解析[se_data_from]
	bool sefield4(string& str);					//LE字段4解析[tagMode]
	//fg
	pair<int, int> fgfield1(string& str);		//FG字段1解析
	vector<int> fgfield2(string& str);
	vector<int> fgfield3(string& str);
	vector<int> fgfieldo(string& str);			//FG其他字段解析，主要针对join的细粒度节点
	bool fgfield4(string& str);					//FG字段4解析[tagMode]
	//ta
	pair<int, int> tafield1(string& str);		//ta字段1解析
	vector<int> tafield2(string& str);			//ta字段2解析

private:
	std::string::size_type get_num_pos(string& str);	//找到字符串中数字字符首次出现的position
	std::string get_string(string& str);

public:
	ConfigParse();
	~ConfigParse();

	vector<vector<string>> vec_config_ori;			//解析之前字符串进入到vector中去
	vector<vector<int>> vec_config_parsed;			//解析之后的配置
	void configFile2vec(ifstream& config_ori);		//从文件中读取配置字符串到vector中
	void configVec2parsed();						//将vector中的配置字符串解析成数字

};
