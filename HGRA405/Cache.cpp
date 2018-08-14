#include "Cache.h"
#include "LSUnit.h"

using namespace DRAMSim;

Cache::Cache()
{
   cycle = 0;
   rdwr = 0;
   lineWord = 0;  
   lineIndex = 0;
   lineTag = 0;
   lineWay = 0;
   hit = 0;
   UPDATE = 0;
   VALID = 0;
   addr = 0;
   state = 0;
   //data = 0;   
   //lsunit = lsu;
   for(uint32_t i = 0; i < CACHE_WAYS_NUM; i++)
   {   for(uint32_t j = 0; j < CACHE_LINE_EACH_WAY; j++)
	   {
		   CacheTopology[CACHE_WAYS_NUM][CACHE_LINE_EACH_WAY] = 0;
		   counter[CACHE_WAYS_NUM][CACHE_LINE_EACH_WAY] = 0;
	   }
   }
}


Cache::~Cache()
{
}

/*
void Cache::attachlsu(LSUnit *lsu)
{
	lsunit = lsu;
}
*/

void Cache::attach(LSUnit* lsu)
{
	lsunit = lsu;
}

uint32_t Cache::addtranscation(uint32_t adr, uint32_t rw)
{
	if(state == 0)
	{
		state = 1;
        switch(rw)
	    {
            case 0:     //if (argc != 3) {printf("parameter error while reading.\n"); return -1;}
                rdwr = 0; addr = adr;
            break;

            case 1:     //if (argc != 4) {printf("parameter error while writing.\n"); return -1;}
                rdwr = 1; addr = adr; 
            break;

            default:
                printf("rw parameter not supported.\n");
            exit(-1);
            }
	    printf("%d on address 0x%x\n", rdwr, addr);
	    cycle = 0;
        do_lookup(addr);   
        return cycle;	
	}
	else if(state == 1)
	{
		printf("Cache:cache busy!\n");
		return 0;
	}
}

	


void Cache::do_lookup(uint32_t addr)
{
    //uint32_t lineIndex = (addr & INDEX_BITS) >> ADDR_INDEX;           //提取组
	cycle = cycle + LOOKUP_DELAY;
    lineIndex = (addr & INDEX_BITS) >> ADDR_INDEX;   
    lineTag = (addr & TAGS_BITS) >> ADDR_TAGS;   //提取TAG
    lineWord = (addr & WORD_BITS) >> ADDR_WORD;
    // search the cache set
    uint32_t i;
    hit = 0;
    for (i = 0; i < CACHE_WAYS_NUM; i++)
        //find the cacheline in the set
        if ((CacheTopology[i][lineIndex] & TAGS_BITS) >> ADDR_TAGS == lineTag)      //组定位后比较TAG
        {	if((VALID = CacheTopology[i][lineIndex] & VALID_BIT))
			hit = 1;
            lineWay = i;
        }  		
    lsunit->hit = hit;    
    

    switch (hit)
    {
        case 0:
        { 	do_replacement();
            /* cache miss替换后仍然要实现读写
             */
            break;
        }
        case 1:
        {   
		    if(REPLACE_MODE == 1)
		    {   
		        counter[lineWay][lineIndex] = CACHE_WAYS_NUM;
		        for(i = 0; i < CACHE_WAYS_NUM; i++)
                    if(counter[i][lineIndex] != 0)
				        counter[i][lineIndex]--;
			        else if(counter[i][lineIndex] < 0)
				        printf("error occurs that cachecounter < 0!\n");
			}	
		    printf("cache hit!\n");
            switch (rdwr) {
                case 0: do_readhit();
                    break;
                case 1: do_writehit();
                    break;
                default:
                    printf("Unknown operation in do_lookup.\n");
                    break;
            }
            break;
        }

        default:
            printf("Error Condition in do_lookup.\n");
            break;
    }
}


