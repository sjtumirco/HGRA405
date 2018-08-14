#include "Localmem.h"
#include "LSUnit.h"

using namespace DRAMSim;

localmem::localmem(LSUnit* lsunit)
{
	size = MEMSIZE;
	addrspace = MEMSIZE / WORDBYTE - 1;
	state = 0;
	lsu = lsunit;
	cycle = 0;
	count = 0;
	addr_ = 0;
	rdwr_ = 0;
	tabline_ = 0;
	state = 0;
}


uint32_t localmem::addtranscation(uint32_t addr, uint32_t rdwr, uint32_t tabline)
{
	if (state == 0)
	{
		addr_ = addr;
		rdwr_ = rdwr;
		tabline_ = tabline;
		state = 1;
		count = 0;
		if (rdwr_ = 0)
			cycle = RDDELAY;
		else if (rdwr_ = 1)
			cycle = WRDELAY;
		return 1;
	}
	else if (state == 1)
	{
		std::cout << "LCM:Localmem busy!" << std::endl;
		return 0;
	}
	else
		exit(-1);
}


void localmem::update()
{
	if (count < cycle)
		count++;
	else if (count == cycle)
	{
		lsu->table[tabline_]->complete = 1;
		state = 0;
	}
}