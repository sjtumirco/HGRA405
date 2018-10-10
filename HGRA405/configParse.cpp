#include "configParse.h"

ConfigParse::ConfigParse() { vec_config_ori.resize(unitTotalNums); 
							vec_config_parsed.resize(unitTotalNums);}
ConfigParse::~ConfigParse() {}

void ConfigParse::configFile2vec(ifstream& config_ori)
{
	const char space = ' ';
	string str_line, str_;
	vector<string> vec_line;
	std::string::size_type pos;
	int i = 0;
	while (getline(config_ori, str_line))
	{		
		pos = str_line.find(space);
		while (pos != string::npos)
		{
			str_ = str_line.substr(0, pos);
			vec_line.push_back(str_);
			str_line = str_line.substr(pos + 1);
			pos = str_line.find(space);
		}
		vec_line.push_back(str_line);
		vec_config_ori[i] = vec_line;		//vec_config_ori里面存储的是原始字符
		vec_line.clear();
		i++;
	}

}

pair<int,int> ConfigParse::pefield1(string& str)		//[category+pe_index]
{
	pair<int,int> pe;
	int num;
	string tmp = str.substr(2);
	stringstream ss(tmp);
	ss >> num;
	pe.first = 8;
	pe.second = num;
	return pe;
}

int ConfigParse::pefield2(string& str)		//[opcode]
{
	if (str == "dor")
		return 0;				//dor----休眠
	else if (str == "add")
		return 1;				//有符号加法
	else if (str == "sub")
		return 3;				//有符号减法
	else if (str == "mul")
		return 2;				//乘法
	else if (str == "div")
		return 4;				//除法
	else if (str == "mod")
		return 5;				//求模
	else if (str == "mac")
		return 9;				//乘累加
	else if (str == "comb")
		return 6;				//比较输出布尔值
	else if (str == "come")
		return 7;				//比较两个数据是否相等
	else if (str == "mux")
		return 8;
	else if (str == "com2i")
		return 100;				//虚拟操作，真实情况不存在
	else if (str == "trans")
		return 11;
	/*else if (str == "shiftl")
		return 10;*/
	else
	{
		//cout << "no opcode matched!" << endl;
	}
}

