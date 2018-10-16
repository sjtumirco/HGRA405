//本文件包含各种程序中需要初始化的量
//每一项都要有，修改是请保证只修改数字，避免增减项目


//for no tag
#define outbuffer_depth 1		//outputBuffer的深度
#define inbuffer_depth 1		//inBuffer的深度
//for tag
#define outtablebuffer_depth 2	//outTableBuffer的深度
#define intablebuffer_depth 2	//intablebuffer的深度
//unit num
#define peNums 15				//PE总数
#define leNums 1				//LE总数
#define seNums 1				//SE总数

#define lbeginNums 2			//loop begin总数
#define lendNums 1				//loop end总数
#define joinNums 0				//join节点总数
#define switchNums 0			//switch节点总数
#define breakNums 0				//break节点总数
#define lendsNums 0				//loop end2节点总数
#define joinbpNums 4			//joinbp节点总数

#define fgNums (lbeginNums+lendNums+joinNums+switchNums+breakNums+lendsNums+joinbpNums)				//总的fg节点的数量

#define unitTotalNums (peNums+leNums+seNums+fgNums)		//总的单元个数

//#define fgNums 4				//细粒度结构总数
#define joininportNums 2			//第三类细粒度模块（join节点）的输入端口数量
#define memoryDepth 2000			//保存输入数据memory的大小
#define memory2depth 2000			//保存输出数据memory的大小

//with memory or not
#define ATTACH_MEMORY 0			//仿真器加上或者不加上memory

//累加寄存器的边界值
#define THRESHOLD 8