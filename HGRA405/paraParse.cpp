#include <iostream>
#include "paraParse.h"

ParaParse::ParaParse() {}
ParaParse::~ParaParse() {}

void ParaParse::parsePara(ifstream& infile)
{
	string ss_line;
	int string2int;
	vector<int> vec_tmp;
	while (getline(infile, ss_line))
	{
		stringstream ss(ss_line);
		while (ss >> string2int)
		{
			vec_tmp.push_back(string2int);
		}
		para_parsed.push_back(vec_tmp);
		vec_tmp.clear();
	}
}
