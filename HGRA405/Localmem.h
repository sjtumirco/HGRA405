#ifndef LOCALMEM               //ÓÐÊý¾Ýdata
#define LOCALMEM
#include <stdint.h>
#include "iostream" 

namespace DRAMSim
{
#define MEMSIZE  32*1024    //32KB
#define WRDELAY     2
#define RDDELAY     1
#define WORDBYTE    4

	class LSUnit;

	class localmem
	{
	private:
		//uint32_t data[MEMSIZE/WORDBYTE];
		uint32_t size;     //Byte
		uint32_t addrspace;    //Word
		uint32_t state;       //0 for idle 1 for busy
		LSUnit* lsu;

		uint32_t addr_;
		uint32_t rdwr_;
		uint32_t tabline_;
		//uint32_t trdata_;
		uint32_t count;
		uint32_t cycle;
		//uint32_t tab;
	public:
		localmem(LSUnit* lsunit);
		~localmem() {};
		uint32_t addtranscation(uint32_t addr, uint32_t rdwr, uint32_t tabline);
		void update();
    };
}

#endif