vector<int> ConfigParse::pefield3_4_5(string& str)			//[in1_from+in1_port+in1_flag]
{
	string head = get_string(str);
	if (head == "pe")//来源自PE
	{
		string::size_type pos;
		string dot_b, dot_a;
		pos = str.find(".");
		vector<int> vec;
		if (pos != string::npos)
		{
			dot_b = str.substr(2, pos - 2);
			dot_a = str.substr(pos + 1);
			stringstream ss1(dot_b);
			stringstream ss2(dot_a);
			int num1, num2;
			ss1 >> num1;
			ss2 >> num2;
			vec.clear();
			vec.push_back(num1);
			vec.push_back(num2 - 1);
			vec.push_back(2);
			return vec;
		}
		else
		{
			//cout << "string field is wrong." << endl;
		}
	}
	else if (head == "le")//来源自LE
	{
		vector<int> vec;
		string tmp;
		//int index;
		tmp = str.substr(2);
		stringstream ss1(tmp);
		int num1;
		ss1 >> num1;
		vec.clear();
		vec.push_back(num1);
		vec.push_back(0);//LE没有端口，使用0来表示默认
		vec.push_back(1);
		return vec;
	}
	else if (head == "fin")//表示悬空,fin
	{
		vector<int> vec;
		vec.clear();
		vec.push_back(0);
		vec.push_back(0);
		vec.push_back(0);
		return vec;
	}
	else if (head == "ta")//来自TA
	{
		vector<int> vec;
		int num;
		string tmp = str.substr(2);
		stringstream ss(tmp);
		ss >> num;
		vec.clear();
		vec.push_back(num);
		vec.push_back(0);//默认是端口1
		vec.push_back(9);
		return vec;
	}
	else if (head == "ti")
	{
		vector<int> vec;
		vec.clear();
		vec.push_back(0);
		vec.push_back(0);//默认是端口1
		vec.push_back(9);//ti的flag是9
		return vec;
	}
	else if (head == "se")
	{
		vector<int> vec;
		string tmp;
		//int index;
		tmp = str.substr(2);
		stringstream ss1(tmp);
		int num1;
		ss1 >> num1;
		vec.clear();
		vec.push_back(num1);
		vec.push_back(0);//SE没有端口，使用0来表示默认端口1
		vec.push_back(10);
		return vec;
	}
	else if(head == "lbegin"||head == "lend"||head == "join"||head == "switch"||head == "break"||head == "lends")//来源于fg
	{
		string::size_type pos1, pos2;
		string dot_b, dot_a, categroy;
		int num1, num2, num3;
		pos2 = str.find(".");
		pos1 = get_num_pos(str);
		vector<int> vec;
		if (pos1 != string::npos && pos2 != string::npos)
		{
			dot_b = str.substr(pos1, pos2 - pos1);
			dot_a = str.substr(pos2 + 1);
			categroy = str.substr(0, pos1);
			stringstream ss1(dot_b);
			stringstream ss2(dot_a);
			if (categroy == "lbegin")
			{
				num3 = 3;
			}
			else if (categroy == "lend")
			{
				num3 = 4;
			}
			else if (categroy == "join")
			{
				num3 = 5;
			}
			else if (categroy == "switch")
			{
				num3 = 6;
			}
			else if (categroy == "break")
			{
				num3 = 7;
			}
			else if (categroy == "lends")
			{
				num3 = 8;
			}			
			ss1 >> num1;
			ss2 >> num2;
			//ss3 >> num3;
			vec.clear();
			vec.push_back(num1);
			vec.push_back(num2 - 1);
			vec.push_back(num3);			
			return vec;
		}
		else
		{
		}
	}
	else if (str == "begin")
	{
		vector<int> vec;
		vec.push_back(0);
		vec.push_back(0);
		vec.push_back(11);
		return vec;
	}
}

int ConfigParse::pefield6(string& str)			//[lr_from]
{
	if (str[0] == 'f' && str[1] == 'l')
		return 0;
	else if (str == "alu")
		return 1;
	else if (str == "in1")
		return 2;
	else if (str == "in2")
		return 3;
	else
		cout << "Config field11 is not in the right range!" << endl;
}

int ConfigParse::pefield7(string& str)			//[alu_in1_from]
{
	if (str == "ib1")
		return 0;
	else if (str == "lr")
		return 1;
	else if (str == "fin")
		return 2;
	else
		cout << "Config field9 is not in the right range!" << endl;
}

int ConfigParse::pefield8(string& str)			//[alu_in2_from]
{
	if (str == "ib2")
		return 0;
	else if (str == "lr")
		return 1;
	else if (str == "fin")
		return 2;
	else
		cout << "Config field10 is not in the right range!" << endl;
}

int ConfigParse::pefield9(string& str)			//[ob1_from]
{
	if (str == "alu")
		return 1;
	else if (str == "lr")
		return 2;
	else if (str[0] == 'f' && str[1] == 'o')
		return 0;
	else if (str == "in1")
		return 3;
	else
		cout << "Config field6 is not in the right range!" << endl;

}

int ConfigParse::pefield10(string& str)			//[ob2_from]
{
	if (str == "alu")
		return 1;
	else if (str == "lr")
		return 2;
	else if (str[0] == 'f' && str[1] == 'o')
		return 0;
	else if (str == "in2")
		return 3;
	else
		cout << "Config field7 is not in the right range!" << endl;
}

int ConfigParse::pefield11(string& str)			//[end_node]
{
	if (str == "nop")
		return 0;
	else if (str == "end")
		return 1;
	else
		cout << "Config field12 is not in the right range!" << endl;


}

int ConfigParse::pefield12(string& str)
{
	if (str == "byp")
		return 0;
	else if (str == "trans")
		return 1;
	else if (str == "cal")
		return 2;
	else if (str == "bac")
		return 3;
	else if (str == "tac")
		return 4;
	else if (str == "bind")
		return 5;
}

