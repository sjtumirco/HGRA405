#include "iostream"                        
#include "stdlib.h"                       
#include "math.h"                         //pay attention to ACK
#include <vector>
#include <map>
#include "MultiChannelMemorySystem.h"
#include "Cache.h"
#include "load.h" 
#include "store.h"
#define NUM_LE 6                         
#define NUM_SE 1                       


typedef     unsigned char       UINT8;
typedef     unsigned short      UINT16;
//typedef     unsigned int        uint32_t;
//typedef     unsigned long       uint64_t;     

using std::vector;
//using std::string;
namespace HGRA
{
	class Load;
	class Store;
}

namespace DRAMSim{
                                        
class TabLine   
{
private:
	 
	 //LE *le_;         

	 //void* DATA;

public:
	 uint8_t counter;
	 uint32_t valid;
	 uint32_t poped;
	 uint32_t rdwr;
	 TabLine();
	 ~TabLine(){};
	 UINT8 complete;
	 uint32_t ADDR_;
	 uint32_t TAG_;    
	 uint32_t cachecycle;
	 uint32_t cachecount;
	 
	 //uint32_t ReplaceWay;
	 //uint32_t lineIndex;
	 //uint32_t lineTag;
	 //bool AddTransLE(uint32_t ADDR, uint32_t V, uint32_t TAG);   
	 //bool AddTrans(uint32_t ADDR, uint32_t TAG, uint32_t V);             
	 void reset();
	 //void counterup();
	 //void pop();
	 //void readcompelete();    
	 //void writecompelete(); 
	 //void update();    
};


class ArbitratorLine
{
private:
   //void*data_; 
   uint32_t TAG_; 
   //uint32_t counter;
    
public:
     HGRA::Load* le_;   
     HGRA::Store* se_;
     uint32_t ADDR_;
	 ArbitratorLine(HGRA::Load* le, HGRA::Store* se, uint32_t TAG);
	 ~ArbitratorLine(){};
	 uint32_t valid;
	 //bool AddTransLE(uint64_t ADDR, uint32_t TAG, uint32_t V);
	 //bool AddTransSE(uint64_t ADDR, void*data, uint32_t TAG, uint32_t V);
	 bool AddTrans(uint32_t ADDR, uint32_t V);
	 void returnACK();
};


class Arbitrator
{
private:
   uint32_t lsize;
   uint32_t ssize;
   
public:	 
     uint32_t pointer;
     vector<ArbitratorLine *> ArbitratorLines;
	 Arbitrator(HGRA::Load** lep, HGRA::Store** sep, uint32_t out_num);
	 ~Arbitrator();
	 //bool AddTransLE(uint64_t ADDR, uint32_t TAG, uint32_t V);   
	 //bool AddTransSE(uint64_t ADDR, void*data, uint32_t TAG, uint32_t V);
	 bool AddTrans(uint32_t ADDR, uint32_t TAG, uint32_t V);  
	 //void update();
};


class LSUnit         
{
private:
    
	 //vector[Trans*] transactionbuffer;                 
	 //vector[uint32_t] pendingbuffer;     
	 Arbitrator* arbitrator;
	 uint32_t completetran;
	 uint64_t ClockCycle;
	 //uint64_t missaddr;                 
	 //bool missrdwr;
	 //uint32_t missvalid; 
     Cache *cache;	
     //uint32_t cachecycle;	 
	 //MultiChannelMemorySystem *mem
	 //cache
   
public:
	 LSUnit(HGRA::Load** lep, HGRA::Store** sep);//在构造函数中的Load**和Store**并没有使用
	 ~LSUnit();
	 //bool AddTransLE(uint64_t ADDR, uint32_t TAG, uint32_t V);   
	 //bool AddTransSE(uint64_t ADDR, void*data, uint32_t TAG, uint32_t V);
	 //void readcomplete();  
	 //void writecomplete();     
	 bool AddTrans(uint32_t ADDR, uint32_t TAG, uint32_t V);
	 void AttachMem(MultiChannelMemorySystem *memory);
	 void update();     //synchronic
	 void mem_read_complete(unsigned id, uint64_t address, uint64_t clock_cycle);      //this and below this are callback functions
	 void mem_write_complete(unsigned id, uint64_t address, uint64_t clock_cycle);
	 //void cache_read_complete(unsigned id, uint64_t address, uint64_t clock_cycle)��     //to be finished about cache!
	 //void cache_write_complete(unsigned id, uint64_t address, uint64_t clock_cycle)��
	 //void cachemiss(uint64_t address, uint32_t rdwr); 
	 MultiChannelMemorySystem *mem; 
	 vector<TabLine *> table;
	 uint32_t finished_trans;
	 uint32_t total_trans;
	 uint32_t finished_rd;
	 uint32_t finished_wr;
	 uint32_t total_wr;
	 uint32_t total_rd;  
     uint32_t hit;
	 //uint32_t lsize;
	 //uint32_t ssize;
	 
};
}


