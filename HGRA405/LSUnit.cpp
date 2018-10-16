#include "LSUnit.h"                                        
#include <vector>

#define LE_NUM    6  
#define SE_NUM    1
#define OUT_NUM   2  
#define TABLELINE_NUM  8

using namespace DRAMSim;

TabLine::TabLine()
{
	//tabTAG_ = tabtag;
	TAG_ = 0;
	ADDR_ = 0;
	rdwr = 0;
	valid = 0;
	counter = 0;
	poped = 0;
	complete = 0;
	cachecycle = 0;
	cachecount = 0;
}



void TabLine::reset()
{
	TAG_ = 0;
	ADDR_ = 0;
	rdwr = 0;
	valid = 0;
	counter = 0;
	poped = 0;
	complete = 0;
	cachecycle = 0;
	cachecount = 0;
}


ArbitratorLine::ArbitratorLine(HGRA::Load* le, HGRA::Store* se, uint32_t TAG)
{
	le_ = le;
	se_ = se;
	TAG_ = TAG;
}


bool ArbitratorLine::AddTrans(uint32_t ADDR, uint32_t V)
{
	valid = V;
	ADDR_ = ADDR;
	return true;
}


void ArbitratorLine::returnACK()
{
	if (TAG_ < LE_NUM)
		le_->callbackACK();    //pay attention to this name
	else if (TAG_ >= LE_NUM && TAG_ < (LE_NUM + SE_NUM))
		se_->callbackACK();
	else
		cout << "error occurs in ArbitratorLine::returnACK()" << endl;
}


Arbitrator::Arbitrator(HGRA::Load** lep, HGRA::Store** sep, uint32_t out_num)                 //out_num unused
{
	lsize = LE_NUM;                                    //as a pointer the return value of sizeof() is fixed as 8
	ssize = SE_NUM;
	pointer = 0;

	if (lsize != LE_NUM || ssize != SE_NUM)
	{
		cout << "error occurs for LE/SE size mismatch!" << "the size of lep is " << lsize << " the size of sep is ssize" << ssize << endl;
		exit(-1);
	}

	for (uint32_t i = 0; i < lsize; i++)
	{
		ArbitratorLine *arbline = new ArbitratorLine(lep[i], NULL, i);                       //隐含了ArbitratorLines的下标即为TAG的意思
		ArbitratorLines.push_back(arbline);
	}

	for (uint32_t i = 0; i < ssize; i++)
	{
		ArbitratorLine *arbline = new ArbitratorLine(NULL, sep[i], i + lsize);
		ArbitratorLines.push_back(arbline);
	}
}


Arbitrator::~Arbitrator()
{
	for (uint32_t i = 0; i < lsize; i++)
	{
		delete ArbitratorLines[i];
	}

	for (uint32_t i = 0; i < ssize; i++)
	{
		delete ArbitratorLines[lsize + i];
	}
}

//利用TAG寻找相应进行动作的tabline
bool Arbitrator::AddTrans(uint32_t ADDR, uint32_t TAG, uint32_t V)
{
	if ((TAG >= lsize + ssize))
	{
		cout << "error occurs in Arbitrator:: AddTrans, the tag is " << TAG << endl;
		return 0;
	}
	else
		return ArbitratorLines[TAG]->AddTrans(ADDR, V);
}



LSUnit::LSUnit(HGRA::Load** lep, HGRA::Store** sep)
{
	ClockCycle = 0;
	finished_trans = 0;
	total_trans = 0;
	finished_rd = 0;
	finished_wr = 0;
	total_wr = 0;
	total_rd = 0;
	hit = 0;
	//missvalid = 0;
	//missrdwr = false;
	//missaddr = 0;


	Arbitrator *arb = new Arbitrator(lep, sep, OUT_NUM);
	arbitrator = arb;

	cache = new Cache();
	cache->attach(this);

	for (uint32_t i = 0; i < TABLELINE_NUM; i++)      //table的例化
	{
		//table[i] = new TabLine();
		TabLine *tabline = new TabLine();
		table.push_back(tabline);
	}

}


LSUnit::~LSUnit()
{
	//delete arbitrator;
	//delete cache;
	//for (uint32_t i = 0; i < TABLELINE_NUM; i++)      //table的例化
	//{
	//	delete table[i];
	//}
}


bool LSUnit::AddTrans(uint32_t ADDR, uint32_t TAG, uint32_t V)
{
	return arbitrator->AddTrans(ADDR, TAG, V);
}


void LSUnit::AttachMem(MultiChannelMemorySystem *memory)
{
	mem = memory;
}