bool ConfigParse::pefield13(string& str)
{
	if (str == "nop")
		return 0;
	else if (str == "tag")
		return 1;
	
}

bool ConfigParse::pefield14_15_16(string& str)
{
	if (str == "nop")
		return 0;
	else if (str == "inport")
		return 1;
}

int ConfigParse::pefield17(string& str)
{
	if (str == "ob")
		return 0;
	else if (str == "alu21")
		return 1;
	else if (str == "alu22")
		return 2;
	else if (str == "alu23")
		return 3;

}

int ConfigParse::pefield18(string& str)
{
	if (str == "nop")
		return 0;
	else if (str == "seq")
		return 1;
}

pair<int,int> ConfigParse::lefield1(string& str)		//[category+index]
{
	pair<int, int> tmp;
	string str_ = str.substr(2);
	int data_i;
	stringstream ss(str_);
	ss >> data_i;
	tmp.first = 9;
	tmp.second = data_i;
	return tmp;
}

vector<int> ConfigParse::lefield2(string& str)			//[in_from+in_port]
{
	string::size_type pos;
	string dot_b, dot_a;
	pos = str.find(".");
	vector<int> vec;
	if (pos != string::npos)
	{
		dot_b = str.substr(2, pos - 2);
		dot_a = str.substr(pos + 1);
		stringstream ss1(dot_b);
		stringstream ss2(dot_a);
		int num1, num2;
		ss1 >> num1;
		ss2 >> num2;
		vec.clear();
		vec.push_back(num1);
		vec.push_back(num2 - 1);
	}
	return vec;
}

bool ConfigParse::lefield3(string& str)
{
	if (str == "nop")
		return 0;
	else if (str == "tag")
		return 1;
}

pair<int, int> ConfigParse::sefield1(string& str)		//[category+index]
{
	pair<int, int> tmp;
	string str_ = str.substr(2);
	int data_i;
	stringstream ss(str_);
	ss >> data_i;
	tmp.first = 0;
	tmp.second = data_i;
	return tmp;
}

vector<int> ConfigParse::sefield2(string& str)			//[se_addr_from]
{
	string head = get_string(str);
	if (head == "pe")
	{
		string::size_type pos;
		string dot_b, dot_a;
		pos = str.find(".");
		vector<int> vec;
		if (pos != string::npos)
		{
			dot_b = str.substr(2, pos - 2);
			dot_a = str.substr(pos + 1);
			stringstream ss1(dot_b);
			stringstream ss2(dot_a);
			int num1, num2;
			ss1 >> num1;
			ss2 >> num2;
			vec.clear();
			vec.push_back(num1);
			vec.push_back(num2 - 1);
			vec.push_back(0);
			return vec;
		}
	}
	else if (head == "le")//存在se from le
	{
		vector<int> vec;
		string index = str.substr(2);
		stringstream ss1(index);
		int index_;
		ss1 >> index_;
		vec.clear();
		vec.push_back(index_);
		vec.push_back(0);
		vec.push_back(1);
		return vec;
	}
}

vector<int> ConfigParse::sefield3(string& str)			//[se_data_from]
{
	string head = get_string(str);
	if (head == "pe")
	{
		string::size_type pos;
		string dot_b, dot_a;
		pos = str.find(".");
		vector<int> vec;
		if (pos != string::npos)
		{
			dot_b = str.substr(2, pos - 2);
			dot_a = str.substr(pos + 1);
			stringstream ss1(dot_b);
			stringstream ss2(dot_a);
			int num1, num2;
			ss1 >> num1;
			ss2 >> num2;
			vec.clear();
			vec.push_back(num1);
			vec.push_back(num2 - 1);
			vec.push_back(0);
			return vec;
		}
	}
	else if (head == "le")
	{
		vector<int> vec;
		string index = str.substr(2);
		stringstream ss1(index);
		int index_;
		ss1 >> index_;
		vec.clear();
		vec.push_back(index_);
		vec.push_back(0);
		vec.push_back(1);
		return vec;
	}
}