void Cache::do_replacement()   //还需要检查UPDATE位，看是否需要写回。     不同替换算法的实现   writeback需不需要delay
{
	cycle = cycle + REPLACE_DELAY;
    uint32_t j;
    uint32_t setup = (lineTag << ADDR_TAGS) + (lineIndex << ADDR_INDEX); 
    uint32_t counterbits = CACHE_WAYS_NUM + 1;	
    //uint32_t ReplaceWay;
	
	if(REPLACE_MODE == 0)
		ReplaceWay = rand() % CACHE_WAYS_NUM;
	else if(REPLACE_MODE == 1)
	{
		for(uint32_t i = 0; i < CACHE_WAYS_NUM; i++)
	    {
			if(counterbits > counter[i][lineIndex])
			{	
		        counterbits = counter[i][lineIndex];
				ReplaceWay = i; 
			}	
		}
	}	
	else if(REPLACE_MODE == 2)
	{
		for(uint32_t i = 0; i < CACHE_WAYS_NUM; i++)
	    {
			if(counter[i][lineIndex] == 0)
			{	
				ReplaceWay = i; 
			}
            else 
                printf("error occurs for fifo replace mode that no line replaced!\n");				
		}
	}

	

	if(WRITE_MODE == 1)
        if(CacheTopology[ReplaceWay][lineIndex] & UPDATE_BIT)   //被替换块若UPDATE=1则写回内存否则只需替换
        {
    	   do_writeback(ReplaceWay);                              	 
        }        
    
	if(WRITE_MODE == 0 && rdwr == 1)   //当为直写策略且为写miss时，产生一个write through的延时
	    do_writethrough();


    //for(j = 0; j < CACHE_LINE_SIZE; j++)                                                      //替换
    //CacheTopoFindWord[ReplaceWay][lineIndex][j] = DramTopology[setup + j]; //填充cache_line 
	if(REPLACE_MODE == 0)
        CacheTopology[ReplaceWay][lineIndex] = (lineTag << ADDR_TAGS) + VALID_BIT;  //更改TAG标记，UPDATE标记为0 
    else if(REPLACE_MODE == 1)   
	{
		CacheTopology[ReplaceWay][lineIndex] = (lineTag << ADDR_TAGS) + VALID_BIT;
		counter[ReplaceWay][lineIndex] = CACHE_WAYS_NUM;
		for(j = 0; j < CACHE_WAYS_NUM; j++)
		{    
			if(counter[j][lineIndex] != 0)
				counter[j][lineIndex]--;
			else if(counter[j][lineIndex] < 0)
				printf("error occurs that cachecounter < 0!\n");
		}
	}
	else if(REPLACE_MODE == 2)
	{	
        CacheTopology[ReplaceWay][lineIndex] = (lineTag << ADDR_TAGS) + VALID_BIT;
		counter[ReplaceWay][lineIndex] = CACHE_WAYS_NUM;
		for(j = 0; j < CACHE_WAYS_NUM; j++)
		{
            if(counter[j][lineIndex] > 0)
				counter[j][lineIndex]--;
			else if(counter[j][lineIndex] < 0)
				printf("error occurs that cachecounter < 0!\n");
		}
	}
	else
		printf("error occurs for replace mode!\n");

	/*miss后的读写在访问mem时完成
    switch (rdwr) 
	{
        case 0: do_read(ReplaceWay);
            break;
        case 1: do_write(ReplaceWay);
            break;
        default:
            printf("Unknown operation in do_lookup.\n");
            break;
    }
    */
	        
    
}

	
void Cache::do_readhit()
{     
    cycle = cycle + READHIT_DELAY;
    uint32_t i;
	  switch(hit)
	  {
      case 1: 
      {
      	  //data = CacheTopoFindWord[lineWay][lineIndex][lineWord];  
		  //printf("hit and read data %x \n",data);                   
          printf("now the cacheline is %x.\n",CacheTopology[lineWay][lineIndex]);
          for(i = 0; i < CACHE_WAYS_NUM; i++)
	      {  
	         printf("TAG %x in this index.\n", CacheTopology[i][lineIndex]);
	      }
          break;
      }
     
      
      default:
      	  printf("Error Condition In do_readhit() and %d.\n", hit);
          break;
    }
     
}