void LSUnit::update()   //从后往前，并加入timing信息
{
	ClockCycle++;
	mem->update();
	std::cout << "update!" << " " << ClockCycle << std::endl;
	for (uint32_t i = 0; i < TABLELINE_NUM; i++)
		cout << "table " << i  << " addr "<< "=" << table[i]->ADDR_ << "  cachecycle and cachecount is "<< table[i]->cachecycle << " " << table[i]->cachecount << endl;
	//cout << "still alive1" << endl;

	for (uint32_t i = 0; i < TABLELINE_NUM; i++)                              //第一步，将已完成的读写反馈
	{
		if (table[i]->complete == 1)
		{
			if (table[i]->rdwr == 1)
			{
				if (table[i]->cachecount == table[i]->cachecycle)
				{
					uint32_t temp = table[i]->TAG_;
					if (temp < NUM_LE || temp >= (NUM_LE + NUM_SE))
						cout << "error occurs in LSUnit::update() for callback missmatch!" << endl;

					(arbitrator->ArbitratorLines[temp]->se_)->SEcallback(table[i]->ADDR_, ClockCycle);    //是否需要调用上层se的callback   
					finished_wr++;
					finished_trans++;
					table[i]->reset();
				}
				else if (table[i]->cachecount < table[i]->cachecycle)
					table[i]->cachecount++;
				else if (table[i]->cachecount > table[i]->cachecycle)
					cout << "error occurs in cachecount > cachecycle!" << endl;
			}
			else if (table[i]->rdwr == 0)
			{
				if (table[i]->cachecount == table[i]->cachecycle)
				{
					uint32_t temp = table[i]->TAG_;
					if (temp >= NUM_LE)
						cout << "error occurs in LSUnit::update() for callback missmatch!" << endl;

					//cout << "still alive1.5 temp and i are"<< temp << " " << i << endl;
					(arbitrator->ArbitratorLines[temp]->le_)->LEcallback(table[i]->ADDR_, ClockCycle);    //temp means LE tag and i means tab rank
					//cout << "still alive1.75" << endl;
					finished_rd++;
					finished_trans++;
					table[i]->reset();
				}
				else if (table[i]->cachecount < table[i]->cachecycle)
					table[i]->cachecount++;
				else if (table[i]->cachecount > table[i]->cachecycle)
					cout << "error occurs in cachecount > cachecycle!" << endl;
			}
			else
				cout << "error occurs in LSUnit::update() in rdwr" << endl;
		}
	}
	//cout << "still alive2" << endl;

		 //this part here finished by callback function called by mem or cache(to set complete to 1)

	//the trascation missed in cache is accessing mem(may be need a buffer to avoid that membuffer is full)     
	//if(missvalid)     
	//{
	   //  mem->addTransaction(missrdwr, missaddr); 
	   //  missvalid = 0;
	//}


  //from table to cache/mem
	int maxcount = -1;           //can't use uint32_t because the ini is -1
	int maxtabline = -1;
	for (int i = 0; i < TABLELINE_NUM; i++)
	{
		if (table[i]->valid == 1 && table[i]->poped == 0)
		{
			if (table[i]->counter > maxcount)
			{
				maxcount = table[i]->counter;
				maxtabline = i;
			}
			(table[i]->counter)++;
		}
	}
	//cout << "still alive2.5" <<"maxcount and maxtabline are "<< maxcount << maxtabline << endl;

	if (maxtabline == -1)
		cout << "no tabline poped in this cycle!" << endl;
	else if (maxtabline < TABLELINE_NUM)
	{
		if (table[maxtabline]->rdwr == 0)
		{
			if (table[maxtabline]->cachecycle = cache->addtranscation(table[maxtabline]->ADDR_, table[maxtabline]->rdwr))
			{
				cout << "RD transcation poped to cache from table "<< maxtabline << endl;
				table[maxtabline]->poped = 1;
			}
			else
				cout << "LSU:cache busy!" << endl;

		}
		else if (table[maxtabline]->rdwr == 1)
		{
			if (table[maxtabline]->cachecycle = cache->addtranscation(table[maxtabline]->ADDR_, table[maxtabline]->rdwr))
			{
				cout << "WR transcation poped to cache from table " << maxtabline << endl;
				table[maxtabline]->poped = 1;
			}
			else
				cout << "LSU:Cache busy!" << endl;
		}
		else
			cout << "error occurs in step from table to mem" << endl;


		//若hit则complete置1
		if (hit == 1)
		{
			table[maxtabline]->complete = 1;
			cache->state = 0;	
		}
		else if (hit == 0)
		{
		    if (table[maxtabline]->rdwr == 0 && table[maxtabline]->poped == 1)
			{
				mem->addTransaction(false, uint64_t(table[maxtabline]->ADDR_));
				cout << "RD transcation miss and send to mem!" << endl;
			}
			else if (table[maxtabline]->rdwr == 1 && table[maxtabline]->poped == 1)
			{
				mem->addTransaction(true, uint64_t(table[maxtabline]->ADDR_));
				cout << "WR transcation miss and send to mem!" << endl;
			}
			
		}
		hit = 0;
	}
	//cout << "maxtabline is " << maxtabline << endl;
	//cout << "still alive3" << endl;

	//from arbitrator to table	 
	uint32_t cyclepointer = arbitrator->pointer;
	//round cycle to find the valid arbitrator
	while (arbitrator->ArbitratorLines[arbitrator->pointer]->valid == 0)
	{
		if (arbitrator->pointer < (LE_NUM + SE_NUM - 1))
			arbitrator->pointer++;
		else if (arbitrator->pointer == (LE_NUM + SE_NUM - 1))
			arbitrator->pointer = 0;

		if (arbitrator->pointer == cyclepointer)
		{
			cout << "no trans moved to table from ArbitratorLine" << endl;
			break;
		}
	}

	uint32_t i;
	if (arbitrator->ArbitratorLines[arbitrator->pointer]->valid == 1)
	{
		for (i = 0; i < TABLELINE_NUM; i++)
		{
			if (table[i]->valid == 0) //find blank line
			{
				table[i]->ADDR_ = arbitrator->ArbitratorLines[arbitrator->pointer]->ADDR_;
				//table[i].DATA = arbitrator->ArbitratorLines[arbitrator->pointer]->data_;
				table[i]->rdwr = arbitrator->pointer < LE_NUM ? 0 : 1;
				table[i]->TAG_ = arbitrator->pointer;
				table[i]->valid = 1;
				table[i]->counter = 0;
				table[i]->poped = 0;
				table[i]->complete = 0;

				arbitrator->ArbitratorLines[arbitrator->pointer]->returnACK();     //take the data and return ACK to LE/SE
				arbitrator->ArbitratorLines[arbitrator->pointer]->valid = 0;

				cout << "trans send from arbitratorline " << arbitrator->pointer << " to table " << i << endl;

				if (table[i]->TAG_ < LE_NUM)
				{
					total_rd++;
					total_trans++;
				}
				else if (table[i]->TAG_ >= LE_NUM && table[i]->TAG_ < (LE_NUM + SE_NUM))
				{
					total_wr++;
					total_trans++;
				}
				else
					cout << "error occurs in table[i]->TAG_ overflow!" << endl;

				//cout << "still alive5" << endl;

				if (arbitrator->pointer < (LE_NUM + SE_NUM - 1))
					arbitrator->pointer++;
				else if (arbitrator->pointer == (LE_NUM + SE_NUM - 1))
					arbitrator->pointer = 0;

				break;
			}
		}
		if (i == TABLELINE_NUM - 1 && table[i]->valid == 1)
			cout << "no tabline blank!" << endl;

	}
	cout << "----------" << ClockCycle << "CYCLE END----------" << endl;
	cout << endl;
}


