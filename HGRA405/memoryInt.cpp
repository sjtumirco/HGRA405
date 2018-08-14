#include <iostream>
#include "memoryInt.h"


MemoryInt::MemoryInt() {}
MemoryInt::MemoryInt(unsigned int depth) 
{
	this->depth_ = depth;
	mem.resize(depth);
	mem_state.resize(depth, 0);
}
MemoryInt::~MemoryInt() {}
int MemoryInt::random_int(unsigned int min, unsigned int max, unsigned int seed = 0)
{
	static std::default_random_engine e(seed);
	static std::uniform_int_distribution<int> u(min, max);
	return u(e);
}


pair<unsigned int, int> MemoryInt::read(unsigned int& addr)
{	//memory出数会有延迟，每来一个地址，先存到FIFO队列中去，判断会不会出数延迟，没有延迟从FIFO中取地址，读出mem中的数
	//有延迟不做任何处理，返回指针为空
	//每一次是有地址才能出数，在遇到延迟之后，本周期的地址在仿存结束后会湮灭，要有一个不断提供地址的机构，
	//这个机构只能放在每一个LE中		
	p.first = 0;
	p.second = 0;
	int dst;
	if (addr < depth_)
	{
		if (isInitialized(addr))
		{
			dst = mem[addr];
			p.first = addr;
			p.second = dst;
			return p;
		}
		else
		{
			std::cerr << "ERROR! Reading uninitialized mem unit!" << std::endl;
			return p;
		}

	}
	else
	{
		std::cerr << "ERROR! Address exceeds the boundary of memory!" << std::endl;
		return p;
	}
	/*int rand = random_int(0, 1);
	cout << "rand is :" << rand << endl;
	if (rand == 0)
	{
		if (addr < depth_)
		{
			if (isInitialized(addr))
			{
				dst = mem[addr];
				p.first = addr;
				p.second = dst;
				return p;
			}
			else
			{
				std::cerr << "ERROR! Reading uninitialized mem unit!" << std::endl;
				return p;
			}

		}
		else
		{
			std::cerr << "ERROR! Address exceeds the boundary of memory!" << std::endl;
			return p;
		}

	}
	else
	{
		std::cerr << "memory出数延迟！" << endl;
		return p;
	}*/

}

void MemoryInt::write(unsigned int& addr, int& dst)
{
	if (addr < depth_)
	{
		mem[addr] = dst;
		setInitialized(addr);
	}
	else
		std::cerr << "ERROR! Address exceeds the boundary of memory!" << std::endl;
}

void MemoryInt::readFromFile(ifstream& infile)		//从文件中读入到memory中去，相当于memory初始化
{
	string temp_string;
	int target;
	unsigned int addr_ = 0;
	while (getline(infile, temp_string))
	{
		stringstream ss(temp_string);
		while (ss >> target)
		{
			mem[addr_] = target;
			mem_state[addr_] = 1;
			addr_++;
		}
	}
}

void MemoryInt::writeToFile(ofstream& outfile)
{
	//unsigned int addr__ = 0;
	int out_tmp;
	for (vector<bool>::size_type addr__ = 0; addr__ < mem_state.size(); addr__++)
	{
		if (mem_state[addr__])
		{
			out_tmp = mem[addr__];
			outfile << out_tmp << "  @" << addr__ << endl;
		}
	}	
}