bool ConfigParse::sefield4(string& str)
{
	if (str == "nop")
		return 0;
	else if (str == "tag")
		return 1;
}

pair<int, int> ConfigParse::fgfield1(string& str)
{
	pair<int, int> out;
	int index;
	int category;
	string::size_type pos = get_num_pos(str);
	string str_tmp1, str_tmp2;
	str_tmp1 = str.substr(0, pos);
	str_tmp2 = str.substr(pos);
	stringstream ss2(str_tmp2);
	ss2 >> index;
	if (str_tmp1 == "lbegin")
		category = 1;
	else if (str_tmp1 == "lend")
		category = 2;
	else if (str_tmp1 == "join")
		category = 3;
	else if (str_tmp1 == "switch")
		category = 4;
	else if (str_tmp1 == "break")
		category = 5;
	else if (str_tmp1 == "lends")
		category = 6;
	else if (str_tmp1 == "joinbp")
		category = 10;

	out.first = category;
	out.second = index;
	return out;	
}

vector<int> ConfigParse::fgfield2(string& str)
{
	vector<int> vec;
	string head = get_string(str);
	if (str == "begin")//该字段是表示来源是begin
	{
		vec.clear();
		vec.push_back(0);
		vec.push_back(0);
		vec.push_back(0);
		return vec;
	}
	if (head == "pe")//该字段是表示来源是PE
	{
		string::size_type pos;
		string dot_b, dot_a;
		pos = str.find(".");

		if (pos != string::npos)
		{
			dot_b = str.substr(2, pos - 2);
			dot_a = str.substr(pos + 1);
			stringstream ss1(dot_b);
			stringstream ss2(dot_a);
			int num1, num2;
			ss1 >> num1;
			ss2 >> num2;
			vec.clear();
			vec.push_back(num1);
			vec.push_back(num2 - 1);
			vec.push_back(1);
			return vec;
		}

	}
	else if (head == "lbegin"||head == "lend"||head == "join"||head == "switch"||head == "break"||head == "lends")//该字段是表示来源是FG
	{
		string::size_type pos1, pos2;
		string dot_b, dot_a, categroy;
		int num1, num2, num3;
		pos2 = str.find(".");
		pos1 = get_num_pos(str);
		vector<int> vec;
		if (pos1 != string::npos && pos2 != string::npos)
		{
			dot_b = str.substr(pos1, pos2 - pos1);
			dot_a = str.substr(pos2 + 1);
			categroy = str.substr(0, pos1);
			stringstream ss1(dot_b);
			stringstream ss2(dot_a);
			if (categroy == "lbegin")
			{
				num3 = 2;
			}
			else if (categroy == "lend")
			{
				num3 = 3;
			}
			else if (categroy == "join")
			{
				num3 = 4;
			}
			else if (categroy == "switch")
			{
				num3 = 5;
			}
			else if (categroy == "break")
			{
				num3 = 6;
			}
			else if (categroy == "lends")
			{
				num3 = 7;
			}
			ss1 >> num1;
			ss2 >> num2;
			vec.clear();
			vec.push_back(num1);
			vec.push_back(num2 - 1);
			vec.push_back(1);
			return vec;
		}
	}
	else if (head == "se")//from se
	{
		string str_ = str.substr(2);
		stringstream ss(str_);
		int index__;
		ss >> index__;
		vec.clear();
		vec.push_back(index__);
		vec.push_back(0);
		vec.push_back(9);
		return vec;
	}
	else
	{
	}
}