void Cache::do_writehit()
{   
    if(WRITE_MODE == 1)
        cycle = cycle + WRITEHIT_DELAY;
    else if(WRITE_MODE == 0)
	{
		cycle = cycle + WRITEHIT_DELAY;
		do_writethrough();
	}
	
    uint32_t i;
    switch(hit)
	{
      case 1: 
      {
      	  //CacheTopoFindWord[lineWay][lineIndex][lineWord] = data;
		  CacheTopology[lineWay][lineIndex] |= UPDATE_BIT;           //写过以后update置位 
      	  //printf("hit and write data %x \n",CacheTopoFindWord[lineWay][lineIndex][lineWord]);
          
          //std::cout << "write data " << hex << CacheTopoFindWord[i][lineIndex][lineWord] << std::endl;
          printf("now the cacheline is %x.\n",CacheTopology[lineWay][lineIndex]);
          for(i = 0; i < CACHE_WAYS_NUM; i++)
	      {  
	         printf("TAG %x in this index.\n", CacheTopology[i][lineIndex]);
	      }
          break;
      }
      
      
      default:
      	  printf("Error Condition In do_writehit().\n");
          break;
    }
    
}	
	
	
		
void Cache::do_writeback(uint32_t ReplaceWay)
{
	  cycle = cycle + WRITEBACK_DELAY;
	  uint32_t RepTag;
	  uint32_t RepIndex;
	  uint32_t RepAdr;
	  uint32_t k;
	  
	  RepTag = (CacheTopology[ReplaceWay][lineIndex] & TAGS_BITS) >> ADDR_TAGS;
	  RepIndex = lineIndex;
	  RepAdr = (RepTag << ADDR_TAGS) + (RepIndex << ADDR_INDEX);
	  
	  
	  for(k = 0; k < CACHE_LINE_SIZE; k++)
	  {
	  	 //DramTopology[RepAdr + k] = CacheTopoFindWord[ReplaceWay][lineIndex][k];
	  	 printf("write back.\n");
		 lsunit->mem->addTransaction(true, uint64_t(RepAdr + k));
	  	 //std::cout << "data " << hex << CacheTopoFindWord[ReplaceWay][lineIndex][j] << " write back." << std::endl;
	  }
	  //std::cout << "The " << hex << RepAdr << " Address write back." << std::endl;
	  printf("The %x to %x Address write back.\n", RepAdr, RepAdr+CACHE_LINE_SIZE-1);
}

void Cache::do_writethrough()
{
	cycle = cycle + WRITETHROUGH_DELAY;
	lsunit->mem->addTransaction(true, uint64_t(addr));
}

/*
void Cache::do_read(uint32_t ReplaceWay)
{   
    cycle = cycle + READ_DEALY;
    uint32_t i;
	if(!hit)
	{
	   data = CacheTopoFindWord[ReplaceWay][lineIndex][lineWord];
	   printf("not hit, read data %x \n",data);
    }
	else
	printf("error occurs in do_read\n");
	
	printf("now the cacheline is %x.\n",CacheTopology[ReplaceWay][lineIndex]);
	for(i = 0; i < CACHE_WAYS_NUM; i++)
	{  
	   printf("TAG %x in this index.\n", CacheTopology[i][lineIndex]);
	}	
	
	
}


void Cache::do_write(uint32_t ReplaceWay)
{
	cycle = cycle + WRITE_DELAY;
	uint32_t i;
    if(!hit)
	{
	   CacheTopoFindWord[ReplaceWay][lineIndex][lineWord] = data;
	   CacheTopology[ReplaceWay][lineIndex] |= UPDATE_BIT;           //写过以后update置位
	   printf("not hit, write data %x \n",CacheTopoFindWord[ReplaceWay][lineIndex][lineWord]);
    }
	else
	   printf("error occurs in do_write\n");
	
	printf("now the cacheline is %x.\n",CacheTopology[ReplaceWay][lineIndex]);
	for(i = 0; i < CACHE_WAYS_NUM; i++)
	{  
	   printf("TAG %x in this index.\n", CacheTopology[i][lineIndex]);
	}
	
}
*/