void LSUnit::mem_read_complete(unsigned id, uint64_t address, uint64_t clock_cycle)
{
	//uint32_t j;
	//printf("[Callback] read complete from mem: %d 0x%llx cycle=%llu\n", id, address, clock_cycle);
	std::cout << "[Callback] read complete from mem: " << id << " " << address << " " << clock_cycle << endl;
	uint32_t temp1 = 0;
	for (uint32_t i = 0; i < TABLELINE_NUM; i++)
	{
		if (uint32_t(address) == table[i]->ADDR_ && table[i]->rdwr == 0 && table[i]->valid == 1 && table[i]->poped == 1 && table[i]->complete == 0)
		{
			table[i]->complete = 1;
			temp1++;
			cache->state = 0;
		}

	}

	if (temp1 < 1)
	{
		cout << "error occurs in LSUnit::mem_read_complete() for no tableline finished when mem rdcallback " << endl;
		cout << "returned addr is " << address << endl;
	}
	else if (temp1 > 1)
		cout << "error occurs in LSUnit::mem_read_complete() for more than one tableline finished when mem rdcallback " << endl;
}


void LSUnit::mem_write_complete(unsigned id, uint64_t address, uint64_t clock_cycle)
{
	//uint32_t j;
	printf("[Callback] write complete from mem: %d 0x%llx cycle=%llu\n", id, address, clock_cycle);

	uint32_t temp = 0;
	for (uint32_t i = 0; i < TABLELINE_NUM; i++)
	{
		if (uint32_t(address) == table[i]->ADDR_ && table[i]->rdwr == 1 && table[i]->valid == 1 && table[i]->poped == 1 && table[i]->complete == 0)
		{
			table[i]->complete = 1;
			temp++;
			cache->state = 0;
		}
		else
		{
			printf("cache write_back finished!\n");
		}
	}

	if (temp < 1)
		cout << "error occurs in LSUnit::mem_write_complete() for no tableline finished when mem wrcallback " << endl;
	else if (temp > 1)
		cout << "error occurs in LSUnit::mem_write_complete() for more than one tableline finished when mem wrcallback " << endl;
}


//void LSUnit::cache_read_complete(unsigned id, uint64_t_t address, uint64_t_t clock_cycle)	
//{
//	
//}


//void LSUnit::cache_write_complete(unsigned id, uint64_t_t address, uint64_t_t clock_cycle)	
//{
//	
//}


//void LSUnit::cachemiss(uint64_t_t address, bool rdwr)
//{
//	 missvalid = 1;
//	 missaddr = address;
//	 missrdwr = rdwr;
//}