vector<int> ConfigParse::fgfield3(string& str)
{
	vector<int> vec;
	string head = get_string(str);
	if (str == "begin")//该字段是表示来源是begin
	{
		vec.clear();
		vec.push_back(0);
		vec.push_back(0);
		vec.push_back(0);
		return vec;
	}
	if (head == "pe")//表示来源是PE
	{
		string::size_type pos;
		string dot_b, dot_a;
		pos = str.find(".");

		if (pos != string::npos)
		{
			dot_b = str.substr(2, pos - 2);
			dot_a = str.substr(pos + 1);
			stringstream ss1(dot_b);
			stringstream ss2(dot_a);
			int num1, num2;
			ss1 >> num1;
			ss2 >> num2;
			vec.clear();
			vec.push_back(num1);
			vec.push_back(num2 - 1);//port要减1
			vec.push_back(1);
			return vec;
		}

	}
	else if (head == "lbegin"||head == "lend"||head == "join"||head == "switch"||head == "break"||head == "lends")//该字段是表示来源是FG
	{
		string::size_type pos1, pos2;
		string dot_b, dot_a, categroy;
		int num1, num2, num3;
		pos2 = str.find(".");
		pos1 = get_num_pos(str);
		vector<int> vec;
		if (pos1 != string::npos && pos2 != string::npos)
		{
			dot_b = str.substr(pos1, pos2 - pos1);
			dot_a = str.substr(pos2 + 1);
			categroy = str.substr(0, pos1);
			stringstream ss1(dot_b);
			stringstream ss2(dot_a);
			if (categroy == "lbegin")
			{
				num3 = 2;
			}
			else if (categroy == "lend")
			{
				num3 = 3;
			}
			else if (categroy == "join")
			{
				num3 = 4;
			}
			else if (categroy == "switch")
			{
				num3 = 5;
			}
			else if (categroy == "break")
			{
				num3 = 6;
			}
			else if (categroy == "lends")
			{
				num3 = 7;
			}
			ss1 >> num1;
			ss2 >> num2;
			vec.clear();
			vec.push_back(num1);
			vec.push_back(num2 - 1);
			vec.push_back(num3);
			return vec;
		}
	}
	else if(head == "se")//from se
	{
		string str_ = str.substr(2);
		stringstream ss(str_);
		int index__;
		ss >> index__;
		vec.clear();
		vec.push_back(index__);
		vec.push_back(0);
		vec.push_back(9);
		return vec;
	}
	else
	{

	}
}

bool ConfigParse::fgfield4(string& str)
{
	if (str == "nop")
		return 0;
	else if (str == "tag")
		return 1;
}

