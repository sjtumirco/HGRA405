#pragma once
#include "processingElement.h"
#include "load.h"
#include "store.h"
#include "fineGrain.h"
//#include "tagAttach.h"
#include "TagIssue.h"
#include <fstream>
#include <map>
#include <iomanip>
#include "LSUnit.h"

typedef unsigned int uint32_t;
using namespace std;
//using namespace DRAMSim;
//namespace DRAMSim
//{
//	class LSUnit;
//}
using DRAMSim::LSUnit;
using HGRA::Load;
using HGRA::Store;
//using LSUnit;
extern bool begin_signal;
extern ProcessingElement ** pe;
extern Load** le;
extern Store** se;
//extern LSUnit* lsunit;
extern LoopBegin** lbegin;
extern LoopEnd** lend ;
extern Switch** switch_;
extern Join** join;
extern JoinBp** joinbp;
extern Break** break_ ;
extern LoopEnds** lends;
//extern TagAttach** ta ;
extern TagIssue* ti;
extern MemoryInt memory;
extern MemoryInt memory2;//写结果的memory
extern bool begin_signal;
extern ofstream outfile;
extern ofstream outfile2;
extern vector<vector<int>> vec_config_parsed_tmp;
extern vector<vector<int>> port_fanout;

void PeSimProcess(ProcessingElement* pe_current);//index 是vec_config_parsed的索引值，用来表示当前仿真的具体进度
void LeSimProcess(Load* le_current,LSUnit* lsunit);
void SeSimProcess(Store* se_current,LSUnit* lsunit);
//Finegrained
void LbeginSimProcess(LoopBegin* lbegin_current);
void LendSimProcess(LoopEnd* lend_current);
void JoinSimProcess(Join* join_current);
void JoinBpSimProcess(JoinBp* joinbp_current);
void SwitchSimProcess(Switch* switch_current);
void BreakSimProcess(Break* break_current);
void LendsSimProcess(LoopEnds* lends_current);
//void TaSimProcess(TagAttach* ta_current);
void TiSimProcess();
void pe_sim_step_connect(ProcessingElement* pe_current);//是针对当前PE的，当前PE的一个取数操作,for both tag mode and no tag mode
//for tag
void pe_sim_step1(ProcessingElement* pe_current);//fetch data from input port to inTableBuffer
void pe_sim_step2(ProcessingElement* pe_current);//inTableBuffer data out -> ALU operate -> alu's out get into outTableBuffer
void pe_outbuffer_out(ProcessingElement* pe_current,uint32_t ob_index);//outTableBuffer get out
void pe_sim_step3(ProcessingElement* pe_current);//针对当前PE，函数参数是当前PE的指针
//for no tag
void pe_sim_step1_no_tag(ProcessingElement* pe_current);//只针对计算模式
void pe_sim_step2_no_tag(ProcessingElement* pe_current);//只针对计算模式
void pe_outbuffer_out_no_tag(ProcessingElement* pe_current,uint32_t ob_index);//函数参数是需要出数的那个PE的指针
void pe_sim_step3_no_tag(ProcessingElement* pe_current);//针对当前PE，函数参数是当前PE的指针
//step3 and step1 combined
//void pe_sim_step1_new(ProcessingElement* pe_current);
//void pe_sim_step1_new_no_tag(ProcessingElement* pe_current);

//find fanout for each PE port
void port_fanout_collect();


