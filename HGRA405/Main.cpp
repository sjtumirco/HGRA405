#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <map>
//#include "configMan.h"
#include "configParse.h"
//#include "exeMan.h"
#include "processingElement.h"
//#include "load.h"
//#include "store.h"
#include "fineGrain.h"
//#include "tagAttach.h"
#include "TagIssue.h"
#include "memoryInt.h"
#include "paraParse.h"
#include "graph.h"
#include "define.h"
#include "unitSimFunc.h"
//#include "LSUnit.h"
#include <stdio.h>
//

template<typename T>
void connector(bool source_v, T source, bool destination_v, T destination);
template<typename T> void connector(bool source_v, T source, bool destination_v, T destination)
{
	destination_v = source_v;
	destination = source;
}
//ȫ�ֱ�������
using HGRA::Load;
using HGRA::Store;
ProcessingElement ** pe = new ProcessingElement*[peNums];
Load** le = new Load*[leNums];
Store** se = new Store*[seNums];
LoopBegin** lbegin = new LoopBegin*[lbeginNums];
LoopEnd** lend = new LoopEnd*[lendNums];
Switch** switch_ = new Switch*[switchNums];
Join** join = new Join*[joinNums];
Break** break_ = new Break*[breakNums];
LoopEnds** lends = new LoopEnds*[lendsNums];
//TagAttach** ta = new TagAttach*[taNums];
JoinBp** joinbp = new JoinBp*[joinbpNums];
TagIssue* ti = new TagIssue;

//Memory init
MemoryInt memory(memoryDepth);//����д
MemoryInt memory2(memory2depth);//д�����memory
//�����ļ�
vector<vector<int>> vec_config_parsed_tmp;
vector<vector<int>> port_fanout;//port_fanout[i]�ĵ�һ��Ԫ�ر�ʾPE����LE����2��Ԫ�ر�ʾ��ţ���3,4,5��Ԫ�طֱ��Ǹ�PE������˿ڵ��ȳ�
bool begin_signal = 1;

ofstream outfile("DEBUG.txt");
ofstream outfile2("DEBUG2.txt");

void power_callback(double a, double b, double c, double d) {}