vector<int> ConfigParse::fgfieldo(string& str)
{
	vector<int> vec;
	string head = get_string(str);
	if (str == "begin")//该字段是表示来源是begin
	{
		vec.clear();
		vec.push_back(0);
		vec.push_back(0);
		vec.push_back(0);
		return vec;
	}
	if (head == "pe")//表示来源是PE
	{
		string::size_type pos;
		string dot_b, dot_a;
		pos = str.find(".");

		if (pos != string::npos)
		{
			dot_b = str.substr(2, pos - 2);
			dot_a = str.substr(pos + 1);
			stringstream ss1(dot_b);
			stringstream ss2(dot_a);
			int num1, num2;
			ss1 >> num1;
			ss2 >> num2;
			vec.clear();
			vec.push_back(num1);
			vec.push_back(num2 - 1);//port要减1
			vec.push_back(1);
			return vec;
		}

	}
	//else if (str[0] == 'f')//表示来源是FG
	//{
	//	const char dot = '.';
	//	const char _line = '_';
	//	int aa, bb, cc;
	//	string str_tmp1, str_tmp2, str_tmp3;
	//	string::size_type pos_dot, pos_;
	//	pos_dot = str.find(dot);
	//	pos_ = str.find(_line);
	//	if (pos_dot != string::npos && pos_ != string::npos)
	//	{
	//		str_tmp1 = str.substr(pos_dot + 1, pos_ - pos_dot - 1);//下划线之前dot之后
	//		str_tmp2 = str.substr(pos_ + 1);//下划线之后
	//		str_tmp3 = str.substr(2, pos_dot - 2);
	//		stringstream ss1(str_tmp1); stringstream ss2(str_tmp2); stringstream ss3(str_tmp3);
	//		ss1 >> aa; ss2 >> bb; ss3 >> cc;
	//		vec.push_back(aa);
	//		vec.push_back(bb - 1);
	//		vec.push_back(cc + 1);
	//		return vec;
	//	}
	//}
	else if (head == "se")//from se
	{
		string str_ = str.substr(2);
		stringstream ss(str_);
		int index__;
		ss >> index__;
		vec.clear();
		vec.push_back(index__);
		vec.push_back(0);
		vec.push_back(9);
		return vec;
	}
	else
	{

	}
}
//字段整体解析
void ConfigParse::configVec2parsed()
{
	string head_;
	for (unsigned int it1 = 0; it1 < vec_config_ori.size(); it1++)
	{
		head_ = get_string(vec_config_ori[it1][0]);
		//PE的解析
		if (head_ == "pe")
		{
			pair<int,int> p_return;
			int f_return;
			bool b_return;
			vector<int> v_return1, v_return2, v_return3;
			//字段1
			p_return = pefield1(vec_config_ori[it1][0]);
			vec_config_parsed[it1].push_back(p_return.first);		//vec_config_parsed里面存储 的都是一行行的数字化的信息
			vec_config_parsed[it1].push_back(p_return.second);	
			//字段2
			f_return = pefield2(vec_config_ori[it1][1]);
			vec_config_parsed[it1].push_back(f_return);
			//字段3
			v_return1 = pefield3_4_5(vec_config_ori[it1][2]);
			vec_config_parsed[it1].push_back(v_return1[0]);
			vec_config_parsed[it1].push_back(v_return1[1]);
			vec_config_parsed[it1].push_back(v_return1[2]);
			v_return1.clear();
			//字段4
			v_return2 = pefield3_4_5(vec_config_ori[it1][3]);
			vec_config_parsed[it1].push_back(v_return2[0]);
			vec_config_parsed[it1].push_back(v_return2[1]);
			vec_config_parsed[it1].push_back(v_return2[2]);
			v_return2.clear();
			//字段5
			v_return3 = pefield3_4_5(vec_config_ori[it1][4]);
			vec_config_parsed[it1].push_back(v_return3[0]);
			vec_config_parsed[it1].push_back(v_return3[1]);
			vec_config_parsed[it1].push_back(v_return3[2]);
			v_return3.clear();
			//字段6
			f_return = pefield6(vec_config_ori[it1][5]);
			vec_config_parsed[it1].push_back(f_return);
			//字段7
			f_return = pefield7(vec_config_ori[it1][6]);
			vec_config_parsed[it1].push_back(f_return);
			//字段8
			f_return = pefield8(vec_config_ori[it1][7]);
			vec_config_parsed[it1].push_back(f_return);
			//字段9
			f_return = pefield9(vec_config_ori[it1][8]);
			vec_config_parsed[it1].push_back(f_return);
			//字段10
			f_return = pefield10(vec_config_ori[it1][9]);
			vec_config_parsed[it1].push_back(f_return);
			//字段11
			f_return = pefield11(vec_config_ori[it1][10]);
			vec_config_parsed[it1].push_back(f_return);
			//字段12
			f_return = pefield12(vec_config_ori[it1][11]);
			vec_config_parsed[it1].push_back(f_return);
			//字段13
			b_return = pefield13(vec_config_ori[it1][12]);
			vec_config_parsed[it1].push_back(b_return);
			//字段14
			b_return = pefield14_15_16(vec_config_ori[it1][13]);
			vec_config_parsed[it1].push_back(b_return);
			//字段15
			b_return = pefield14_15_16(vec_config_ori[it1][14]);
			vec_config_parsed[it1].push_back(b_return);
			//字段16
			b_return = pefield14_15_16(vec_config_ori[it1][15]);
			vec_config_parsed[it1].push_back(b_return);
			//字段17
			f_return = pefield17(vec_config_ori[it1][16]);
			vec_config_parsed[it1].push_back(f_return);
			//字段18
			f_return = pefield18(vec_config_ori[it1][17]);
			vec_config_parsed[it1].push_back(f_return);
		}
		//le字段解析
		else if (head_ == "le")
		{
			vector<int> v_tmp_tmp;
			pair<int, int> p_return = lefield1(vec_config_ori[it1][0]);
			vec_config_parsed[it1].push_back(p_return.first);
			vec_config_parsed[it1].push_back(p_return.second);
			v_tmp_tmp = lefield2(vec_config_ori[it1][1]);
			vec_config_parsed[it1].push_back(v_tmp_tmp[0]);
			vec_config_parsed[it1].push_back(v_tmp_tmp[1]);
			v_tmp_tmp.clear();
			vec_config_parsed[it1].push_back(lefield3(vec_config_ori[it1][2]));
		}
		//se字段解析
		else if (head_ == "se")
		{
			vector<int> v_tmp_tmp;
			pair<int, int> p_return = sefield1(vec_config_ori[it1][0]);
			vec_config_parsed[it1].push_back(p_return.first);
			vec_config_parsed[it1].push_back(p_return.second);
			v_tmp_tmp = sefield2(vec_config_ori[it1][1]);
			vec_config_parsed[it1].push_back(v_tmp_tmp[0]);
			vec_config_parsed[it1].push_back(v_tmp_tmp[1]);
			vec_config_parsed[it1].push_back(v_tmp_tmp[2]);
			v_tmp_tmp.clear();
			v_tmp_tmp = sefield3(vec_config_ori[it1][2]);
			vec_config_parsed[it1].push_back(v_tmp_tmp[0]);
			vec_config_parsed[it1].push_back(v_tmp_tmp[1]);
			vec_config_parsed[it1].push_back(v_tmp_tmp[2]);
			v_tmp_tmp.clear();
			vec_config_parsed[it1].push_back(sefield4(vec_config_ori[it1][3]));

		}
		//fg字段解析
		else if (head_ == "lbegin"||head_ == "lend"||head_ == "join"||head_ == "switch"||head_ == "break"||head_ == "lends" || head_ == "joinbp")
		{
			pair<int, int> return_tmp;
			vector<int> f_return1, f_return2, f_return;
			return_tmp = fgfield1(vec_config_ori[it1][0]);
			vec_config_parsed[it1].push_back(return_tmp.first);
			vec_config_parsed[it1].push_back(return_tmp.second);
			if (vec_config_ori[it1].size() == 3)//只有三个字段，减去两个无用字段(category+index and tagmode)，表示该细粒度单元只有一个输入
			{
				f_return1.clear();
				f_return1 = fgfield2(vec_config_ori[it1][1]);
				vec_config_parsed[it1].push_back(f_return1[0]);
				vec_config_parsed[it1].push_back(f_return1[1]);
				vec_config_parsed[it1].push_back(f_return1[2]);
			}
			else if (vec_config_ori[it1].size() == 4)//有三个字段，表示该细粒度单元有两个输入
			{
				f_return1.clear();
				f_return1 = fgfield2(vec_config_ori[it1][1]);
				vec_config_parsed[it1].push_back(f_return1[0]);
				vec_config_parsed[it1].push_back(f_return1[1]);
				vec_config_parsed[it1].push_back(f_return1[2]);
				f_return2.clear();
				f_return2 = fgfield3(vec_config_ori[it1][2]);
				vec_config_parsed[it1].push_back(f_return2[0]);
				vec_config_parsed[it1].push_back(f_return2[1]);
				vec_config_parsed[it1].push_back(f_return2[2]);
			}
			else
			{
				//不止三个字段，针对join节点不定输入的情况
				int port_size = vec_config_ori[it1].size() - 2;
				for (int i = 0; i < port_size; i++)
				{
					f_return.clear();
					f_return = fgfieldo(vec_config_ori[it1][i+1]);
					vec_config_parsed[it1].push_back(f_return[0]);
					vec_config_parsed[it1].push_back(f_return[1]);
					vec_config_parsed[it1].push_back(f_return[2]);
					f_return.clear();
				}
			}
			vec_config_parsed[it1].push_back(fgfield4(vec_config_ori[it1].back()));
		}
		//ta字段解析
		else if(head_ == "ta")
		{
			pair<int, int> return_tmp;
			vector<int> v_return;
			return_tmp = tafield1(vec_config_ori[it1][0]);
			vec_config_parsed[it1].push_back(return_tmp.first);
			vec_config_parsed[it1].push_back(return_tmp.second);
			v_return = tafield2(vec_config_ori[it1][1]);
			vec_config_parsed[it1].push_back(v_return[0]);
			vec_config_parsed[it1].push_back(v_return[1]);
			vec_config_parsed[it1].push_back(v_return[2]);
			v_return.clear();

		}
	}

}

