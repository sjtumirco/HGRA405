//封装在类内的cache，无数据
#ifndef CACHE
#define CACHE
#include <stdint.h>
#include <iostream>

namespace DRAMSim
{
	//可配置工作模式    cache分为v个组，每组k路,v*k=m
	//#define CACHE_MODE      1        //1 2 3分表组关联、全关联、直接映射  配置，v=m,ways=1为直接映射，v=1,ways=m为关联映射，在中间的为组关联
#define WRITE_MODE      1        //0 1 分别表示write_through 和 write_back，与write_back次数有关，若有write_back延时则有影响
#define REPLACE_MODE    0        //0 1 2分别表示随机替换、最近最少使用（每组ways越多，硬件代价越大）、FIFO， 影响hit rate,   the function of 1 2 have not been tested!!
//#define FIFO_DEPTH      5
#define DATA_CACHE_SIZE         32*1024     // 32KB
#define DATA_LINE_SIZE          8           // 一块几个字
#define CACHE_WAYS_NUM          4
#define CACHE_LINE_EACH_WAY     (DATA_CACHE_SIZE/(DATA_LINE_SIZE*CACHE_WAYS_NUM*sizeof(uint32_t)))   //sizeof表示字节数  组数256


//延时定义
#define LOOKUP_DELAY            1
#define REPLACE_DELAY           2
#define WRITEBACK_DELAY         0     //并行？
#define WRITETHROUGH_DELAY      0
#define READHIT_DELAY           1
#define WRITEHIT_DELAY          1
//#define READ_DELAY           1
//#define WRITE_DELAY          1


// cacheline bit masks
#define     CACHE_LINE_SIZE         (DATA_LINE_SIZE)      // 一个cache行的大小 words
#define     UPDATE_BIT               (1 << 0)              //UPDATE位用于替换时判断写回
#define     VALID_BIT                (1 << 1)              //标注起始cache内数据not valid 
//#define     COUNTER_BIT              (0x3 << 2)              //三、四位用于replace的计算,若超过4路组关联需要更多的位数

// address bit masks
#define     ADDR_TAGS               11
#define     TAGS_BITS               (0x1ffff << ADDR_TAGS)   
#define     ADDR_INDEX              3
#define     INDEX_BITS              (0xff << ADDR_INDEX)
#define     ADDR_WORD               0
#define     WORD_BITS               (0x7 << ADDR_WORD)
//#define     ADDR_BYTE               (0x3 << 0)

//UINT32 CacheTopoFindWord[CACHE_WAYS_NUM][CACHE_LINE_EACH_WAY][CACHE_LINE_SIZE];    //拓扑逻辑定位words 路-组-字，用于存放数据


 /* address bit fields:      28位地址寻址空间2^28words
 * bits[31:27] = useless
 * bits[27:11] = Tag
 * bits[10:3] = Set(=index)
 * bits[2:0] = Word
 */


#define     DRAM_SIZE       0xfffffff     // dram size = 2^28 words

 //#define     DRAM_BASE       0x0000      // dram address = DRAM_BASE + offset in DramTopology
 //UINT32 DramTopology[DRAM_SIZE];   //DRAM 拓扑逻辑


 //class FIFOLINE
 //{
 //private:
 //   uint32_t addr;
 //   uint32_t rdwr;
 //   uint32_t valid;
 //public:
 //   FIFOLINE();
 //   ~FIFOLINE();
 //   bool addtranscation(uint32_t adr, uint32_t rw);
 //   //void shift();
 //}

 //using namespace DRAMSim;
	class LSUnit;


	class Cache
	{
	private:
		uint32_t cycle;
		uint32_t rdwr;
		uint32_t hit;
		uint32_t UPDATE;
		uint32_t VALID;
		//uint32_t alloc_mode;
		//uint32_t write_mode;
		uint32_t addr;
		LSUnit*  lsunit;
		//uint32_t line;
		//uint32_t data;    //数据 读和写  
		//vector<FIFOLINE *> fifo;

	public:
		Cache();
		~Cache();
		uint32_t addtranscation(uint32_t adr, uint32_t rw);
		uint32_t counter[CACHE_WAYS_NUM][CACHE_LINE_EACH_WAY];  // priority
		uint32_t CacheTopology[CACHE_WAYS_NUM][CACHE_LINE_EACH_WAY];   //用于存放标记和UPDATE位 
		uint32_t ReplaceWay;
		uint32_t lineWord;   //字偏移量 
		uint32_t lineIndex;
		uint32_t lineTag;
		uint32_t lineWay;
		uint32_t state;     //idle=0,busy=1
		//void update();
		//void attachlsu();
		void attach(LSUnit* lsu);
		void do_replacement();
		void do_writeback(uint32_t ReplaceWay);
		void do_writethrough();
		void do_readhit();
		void do_writehit();
		void do_lookup(uint32_t addr);
		void do_read(uint32_t ReplaceWay);
		void do_write(uint32_t ReplaceWay);
	};
}
#endif