int main(int argc,char* argv[])
{

	using namespace DRAMSim;
	bool end = 0;
	uint cnt = 0;
	/*string str_;
	ifstream noti("notification.txt");
	while (getline(noti, str_))
	{
		cout << str_ << endl;
	}
	system("pause");*/

	//hardware generate
	/*ConfigMan configMan;
	ExeMan exeMan;*/
	//unit definition
	if (1)
	{
		for (int i = 0; i < peNums; i++)
		{
			pe[i] = new ProcessingElement;
		}
		for (int i = 0; i < leNums; i++)
		{
			le[i] = new ::Load;
		}
		for (int i = 0; i < seNums; i++)
		{
			se[i] = new ::Store;
		}
		/*for (int i = 0; i < taNums; i++)
		{
			ta[i] = new TagAttach;
		}*/
		for (int i = 0; i < lbeginNums; i++)
		{
			lbegin[i] = new LoopBegin;
		}
		for (int i = 0; i < lendNums; i++)
		{
			lend[i] = new LoopEnd;
		}		
		for (int i = 0; i < switchNums; i++)
		{
			switch_[i] = new Switch;
		}
		for (int i = 0; i < joinNums; i++)
		{
			join[i] = new Join;
		}
		for (int i = 0; i < breakNums; i++)
		{
			break_[i] = new Break;
		}
		for (int i = 0; i < lendsNums; i++)
		{
			lends[i] = new LoopEnds;
		}
		for (int i = 0; i < joinbpNums; i++)
		{
			joinbp[i] = new JoinBp;
		}
	}
	
	//memory 
	ifstream memoryInFile;
	memoryInFile.open("MemoryInFile8x8.txt");
	memory.readFromFile(memoryInFile);

	//GET DRAM INSTANCE
	
	
	DRAMSim::LSUnit *lsunit = new DRAMSim::LSUnit(le, se);
	DRAMSim::MultiChannelMemorySystem *mem = new DRAMSim::MultiChannelMemorySystem("ini/DDR2_micron_16M_8b_x8_sg3E.ini", "system.ini", ".", "example_app", 16384);
	TransactionCompleteCB *read_cb = new Callback<DRAMSim::LSUnit, void, unsigned, uint64_t, uint64_t>(&(*lsunit), &DRAMSim::LSUnit::mem_read_complete);
	TransactionCompleteCB *write_cb = new Callback<DRAMSim::LSUnit, void, unsigned, uint64_t, uint64_t>(&(*lsunit), &DRAMSim::LSUnit::mem_write_complete);
	mem->RegisterCallbacks(read_cb, write_cb, power_callback);
	lsunit->AttachMem(mem);      //attach
	
	

	//Config file parse
	//Config config;
	ConfigParse cp;

	vector<pe_line_int> vec_pe_config_;
	////handwrite config file parse to auto-generated config file////
	ifstream config_infile;
	config_infile.open(argv[1]);//�����ļ�·������config_ori.txt,config_ori_s.txt
	cp.configFile2vec(config_infile);
	cp.configVec2parsed();
	vec_config_parsed_tmp = cp.vec_config_parsed;//parsed config vector,use in this main function
	/*for (auto s : vec_config_parsed_tmp)
	{
		for (auto s_ : s)
		{
			cout << s_  << " ";
		}
		cout << endl;
	}*/


	////ͼ����
	//
	//Graph graph(peNums + leNums + seNums + fgNums);
	//vector<pair<int, int>> edge = graph.edgeConstruct(vec_config_parsed_tmp);
	//for (vector<pair<int,int>>::iterator it = edge.begin();it != edge.end();it++)
	//{
	//	graph.addEdge((*it).first, (*it).second);
	//}
	//graph.topological_sort();
	//


	//find fanout for each PE port
	port_fanout_collect();
	
	//��������
	int idx_config;
	for (unsigned int i = 0; i < vec_config_parsed_tmp.size(); i++)
	{
		idx_config = vec_config_parsed_tmp[i][1];
		if (vec_config_parsed_tmp[i][0] == 8)//����PE������
		{			
			pe[idx_config]->config_reg.push(vec_config_parsed_tmp[i]);
		}
		else if (vec_config_parsed_tmp[i][0] == 9)//����LE������
		{
			le[idx_config]->config_reg.push(vec_config_parsed_tmp[i]);
		}
		else if (vec_config_parsed_tmp[i][0] == 0)//����SE������
		{
			se[idx_config]->config_reg.push(vec_config_parsed_tmp[i]);
		}
		//����FG������
		else if(vec_config_parsed_tmp[i][0] == 1)//lbegin												 
		{
			lbegin[idx_config]->config_reg.push(vec_config_parsed_tmp[i]);
		}
		else if (vec_config_parsed_tmp[i][0] == 2)//lend												 
		{
			lend[idx_config]->config_reg.push(vec_config_parsed_tmp[i]);
		}
		else if (vec_config_parsed_tmp[i][0] == 3)//join												 
		{
			join[idx_config]->config_reg.push(vec_config_parsed_tmp[i]);
		}
		else if (vec_config_parsed_tmp[i][0] == 4)//switch												 
		{
			switch_[idx_config]->config_reg.push(vec_config_parsed_tmp[i]);
		}
		else if (vec_config_parsed_tmp[i][0] == 5)//break												 
		{
			break_[idx_config]->config_reg.push(vec_config_parsed_tmp[i]);
		}
		else if (vec_config_parsed_tmp[i][0] == 6)//lends												 
		{
			lends[idx_config]->config_reg.push(vec_config_parsed_tmp[i]);
		}
		//else if (vec_config_parsed_tmp[i][0] == 7)//ta												 
		//{
		//	ta[idx_config]->config_reg.push(vec_config_parsed_tmp[i]);
		//}
		else if(vec_config_parsed_tmp[i][0] == 10)//joinbp
		{ 
			joinbp[idx_config]->config_reg.push(vec_config_parsed_tmp[i]);
		}
		cout << endl;
		cout << endl;
		cout << endl;

	}

	//parameter import,only for PE
	ifstream para_file;
	para_file.open(argv[2]);//�����ļ�·������parameter_file.txt
	ParaParse paraparse;
	paraparse.parsePara(para_file);
	vector<int> v_tmp;
	int pe_idx;
	for (unsigned int i = 0; i < paraparse.para_parsed.size(); i++)
	{
		v_tmp = paraparse.para_parsed[i];
		pe_idx = v_tmp[0];
		pe[pe_idx]->loc_reg = v_tmp[1];	
		pe[pe_idx]->loc_reg_v = 1;
	}

	
	while(1)
	{
		outfile << "==========clock--" << cnt <<"======================================================================================" << endl;
		outfile2 << "====CLOCK-" << cnt << "======================================================================================" << endl;
		//���������漰���ĸ����ṹ�����ϳ�һ������������Ĺ��̾��Ƕ����������ÿһ��������б���
		for (int i = vec_config_parsed_tmp.size() - 1; i >= 0; i--)//int i = vec_config_parsed_tmp.size()-1;i>=0; i--)
		{
			cout << endl;
			//���ڷ������PE
			if (vec_config_parsed_tmp[i][0] == 8)
			{
				outfile << "----------------------------------------------------------------" << endl;
				outfile << "CLOCK" << cnt << "-PE" << vec_config_parsed_tmp[i][1] << "-" << endl;
				outfile << endl;
				//
				outfile2 << "---------------------------------------------------------------" << endl;
				outfile2 << "CLOCK" << cnt << "-PE" << vec_config_parsed_tmp[i][1] << "-" << endl;
				if (vec_config_parsed_tmp[i][1] == 3 | vec_config_parsed_tmp[i][1] == 4)
					continue;
				outfile2 << "CLOCK" << cnt << "-PE[" << vec_config_parsed_tmp[i][1] << "]���ֵ" << endl;
				PeSimProcess(pe[vec_config_parsed_tmp[i][1]]);
				//for debug	
				/*
				outfile2 << "CLOCK" << cnt << "-PE[" << vec_config_parsed_tmp[i][1] << "]���ֵ" << endl;
				outfile2 << setw(15) << "dout1_t" << setw(15) << "dout1_v" << setw(15) << "dout1" << setw(15) << "dout2_t" << setw(15) << "dout2_v"
					<< setw(15) << "dout" << setw(15) << "bout_t" << setw(15) << "bout_v" << setw(15) << "bout" << endl;
				outfile2 << setw(15) << pe[vec_config_parsed_tmp[i][1]]->dout1_tag << setw(15) << pe[vec_config_parsed_tmp[i][1]]->dout1_v
					<< setw(15) << pe[vec_config_parsed_tmp[i][1]]->dout1 << setw(15) << pe[vec_config_parsed_tmp[i][1]]->dout2_tag
					<< setw(15) << pe[vec_config_parsed_tmp[i][1]]->dout2_v << setw(15) << pe[vec_config_parsed_tmp[i][1]]->dout2
					<< setw(15) << pe[vec_config_parsed_tmp[i][1]]->bout_tag << setw(15) << pe[vec_config_parsed_tmp[i][1]]->bout_v
					<< setw(15) << pe[vec_config_parsed_tmp[i][1]]->bout << endl;
				//outfile2 << "CLOCK" << cnt << "-PE" << vec_config_parsed_tmp[i][1] << endl;
				*/
				outfile << endl;
				//
				outfile2 << endl;
				
				
			}
			//���ڷ������LE
			else if (vec_config_parsed_tmp[i][0] == 9)
			{
				outfile << "----------------------------------------------------------------" << endl;
				outfile << "CLOCK" << cnt << "-LE" << vec_config_parsed_tmp[i][1] << "-" << endl;
				outfile << endl;
				//
				outfile2 << "--------------------------------------" << endl;
				outfile2 << "CLOCK" << cnt << "-LE" << vec_config_parsed_tmp[i][1] << "-" << endl;
				outfile2 << endl;
				LeSimProcess(le[vec_config_parsed_tmp[i][1]],lsunit);
				//for debug
				outfile2 << "LE[" << vec_config_parsed_tmp[i][1] << "]���ֵ" << endl;
				outfile2 << setw(15) << "data_out_t" << setw(15) << "data_out_v" << setw(15) << "data_out" << endl;
				outfile2 << setw(15) << le[vec_config_parsed_tmp[i][1]]->data_out_tag << setw(15) << le[vec_config_parsed_tmp[i][1]]->data_out_v 
					<< setw(15) << le[vec_config_parsed_tmp[i][1]]->data_out << endl;
				outfile << endl;
				//
				outfile2 << endl;
				
				
			}
			//���ڷ������SE
			else if (vec_config_parsed_tmp[i][0] == 0)
			{
				outfile << "----------------------------------------------------------------" << endl;
				outfile << "CLOCK" << cnt << "-SE" << vec_config_parsed_tmp[i][1] << "-" << endl;
				outfile << endl;
				//
				outfile2 << "--------------------------------------" << endl;				
				outfile2 << "CLOCK" << cnt << "-SE" << vec_config_parsed_tmp[i][1] << "-" << endl;
				outfile2 << endl;
				SeSimProcess(se[vec_config_parsed_tmp[i][1]],lsunit);
				//for debug
				outfile2 <<"SE[" << vec_config_parsed_tmp[i][1] << "]���ֵ" << endl;
				outfile2 << setw(15) << "out4end_v" << setw(15) << "out4end" << endl;
				outfile2 << setw(15) << se[vec_config_parsed_tmp[i][1]]->se_extra_out_for_end_v << setw(15) << se[vec_config_parsed_tmp[i][1]]->se_extra_out_for_end << endl;
				outfile << "CLOCK" << cnt << "-SE" << vec_config_parsed_tmp[i][1] << endl;
				outfile << endl;
				//
				outfile2 << endl;

			}
			//���ڷ������FG
			else if (vec_config_parsed_tmp[i][0] == 1)//lbegin
			{
				outfile2 << "--------------------------------------" << endl;
				outfile2 << "CLOCK" << cnt << "-lbegin" << vec_config_parsed_tmp[i][1] << "-" << endl;
				outfile2 << endl;
				LbeginSimProcess(lbegin[vec_config_parsed_tmp[i][1]]);
				//for debug
				outfile2 <<"lbegin[" << vec_config_parsed_tmp[i][1] << "]���ֵ" << endl;
				outfile2 << setw(15) << "out_v" << setw(15) << "out" << endl;
				outfile2 << setw(15) << lbegin[vec_config_parsed_tmp[i][1]]->out_v << setw(15) << lbegin[vec_config_parsed_tmp[i][1]]->out << endl;
				outfile2 << endl;
			}
			else if (vec_config_parsed_tmp[i][0] == 2)//lend
			{
				outfile2 << "--------------------------------------" << endl;
				outfile2 << "CLOCK" << cnt << "-lend" << vec_config_parsed_tmp[i][1] << "-" << endl;
				outfile2 << endl;
				LendSimProcess(lend[vec_config_parsed_tmp[i][1]]);
				//for debug
				outfile2 <<"lend[" << vec_config_parsed_tmp[i][1] << "]���ֵ" << endl;
				outfile2 << setw(15) << "out2back" << setw(15) << "out2end" << endl;
				outfile2 << setw(15) << lend[vec_config_parsed_tmp[i][1]]->out2back << setw(15) << lend[vec_config_parsed_tmp[i][1]]->out2end << endl;				
				outfile2 << endl;
			}
			else if (vec_config_parsed_tmp[i][0] == 3)//join
			{
				outfile << "                " << endl;
				outfile << "���ڴ������join�ڵ�......" << endl;
				JoinSimProcess(join[vec_config_parsed_tmp[i][1]]);
			}
			else if (vec_config_parsed_tmp[i][0] == 4)//switch
			{
				outfile << "                " << endl;
				outfile << "���ڴ������Switch�ڵ�......" << endl;
				SwitchSimProcess(switch_[vec_config_parsed_tmp[i][1]]);
			}
			else if (vec_config_parsed_tmp[i][0] == 5)//break
			{
				outfile << "                " << endl;
				outfile << "���ڴ������break�ڵ�......" << endl;
				BreakSimProcess(break_[vec_config_parsed_tmp[i][1]]);
			}
			else if (vec_config_parsed_tmp[i][0] == 6)//lends
			{
				outfile << "                " << endl;
				outfile << "���ڴ������lends�ڵ�......" << endl;
				LendsSimProcess(lends[vec_config_parsed_tmp[i][1]]);
			}
			else if (vec_config_parsed_tmp[i][0] == 10)//joinbp
			{
				outfile << "                " << endl;
				outfile << "���ڴ������joinbp�ڵ�......" << endl;
				JoinBpSimProcess(joinbp[vec_config_parsed_tmp[i][1]]);
			}
			//���ڷ������TA�ڵ�
			//else if (vec_config_parsed_tmp[i][0] == 7)//ta
			//{
			//	outfile << "                " << endl;
			//	outfile << "���ڴ������ta�ڵ�......" << endl;
			//	TaSimProcess(ta[vec_config_parsed_tmp[i][1]]);
			//	//for debug
			//	outfile2 << "CLOCK" << cnt << "-TA[" << vec_config_parsed_tmp[i][1] << "]���ֵ" << endl;
			//	outfile2 << setw(15) << "out_tag" << setw(15) << "out_v" << setw(15) << "out" << endl;
			//	outfile2 << setw(15) << ta[vec_config_parsed_tmp[i][1]]->out_tag << setw(15) << ta[vec_config_parsed_tmp[i][1]]->out_v 
			//		<< setw(15) << ta[vec_config_parsed_tmp[i][1]]->out << endl;
			//}		
			//tag issue����
			outfile2 << "--------------------------------------" << endl;
			outfile2 << "CLOCK" << cnt << "-ti" << endl;
			outfile2 << endl;
			TiSimProcess();
			outfile2 << "Ti�ڵ�����ֵ" << endl;
			outfile2 << setw(10) << "out_v" << setw(10) << "out" << endl;
			outfile2 << setw(10) << ti->out_v << setw(10) << ti->out << endl;
			outfile2 << endl;
			
		}
		//Ti update
		//outfile2 << "--------------------------------------" << endl;
		//outfile2 << "CLOCK" << cnt << "-ti" << endl;
		//outfile2 << endl;
		//TiSimProcess();
		//outfile2 << "Ti�ڵ�����ֵ" << endl;
		//outfile2 << setw(10) << "out_v" << setw(10) << "out" << endl;
		//outfile2 << setw(10) << ti->out_v << setw(10) << ti->out << endl;
		//outfile2 << endl;

		//memmory update
		lsunit->update();
		//��̬��PE outbuffer����ˢ��
		/*
		for (int i = vec_config_parsed_tmp.size() - 1; i >= 0; i--)
		{
			if (vec_config_parsed_tmp[i][0] == 8)//pe
			{
				if (vec_config_parsed_tmp[i][19] == 0)//no tag mode
				{
					pe[vec_config_parsed_tmp[i][1]]->outbuffer1_v = 0;
					pe[vec_config_parsed_tmp[i][1]]->outbuffer2_v = 0;
					pe[vec_config_parsed_tmp[i][1]]->outbuffer3_v = 0;
				}
			}

		}
		*/

		
		if (cnt == 0)
		{
			begin_signal = 0;
		}
		
		//for debug
		//----------------------------------------

		//ע��һ�£�lend0,lend1�ڲ�ͬ�����¶��п����ǽ����ڵ�
		uint lend_end_index = vec_config_parsed_tmp[vec_config_parsed_tmp.size() - 1][1];
		end = lend[lend_end_index]->out2end;

		if (end)
			break;
		if (cnt == 50)
			break;

		cnt++;
	}
	//ALU��������ʱ������ӡ
	outfile << "||||||||||||||||||||||||||" << endl;
	outfile << "PE alu����������" << endl;
	outfile << cnt << endl;
	ofstream ofile("memoryOutFile.txt");
	memory2.writeToFile(ofile);
	if (1)
	{
		for (int i = 0; i < peNums; i++)
		{
			delete pe[i] ;
		}
		for (int i = 0; i < leNums; i++)
		{
			delete le[i];
		}
		for (int i = 0; i < seNums; i++)
		{
			delete se[i];
		}
		/*for (int i = 0; i < taNums; i++)
		{
		ta[i] = new TagAttach;
		}*/
		for (int i = 0; i < lbeginNums; i++)
		{
			delete lbegin[i];
		}
		for (int i = 0; i < lendNums; i++)
		{
			delete lend[i];
		}
		for (int i = 0; i < switchNums; i++)
		{
			delete switch_[i];
		}
		for (int i = 0; i < joinNums; i++)
		{
			delete join[i] ;
		}
		for (int i = 0; i < breakNums; i++)
		{
			delete break_[i];
		}
		for (int i = 0; i < lendsNums; i++)
		{
			delete lends[i];
		}
		for (int i = 0; i < joinbpNums; i++)
		{
			delete joinbp[i];
		}
		delete[] pe;
		delete[] le;
		delete[] se;
		delete[] lbegin;
		delete[] lend;
		delete[] switch_;
		delete[] join;
		delete[] break_;
		delete[] lends;
		delete[] joinbp;
	}
	system("pause");
	return 0;
}