std::string::size_type ConfigParse::get_num_pos(string& str)
{
	stringstream ss(str);
	char c_tmp;
	uint index = 0;
	while (ss >> c_tmp)
	{
		if (c_tmp <= '9' && c_tmp >= '0')
		{
			return (std::string::size_type)index;			
		}
		index++;
	}

}

pair<int, int> ConfigParse::tafield1(string& str)
{
	pair<int, int> tmp;
	int num;
	string str_num = str.substr(2);
	stringstream ss(str_num);
	ss >> num;
	tmp.first = 7;
	tmp.second = num;
	return tmp;
	
}
vector<int> ConfigParse::tafield2(string& str)
{
	string head = get_string(str);
	if (head == "pe")//来源自PE
	{
		string::size_type pos;
		string dot_b, dot_a;
		pos = str.find(".");
		vector<int> vec;
		if (pos != string::npos)
		{
			dot_b = str.substr(2, pos - 2);
			dot_a = str.substr(pos + 1);
			stringstream ss1(dot_b);
			stringstream ss2(dot_a);
			int num1, num2;
			ss1 >> num1;
			ss2 >> num2;
			vec.clear();
			vec.push_back(num1);
			vec.push_back(num2 - 1);
			vec.push_back(1);
			return vec;
		}
		
	}
	else if (head == "le")//来源自LE
	{
		vector<int> vec;
		string tmp;
		//int index;
		tmp = str.substr(2);
		stringstream ss1(tmp);
		int num1;
		ss1 >> num1;
		vec.clear();
		vec.push_back(num1);
		vec.push_back(0);//LE没有端口，使用0来表示默认
		vec.push_back(10);
		return vec;
	}
	//else if (str[0] == 'f' && str[1] == 'i')//表示悬空,fin
	//{
	//	vector<int> vec;
	//	vec.clear();
	//	vec.push_back(0);
	//	vec.push_back(0);
	//	vec.push_back(0);
	//	return vec;
	//}
	else if (head == "lbegin"||head == "lend"||head == "join"||head == "switch"||head == "break"||head == "lends")//来源于fg
	{
		string::size_type pos1, pos2;
		string dot_b, dot_a, categroy;
		int num1, num2, num3;
		pos2 = str.find(".");
		pos1 = get_num_pos(str);
		vector<int> vec;
		if (pos1 != string::npos && pos2 != string::npos)
		{
			dot_b = str.substr(pos1, pos2 - pos1);
			dot_a = str.substr(pos2 + 1);
			categroy = str.substr(0, pos1);
			stringstream ss1(dot_b);
			stringstream ss2(dot_a);
			if (categroy == "lbegin")
			{
				num3 = 2;
			}
			else if (categroy == "lend")
			{
				num3 = 3;
			}
			else if (categroy == "join")
			{
				num3 = 4;
			}
			else if (categroy == "switch")
			{
				num3 = 5;
			}
			else if (categroy == "break")
			{
				num3 = 6;
			}
			else if (categroy == "lends")
			{
				num3 = 7;
			}
			ss1 >> num1;
			ss2 >> num2;
			//ss3 >> num3;
			vec.clear();
			vec.push_back(num1);
			vec.push_back(num2 - 1);
			vec.push_back(num3);
			return vec;
		}
		else
		{
		}
	}
}

std::string ConfigParse::get_string(string& str)
{
	auto pos = get_num_pos(str);
	return str.substr(0, pos);
}
