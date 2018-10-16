#include "unitSimFunc.h"

typedef unsigned int uint;



void PeSimProcess(ProcessingElement* pe_current)
{
	int opcode = pe_current->config_reg.front()[2];
	int pe_index_current = pe_current->config_reg.front()[1];//当前正在处理的PE标号
	//in_from index
	int din1_from_index = pe_current->config_reg.front()[3];
	int din2_from_index = pe_current->config_reg.front()[6];
	int bin_from_index = pe_current->config_reg.front()[9];
	//in_port
	int din1_from_port = pe_current->config_reg.front()[4];
	int din2_from_port = pe_current->config_reg.front()[7];
	int bin_from_port = pe_current->config_reg.front()[10];
	//in_flag
	int din1_from_flag = pe_current->config_reg.front()[5];
	int din2_from_flag = pe_current->config_reg.front()[8];
	int bin_from_flag = pe_current->config_reg.front()[11];
	//判断是否悬空
	bool is_din1_float = din1_from_flag;//0--悬空；非0--非悬空
	bool is_din2_float = din2_from_flag;
	bool is_bin_float = bin_from_flag;

	//outfile2 << "CLOCK" << cnt << "-PE[" << vec_config_parsed_tmp[i][1] << "]输出值" << endl;
	outfile2 << setw(15) << "dout1_t" << setw(15) << "dout1_v" << setw(15) << "dout1" << setw(15) << "dout2_t" << setw(15) << "dout2_v"
		<< setw(15) << "dout" << setw(15) << "bout_t" << setw(15) << "bout_v" << setw(15) << "bout" << endl;
	outfile2 << setw(15) << pe_current->dout1_tag << setw(15) << pe_current->dout1_v
		<< setw(15) << pe_current->dout1 << setw(15) << pe_current->dout2_tag
		<< setw(15) << pe_current->dout2_v << setw(15) << pe_current->dout2
		<< setw(15) << pe_current->bout_tag << setw(15) << pe_current->bout_v
		<< setw(15) << pe_current->bout << endl;
	//outfile2 << "CLOCK" << cnt << "-PE" << vec_config_parsed_tmp[i][1] << endl;
	
	//开始仿真前输出端口数据清零

	pe_current->dout1_v = 0;
	pe_current->dout2_v = 0;
	pe_current->bout_v = 0;
	pe_current->dout1 = 0;
	pe_current->dout2 = 0;
	pe_current->bout = 0;

	bool tag_mode = pe_current->config_reg.front()[19];
	uint mode = pe_current->config_reg.front()[18];
	if (tag_mode)//需要进行tag的处理
	{
		outfile << "PE需要tag处理...." << endl;
		if (mode == 0)//bypass
		{
			outfile << "pe[" << pe_index_current << "]处于旁路模式...." << endl;
			pe_sim_step3(pe_current);//排除了LE,SE等不需要ob出数的操作和PE处于旁路和中转模式下的ob出数操作
			pe_sim_step_connect(pe_current);
			if (pe_current->din1_v)
			{
				pe_current->outTableBuffer1.in = pe_current->din1;
				pe_current->outTableBuffer1.in_v = pe_current->din1_v;
				pe_current->outTableBuffer1.in_tag = pe_current->din1_tag;
				pe_current->outTableBuffer1.dataIn();
				pe_current->outTableBuffer1.dataOut();
				pe_current->dout1 = pe_current->outTableBuffer1.out;
				pe_current->dout1_v = pe_current->outTableBuffer1.out_v;
				pe_current->dout1_tag = pe_current->outTableBuffer1.out_tag;
			}
			if (pe_current->din2_v)
			{
				pe_current->outTableBuffer2.in = pe_current->din2;
				pe_current->outTableBuffer2.in_v = pe_current->din2_v;
				pe_current->outTableBuffer2.in_tag = pe_current->din2_tag;
				pe_current->outTableBuffer2.dataIn();
				pe_current->outTableBuffer2.dataOut();
				pe_current->dout2 = pe_current->outTableBuffer2.out;
				pe_current->dout2_v = pe_current->outTableBuffer2.out_v;
				pe_current->dout2_tag = pe_current->outTableBuffer2.out_tag;
			}
			//bypass完成，清空valid值
			pe_current->din1_v = 0; pe_current->din1 = 0; pe_current->din1_tag = 0;						
			pe_current->din2_v = 0; pe_current->din2 = 0; pe_current->din2_tag = 0;
			pe_current->bin_v = 0; pe_current->bin = 0; pe_current->bin_tag = 0;
			
			
			outfile << "pe[" << pe_index_current << "]是旁路模式" << endl;
			outfile << "PE[" << pe_current->config_reg.front()[1] << "]的输出是：dout1: " << pe_current->dout1 << " dout1_v: " << pe_current->dout1_v << " dout1_tag: " << pe_current->dout1_tag << endl;
			outfile << "                                        " << "dout2: " << pe_current->dout2 << " dout2_v: " << pe_current->dout2_v << " dout2_tag: " << pe_current->dout2_tag << endl;
			//outfile << "                                        " << "bout: " << pe_current->bout << " bout_v: " << pe_current->bout_v << " bout_tag: " << pe_current->bout_tag << endl;
		}
		else if (mode == 1)//transmit
		{
			//默认只使用din1和dout1
			//需要在tag模式下工作
			
			//出数
			outfile << "pe[" << pe_index_current << "]处于中转模式...." << endl;
			pe_sim_step3(pe_current);//排除了LE,SE等不需要ob出数的操作和PE处于旁路和中转模式下的ob出数操作
			pe_sim_step_connect(pe_current);
			if (pe_current->bin_v & pe_current->loc_reg_v)
			{
				if (pe_current->bin)
				{
					pe_current->outbuffer1_in = pe_current->loc_reg;
					pe_current->outbuffer1_in_v = pe_current->loc_reg_v;
					//默认使用ob1
					(pe_current->outBuffer1).dataIn(pe_current->outbuffer1_in, pe_current->outbuffer1_in_v);
					
					if (port_fanout[pe_index_current][2] == 1)   //一拖一
					{
						(pe_current->outBuffer1).dataOut(pe_current->dout1, pe_current->dout1_v);
						//(pe_current->outBuffer1).outputBuffer.pop();
					}
				}
			}
			
			//入数			
			if (pe_current->din1_v)
			{
				pe_current->loc_reg = pe_current->din1;
				//pe_current->loc_reg_tag = pe_current->din1_tag;
				pe_current->loc_reg_v = pe_current->din1_v;
			}
			
			outfile << "中转模式下只有输出1有结果" << endl;
			outfile << "pe[" << pe_index_current << "]的输出结果: " << "dout1: " << pe_current->dout1 << "dout1_v: " << pe_current->dout1_v << endl;
		}
		else if (mode == 2)//calculate
		{
			outfile << "pe[" << pe_index_current << "]处于计算模式...." << endl;

			pe_sim_step2(pe_current);

			pe_sim_step3(pe_current);
			pe_sim_step_connect(pe_current);
			pe_sim_step1(pe_current);
			
			/*
			if (1)
			{
				if (is_din1_float != 0 && is_din2_float != 0 && is_bin_float != 0)// 111
				{
					//last PE出数
					if (is_din1_float)//din1非悬空
					{
						//din1 from PE ob出数
						pe_sim_step3(pe[din1_from_index], din1_from_port);
					}
					if (is_din2_float)//din2非悬空
					{
						//din2 from PE ob出数
						pe_sim_step3(pe[din2_from_index], din2_from_port);
					}
					if (is_bin_float)//bin非悬空
					{
						//bin from PE ob出数
						pe_sim_step3(pe[bin_from_index], bin_from_index);
					}
					//data trans
					pe_sim_step_connect(pe_current);
					//data store
					pe_sim_step1(pe_current);
					pe_sim_step2(pe_current);
				}
				else if (is_din1_float == 0 && is_din2_float == 0 && is_bin_float != 0)//001
				{
					//场景是：bin输入一个激活信号，loc_reg在激活信号下输出一个常值


				}
				else if (is_din1_float != 0 && is_din2_float != 0 && is_bin_float == 0)//110
				{
					//last PE出数
					if (is_din1_float)//din1非悬空
					{
						//din1 from PE ob出数
						pe_sim_step3(pe[din1_from_index], din1_from_port);
					}
					if (is_din2_float)//din2非悬空
					{
						//din2 from PE ob出数
						pe_sim_step3(pe[din2_from_index], din2_from_port);
					}
					if (is_bin_float)//bin非悬空
					{
						//bin from PE ob出数
						pe_sim_step3(pe[bin_from_index], bin_from_index);
					}
					//data trans
					pe_sim_step_connect(pe_current);
					//data store
					pe_sim_step1(pe_current);
					pe_sim_step2(pe_current);
				}
				else if (is_din1_float != 0 && is_din2_float == 0 && is_bin_float == 0)//100
				{
					//last PE出数
					if (is_din1_float)//din1非悬空
					{
						//din1 from PE ob出数
						pe_sim_step3(pe[din1_from_index], din1_from_port);
					}
					if (is_din2_float)//din2非悬空
					{
						//din2 from PE ob出数
						pe_sim_step3(pe[din2_from_index], din2_from_port);
					}
					if (is_bin_float)//bin非悬空
					{
						//bin from PE ob出数
						pe_sim_step3(pe[bin_from_index], bin_from_index);
					}
					//data trans
					pe_sim_step_connect(pe_current);
					//data store
					pe_sim_step1(pe_current);
					pe_sim_step2(pe_current);
				}
				else if (is_din1_float == 0 && is_din2_float != 0 && is_bin_float == 0)//010
				{
					//last PE出数
					if (is_din1_float)//din1非悬空
					{
						//din1 from PE ob出数
						pe_sim_step3(pe[din1_from_index], din1_from_port);
					}
					if (is_din2_float)//din2非悬空
					{
						//din2 from PE ob出数
						pe_sim_step3(pe[din2_from_index], din2_from_port);
					}
					if (is_bin_float)//bin非悬空
					{
						//bin from PE ob出数
						pe_sim_step3(pe[bin_from_index], bin_from_index);
					}
					//data trans
					pe_sim_step_connect(pe_current);
					//data store
					pe_sim_step1(pe_current);
					pe_sim_step2(pe_current);
				}
				else if (is_din1_float != 0 && is_din2_float == 0 && is_bin_float != 0)//101
				{
					//last PE出数
					if (is_din1_float)//din1非悬空
					{
						//din1 from PE ob出数
						pe_sim_step3(pe[din1_from_index], din1_from_port);
					}
					if (is_din2_float)//din2非悬空
					{
						//din2 from PE ob出数
						pe_sim_step3(pe[din2_from_index], din2_from_port);
					}
					if (is_bin_float)//bin非悬空
					{
						//bin from PE ob出数
						pe_sim_step3(pe[bin_from_index], bin_from_index);
					}
					//data trans
					pe_sim_step_connect(pe_current);
					//data store
					pe_sim_step1(pe_current);
					pe_sim_step2(pe_current);
				}
				else if (is_din1_float == 0 && is_din2_float != 0 && is_bin_float != 0)//011
				{
					//last PE出数
					if (is_din1_float)//din1非悬空
					{
						//din1 from PE ob出数
						pe_sim_step3(pe[din1_from_index], din1_from_port);
					}
					if (is_din2_float)//din2非悬空
					{
						//din2 from PE ob出数
						pe_sim_step3(pe[din2_from_index], din2_from_port);
					}
					if (is_bin_float)//bin非悬空
					{
						//bin from PE ob出数
						pe_sim_step3(pe[bin_from_index], bin_from_index);
					}
					//data trans
					pe_sim_step_connect(pe_current);
					//data store
					pe_sim_step1(pe_current);
					pe_sim_step2(pe_current);
				}
			}
			*/
			

		}
		else if (mode == 3)//bypass and calculate
		{
		}
		else if (mode == 4)//transmit and calculate
		{
			//
			outfile << "pe[" << pe_index_current << "]处于计算+中转模式...." << endl;
			if (is_din1_float != 0 && is_din2_float != 0 && is_bin_float != 0)// 111
			{	
				pe_sim_step2(pe_current);

				pe_sim_step3(pe_current);
				pe_sim_step_connect(pe_current);
				//中转入数
				if (pe_current->din2_v)
				{
					pe_current->loc_reg = pe_current->din2;
				}
				pe_sim_step1(pe_current);
				

				
			}
			else if (is_din1_float != 0 && is_din2_float != 0 && is_bin_float == 0)// 110
			{
				pe_sim_step2(pe_current);

				pe_sim_step3(pe_current);
				pe_sim_step_connect(pe_current);
				//中转入数
				if (pe_current->din2_v)
				{
					pe_current->loc_reg = pe_current->din2;
				}
				pe_sim_step1(pe_current);
			}
		}
		/*
		else if (mode == 5)//tag 绑定模式
		{
			//tag绑定模式默认输入1是tag值，输入2是data值
			struct tmp { bool valid; int data; };
			outfile << "pe[" << pe_index_current << "]处于tag绑定模式...." << endl;
			//ack清零
			pe_current->ack2in1port = 0;
			pe_current->ack2in2port = 0;
			//出口数据清零
			pe_current->dout1_v = 0;
			pe_current->dout2_v = 0;
			//数据入FIFO
			if (pe_current->din1_v && pe_current->din2_v)
			{

				 TableLine4Fifo tmp1,tmp2;
				if (!pe_current->tableBuffer_fifo_full1())
				{
					tmp1.valid = pe_current->din1_v;
					tmp1.data = pe_current->din1;
					pe_current->tableBuffer_fifo1.push(tmp1);
					pe_current->ack2in1port = 1;
					//fifo1入数完成，入口数据清零
					pe_current->din1_v = 0;
				}
				if (!pe_current->tableBuffer_fifo_full2())
				{
					tmp2.valid = pe_current->din2_v;
					tmp2.data = pe_current->din2;
					pe_current->tableBuffer_fifo2.push(tmp2);
					pe_current->ack2in2port = 0;
					//fifo2入数完成，入口数据清零
					pe_current->din2_v = 0;
				}
				
			}

			//匹配后的数据-输出
			if (!pe_current->tableBuffer_fifo1.empty() && !pe_current->tableBuffer_fifo2.empty())
			{
				pe_current->outbuffer1_in = pe_current->tableBuffer_fifo2.front().data;
				pe_current->outbuffer1_in_tag = pe_current->tableBuffer_fifo1.front().data;
				pe_current->outbuffer1_in_v = pe_current->tableBuffer_fifo1.front().valid;
				pe_current->outTableBuffer1.dataIn();
				//pe_current->outTableBuffer1.dataOut();//只是从ob中拿出数据放置在输出端口，没有清空ob中的数据
			}
		}
		*/
		
	}
	else
	{
		//不需要进行tag的处理
		if (mode == 0)//bypass
		{
			outfile2 << "当前PE处于旁路模式" << endl;
			pe_sim_step_connect(pe_current);
			if (pe_current->din1_v)
			{
				pe_current->outbuffer1_in = pe_current->din1;
				pe_current->outbuffer1_in_v = 1;
				if (pe_current->outBuffer1.dataIn(pe_current->outbuffer1_in, pe_current->outbuffer1_in_v))
				{
					//out
					if (port_fanout[pe_index_current][2] == 1)   //一拖一
					{
						pe_current->outBuffer1.dataOut(pe_current->dout1, pe_current->dout1_v);
						//pe_current->outBuffer1.outputBuffer.pop();
						pe_current->ack_outbuffer12alu = 1;
					}
				}

				//处理完成，入口数据清零
				pe_current->din1_v = 0;

			}
			if (pe_current->din2_v)
			{
				pe_current->outbuffer2_in = pe_current->din2;
				pe_current->outbuffer2_in_v = 1;
				if (pe_current->outBuffer2.dataIn(pe_current->outbuffer2_in, pe_current->outbuffer2_in_v))
				{
					//out
					if (port_fanout[pe_index_current][3] == 1)   //一拖一
					{
						pe_current->outBuffer2.dataOut(pe_current->dout2, pe_current->dout2_v);
						//(pe_current->outBuffer2).outputBuffer.pop();
						pe_current->ack_outbuffer22alu = 1;
					}
				}

				//处理完成，入口数据清零
				pe_current->din2_v = 0;
			}
			outfile2 << "PE[" << pe_index_current << "]输出值" << endl;
			outfile2 << setw(15) << "out1_v" << setw(15) << "out1" << endl;
			outfile2 << setw(15) << pe_current->dout1_v << setw(15) << pe_current->dout1 << endl;
			outfile2 << setw(15) << "out2_v" << setw(15) << "out2" << endl;
			outfile2 << setw(15) << pe_current->dout2_v << setw(15) << pe_current->dout2 << endl;
			outfile2 << setw(15) << "out3_v" << setw(15) << "out3" << endl;
			outfile2 << setw(15) << pe_current->bout_v << setw(15) << pe_current->bout << endl;
		}
		else if (mode == 1)//transmit     只过outbuffer，顺序应该为3c12
		{
			//开始计算前各子模块出口数据清零
			pe_current->outbuffer1_in_v = 0; pe_current->outbuffer2_in_v = 0; pe_current->outbuffer3_in_v = 0;

			//中转模式只允许中转一个操作数，默认使用din1
			pe_sim_step3_no_tag(pe_current);
			pe_sim_step_connect(pe_current);


			
			//进入outbuffer
			if (pe_current->config_reg.front()[15])//ob1_from 非悬空
			{
				if (pe_current->config_reg.front()[15] == 2)//ob1 from lr
				{
					if (pe_current->bin_v)
					{
						if (pe_current->bin & pe_current->loc_reg_v)
						{
							pe_current->outbuffer1_in = pe_current->loc_reg;
							pe_current->outbuffer1_in_v = pe_current->loc_reg_v;
							//处理完成，入口数据清零
							pe_current->bin_v = 0;
							pe_current->bin = 0;
						}
					}
				}
			}
			//outbuffer入数
			if (pe_current->outbuffer1_in_v)
			{
				pe_current->outBuffer1.dataIn(pe_current->outbuffer1_in, pe_current->outbuffer1_in_v);
				if (pe_current->outBuffer1.isOutBufferFull())
				{
					pe_current->ack_outbuffer12alu = 0;
					pe_current->ack2in3port = 0;   //中转节点不过inbuffer，inbuffer的bp即为outbuffer的bp，注意下一个PE的step3是需要同时更新两个bp信号！
					//pe_current->ack2in1port = 0;   由于in1不仅buffer而是进locreg，需不需要堵住呢
				}
				else
				{
					pe_current->ack_outbuffer12alu = 1;
					pe_current->ack2in3port = 1;   //中转节点不过inbuffer，inbuffer的bp即为outbuffer的bp
					//pe_current->ack2in1port = 0;   由于in1不仅buffer而是进locreg，需不需要堵住呢
				}

			}

			//输入
			if (pe_current->din1_v)
			{
				pe_current->loc_reg = pe_current->din1;
				pe_current->loc_reg_v = pe_current->din1_v;
				//处理完成，入口数据清零
				pe_current->din1_v = 0;
			}

			//ob打印
			//中间变量
			queue<OutBuffer_no_tag> ob1_tmp = pe_current->outBuffer1.outputBuffer;
			queue<OutBuffer_no_tag> ob2_tmp = pe_current->outBuffer2.outputBuffer;
			if (!ob1_tmp.empty())
			{
				outfile << "outbuffer1值打印" << endl;
				outfile << setw(15) << "outbuffer1_v" << setw(15) << "outbuffer1" << endl;
				outfile << setw(15) << (ob1_tmp.front()).valid << setw(15) << (ob1_tmp.front()).data << endl;
			}

			if (!ob2_tmp.empty())
			{
				outfile << "outbuffer2值打印" << endl;
				outfile << setw(15) << "outbuffer2_v" << setw(15) << "outbuffer2" << endl;
				outfile << setw(15) << (ob2_tmp.front()).valid << setw(15) << (ob2_tmp.front()).data << endl;
			}

			//打印outbuffer中的内容
			//outfile << "outbuffer值打印" << endl;
			if (pe_current->outbuffer3_v)
			{
				outfile << "outbuffer3值打印" << endl;
				outfile << setw(15) << "outbuffer3_v" << setw(15) << "outbuffer3" << endl;
				outfile << setw(15) << pe_current->outbuffer3_v << setw(15) << pe_current->outbuffer3 << endl;
			}
			

			/*
			if (pe_current->bin_v)
			{
				if (pe_current->bin & pe_current->loc_reg_v)//bool值有效才输出
				{
					pe_current->dout1 = pe_current->loc_reg;
					pe_current->dout1_v = pe_current->loc_reg_v;
					//pe_current->outBuffer1.isOutBufferFull();
					//pe_current->outBuffer1In();
					//pe_current->peExportCtr1();
					//处理完成，入口数据清零
					pe_current->bin_v = 0;
					pe_current->bin = 0;
				}
			}	
			*/
			
			outfile2 << "PE[" << pe_index_current << "]输出值" << endl;
			outfile2 << setw(15) << "dout1_v" << setw(15) << "dout1" << endl;
			outfile2 << setw(15) << pe_current->dout1_v << setw(15) << pe_current->dout1 << endl;

		}
		else if (mode == 2)//calculate
		{

			//if (pe_current->config_reg.front()[24] == 1)//顺序仿真各个stage
			//{				
			//	pe_sim_step3_no_tag(pe_current);
			//	pe_sim_step_connect(pe_current);
			//	pe_sim_step1_no_tag(pe_current);

			//	pe_sim_step2_no_tag(pe_current);

			//}
			if (pe_current->config_reg.front()[1] == 5)//pe5开始的组合路径，联仿完PE5 PE4 PE3
			{
				
				//PE5的out_v在PESIM中清过了，但PE4和3没有
				pe[5]->dout1_v = 0;
				pe[5]->dout2_v = 0;
				pe[5]->bout_v = 0;
				pe[5]->dout1 = 0;
				pe[5]->dout2 = 0;
				pe[5]->bout = 0;
				pe_sim_step3_no_tag(pe[5]);
				pe_sim_step_connect(pe[5]);
				pe_sim_step1_no_tag(pe[5]);
				pe_sim_step2_no_tag(pe[5]);

				//JoinBpSimProcess(joinbp[2]);
				LbeginSimProcess(lbegin[1]);

				//pe4,注意同样要有清out_v的步骤
				pe[4]->dout1_v = 0;
				pe[4]->dout2_v = 0;
				pe[4]->bout_v = 0;
				pe[4]->dout1 = 0;
				pe[4]->dout2 = 0;
				pe[4]->bout = 0;
				pe_sim_step3_no_tag(pe[4]);
				pe_sim_step_connect(pe[4]);
				pe_sim_step1_no_tag(pe[4]);
				pe_sim_step2_no_tag(pe[4]);

				pe[3]->dout1_v = 0;
				pe[3]->dout2_v = 0;
				pe[3]->bout_v = 0;
				pe[3]->dout1 = 0;
				pe[3]->dout2 = 0;
				pe[3]->bout = 0;
				pe_sim_step3_no_tag(pe[3]);
				pe_sim_step_connect(pe[3]);
				pe_sim_step1_no_tag(pe[3]);
				pe_sim_step2_no_tag(pe[3]);
				
			}
			else
			{
				//s3->sc->s1->s2
				pe_sim_step2_no_tag(pe_current);

				pe_sim_step3_no_tag(pe_current);
				pe_sim_step_connect(pe_current);
				pe_sim_step1_no_tag(pe_current);
			}
			
		}
									
		else if (mode == 3)//bypass and calculate
		{

		}
		else if (mode == 5)//tag 绑定模式
		{
			//tag绑定模式默认输入1是tag值，输入2是data值
			outfile << "pe[" << pe_index_current << "]处于tag绑定模式...." << endl;
			outfile2 << "Tag Bing Mode" << endl;
			//开始仿真前输出端口清零
			pe_current->dout1_v = 0; pe_current->dout2_v = 0; pe_current->bout_v = 0;
			pe_current->dout1_tag = 0; pe_current->dout2_tag = 0; pe_current->bout_tag = 0;
			//s3->sc

			//fifo输出
			outfile << "PE[" << pe_index_current << "]infifo1的大小" << endl;
			outfile << pe_current->tableBuffer_fifo1.size() << endl;
			outfile << "PE[" << pe_index_current << "]infifo2的大小" << endl;
			outfile << pe_current->tableBuffer_fifo2.size() << endl;

			pe_sim_step3_no_tag(pe_current);//没有被使用
			pe_sim_step_connect(pe_current);//ti的值可以直接获取

			if (pe_current->din1 == pe_current->last_t4bind)
			{	//接收同一个ti节点的数据
				pe_current->din1_v = 0;
			}
			//ack清零
			//pe_current->ack2in1port = 0;
			//pe_current->ack2in2port = 0;
			//出口数据清零
			pe_current->dout1_v = 0;
			pe_current->dout2_v = 0;
			//数据入FIFO			
			if (pe_current->din1_v)
			{
				TableLine4Fifo tmp1;
				if (!pe_current->tableBuffer_fifo_full1())
				{
					tmp1.valid = pe_current->din1_v;
					tmp1.data = pe_current->din1;
					pe_current->tableBuffer_fifo1.push(tmp1);
					if(pe_current->tableBuffer_fifo_full1())
						pe_current->ack2in1port = 0;
					else
						pe_current->ack2in1port = 1;
					//fifo1入数完成，入口数据清零
					pe_current->din1_v = 0;
					//last_t4bind设置
					pe_current->last_t4bind = pe_current->din1;
				}
			}
			if (pe_current->din2_v)
			{
				TableLine4Fifo tmp2;
				if (!pe_current->tableBuffer_fifo_full2())
				{
					tmp2.valid = pe_current->din2_v;
					tmp2.data = pe_current->din2;
					pe_current->tableBuffer_fifo2.push(tmp2);
					if (pe_current->tableBuffer_fifo_full2())
						pe_current->ack2in2port = 0;
					else
						pe_current->ack2in2port = 1;
					//fifo2入数完成，入口数据清零
					pe_current->din2_v = 0;

				}
			}


			pe_current->outbuffer1_in_v = 0;
			//匹配后的数据-输出
			if(!pe_current->tableBuffer_fifo1.empty() && !pe_current->tableBuffer_fifo2.empty()&& pe_current->ack_outbuffer12alu)
			{
				pe_current->outbuffer1_in = pe_current->tableBuffer_fifo2.front().data;
				pe_current->outbuffer1_in_tag = pe_current->tableBuffer_fifo1.front().data;
				pe_current->outbuffer1_in_v = pe_current->tableBuffer_fifo1.front().valid;
				//数据导入到outTableBuffer类中去
				pe_current->outTableBuffer1.in = pe_current->outbuffer1_in;
				pe_current->outTableBuffer1.in_v = pe_current->outbuffer1_in_v;
				pe_current->outTableBuffer1.in_tag = pe_current->outbuffer1_in_tag;

				pe_current->outTableBuffer1.dataIn();
				//输出结束，FIFO弹出一个数据
				pe_current->tableBuffer_fifo1.pop();
				pe_current->tableBuffer_fifo2.pop();
				//出数后fifo不满
				pe_current->ack2in1port = 1;
				pe_current->ack2in2port = 1;
				//检测Outtablebuffer是否满，若满则置起来ack_outbuffer
				if (pe_current->outTableBuffer1.isOutBufferFull())
				{
					pe_current->ack_outbuffer22alu = 0;
				}
				else
					pe_current->ack_outbuffer22alu = 1;
			}

			outfile2 << "PE[" << pe_index_current << "]FIFO输出值" << endl;
			outfile2 << setw(15) << "ob1_in_t" << setw(15) << "ob1_in_v" << setw(15) << "ob1_in" << endl;
			outfile2 << setw(15) << pe_current->outbuffer1_in_tag << setw(15) << pe_current->outbuffer1_in_v
				<< setw(15) << pe_current->outbuffer1_in << endl;
			/*outfile2 << "tag bind mode下输出" << endl;
			outfile2 << setw(15) << "dout1_t" << setw(15) << "dout1_v" << setw(15) << "dout1" << endl;
			outfile2 << setw(15) << pe_current->dout1_tag << setw(15) << pe_current->dout1_v << setw(15) << pe_current->dout1 << endl;*/
		}
	}
	
}


void pe_outbuffer_out_no_tag(ProcessingElement* pe_current,uint32_t ob_index)
{
	int pe_index_current = pe_current->config_reg.front()[1];//当前正在处理的PE标号

	//step3---output buffer出数仿真
	//开始计算之前出口处的数据清零
	/*
	pe_current->dout1_v = 0;
	pe_current->dout1 = 0;
	pe_current->dout2_v = 0;
	pe_current->dout2 = 0;
	pe_current->bout_v = 0;
	pe_current->bout = 0;
	*/

	if (ob_index == 0)//ob1需要出数
	{
		//ob1出数
		if (pe_current->outBuffer1.outputBuffer.size() != 0)
		{		
			if (port_fanout[pe_index_current][2] <= 1)   //一拖一
			{
				pe_current->outBuffer1.dataOut(pe_current->dout1, pe_current->dout1_v);
				//pe_current->outBuffer1.outputBuffer.pop();
				pe_current->ack_outbuffer12alu = 1;

				//若为中转模式，inbuffer的bp也跟着obuffer的bp改变
				uint mode = pe_current->config_reg.front()[18];
				if (mode == 1)    //中转模式，no_tag
				{
					//pe_current->ack2in1port = 1;   
					pe_current->ack2in3port = 1;    
				}
			}
		}
	}	
	else if (ob_index == 1)//ob2需要出数
	{
		//ob2出数
		if (pe_current->outBuffer2.outputBuffer.size() != 0)
		{			
			if (port_fanout[pe_index_current][3] <= 1)   //一拖一
			{
				pe_current->outBuffer2.dataOut(pe_current->dout2, pe_current->dout2_v);
				//pe_current->outBuffer2.outputBuffer.pop();
				pe_current->ack_outbuffer22alu = 1;
				//若为中转模式，inbuffer的bp也跟着obuffer的bp改变
				uint mode = pe_current->config_reg.front()[18];
				if (mode == 1)    //中转模式，no_tag
				{
					//pe_current->ack2in2port = 1;
					pe_current->ack2in3port = 1;
				}
			}
		}
	}
	
	else if (ob_index == 2)//ob3需要出数
	{
		//ob3出数
		if (pe_current->outbuffer3_v)
		{
			if (port_fanout[pe_index_current][4] <= 1)   //一拖一
			{
				pe_current->bout = pe_current->outbuffer3;
				pe_current->bout_v = pe_current->outbuffer3_v;
				pe_current->ack_outbuffer32alu = 1;
				pe_current->outbuffer3_v = 0;
			}
		}
	}
	

	//检测是否是end节点
	if (pe_current->config_reg.front()[17])
	{
		if (pe_current->dout1_v | pe_current->dout2_v)
		{
			pe_current->bout = 1;
			pe_current->bout_v = 1;//使用out3_v出口作为end节点信号的出口（要在out3口没有被使用的情况下才能成立）
		}

	}
}

void pe_sim_step3_no_tag(ProcessingElement* pe_current)
{
	int pe_index_current = pe_current->config_reg.front()[1];//当前正在处理的PE标号
	int din1_from_index = pe_current->config_reg.front()[3];
	int din2_from_index = pe_current->config_reg.front()[6];
	int bin_from_index = pe_current->config_reg.front()[9];
	//in_port
	int din1_from_port = pe_current->config_reg.front()[4];
	int din2_from_port = pe_current->config_reg.front()[7];
	int bin_from_port = pe_current->config_reg.front()[10];
	//in_flag
	int din1_from_flag = pe_current->config_reg.front()[5];
	int din2_from_flag = pe_current->config_reg.front()[8];
	int bin_from_flag = pe_current->config_reg.front()[11];
	//判断是否悬空
	bool is_din1_float = din1_from_flag;//0--悬空；非0--非悬空
	bool is_din2_float = din2_from_flag;
	bool is_bin_float = bin_from_flag;
	//判断当前PE各个输入的来源情况
	//in1port
	if (din1_from_flag == 2)//PE
	{
		if (pe_current->ack2in1port == 1)        //当前PE inbuffer未满时，上一PE可以出数
		{
			if (pe[din1_from_index]->config_reg.front()[18] == 0)
			{
				//上一个PE处于旁路模式不需要再额外的ob出数,中转 模式仍需要正常出数
			}
			else
			{
				pe_outbuffer_out_no_tag(pe[din1_from_index], din1_from_port);
				/*
				if (din1_from_port == 0)         //根据来源的PE和其端口维护相应的obuffer的bp，注意PE的端口解析时减过1
				{
					pe[din1_from_index]->ack_outbuffer12alu = 1;    //出数后不满
				}
				else if (din1_from_port == 1)
				{
					pe[din1_from_index]->ack_outbuffer22alu = 1;
				}
				else if (din1_from_port == 2)                 //这种情况应该不会发生
				{
					pe[din1_from_index]->ack_outbuffer32alu = 1;
				}
				*/
			}
		}
		
	}
	else if (din1_from_flag == 1)//LE
	{
		//不需要额外增加LE的outbuffer出数步骤
	}
	else if (din1_from_flag == 3)//lbegin
	{
	}
	else if (din1_from_flag == 9)//ti
	{
		//if (pe_current->ack2in1port == 1)
			//pe_outbuffer_out_no_tag(pe[din1_from_index], din1_from_port);
	}
	else if (din1_from_flag == 10)//se
	{
	}
	//in2port
	if (din2_from_flag == 2)//PE
	{
		if (pe_current->ack2in2port == 1)   //查本级PE的inbuffer是否满了
		{
			if (pe[din2_from_index]->config_reg.front()[18] == 0)
			{
				//上一个PE处于旁路模式不需要再额外的ob出数,中转模式仍需要正常出数
			}
			else
			{
				pe_outbuffer_out_no_tag(pe[din2_from_index], din2_from_port);
				/*
				if (din2_from_port == 0)         //根据来源的PE和其端口维护相应的obuffer的bp，注意PE的端口解析时减过1
				{
					pe[din2_from_index]->ack_outbuffer12alu = 1;    //出数后不满
				}
				else if (din2_from_port == 1)
				{
					pe[din2_from_index]->ack_outbuffer22alu = 1;
				}
				else if (din2_from_port == 2)                 //这种情况应该不会发生
				{
					pe[din2_from_index]->ack_outbuffer32alu = 1;
				}
				*/
			}
		}
		
	}
	else if (din2_from_flag == 1)//LE
	{
		//不需要额外增加LE的outbuffer出数步骤
	}
	else if (din2_from_flag == 3)//lbegin
	{
	}
	else if (din2_from_flag == 9)//ti
	{
	}
	else if (din2_from_flag == 10)//se
	{
	}
	//in3port
	if (bin_from_flag == 2)//PE
	{
		if (pe_current->ack2in3port == 1)   //中转模式中应该和obuffer的bp相同
		{
			if (pe[bin_from_index]->config_reg.front()[18] == 0 | pe[bin_from_index]->config_reg.front()[18] == 1)
			{
				//上一个PE处于中转和旁路模式不需要再额外的ob出数
			}
			else
			{
				pe_outbuffer_out_no_tag(pe[bin_from_index], bin_from_port);
				/*
				if (bin_from_port == 0)         //根据来源的PE和其端口维护相应的obuffer的bp，注意PE的端口解析时减过1
				{
					pe[bin_from_index]->ack_outbuffer12alu = 1;    //出数后不满
				}
				else if (bin_from_port == 1)
				{
					pe[bin_from_index]->ack_outbuffer22alu = 1;
				}
				else if (bin_from_port == 2)                 
				{
					pe[bin_from_index]->ack_outbuffer32alu = 1;
				}
				*/
			}
		}
		
	}
	else if (bin_from_flag == 1)//LE
	{
		//不需要额外增加LE的outbuffer出数步骤
	}
	else if (bin_from_flag == 3)//lbegin
	{
	}
	else if (bin_from_flag == 9)//ti
	{
	}
	else if (bin_from_flag == 10)//se
	{
	}
}

void pe_sim_step2_no_tag(ProcessingElement* pe_current)
{
	//step2---ib出数->ALU仿真->ob入数
	//开始计算前各子模块出口数据清零
	pe_current->alu_out_v = 0;pe_current->alu_out_b_v = 0;
	pe_current->inbuffer1_out_v = 0; pe_current->inbuffer2_out_v = 0; pe_current->inbuffer3_out_v = 0;
	pe_current->outbuffer1_in_v = 0; pe_current->outbuffer2_in_v = 0; pe_current->outbuffer3_in_v = 0;
	//开始前ob中的内容清零，只让ob中的把内容保持一个周期的生存周期
	//pe_current->outbuffer1_v = 0; pe_current->outbuffer2_v = 0; pe_current->outbuffer3_v = 0;

	//ib1 out 
	//首先查询obuffer的bp是否为0（0表示obuffer满）
	//inbuffer出数后也需要维护inbuffer的bp
	if (pe_current->ack_outbuffer12alu == 1)     //查询obuffer是否已满
	{
		if (pe_current->inBuffer1.inputBuffer.size() != 0)
		{
			pe_current->inBuffer1.dataOut(pe_current->inbuffer1_out, pe_current->inbuffer1_out_v);
			//pe_current->inbuffer1_out = pe_current->inBuffer1.inputBuffer.front().data;
			//pe_current->inbuffer1_out_v = pe_current->inBuffer1.inputBuffer.front().valid;
			pe_current->ack2in1port = 1;                                                  //inbuffer出数后由于inbuffer未满，bp=1
		}
		//pe_current->inBuffer1.dataIn(pe_current->inbuffer1_out, pe_current->inbuffer1_out_v);   更好的表达方式！
	}
		//ib2 out
	if (pe_current->ack_outbuffer22alu == 1)     //未满
	{
		if (pe_current->inBuffer2.inputBuffer.size() != 0)
		{
			pe_current->inBuffer2.dataOut(pe_current->inbuffer2_out, pe_current->inbuffer2_out_v);
			//pe_current->inbuffer2_out = pe_current->inBuffer2.inputBuffer.front().data;
			//pe_current->inbuffer2_out_v = pe_current->inBuffer2.inputBuffer.front().valid;
			pe_current->ack2in2port = 1;
		}
	}
		//ib3 out
	if (pe_current->ack_outbuffer32alu == 1)     //未满
	{
		if (pe_current->inbuffer3_v)
		{
			pe_current->inbuffer3_out = pe_current->inbuffer3;
			pe_current->inbuffer3_out_v = pe_current->inbuffer3_v;
			pe_current->ack2in3port = 1;
			pe_current->inbuffer3_v = 0;     //清inbuffer3
		}
	}
	
	//打印中间值1
	outfile2 << "PE[" << pe_current->config_reg.front()[1] << "]中间值1" << endl;
	outfile2 << setw(15) << "ib1_out_t" << setw(15) << "ib1_out_v" << setw(15) << "ib1_out" << setw(15) << "ib2_out_t" << setw(15) << "ib2_out_v"
		<< setw(15) << "ib2_out" << setw(15) << "ib3_out_t" << setw(15) << "ib3_out_v" << setw(15) << "ib3_out" << endl;
	outfile2 << setw(15) << 0 << setw(15) << pe_current->inbuffer1_out_v
		<< setw(15) << pe_current->inbuffer1_out << setw(15) << 0
		<< setw(15) << pe_current->inbuffer2_out_v << setw(15) << pe_current->inbuffer2_out
		<< setw(15) << 0 << setw(15) << pe_current->inbuffer3_out_v
		<< setw(15) << pe_current->inbuffer3_out << endl;
	

	//ALU计算
	//取数到端口
	if (pe_current->config_reg.front()[13] == 0)//alu_in1_from ib1
	{
		if (pe_current->inbuffer1_out_v)
		{
			pe_current->alu_in1 = pe_current->inbuffer1_out;
			pe_current->alu_in1_v = pe_current->inbuffer1_out_v;
		}

	}
	else if (pe_current->config_reg.front()[13] == 1) //alu_in1_from lr
	{
		if (pe_current->loc_reg_v)
		{
			pe_current->alu_in1 = pe_current->loc_reg;
			pe_current->alu_in1_v = pe_current->loc_reg_v;
		}

	}

	if (pe_current->config_reg.front()[14] == 0)//alu_in2_from ib2
	{
		if (pe_current->inbuffer2_out_v)
		{
			pe_current->alu_in2 = pe_current->inbuffer2_out;
			pe_current->alu_in2_v = pe_current->inbuffer2_out_v;
		}

	}
	else if (pe_current->config_reg.front()[14] == 1)//alu_in2_from lr
	{
		if (pe_current->loc_reg_v)
		{
			pe_current->alu_in2 = pe_current->loc_reg;
			pe_current->alu_in2_v = pe_current->loc_reg_v;
		}

	}
	//alu运算
	//alu仿真结束后该子模块入口数据不能清零，有一个数据提前到来没有激活计算，要在出口处keep住
	if (pe_current->config_reg.front()[2] == 8)//mux操作。特殊对待
	{
		if (pe_current->inbuffer3_out_v)
		{
			if (pe_current->inbuffer3_out == 0)//选择in2
			{
				if (pe_current->alu_in2_v)//din2的输出结果有效就可以进行mux操作
				{
					pe_current->ALU(pe_current->config_reg.front()[2], pe_current->alu_in1, pe_current->alu_in2, pe_current->inbuffer3_out);					
					//alu仿真完成，入口数据清零
					pe_current->alu_in1_v = 0;
					pe_current->alu_in2_v = 0;
					pe_current->inbuffer1_out_v = 0;
					pe_current->inbuffer2_out_v = 0;
					pe_current->inbuffer3_out_v = 0;
					//inBuffer数据弹出，请补充
					//pe_current->inbuffer1_v = 0; pe_current->inbuffer2_v = 0; pe_current->inbuffer3_v = 0;					
				}

			}
			else if (pe_current->inbuffer3_out == 1)//选择in1
			{
				if (pe_current->alu_in1_v)//din1有效就可以进行mux操作
				{
					pe_current->ALU(pe_current->config_reg.front()[2], pe_current->alu_in1, pe_current->alu_in2, pe_current->inbuffer3_out);
					//仿真完成，入口数据清零
					pe_current->alu_in1_v = 0;
					pe_current->alu_in2_v = 0;
					pe_current->inbuffer1_out_v = 0;
					pe_current->inbuffer2_out_v = 0;
					pe_current->inbuffer3_out_v = 0;
					//inBuffer数据弹出，请补充
					//pe_current->inbuffer1_v = 0; pe_current->inbuffer2_v = 0; pe_current->inbuffer3_v = 0;					
				}				
			}

		}
	}
	else
	{
		//非mux操作
		if (pe_current->alu_in1_v & pe_current->alu_in2_v)//两个操作数都有效才开始计算
		{
			//alu 操作
			pe_current->ALU(pe_current->config_reg.front()[2], pe_current->alu_in1, pe_current->alu_in2, pe_current->inbuffer3_out);
			//仿真完成，入口数据清零
			pe_current->alu_in1_v = 0;
			pe_current->alu_in2_v = 0;
			pe_current->inbuffer1_out_v = 0;             //alu开始计算后清inbuffer的valid
			pe_current->inbuffer2_out_v = 0;
			pe_current->inbuffer3_out_v = 0;
			//inBuffer数据弹出，请补充
			//pe_current->inbuffer1_v = 0; pe_current->inbuffer2_v = 0; pe_current->inbuffer3_v = 0;			
		}
	}

	//打印中间值2
	outfile2 << "PE[" << pe_current->config_reg.front()[1] << "]中间值2" << endl;
	outfile2 << setw(15) << "alu_out_v" << setw(15) << "alu_out" << setw(15) << "alu_out_b_v" << setw(15) << "alu_out_b" << endl;
	outfile2 << setw(15) << pe_current->alu_out_v << setw(15) << pe_current->alu_out
		<< setw(15) << pe_current->alu_out_b_v << setw(15) << pe_current->alu_out_b << endl;

	//lr 仿真,要放在之前ob访问之前，因为pe_current->alu_out_v会被清零。
	if (pe_current->config_reg.front()[12] == 1)	//lr from alu
	{
		if (pe_current->alu_out_v)
		{
			pe_current->loc_reg = pe_current->alu_out;
			pe_current->loc_reg_v = 1;
		}
		//额外增加的loc reg 清零的功能
		if (pe_current->loc_reg == THRESHOLD)
		{
			pe_current->loc_reg = 0;
		}

	}
	//ob入数
	//入数后检查是否要改变bp
	if (1)
	{
		if (pe_current->config_reg.front()[15])//ob1_from 非悬空
		{
			//数据fetch到端口
			if (pe_current->config_reg.front()[15] == 1)//ob1 from alu
			{
				if (pe_current->alu_out_v)
				{
					pe_current->outbuffer1_in = pe_current->alu_out;
					pe_current->outbuffer1_in_v = pe_current->alu_out_v;
					pe_current->alu_out_v = 0;
				}
			}

			else if (pe_current->config_reg.front()[15] == 3)//ob1 from in1
			{
			}
			else if (pe_current->config_reg.front()[15] == 2)//ob1 from lr
			{
				if (pe_current->loc_reg_v)
				{
					pe_current->outbuffer1_in = pe_current->loc_reg;
					pe_current->outbuffer1_in_v = pe_current->loc_reg_v;
				}

			}
			//入数
			if (pe_current->outbuffer1_in_v)
			{
				pe_current->outBuffer1.dataIn(pe_current->outbuffer1_in, pe_current->outbuffer1_in_v);
				if (pe_current->outBuffer1.isOutBufferFull())
				{
					pe_current->ack_outbuffer12alu = 0;
				}
				else
					pe_current->ack_outbuffer12alu = 1;
			}

		}

		if (pe_current->config_reg.front()[16])//ob2非悬空
		{
			if (pe_current->config_reg.front()[16] == 1)//ob2 from alu
			{
				if (pe_current->alu_out_v)
				{
					pe_current->outbuffer2_in = pe_current->alu_out;
					pe_current->outbuffer2_in_v = pe_current->alu_out_v;
					pe_current->alu_out_v = 0;
				}
			}

			else if (pe_current->config_reg.front()[16] == 3)//ob2 from in2
			{
			}
			else if (pe_current->config_reg.front()[16] == 2)//ob2 from lr
			{
				if (pe_current->loc_reg_v)
				{
					pe_current->outbuffer2_in = pe_current->loc_reg;
					pe_current->outbuffer2_in_v = pe_current->loc_reg_v;
				}

			}
			//入数
			if (pe_current->outbuffer2_in_v)
			{
				pe_current->outBuffer2.dataIn(pe_current->outbuffer2_in, pe_current->outbuffer2_in_v);
				if (pe_current->outBuffer2.isOutBufferFull())
				{
					pe_current->ack_outbuffer22alu = 0;
				}
				else
					pe_current->ack_outbuffer22alu = 1;
			}

		}

		if (pe_current->config_reg.front()[15] == 0 && pe_current->config_reg.front()[16] == 0)//ob3非悬空，来自于alu
		{
			if (pe_current->alu_out_b_v)
			{
				pe_current->outbuffer3 = pe_current->alu_out_b;//ob3入数		
				pe_current->outbuffer3_v = pe_current->alu_out_b_v;
				pe_current->ack_outbuffer32alu = 0;
				pe_current->alu_out_b_v = 0;
			}
		}
	}
	

	//中间变量
	queue<OutBuffer_no_tag> ob1_tmp = pe_current->outBuffer1.outputBuffer;
	queue<OutBuffer_no_tag> ob2_tmp = pe_current->outBuffer2.outputBuffer;

	

	if (!ob1_tmp.empty())
	{
		outfile << "outbuffer1值打印" << endl;
		outfile << setw(15) << "outbuffer1_v" << setw(15) << "outbuffer1" << endl;
		outfile << setw(15) << (ob1_tmp.front()).valid << setw(15) << (ob1_tmp.front()).data << endl;
	}

	if (!ob2_tmp.empty())
	{
		outfile << "outbuffer2值打印" << endl;
		outfile << setw(15) << "outbuffer2_v" << setw(15) << "outbuffer2" << endl;
		outfile << setw(15) << (ob2_tmp.front()).valid << setw(15) << (ob2_tmp.front()).data << endl;
	}

	//打印outbuffer中的内容
	//outfile << "outbuffer值打印" << endl;
	if (pe_current->outbuffer3_v)
	{
		outfile << "outbuffer3值打印" << endl;
		outfile << setw(15) << "outbuffer3_v" << setw(15) << "outbuffer3" << endl;
		outfile << setw(15) << pe_current->outbuffer3_v << setw(15) << pe_current->outbuffer3 << endl;
	}
	

	else if (pe_current->config_reg.front()[12] == 2)	//lr from in1 for 中转
	{
	}
	else if (pe_current->config_reg.front()[12] == 3)	//lr from in2 for中转
	{
	}			
}

void pe_sim_step1_no_tag(ProcessingElement* pe_current)
{		
	//step1---inbuffer入数仿真
	//开始仿真前该子模块出口数据不能清零，有一个数据提前到来要在出口处keep住
	//由于静态时bp是一个持续性的信号，不需要进行bp的初始化
	//pe_current->ack2in1port = 0; pe_current->ack2in2port = 0; pe_current->ack2in3port = 0;

	//inbuffer1
	if (pe_current->config_reg.front()[20] == 1)//输入连接到了inbuffer上
	{
		//入数
		if (pe_current->din1_v)
		{
			if (pe_current->inBuffer1.dataIn(pe_current->din1, pe_current->din1_v))
				pe_current->din1_v = 0;                   //入数成功，清valid
			//入buffer成功，若满了则置bp信号
			if (pe_current->inBuffer1.isInBufferFull())   //入数后若buffer满了则将bp信号拉低
			{
				pe_current->ack2in1port = 0;
			}
			else
				pe_current->ack2in1port = 1;   
			
			//if (pe_current->config_reg.front()[5] == 2)//from pe
			//{
			//	pe[pe_current->config_reg.front()[3]]->dout1_v = 0;
			//}
			//else if (pe_current->config_reg.front()[5] == 1)//from le
			//{
			//	le[pe_current->config_reg.front()[3]]->data_out_v = 0;
			//}
			
		}
		
	}
	else
	{
		//cout << "输入数据没有进入到inbuffer中" << endl;
	}
		
	//inbuffer2
	if(pe_current->config_reg.front()[21] == 1)//输入连接到了inbuffer上
	{
		//入数
		if (pe_current->din2_v)
		{
			if (pe_current->inBuffer2.dataIn(pe_current->din2, pe_current->din2_v))
				pe_current->din2_v = 0;

			//入buffer成功，改变bp信号
			if (pe_current->inBuffer2.isInBufferFull())   //入数后若buffer满了则将bp信号拉低
			{
				pe_current->ack2in2port = 0;
			}
			else
				pe_current->ack2in2port = 1;
			
			/*    静态PE由自身清数据
			if (pe_current->config_reg.front()[8] == 2)//from pe
			{
				pe[pe_current->config_reg.front()[6]]->dout1_v = 0;
			}
			else if (pe_current->config_reg.front()[8] == 1)//from le
			{
				le[pe_current->config_reg.front()[6]]->data_out_v = 0;
			}
			*/
		}
		
	}
	else
		//cout << "输入数据没有进入到inbuffer中" << endl;

	//inbuffer3
	if (pe_current->config_reg.front()[22] == 1)//输入连接到了inbuffer上
	{
		//入数
		if (pe_current->bin_v)
		{
			pe_current->inbuffer3 = pe_current->bin;
			pe_current->inbuffer3_v = pe_current->bin_v;
			//入buffer成功，置bp信号
			pe_current->ack2in3port = 0;
			//清除入口
			pe_current->bin_v = 0;

			/*
			if (pe_current->config_reg.front()[11] == 2)//from pe
			{
				pe[pe_current->config_reg.front()[9]]->bout_v = 0;
			}
			else if (pe_current->config_reg.front()[11] == 1)//from le
			{
				le[pe_current->config_reg.front()[9]]->data_out_v = 0;
			}
			*/
		}
		
	}
	else
		//cout << "输入数据没有进入到inbuffer中" << endl;
	//inbuffer入数出数完成，清空当前PE入口数据
	pe_current->din1_v = 0; pe_current->din2_v = 0; pe_current->bin_v = 0;
	//清空last PE的出口数据

	//中间变量
	queue<OutBuffer_no_tag> ib1_tmp, ib2_tmp;
	ib1_tmp = pe_current->inBuffer1.inputBuffer;
	ib2_tmp = pe_current->inBuffer2.inputBuffer;

	if (!ib1_tmp.empty())
	{
		//打印inbuffer1中的内容
		outfile << "inbuffer1值打印" << endl;
		outfile << setw(15) << "inbuffer1_v" << setw(15) << "inbuffer1"  << endl;
		outfile << setw(15) << (ib1_tmp.front()).valid << setw(15) << (ib1_tmp.front()).data  << endl;
	}
	if (!ib2_tmp.empty())
	{
		//打印inbuffer2中的内容
		outfile << "inbuffer2值打印" << endl;
		outfile << setw(15) << "inbuffer2_v" << setw(15) << "inbuffer2" << endl;
		outfile << setw(15) << (ib2_tmp.front()).valid << setw(15) << (ib2_tmp.front()).data << endl;
	}
	if (pe_current->inbuffer3_v)
	{
		outfile << "inbuffer3值打印" << endl;
		outfile << setw(15) << "inbuffer3_v" << setw(15) << "inbuffer3" << endl;
		outfile << setw(15) << pe_current->inbuffer3_v << setw(15) << pe_current->inbuffer3 << endl;
	}
	

}

void pe_sim_step_connect(ProcessingElement* pe_current)
{
	int pe_index_current = pe_current->config_reg.front()[1];//当前正在处理的PE标号
	int din1_from_index = pe_current->config_reg.front()[3];
	int din2_from_index = pe_current->config_reg.front()[6];
	int bin_from_index = pe_current->config_reg.front()[9];
	//in_port
	int din1_from_port = pe_current->config_reg.front()[4];
	int din2_from_port = pe_current->config_reg.front()[7];
	int bin_from_port = pe_current->config_reg.front()[10];
	//in_flag
	int din1_from_flag = pe_current->config_reg.front()[5];
	int din2_from_flag = pe_current->config_reg.front()[8];
	int bin_from_flag = pe_current->config_reg.front()[11];
	//判断是否悬空
	bool is_din1_float = din1_from_flag;//0--悬空；非0--非悬空
	bool is_din2_float = din2_from_flag;
	bool is_bin_float = bin_from_flag;
	
	//din1 非悬空
	if (is_din1_float) //din1 非悬空
	{
		if (din1_from_flag == 2) //from PE
		{
			if (din1_from_port == 0)//PE的port1
			{
				if (pe[din1_from_index]->dout1_v)//数据有效，取数
				{
					pe_current->din1 = pe[din1_from_index]->dout1;
					pe_current->din1_v = pe[din1_from_index]->dout1_v;
					pe_current->din1_tag = pe[din1_from_index]->dout1_tag;
				}
				else
				{
					//cout << "上一个PE输出的数没有valid." << endl;
					//pe_current->in1_v = 0;
				}
			}
			else if (din1_from_port == 1)//上一个PE的port2
			{
				if (pe[din1_from_index]->dout2_v)//data valid,fetch
				{
					pe_current->din1 = pe[din1_from_index]->dout2;
					pe_current->din1_v = pe[din1_from_index]->dout2_v;
					pe_current->din1_tag = pe[din1_from_index]->dout2_tag;
				}
				else
				{
					//cout << "上一个PE输出的数没有valid." << endl;
					//pe_current->in1_v = 0;
				}

			}
		}
		else if (din1_from_flag == 1) //输入来自于LE
		{
			if (le[din1_from_index]->data_out_v)//data valid,fetch
			{
				pe_current->din1 = le[din1_from_index]->data_out;
				pe_current->din1_v = le[din1_from_index]->data_out_v;
				pe_current->din1_tag = le[din1_from_index]->data_out_tag;
			}
			else
			{
				//cout << "上一个LE输出的数据没有valid" << endl;
				//pe_current->in1_v = 0;

			}
		}
		else if (din1_from_flag == 3) //输入来自于lbegin
		{
			if (lbegin[din1_from_index]->out_v)//数据有效，取数
			{
				pe_current->din1 = lbegin[din1_from_index]->out;
				pe_current->din1_v = lbegin[din1_from_index]->out_v;
				pe_current->din1_tag = 0;
			}
		}
		else if (din1_from_flag == 9) //输入来自于ti
		{
			if (ti->out_v)//数据准备好，取数
			{
				pe_current->din1 = ti->out;
				pe_current->din1_v = ti->out_v;
				pe_current->din1_tag = ti->out_t;
			}
		}
	}
	else
	{
		//cout << "PE[" << pe_index_current << "]的输入1悬空" << endl;
		pe_current->din1 = 0;
		pe_current->din1_v = 0;
		pe_current->din1_tag = 0;

	}
	//din2非悬空
	if (is_din2_float)//in2非悬空
	{
		if (din2_from_flag == 2) //输入2来自于PE
		{
			if (din2_from_port == 0)//上一个PE的port1
			{
				if (pe[din2_from_index]->dout1_v)//data valid,fetch
				{
					pe_current->din2 = pe[din2_from_index]->dout1;
					pe_current->din2_v = pe[din2_from_index]->dout1_v;
					pe_current->din2_tag = pe[din2_from_index]->dout1_tag;
				}
				else
				{
					//cout << "上一个PE输出的数没有valid." << endl;
					//pe_current->in2_v = 0;
				}

			}
			else if (din1_from_port == 1)//上一个PE的port2
			{
				if (pe[din2_from_index]->dout2_v)//data valid,fetch
				{
					pe_current->din2 = pe[din2_from_index]->dout2;
					pe_current->din2_v = pe[din2_from_index]->dout2_v;
					pe_current->din2_tag = pe[din2_from_index]->dout2_tag;
				}
				else
				{
					//cout << "上一个PE输出的数没有valid." << endl;
					//pe_current->in2_v = 0;
				}
			}
		}
		else if (din2_from_flag == 1) //输入2来自于LE
		{
			if (le[din2_from_index]->data_out_v)
			{
				pe_current->din2 = le[din2_from_index]->data_out;
				pe_current->din2_v = le[din2_from_index]->data_out_v;
				pe_current->din2_tag = le[din2_from_index]->data_out_tag;
			}
			else
			{
				//pe_current->in2_v = 0;
				//cout << "上一个LE输出的数据没有valid" << endl;
			}
		}
		else if (din2_from_flag == 3) //输入来自于lbegin
		{
			if (lbegin[din2_from_index]->out_v)//数据有效，取数
			{
				pe_current->din2 = lbegin[din2_from_index]->out;
				pe_current->din2_v = lbegin[din2_from_index]->out_v;
				pe_current->din2_tag = 0;
			}
		}
	}
	else
	{
		//cout << "PE的输入2悬空" << endl;
		pe_current->din2 = 0;
		pe_current->din2_v = 0;
	}
	//bin非悬空
	if (is_bin_float)//in3非悬空
	{
		if (bin_from_flag == 2)//输入3来自于PE
		{
			if (bin_from_port == 2)//port3
			{
				if (pe[bin_from_index]->bout_v)
				{
					pe_current->bin = pe[bin_from_index]->bout;
					pe_current->bin_v = pe[bin_from_index]->bout_v;
				}
				/*else
				{
					cout << "上一个PE的输出数据没有valid" << endl;
					//pe_current->in3_v = 0;
				}*/

			}
			else if (bin_from_port == 0)//port1
			{
				if (pe[bin_from_index]->dout1_v)
				{
					pe_current->bin_v = pe[bin_from_index]->dout1_v;//默认只把valid信号传输给bin_v和bin
					pe_current->bin = pe[bin_from_index]->dout1_v;
				}
			}
			else if (bin_from_port == 1)//port2
			{
				if (pe[bin_from_index]->dout2_v)
				{
					pe_current->bin_v = pe[bin_from_index]->dout2_v;//默认只把valid信号传输给bin_v和bin
					pe_current->bin = pe[bin_from_index]->dout2_v;
				}
			}
			else
				cout << "error" << endl;
		}
		else if (bin_from_flag == 1)//输入3来源自LE
		{
			cout << "PE的输入3不可能来自于LE，配置编写错误，请更改." << endl;
		}
		else if (bin_from_flag == 10)//from SE
		{
			pe_current->bin_v = se[bin_from_index]->se_extra_out_for_end_v;
			pe_current->bin = se[bin_from_index]->se_extra_out_for_end;
		}
		else if (bin_from_flag == 3)//输入3来源自lbegin
		{
			pe_current->bin_v = lbegin[bin_from_index]->out_v;
			pe_current->bin = lbegin[bin_from_index]->out;

		}
		else if (bin_from_flag == 4)//输入3来源自lend
		{
			if (bin_from_port == 0)//lend的port2back
			{

				pe_current->bin_v = lend[bin_from_index]->out2back;//使能信号从valid口进入																					   											
			}
			else if (bin_from_port == 1)//FG2的port2end
			{
				pe_current->bin_v = lend[bin_from_index]->out2end;//使能信号从valid口进入																					  												
			}
			else
				cout << "port 超出了范围" << endl;
		}

		else if (bin_from_flag == 5)//输入3来源自join
		{
			//
		}
		else if (bin_from_flag == 10)//输入3来源于begin
		{
			//pe_current->bin_v = begin_signal;
		}
		else if (bin_from_flag == 6)//输入3来源于switch
		{
			if (bin_from_port == 0)//switch的port1(left)
			{

				pe_current->bin_v = switch_[bin_from_index]->out1;
			}
			else if (bin_from_port == 1)//switch的port2(right)
			{
				pe_current->bin_v = switch_[bin_from_index]->out2;
			}
			else
				cout << "port 超出了范围" << endl;
		}
		else if (bin_from_flag == 7)//输入3来源于break
		{
			//
		}

		//......
		else
		{
			cout << "超出了部件范围" << endl;
		}
	}
	else
	{
		pe_current->bin = 0;
		pe_current->bin_v = 0;
	}
	//cycle count
	//pe_current->cycle_entryLogic();

	////入口处数据打印
	//outfile << "PE[" << pe_index_current << "]的输入是： din1: " << pe_current->din1 << "  din1_v: " << pe_current->din1_v << "  din1_tag: " << pe_current->din1_tag << endl;
	//outfile << "                    din2: " << pe_current->din2 << "  din2_v: " << pe_current->din2_v << "  din2_tag: " << pe_current->din2_tag << endl;
	//outfile << "                    bin: " << pe_current->bin << "  bin_v: " << pe_current->bin_v << "  bin_tag: " << pe_current->bin_tag << endl;
	//for debug
	if (1)//打印数据
	{
		outfile2 << endl;
		//outfile2 << "PE[ " << pe_index_current << " ]" << endl;
		
		outfile2 << "PE[" << pe_index_current << "]输入值" << endl;
		outfile2 << setw(15) << "din1_t" << setw(15) << "din1_v" << setw(15) << "din1" << setw(15) << "din2_t" << setw(15) << "din2_v" << setw(15)
			<< "din2" << setw(15) << "bin_t" << setw(15) << "bin_v" << setw(15) << "bin" << endl;		
		outfile2 << setw(15) << pe_current->din1_tag << setw(15) << pe_current->din1_v << setw(15) << pe_current->din1 << setw(15) << pe_current->din2_tag << setw(15)
			<< pe_current->din2_v << setw(15) << pe_current->din2 << setw(15) << pe_current->bin_tag << setw(15) << pe_current->bin_v << setw(15) << pe_current->bin << endl;
		//outfile2 << "---------------------------------------------------------------" << endl;
	}
}

void pe_sim_step1(ProcessingElement* pe_current)
{
	int pe_index_current = pe_current->config_reg.front()[1];//当前正在处理的PE标号
	int din1_from_index = pe_current->config_reg.front()[3];
	int din2_from_index = pe_current->config_reg.front()[6];
	int bin_from_index = pe_current->config_reg.front()[9];
	//in_port
	int din1_from_port = pe_current->config_reg.front()[4];
	int din2_from_port = pe_current->config_reg.front()[7];
	int bin_from_port = pe_current->config_reg.front()[10];
	//in_flag
	int din1_from_flag = pe_current->config_reg.front()[5];
	int din2_from_flag = pe_current->config_reg.front()[8];
	int bin_from_flag = pe_current->config_reg.front()[11];
	//判断是否悬空
	bool is_din1_float = din1_from_flag;//0--悬空；非0--非悬空
	bool is_din2_float = din2_from_flag;
	bool is_bin_float = bin_from_flag;

	//inTableBuffer入数
	//data in
	pe_current->inTableBuffer.dataIn(pe_current);
	//ack==1，数据入表成功，清空入口的数据
	if (pe_current->ack2in1port)
		pe_current->din1_v = 0;
	if (pe_current->ack2in2port)
		pe_current->din2_v = 0;
	if (pe_current->ack2in3port)
		pe_current->bin_v = 0;


	
	//在动态数据流下，PE的输出端口的置0操作要分两种情况。1，该PE某一输出端口的扇出为1，那么在数据存入下一个PE的tableBuffer之后就可以执行上一个PE输出端口的置0操作
												//  2，该PE某一输出端口的扇出>1，那么在数据存入tableBuffer之后由相应的joinbp执行置0操作
	//获取来源PE 1,2,3端口的扇出,并进行清空相应端口的数据
	if (1)                                        //清前级输入
	{
		int port1_fanout_num = 0, port2_fanout_num = 0, port3_fanout_num = 0;//--这三个变量是该PE的上一级PE输出端口的扇出

		if (is_din1_float)   //当din1有输入时统计其扇出
		{
			for (int i = 0; i < (int)port_fanout.size(); i++)
			{
				if (din1_from_index == port_fanout[i][1] && ((din1_from_flag == 2 && port_fanout[i][0] == 0) || (din1_from_flag == 1 && port_fanout[i][0] == 1)))
				{
					if (din1_from_port == 0)//port1
					{
						port1_fanout_num = port_fanout[i][2];
					}
					else if (din1_from_port == 1)//port2
					{
						port2_fanout_num = port_fanout[i][3];
					}
					else if (din1_from_port == 2)//port3
					{
						port3_fanout_num = port_fanout[i][4];
					}

				}
			}
			//开始清空相应的端口数据
			//数据进入tableBuffer之后才开始清数据
			if (din1_from_port == 0)//port1
			{
				if (port1_fanout_num <= 1)//一对一的情况
				{
					if (pe_current->ack2in1port)
					{
						if (din1_from_flag == 2)//from PE
						{
							pe[din1_from_index]->outTableBuffer1.buffer_clear();
							pe[din1_from_index]->ack_outbuffer12alu = 1;
							//pe[din1_from_index]->dout1_v = 0;应在访问pe[din1_from_index]的时候清除
						}
						else if (din1_from_flag == 1)//from LE
						{
							//
						}
					}
				}
			}
			else if (din1_from_port == 1)//port2
			{
				if (port2_fanout_num <= 1)
				{
					if (pe_current->ack2in2port)
					{
						if (din2_from_flag == 2)//from PE
						{
							pe[din1_from_index]->outTableBuffer2.buffer_clear();
							pe[din1_from_index]->ack_outbuffer22alu = 1;
							//pe[din1_from_index]->dout2_v = 0;
						}
						else if (din2_from_flag == 1)//from LE
						{
							//
						}

					}
				}
			}
			else if (din1_from_port == 2)//port3
			{
				if (port3_fanout_num <= 1)
				{
					if (pe_current->ack2in3port)
					{
						if (bin_from_flag == 2)//from PE
						{
							pe[din1_from_index]->bout_v = 0;
						}
						else if (bin_from_flag == 1)//from LE
						{
							//
						}

					}
				}
			}
		}
		//in2 from的端口清零
		//
		if (is_din2_float)
		{
			for (int i = 0; i < (int)port_fanout.size(); i++)
			{
				if (din2_from_index == port_fanout[i][1] && ((din1_from_flag == 2 && port_fanout[i][0] == 0) || (din1_from_flag == 1 && port_fanout[i][0] == 1)))
				{
					if (din2_from_port == 0)//port1
					{
						port1_fanout_num = port_fanout[i][2];
					}
					else if (din2_from_port == 1)//port2
					{
						port2_fanout_num = port_fanout[i][3];
					}
					else if (din2_from_port == 2)//port3
					{
						port3_fanout_num = port_fanout[i][4];
					}

				}
			}
			if (din2_from_port == 0)//port1
			{
				if (port1_fanout_num <= 1)
				{
					if (pe_current->ack2in2port)
					{
						if (din2_from_flag == 2)//from PE
						{
							pe[din2_from_index]->outTableBuffer1.buffer_clear();
							pe[din2_from_index]->ack_outbuffer12alu = 1;
							//pe[din2_from_index]->dout1_v = 0;
						}
						else if (din2_from_flag == 1)//from LE
						{
							//
						}

					}
				}
			}
			else if (din2_from_port == 1)//port2
			{
				if (port2_fanout_num <= 1)
				{
					if (pe_current->ack2in2port)
					{
						if (din2_from_flag == 2)//from PE
						{
							pe[din2_from_index]->outTableBuffer2.buffer_clear();
							pe[din2_from_index]->ack_outbuffer22alu = 1;
							//pe[din2_from_index]->dout2_v = 0;
						}

					}
				}
			}
			else if (din2_from_port == 2)//port3
			{
				if (port3_fanout_num <= 1)
				{
					if (pe_current->ack2in3port)
					{
						if (din2_from_flag == 2)//from PE
						{
							pe[din2_from_index]->bout_v = 0;
						}

					}
				}
			}
		}
		//in3 from的端口清零
		//in3 from各端口的扇出
		if (is_bin_float)
		{
			for (int i = 0; i < (int)port_fanout.size(); i++)
			{
				if (bin_from_index == port_fanout[i][1] && ((din1_from_flag == 2 && port_fanout[i][0] == 0) || (din1_from_flag == 1 && port_fanout[i][0] == 1)))
				{
					if (bin_from_port == 0)//port1
					{
						port1_fanout_num = port_fanout[i][2];
					}
					else if (bin_from_port == 1)//port2
					{
						port2_fanout_num = port_fanout[i][3];
					}
					else if (bin_from_port == 2)//port3
					{
						port3_fanout_num = port_fanout[i][4];
					}

				}
			}
			//in3 from各端口清零
			if (bin_from_port == 0)//port1
			{
				if (port1_fanout_num <= 1)
				{
					if (pe_current->ack2in1port)
					{
						if (bin_from_flag == 2)//from PE
						{
							pe[bin_from_index]->outTableBuffer1.buffer_clear();
							pe[bin_from_index]->dout1_v = 0;
						}

					}
				}
			}
			else if (bin_from_port == 1)//port2
			{
				if (port2_fanout_num <= 1)
				{
					if (pe_current->ack2in2port)
					{
						if (bin_from_flag == 2)//from PE
						{
							pe[bin_from_index]->outTableBuffer2.buffer_clear();
							pe[bin_from_index]->dout2_v = 0;
						}

					}
				}
			}
			else if (bin_from_port == 2)//port3
			{
				if (port3_fanout_num <= 1)
				{
					if (pe_current->ack2in3port)
					{
						if (bin_from_flag == 2)//from PE
						{
							pe[bin_from_index]->bout_v = 0;
						}
						else if (bin_from_flag == 1)//from LE
						{
							//
						}

					}
				}
			}
		}
	}

	//PE inTableBuffer中的内容打印
	if (1)
	{
		outfile << "inTableBuffer中的内容" << endl;
		outfile.width(10);
		outfile << "tag";
		outfile.width(10);
		outfile << "valid1";
		outfile.width(10);
		outfile << "data1";
		outfile.width(10);
		outfile << "valid2";
		outfile.width(10);
		outfile << "data2";
		outfile.width(10);
		outfile << "valid3";
		outfile.width(10);
		outfile << "data3";
		outfile.width(10);
		outfile << "lineOK" << endl;
		for (int i = 0; i < intablebuffer_depth; i++)
		{
			outfile.width(10);
			outfile << pe_current->inTableBuffer.InTableBufferEntity[i].tag;
			outfile.width(10);
			outfile << pe_current->inTableBuffer.InTableBufferEntity[i].valid1;
			outfile.width(10);
			outfile << pe_current->inTableBuffer.InTableBufferEntity[i].data1;
			outfile.width(10);
			outfile << pe_current->inTableBuffer.InTableBufferEntity[i].valid2;
			outfile.width(10);
			outfile << pe_current->inTableBuffer.InTableBufferEntity[i].data2;
			outfile.width(10);
			outfile << pe_current->inTableBuffer.InTableBufferEntity[i].valid3;
			outfile.width(10);
			outfile << pe_current->inTableBuffer.InTableBufferEntity[i].data3;
			outfile.width(10);
			outfile << pe_current->inTableBuffer.InTableBufferEntity[i].line_ok;
			outfile << endl;
		}
		outfile << endl;
	}			
}

void pe_sim_step2(ProcessingElement* pe_current)
{
	int opcode = pe_current->config_reg.front()[2];
	int pe_index_current = pe_current->config_reg.front()[1];//当前正在处理的PE标号
															 //in_from index
	int din1_from_index = pe_current->config_reg.front()[3];
	int din2_from_index = pe_current->config_reg.front()[6];
	int bin_from_index = pe_current->config_reg.front()[9];
	//in_port
	int din1_from_port = pe_current->config_reg.front()[4];
	int din2_from_port = pe_current->config_reg.front()[7];
	int bin_from_port = pe_current->config_reg.front()[10];
	//in_flag
	int din1_from_flag = pe_current->config_reg.front()[5];
	int din2_from_flag = pe_current->config_reg.front()[8];
	int bin_from_flag = pe_current->config_reg.front()[11];
	//判断是否悬空
	//bool is_din1_float = din1_from_flag;//0--悬空；非0--非悬空
	//bool is_din2_float = din2_from_flag;
	//bool is_bin_float = bin_from_flag;
	//开始仿真之前子模块输出端口数据清零
	pe_current->alu_out_v = 0;
	pe_current->alu_out = 0;
	pe_current->alu_out_b_v = 0;
	pe_current->alu_out_b = 0;
	

	//ack clear
	pe_current->ack_alu2ib1 = 0;
	pe_current->ack_alu2ib2 = 0;

	//inTableBuffer输出
	//data out
	if (pe_current->ack_outbuffer12alu == 1)     //查询obuffer是否已满
	{
		pe_current->inTableBuffer.dataOut(pe_current);
	}
	
	outfile2 << "PE[" << pe_current->config_reg.front()[1] << "]中间值1" << endl;
	outfile2 << setw(15) << "ib1_out_t" << setw(15) << "ib1_out_v" << setw(15) << "ib1_out" << setw(15) << "ib2_out_t" << setw(15) << "ib2_out_v"
		<< setw(15) << "ib2_out" << setw(15) << "ib3_out_t" << setw(15) << "ib3_out_v" << setw(15) << "ib3_out" << endl;
	outfile2 << setw(15) << pe_current->inbuffer1_out_tag << setw(15) << pe_current->inbuffer1_out_v
		<< setw(15) << pe_current->inbuffer1_out << setw(15) << pe_current->inbuffer2_out_tag
		<< setw(15) << pe_current->inbuffer2_out_v << setw(15) << pe_current->inbuffer2_out
		<< setw(15) << pe_current->inbuffer3_out_tag << setw(15) << pe_current->inbuffer3_out_v
		<< setw(15) << pe_current->inbuffer3_out << endl;
	outfile2 << endl;
	
	//tag默认提前传输到ob入口处
	if (pe_current->config_reg.front()[15])//ob1_from 非悬空
	{
		pe_current->outbuffer1_in_tag = pe_current->inbuffer1_out_tag;
	}
	if (pe_current->config_reg.front()[16])//ob2_from 非悬空
	{
		pe_current->outbuffer2_in_tag = pe_current->inbuffer2_out_tag;
	}

	pe_current->outbuffer3_tag = pe_current->inbuffer3_out_tag;

	//ALU端口取数
	//alu_in1
	if (pe_current->config_reg.front()[13] == 0)//alu_in1_from ib1
	{
		if (pe_current->inbuffer1_out_v)
		{
			pe_current->alu_in1 = pe_current->inbuffer1_out;
			pe_current->alu_in1_v = pe_current->inbuffer1_out_v;
			//置ack信号
			pe_current->ack_alu2ib1 = 1;
		}

	}
	else if (pe_current->config_reg.front()[13] == 1) //alu_in1_from lr
	{
		if (pe_current->loc_reg_v)
		{
			pe_current->alu_in1 = pe_current->loc_reg;
			pe_current->alu_in1_v = pe_current->loc_reg_v;
			//置ack信号
			pe_current->ack_alu2ib1 = 1;
		}

	}
	//alu_in2
	if (pe_current->config_reg.front()[14] == 0)//alu_in2_from ib2
	{
		if (pe_current->inbuffer2_out_v)
		{
			pe_current->alu_in2 = pe_current->inbuffer2_out;
			pe_current->alu_in2_v = pe_current->inbuffer2_out_v;
			//置ack信号
			pe_current->ack_alu2ib2 = 1;
		}

	}
	else if (pe_current->config_reg.front()[14] == 1)  //alu_in2_from lr
	{
		if (pe_current->loc_reg_v)
		{
			pe_current->alu_in2 = pe_current->loc_reg;
			pe_current->alu_in2_v = pe_current->loc_reg_v;
			//置ack信号
			pe_current->ack_alu2ib2 = 1;
		}

	}	
	/*outfile << "PE[" << pe_current->config_reg.front()[1] << "] ALU的输入是：in1: " << pe_current->alu_in1 << " in2: " << pe_current->alu_in2 << endl;*/
	//执行计算
	if (pe_current->alu_in1_v & pe_current->alu_in2_v)
	{
		pe_current->ALU(pe_current->config_reg.front()[2], pe_current->alu_in1, pe_current->alu_in2, pe_current->inbuffer3_out);
		//计算结束之后清空inTableBuffer的某一行并且清除request请求
		pe_current->alu_in1_v = 0;
		pe_current->alu_in2_v = 0;
		pe_current->inbuffer1_out_v = 0;
		pe_current->inbuffer2_out_v = 0;
		pe_current->inbuffer3_out_v = 0;	
		pe_current->inTableBuffer.buffer_clear();
	}
	
	outfile2 << "PE[" << pe_current->config_reg.front()[1] << "]中间值2" << endl;
	outfile2 << setw(15) << "alu_out_v" << setw(15) << "alu_out" << setw(15) << "alu_out_b_v" << setw(15) << "alu_out_b" << endl;
	outfile2 << setw(15) << pe_current->alu_out_v << setw(15) << pe_current->alu_out
		<< setw(15) << pe_current->alu_out_b_v << setw(15) << pe_current->alu_out_b << endl;

	//alu_out get into outTableBuffer
	//取数到端口
	if (pe_current->config_reg.front()[15])//ob1_from 非悬空
	{
		if (pe_current->config_reg.front()[15] == 1)//ob1 from alu
		{
			if (pe_current->alu_out_v)
			{
				pe_current->outbuffer1_in = pe_current->alu_out;
				pe_current->outbuffer1_in_v = pe_current->alu_out_v;

			}
		}

		else if (pe_current->config_reg.front()[15] == 3)//ob1 from in1
		{
		}
		else if (pe_current->config_reg.front()[15] == 2)//ob1 from lr
		{
			if (pe_current->loc_reg_v)
			{
				pe_current->outbuffer1_in = pe_current->loc_reg;
				pe_current->outbuffer1_in_v = pe_current->loc_reg_v;
				/*if (pe_current->outTableBuffer2.isOutBufferFull())
				{
					pe_current->ack_outbuffer12alu = 0;
				}
				else
					pe_current->ack_outbuffer12alu = 1;*/
			}
		}
		//pe_current->outbuffer1_in_tag = pe_current->inbuffer1_out_tag;
		//pe_current->outbuffer1_in_v = 1;
		//有效开始入数
		if (pe_current->outbuffer1_in_v)
		{
			//数据导入到outTableBuffer类中去
			pe_current->outTableBuffer1.in = pe_current->outbuffer1_in;
			pe_current->outTableBuffer1.in_v = pe_current->outbuffer1_in_v;
			pe_current->outTableBuffer1.in_tag = pe_current->outbuffer1_in_tag;
			//入数
			pe_current->outTableBuffer1.dataIn();		
			//pe操作完成，子模块入口处的valid值清零
			pe_current->outbuffer1_in_v = 0;
			pe_current->alu_out_v = 0;
			if (pe_current->outTableBuffer1.isOutBufferFull())
			{
				pe_current->ack_outbuffer12alu = 0;
			}
			else
				pe_current->ack_outbuffer12alu = 1;
		}

	}

	if (pe_current->config_reg.front()[16])//ob2非悬空
	{
		pe_current->outbuffer2_in_tag = pe_current->inbuffer1_out_tag;
		if (pe_current->config_reg.front()[16] == 1)//ob2 from alu
		{
			pe_current->outbuffer2_in = pe_current->alu_out;
			pe_current->outbuffer2_in_v = pe_current->alu_out_v;

		}

		else if (pe_current->config_reg.front()[16] == 3)//ob2 from in2
		{
		}
		else if (pe_current->config_reg.front()[16] == 2)//ob2 from lr
		{
			pe_current->outbuffer2_in = pe_current->loc_reg;
			pe_current->outbuffer2_in_v = pe_current->loc_reg_v;
			/*if (pe_current->outBuffer2.isOutBufferFull())
			{
				pe_current->ack_outbuffer22alu = 0;
			}
			else
				pe_current->ack_outbuffer22alu = 1;*/
		}


		//pe_current->outbuffer2_in_v = 1;
		//有效开始入数
		if (pe_current->outbuffer2_in_v)
		{
			//数据导入到outTableBuffer类中去
			pe_current->outTableBuffer2.in = pe_current->outbuffer2_in;
			pe_current->outTableBuffer2.in_v = pe_current->outbuffer2_in_v;
			pe_current->outTableBuffer2.in_tag = pe_current->outbuffer2_in_tag;
			//入数
			pe_current->outTableBuffer2.dataIn();
			//pe操作完成，子模块入口处的valid值清零
			pe_current->outbuffer2_in_v = 0;
			if (pe_current->outTableBuffer2.isOutBufferFull())
			{
				pe_current->ack_outbuffer22alu = 0;
			}
			else
				pe_current->ack_outbuffer22alu = 1;
		}

	}

	if (pe_current->config_reg.front()[15] == 0 && pe_current->config_reg.front()[16] == 0)//ob3非悬空，来自于alu_out_b
	{
		pe_current->outbuffer3_tag = pe_current->inbuffer1_out_tag;
		if (pe_current->alu_out_b_v)
		{
			pe_current->outbuffer3 = pe_current->alu_out_b;//ob3入数			
			//pe操作完成，子模块入口处的valid值清零
			pe_current->outbuffer3_in_v = 0;
			pe_current->ack_outbuffer32alu = 0;
		}

	}
	//ob队首元素打印
	if (1)
	{

		if (!pe_current->outTableBuffer1.outBuffer.empty())
		{
			outfile << "outTableBuffer1的大小" << endl;
			outfile << pe_current->outTableBuffer1.outBuffer.size()<<endl;
			outfile << "PE[" << pe_index_current << "]ob1队首元素值" << endl;
			outfile << setw(10) << "ob1_t" << setw(10) << "ob1_v" << setw(10) << "ob1_d" << endl;
			outfile << setw(10) << pe_current->outTableBuffer1.outBuffer.front().tag << setw(10) << pe_current->outTableBuffer1.outBuffer.front().valid
				<< setw(10) << pe_current->outTableBuffer1.outBuffer.front().data << endl;
		}
		if (!pe_current->outTableBuffer2.outBuffer.empty())
		{	
			outfile << "PE[" << pe_index_current << "]ob2队首元素值" << endl;
			outfile << setw(10) << "ob2_t" << setw(10) << "ob2_v" << setw(10) << "ob2_d" << endl;
			outfile << setw(10) << pe_current->outTableBuffer2.outBuffer.front().tag << setw(10) << pe_current->outTableBuffer2.outBuffer.front().valid 
				<< setw(10) << pe_current->outTableBuffer2.outBuffer.front().data << endl;
		}
		

	}
	//lr 仿真
	if (pe_current->config_reg.front()[12] == 1)//from ALU
	{
		if (pe_current->alu_out_v)
		{
			pe_current->loc_reg_v = pe_current->alu_out_v;
			pe_current->loc_reg = pe_current->alu_out;
		}
		////额外增加的loc reg 清零的功能
		//if (pe_current->loc_reg == THRESHOLD)
		//{
		//	pe_current->loc_reg = 0;
		//}
	}
}

void pe_sim_step3(ProcessingElement* pe_current)
{
	int pe_index_current = pe_current->config_reg.front()[1];//当前正在处理的PE标号
	int din1_from_index = pe_current->config_reg.front()[3];
	int din2_from_index = pe_current->config_reg.front()[6];
	int bin_from_index = pe_current->config_reg.front()[9];
	//in_port
	int din1_from_port = pe_current->config_reg.front()[4];
	int din2_from_port = pe_current->config_reg.front()[7];
	int bin_from_port = pe_current->config_reg.front()[10];
	//in_flag
	int din1_from_flag = pe_current->config_reg.front()[5];
	int din2_from_flag = pe_current->config_reg.front()[8];
	int bin_from_flag = pe_current->config_reg.front()[11];
	//判断是否悬空
	bool is_din1_float = din1_from_flag;//0--悬空；非0--非悬空
	bool is_din2_float = din2_from_flag;
	bool is_bin_float = bin_from_flag;
	//判断当前PE各个输入的来源情况
	//in1port
	if (din1_from_flag == 2)//PE
	{
		if (pe_current->config_reg.front()[18] == 0 | pe_current->config_reg.front()[18] == 1)
		{
			//旁路模式不需要再额外执行ob出数操作
		}
		else
		{
			//不需要区分上一个PE是出于静态还是动态，tag绑定的PE虽然处于静态数据流但是绑定的数剧是输入到outTableBuffer中去的
			pe_outbuffer_out(pe[din1_from_index], din1_from_port);
			/*
			if (din1_from_port == 0)
			{
				pe[din1_from_index]->ack_outbuffer12alu = 1;
			}
			else if (din1_from_port == 1)
			{
				pe[din1_from_index]->ack_outbuffer22alu = 1;
			}
			else if (din1_from_port == 2)
			{
				pe[din1_from_index]->ack_outbuffer32alu = 1;
			}
			*/
		}
		
	}
	else if (din1_from_flag == 1)//LE
	{
		//不需要额外增加LE的outbuffer出数步骤
	}
	else if (din1_from_flag == 3)//lbegin
	{
	}
	else if (din1_from_flag == 9)//ti
	{
	}
	else if (din1_from_flag == 10)//se
	{
	}
	//in2port
	if (din2_from_flag == 2)//PE
	{
		if (pe_current->config_reg.front()[18] == 0 | pe_current->config_reg.front()[18] == 1)
		{
			//旁路模式不需要再额外的ob出数
		}
		else
		{
			pe_outbuffer_out(pe[din2_from_index], din2_from_port);
			/*
			if (din2_from_port == 0)
			{
				pe[din2_from_index]->ack_outbuffer12alu = 1;
			}
			else if (din1_from_port == 1)
			{
				pe[din2_from_index]->ack_outbuffer22alu = 1;
			}
			else if (din1_from_port == 2)
			{
				pe[din2_from_index]->ack_outbuffer32alu = 1;
			}
			*/
		}
		
	}
	else if (din2_from_flag == 1)//LE
	{
		//不需要额外增加LE的outbuffer出数步骤
	}
	else if (din2_from_flag == 3)//lbegin
	{
	}
	else if (din2_from_flag == 9)//ti
	{
	}
	else if (din2_from_flag == 10)//se
	{
	}
	//in3port
	if (bin_from_flag == 2)//PE
	{
		if (pe_current->config_reg.front()[18] == 0 | pe_current->config_reg.front()[18] == 1)
		{
			//旁路模式不需要再额外的ob出数
		}
		else
		{
			pe_outbuffer_out(pe[bin_from_index], bin_from_port);
			/*
			if (bin_from_port == 0)
			{
				pe[bin_from_index]->ack_outbuffer12alu = 1;
			}
			else if (bin_from_port == 1)
			{
				pe[bin_from_index]->ack_outbuffer22alu = 1;
			}
			else if (bin_from_port == 2)
			{
				pe[bin_from_index]->ack_outbuffer32alu = 1;
			}
			*/
		}
		
	}
	else if (bin_from_flag == 1)//LE
	{
		//不需要额外增加LE的outbuffer出数步骤
	}
	else if (bin_from_flag == 3)//lbegin
	{
	}
	else if (bin_from_flag == 9)//ti
	{
	}
	else if (bin_from_flag == 10)//se
	{
	}
}

void pe_outbuffer_out(ProcessingElement* pe_current,uint32_t ob_index)
{
	int opcode = pe_current->config_reg.front()[2];
	int pe_index_current = pe_current->config_reg.front()[1];//当前正在处理的PE标号
															 //in_from index
	int din1_from_index = pe_current->config_reg.front()[3];
	int din2_from_index = pe_current->config_reg.front()[6];
	int bin_from_index = pe_current->config_reg.front()[9];
	//in_port
	int din1_from_port = pe_current->config_reg.front()[4];
	int din2_from_port = pe_current->config_reg.front()[7];
	int bin_from_port = pe_current->config_reg.front()[10];
	//in_flag
	int din1_from_flag = pe_current->config_reg.front()[5];
	int din2_from_flag = pe_current->config_reg.front()[8];
	int bin_from_flag = pe_current->config_reg.front()[11];
	//判断是否悬空
	//bool is_din1_float = din1_from_flag;//0--悬空；非0--非悬空
	//bool is_din2_float = din2_from_flag;
	//bool is_bin_float = bin_from_flag;

	//子模块出口处的信号值清零
	pe_current->dout1_v = 0;
	pe_current->dout1 = 0;
	pe_current->dout1_tag = 0;
	pe_current->dout2 = 0;
	pe_current->dout2_v = 0;
	pe_current->dout2_tag = 0;
	pe_current->bout_v = 0;
	pe_current->bout = 0;
	pe_current->bout_tag = 0;
	//step3的出数规则是只要ob table不是空状态，就可以出数
	//ob1 out
	if (ob_index == 0)//ob1需要出数
	{
		if (!pe_current->outTableBuffer1.outBuffer.empty())
		{
			if (pe_current->outTableBuffer1.outBuffer.front().valid)
			{
				pe_current->outTableBuffer1.dataOut();
				//数据从类中导出到PE
				pe_current->dout1 = pe_current->outTableBuffer1.out;
				pe_current->dout1_v = pe_current->outTableBuffer1.out_v;
				pe_current->dout1_tag = pe_current->outTableBuffer1.out_tag;
			}
			
		}
	}
	
	else if (ob_index == 1)//ob2需要出数
	{
		//ob2 out
		if (!pe_current->outTableBuffer2.outBuffer.empty())
		{
			if (pe_current->outTableBuffer2.outBuffer.front().valid)
			{
				pe_current->outTableBuffer2.dataOut();
				//数据从类中导出到PE
				pe_current->dout2 = pe_current->outTableBuffer2.out;
				pe_current->dout2_v = pe_current->outTableBuffer2.out_v;
				pe_current->dout2_tag = pe_current->outTableBuffer2.out_tag;
			}			
		}
	}
	else if (ob_index == 2)//ob3需要出数
	{
		//ob3 out
		if (pe_current->outbuffer3_v)
		{
			pe_current->bout = pe_current->outbuffer3;//ob3出数
			pe_current->bout_v = pe_current->outbuffer3_v;
			pe_current->bout_tag = pe_current->inbuffer1_out_tag;
		}
	}						
	

	//检测是否是end节点
	if (pe_current->config_reg.front()[17])
	{
		if (pe_current->dout1_v | pe_current->dout2_v )
		{
			pe_current->bout = 1;
			pe_current->bout_v = 1;//使用out3_v作为end信号的输出，只有在output3没有被使用的情况下才可以使用
		}
		

	}
}

/*
void pe_sim_step1_mux(ProcessingElement* pe_current)
{
	pe_current->inTableBuffer.dataIn(pe_current);
	//PE inTableBuffer中的内容
	outfile << "inTableBuffer中的内容" << endl;
	outfile << "--------------------------------" << endl;
	outfile.width(10);
	outfile << "tag";
	outfile.width(10);
	outfile << "valid1";
	outfile.width(10);
	outfile << "data1";
	outfile.width(10);
	outfile << "valid2";
	outfile.width(10);
	outfile << "data2";
	outfile.width(10);
	outfile << "valid3";
	outfile.width(10);
	outfile << "data3";
	outfile.width(10);
	outfile << "lineOK" << endl;
	for (int i = 0; i < intablebuffer_depth; i++)
	{
		outfile.width(10);
		outfile << pe_current->inTableBuffer.InTableBufferEntity[i].tag;
		outfile.width(10);
		outfile << pe_current->inTableBuffer.InTableBufferEntity[i].valid1;
		outfile.width(10);
		outfile << pe_current->inTableBuffer.InTableBufferEntity[i].data1;
		outfile.width(10);
		outfile << pe_current->inTableBuffer.InTableBufferEntity[i].valid2;
		outfile.width(10);
		outfile << pe_current->inTableBuffer.InTableBufferEntity[i].data2;
		outfile.width(10);
		outfile << pe_current->inTableBuffer.InTableBufferEntity[i].valid3;
		outfile.width(10);
		outfile << pe_current->inTableBuffer.InTableBufferEntity[i].data3;
		outfile.width(10);
		outfile << pe_current->inTableBuffer.InTableBufferEntity[i].line_ok;
		outfile << endl;
	}
	outfile << "--------------------------------" << endl;
	pe_current->inTableBuffer.dataOut(pe_current);
	if (pe_current->config_reg.front()[15])//ob1_from 非悬空
	{
		pe_current->outbuffer1_in_tag = pe_current->inbuffer1_out_tag;
	}
	if (pe_current->config_reg.front()[16])//ob2_from 非悬空
	{
		pe_current->outbuffer2_in_tag = pe_current->inbuffer2_out_tag;
	}
	pe_current->outbuffer3_tag = pe_current->inbuffer3_out_tag;
}

void pe_sim_step2_mux(ProcessingElement* pe_current)
{
	//ALU取数
	if (pe_current->inbuffer1_out_v | pe_current->inbuffer2_out_v | pe_current->inbuffer3_out_v) //有效才取数
	{
		if (pe_current->config_reg.front()[13] == 0)//alu_in1_from ib1
		{
			pe_current->alu_in1 = pe_current->inbuffer1_out;
			pe_current->alu_in1_v = pe_current->inbuffer1_out_v;
		}
		else if (pe_current->config_reg.front()[13] == 1) //alu_in1_from lr
		{
			pe_current->alu_in1 = pe_current->loc_reg;
			pe_current->alu_in1_v = pe_current->loc_reg_v;
		}

		if (pe_current->config_reg.front()[14] == 0)//alu_in2_from ib2
		{
			pe_current->alu_in2 = pe_current->inbuffer2_out;
			pe_current->alu_in2_v = pe_current->inbuffer2_out_v;
		}
		else if (pe_current->config_reg.front()[14] == 1)  //alu_in2_from lr
		{
			pe_current->alu_in2 = pe_current->loc_reg;
			pe_current->alu_in2_v = pe_current->loc_reg_v;
		}
		//取数之后清空inTableBuffer的某一行
		pe_current->inTableBuffer.buffer_clear();

	}
	outfile << "PE[" << pe_current->config_reg.front()[1] << "] ALU的输入是：in1: " << pe_current->alu_in1 << " in2: " << pe_current->alu_in2 << endl;
	if (pe_current->alu_in1_v & pe_current->alu_in2_v)
	{
		pe_current->ALU(pe_current->config_reg.front()[2], pe_current->alu_in1, pe_current->alu_in2, pe_current->inbuffer3_out);
	}
	
	//
	pe_current->inbuffer1_out_v = 0;
	pe_current->inbuffer2_out_v = 0;
	pe_current->inbuffer3_out_v = 0;

	//lr 仿真
	if (pe_current->config_reg.front()[12] == 1)//from ALU
	{
		if (pe_current->alu_out_v)
		{
			pe_current->loc_reg_v = pe_current->alu_out_v;
			pe_current->loc_reg = pe_current->alu_out;
		}
	}
}

void pe_sim_step3_mux(ProcessingElement* pe_current)
{

	//子模块出口处的信号值清零
	pe_current->dout1_v = 0;
	pe_current->dout1 = 0;
	pe_current->dout1_tag = 0;
	pe_current->dout2 = 0;
	pe_current->dout2_v = 0;
	pe_current->dout2_tag = 0;
	pe_current->bout_v = 0;
	pe_current->bout = 0;
	pe_current->bout_tag = 0;
	//ob入数、出数
	//取到端口
	if (pe_current->config_reg.front()[15])//ob1_from 非悬空
	{
		if (pe_current->config_reg.front()[15] == 1)//ob1 from alu
		{
			pe_current->outbuffer1_in = pe_current->alu_out;
			pe_current->outbuffer1_in_v = pe_current->alu_out_v;
		}

		else if (pe_current->config_reg.front()[15] == 3)//ob1 from in1
		{
		}
		else if (pe_current->config_reg.front()[15] == 2)//ob1 from lr
		{
			pe_current->outbuffer1_in = pe_current->loc_reg;
			pe_current->outbuffer1_in_v = pe_current->loc_reg_v;
		}
		
		//pe_current->outbuffer1_in_v = 1;
		//有效开始入数、取数
		if (pe_current->outbuffer1_in_v)
		{
			//数据导入到outTableBuffer类中去
			pe_current->outTableBuffer1.in = pe_current->outbuffer1_in;
			pe_current->outTableBuffer1.in_v = pe_current->outbuffer1_in_v;
			pe_current->outTableBuffer1.in_tag = pe_current->outbuffer1_in_tag;
			//入数、出数
			pe_current->outTableBuffer1.dataIn();
			pe_current->outTableBuffer1.dataOut();
			//数据从类中导出到PE
			pe_current->dout1 = pe_current->outTableBuffer1.out;
			pe_current->dout1_v = pe_current->outTableBuffer1.out_v;
			pe_current->dout1_tag = pe_current->outTableBuffer1.out_tag;
		}
		
	}

	if (pe_current->config_reg.front()[16])//ob2非悬空
	{
		if (pe_current->config_reg.front()[16] == 1)//ob2 from alu
		{
			pe_current->outbuffer2_in = pe_current->alu_out;
			pe_current->outbuffer2_in_v = pe_current->alu_out_v;

		}

		else if (pe_current->config_reg.front()[16] == 3)//ob2 from in2
		{
		}
		else if (pe_current->config_reg.front()[16] == 2)//ob2 from lr
		{
			pe_current->outbuffer2_in = pe_current->loc_reg;
			pe_current->outbuffer2_in_v = pe_current->loc_reg_v;
		}

		//pe_current->outbuffer2_in_v = 1;
		if (pe_current->outbuffer2_in_v)
		{
			//数据导入到outTableBuffer类中去
			pe_current->outTableBuffer2.in = pe_current->outbuffer2_in;
			pe_current->outTableBuffer2.in_v = pe_current->outbuffer2_in_v;
			pe_current->outTableBuffer2.in_tag = pe_current->outbuffer2_in_tag;
			//入数、出数
			pe_current->outTableBuffer2.dataIn();
			pe_current->outTableBuffer2.dataOut();
			//数据从类中导出到PE
			pe_current->dout2 = pe_current->outTableBuffer2.out;
			pe_current->dout2_v = pe_current->outTableBuffer2.out_v;
			pe_current->dout2_tag = pe_current->outTableBuffer2.out_tag;
		}
		
	}
	outfile << "PE[" << pe_current->config_reg.front()[1] << "]的输出是：dout1: " << pe_current->dout1 << " dout1_v: " << pe_current->dout1_v << " dout1_tag: " << pe_current->dout1_tag << endl;
	outfile << "                                        " << "dout2: " << pe_current->dout2 << " dout2_v: " << pe_current->dout2_v << " dout2_tag: " << pe_current->dout2_tag << endl;

	//if (pe_current->config_reg.front()[15] == 0 && pe_current->config_reg.front()[16] == 0)//ob3非悬空，来自于alu
	//{
	//	pe_current->outbuffer3 = pe_current->alu_out_b;//ob3入数
	//	pe_current->bout = pe_current->outbuffer3;//ob3出数
	//	pe_current->bout_v = 1;
	//	pe_current->bout_tag = pe_current->inbuffer1_out_tag;
	//}

	//检测是否是end节点
	if (pe_current->config_reg.front()[17])
	{
		if (pe_current->dout1_v | pe_current->dout2_v)
		{
			pe_current->bout = 1;
			pe_current->bout_v = 1;//使用out3_v作为end信号的输出，只有在output3没有被使用的情况下才可以使用
		}


	}

	//pe操作完成，入口处的valid值清零
	pe_current->din1_v = 0;
	pe_current->din2_v = 0;
	pe_current->bin = 0;
	pe_current->inbuffer1_out_v = 0;
	pe_current->inbuffer2_out_v = 0;
	pe_current->inbuffer3_out_v = 0;

}

*/

void LeSimProcess(Load* le_current,LSUnit* lsunit)
{
	int le_index_current = le_current->config_reg.front()[1];
	int le_infrom_index = le_current->config_reg.front()[2];
	int le_infrom_port = le_current->config_reg.front()[3];
	bool tag_mode = le_current->config_reg.front()[4];
	if (tag_mode)//进行tag处理
	{
		//le输出valid信号置零
		le_current->data_out_v = 0;
		outfile << "LE[" << le_index_current << "]需要进行tag处理" << endl;
		//addr fetch
		if (le_infrom_port == 0)//地址结果来源于PE的port1
		{
			pe_outbuffer_out(pe[le_infrom_index], 0);
			if (pe[le_infrom_index]->dout1_v)
			{
				le_current->addr_in = pe[le_infrom_index]->dout1;
				le_current->addr_in_v = pe[le_infrom_index]->dout1_v;
				le_current->addr_in_tag = pe[le_infrom_index]->dout1_tag;
			}
			else
				le_current->addr_in_v = 0;
		}
		else if (le_infrom_port == 1)//地址输入来自于上一个PE的port2
		{
			pe_outbuffer_out(pe[le_infrom_index], 1);
			if (pe[le_infrom_index]->dout2_v)
			{
				le_current->addr_in = pe[le_infrom_index]->dout2;
				le_current->addr_in_v = pe[le_infrom_index]->dout2_v;
				le_current->addr_in_tag = pe[le_infrom_index]->dout2_tag;
			}
			else
				le_current->addr_in_v = 0;
		}
		/*outfile << "LE[" << le_index_current << "]的地址输入是：" << " addr_in: " << le_current->addr_in << " addr_in_v: " << le_current->addr_in_v << " addr_in_tag: " << le_current->addr_in_tag << endl;*/
		//for debug		
		outfile2 << "LE[" << le_index_current << "]输入值" << endl;
		outfile2 << setw(15) << "addr_t" << setw(15) << "addr_v" << setw(15) << "addr" << endl;
		outfile2 << setw(15) << le_current->addr_in_tag << setw(15) << le_current->addr_in_v << setw(15) << le_current->addr_in << endl;
		


		
		//addr in buffer
		pair<unsigned int, int> addrANDdata;
		if (le_current->addr_in_v)//线上数据有效，进行入table操作
		{
			le_current->addrInTableBuffer();
			//PE->LE情况下LE清空PE的ob，只针对一对一情况，一对多joinbp节点负责
			if (1)
			{
				int port1_fanout_num, port2_fanout_num, port3_fanout_num;//--这三个变量是该节点的上一级节点输出端口的扇出
				for (int i = 0; i < (int)port_fanout.size(); i++)
				{
					if (le_infrom_index == port_fanout[i][1] && port_fanout[i][0] == 0)
					{
						if (le_infrom_port == 0)//port1
						{
							port1_fanout_num = port_fanout[i][2];
						}
						else if (le_infrom_port == 1)//port2
						{
							port2_fanout_num = port_fanout[i][3];
						}
						else if (le_infrom_port == 2)//port3
						{
							port3_fanout_num = port_fanout[i][4];
						}

					}
				}
				//开始清空相应的端口数据
				if (le_infrom_port == 0)//port1
				{
					if (port1_fanout_num <= 1)//一对一的情况
					{
						if (le_current->ack2addrgen)
						{
							pe[le_infrom_index]->outTableBuffer1.buffer_clear();
							pe[le_infrom_index]->dout1_v = 0;
							pe[le_infrom_index]->ack_outbuffer12alu = 1;
						}
					}
				}
				else if (le_infrom_port == 1)//port2
				{
					if (port2_fanout_num <= 1)//一对一的情况
					{
						if (le_current->ack2addrgen)
						{
							pe[le_infrom_index]->outTableBuffer2.buffer_clear();
							pe[le_infrom_index]->dout2_v = 0;
							pe[le_infrom_index]->ack_outbuffer22alu = 1;
						}
					}
				}
				else if (le_infrom_port == 2)//port3
				{
					if (port3_fanout_num <= 1)//一对一的情况
					{
						if (le_current->ack2addrgen)
						{
							pe[le_infrom_index]->bout_v = 0;
						}
					}
				}
			}
		}	
		//addr_out_v置位
		if (!le_current->addr_out_v)
		{
			le_current->addr_out_buffer();
		}
		//load success,addr into buffer,else hold
		//if (le_current->load_success)
		//{
		//	//check whether loading is succeed
		//	le_current->addr_out_buffer();
		//	le_current->load_success = 0;
		//}
		
		//addr into memory
		if (le_current->addr_out_v)
		{
			if (ATTACH_MEMORY == 0)//不附加memory
			{
				addrANDdata = memory.read(le_current->addr_out);
				le_current->data_in = addrANDdata.second;
				le_current->data_in_v = 1;
				le_current->data_in_buffer();
				le_current->data_output_tag();
				//load succeed,clear valid and fetch another new addr				
				le_current->buffer_clear();
			}
			else
			{	//附加memory
				if (!le_current->ack)//ack==0,LSUnit没有接收数据
				{
					lsunit->AddTrans(le_current->addr_out, le_index_current, (int)le_current->addr_out_v);
				}
				
				outfile << "addr_out_v: " << le_current->addr_out_v << "  ack: " << le_current->ack << "  load_success: " << le_current->load_success << endl;
				if (le_current->load_success)
				{
					//data into buffer to match and output
					addrANDdata = memory.read(le_current->addr_out);
					le_current->data_in = addrANDdata.second;
					le_current->data_in_v = 1;
					le_current->data_in_buffer();
					le_current->data_output_tag();
					//load succeed,clear valid and fetch another new addr
					le_current->addr_out_v = 0;
					le_current->ack = 0;
					le_current->buffer_clear();
					le_current->load_success = 0;
				}
			}						
		}
		/*outfile2 << "LE[" << le_index_current << "]输出值" << endl;
		outfile2 << setw(15) << "data_out_t" << setw(15) << "data_out_v" << setw(15) << "data_out" << endl;
		outfile2 << setw(15) << le_current->data_out_tag << setw(15) << le_current->data_out_v << setw(15) << le_current->data_out << endl;*/
		//
	outfile << "inTableBuffer中的内容" << endl;
	outfile.width(8);
	outfile << "tag";
	outfile.width(8);
	outfile << "valid";
	outfile.width(8);
	outfile << "addr";
	outfile.width(8);
	outfile << "data" << endl;	
	for (int i = 0; i < intablebuffer_depth; i++)
	{
		outfile.width(8);
		outfile << le_current->le_table_buffer[i].tag;
		outfile.width(8);
		outfile << le_current->le_table_buffer[i].valid;
		outfile.width(8);
		outfile << le_current->le_table_buffer[i].addr;
		outfile.width(8);
		outfile << le_current->le_table_buffer[i].data;
		outfile << endl;
	}
	outfile << "--------------------------------" << endl;
	/*outfile << "LE[" << le_index_current << "]的输出是：" << " data_out: " << le_current->data_out << " data_out_v: " << le_current->data_out_v << " data_out_tag: " << le_current->data_out_tag << endl;*/
	
	}
	else
	{
		//不需要进行tag处理
		//le输出valid信号置零
		le_current->data_out_v = 0;
		//addr fetch
		if (le_infrom_port == 0)//地址结果来自于上一个PE的port1
		{
			pe_outbuffer_out_no_tag(pe[le_infrom_index], 0);
			if (pe[le_infrom_index]->dout1_v)
			{
				le_current->addr_in = pe[le_infrom_index]->dout1;
				le_current->addr_in_v = pe[le_infrom_index]->dout1_v;
			}
			else
				cout << "上一个计算地址的PE数据没有valid." << endl;
		}
		else if (le_infrom_port == 1)//地址输入来自于上一个PE的port2
		{
			pe_outbuffer_out_no_tag(pe[le_infrom_index], 1);
			if (pe[le_infrom_index]->dout2_v)
			{
				le_current->addr_in = pe[le_infrom_index]->dout2;
				le_current->addr_in_v = pe[le_infrom_index]->dout2_v;
			}
			else
				cout << "上一个计算地址的PE数据没有valid." << endl;
		}
		else
		{
		}
		/*outfile << "LE[" << le_index_current << "]的地址输入是：" << " addr_in: " << le_current->addr_in << " addr_in_v: " << le_current->addr_in_v <<  endl;*/
		//入口数据检查时钟
		//le_current->entry_check();
		//for debug
		outfile2 << "LE[" << le_index_current << "]输入值" << endl;
		outfile2 << setw(15) << "addr_t" << setw(15) << "addr_v" << setw(15) << "addr" << endl;
		outfile2 << setw(15) << le_current->addr_in_tag << setw(15) << le_current->addr_in_v << setw(15) << le_current->addr_in << endl;

		if (le_current->addr_in_v)//检测地址数据是不是准备好了
		{			
			le_current->addrInBuffer();
		}
		if (ATTACH_MEMORY == 1)//外挂了memory
		{
			if (le_current->addr_out_v)
			{
				if (!le_current->ack)//LSUnit没有接收地址数据，继续发
				{
					lsunit->AddTrans(le_current->addr_out, le_index_current, (int)le_current->addr_out_v);
				}
			}
			lsunit->update();
			//
			if (le_current->load_success)
			{
				//if load_success == 1,will trigger three operator
				//1.laod data from self-designed memory
				//2.clear addr_out_v and addr_out
				//3.clear tablebuffer
				pair<unsigned int, int> addrANDdata;
				addrANDdata = memory.read(le_current->addr_out);
				//memory latency cycle
				//le_current->memory_latency();
				le_current->data_in = addrANDdata.second;
				le_current->data_in_v = 1;
				le_current->dataInBuffer();
				le_current->dataOutput();
				le_current->addr_out_v = 0;
				le_current->ack = 0;
				le_current->dataPair.first = 0;
				le_current->dataPair.second = 0;
				le_current->load_success = 0;
			}			
		}
		else
		{
			//没有外挂memory
			if (le_current->addr_out_v)
			{
				pair<unsigned int, int> addrANDdata;
				addrANDdata = memory.read(le_current->addr_out);
				//memory latency cycle
				//le_current->memory_latency();
				le_current->data_in = addrANDdata.second;
				le_current->data_in_v = 1;
				le_current->dataInBuffer();
				le_current->dataOutput();
				le_current->addr_out_v = 0;
				le_current->dataPair.first = 0;
				le_current->dataPair.second = 0;
			}
			
		}				
		//le操作完成，入口处的valid信号清零
		le_current->addr_in_v = 0;
		
		//outfile << "   " << endl;
		////outfile << "clock[" << cnt << "] " << endl;
		//outfile << "LE[" << le_index_current << "]的输出结果是data_out： " << le_current->data_out << "; data_out_v: " << le_current->data_out_v << endl;

		
	}
}

void SeSimProcess(Store* se_current, LSUnit* lsunit)
{
	int se_index_current = se_current->config_reg.front()[1];
	int addr_in_from = se_current->config_reg.front()[2];//index
	int addr_in_port = se_current->config_reg.front()[3];//index
	int addr_in_flag = se_current->config_reg.front()[4];

	int data_in_from = se_current->config_reg.front()[5];
	int data_in_port = se_current->config_reg.front()[6];
	int data_in_flag = se_current->config_reg.front()[7];
	//se extra out for end node 
	se_current->se_extra_out_for_end = 0;
	se_current->se_extra_out_for_end_v = 0;
	bool tag_mode = se_current->config_reg.front()[8];
	if (tag_mode)//进行tag处理
	{
		//addr fetch
		if (addr_in_flag == 0)//from pe
		{
			if (addr_in_port == 0)//port1
			{
				pe_outbuffer_out(pe[addr_in_from], 0);
				if (pe[addr_in_from]->dout1_v)
				{
					se[se_index_current]->addr = pe[addr_in_from]->dout1;
					se[se_index_current]->addr_v = pe[addr_in_from]->dout1_v;
					se[se_index_current]->addr_tag = pe[addr_in_from]->dout1_tag;
					//pe[addr_in_from]->outTableBuffer1.buffer_clear();
				}
			}
			else if (addr_in_port == 1)//port2
			{
				pe_outbuffer_out(pe[addr_in_from], 1);
				if (pe[addr_in_from]->dout2_v)
				{
					se[se_index_current]->addr = pe[addr_in_from]->dout2;
					se[se_index_current]->addr_v = pe[addr_in_from]->dout2_v;
					se[se_index_current]->addr_tag = pe[addr_in_from]->dout2_tag;
					//pe[addr_in_from]->outTableBuffer2.buffer_clear();
				}
			}
		}
		else if (addr_in_flag == 1)//from le
		{
			if (le[addr_in_from]->data_out_v)
			{
				se[se_index_current]->addr = le[addr_in_from]->data_out;
				se[se_index_current]->addr_v = le[addr_in_from]->data_out_v;
				se[se_index_current]->addr_tag = le[addr_in_from]->data_out_tag;
			}
		}

		//data fetch
		if (data_in_flag == 0)//from pe
		{
			if (data_in_port == 0)//port1
			{
				pe_outbuffer_out(pe[data_in_from], 0);
				if (pe[data_in_from]->dout1_v)
				{
					se[se_index_current]->data_in = pe[data_in_from]->dout1;
					se[se_index_current]->data_in_v = pe[data_in_from]->dout1_v;
					se[se_index_current]->data_in_tag = pe[data_in_from]->dout1_tag;
					//pe[data_in_from]->outTableBuffer1.buffer_clear();
				}
				//else
					//cout << "SE需要的数据还没有准备好。" << endl;
			}
			else if (data_in_port == 1)//port2
			{
				pe_outbuffer_out(pe[data_in_from], 1);
				if (pe[data_in_from]->dout2_v)
				{
					se[se_index_current]->data_in = pe[data_in_from]->dout2;
					se[se_index_current]->data_in_v = pe[data_in_from]->dout2_v;
					se[se_index_current]->data_in_tag = pe[data_in_from]->dout1_tag;
					//pe[data_in_from]->outTableBuffer2.buffer_clear();
				}
				//else
					//cout << "SE需要的数据还没有准备好。" << endl;
			}
		}
		else if (data_in_flag == 1)//from le
		{
			if (le[data_in_from]->data_out_v)
			{
				se[se_index_current]->data_in = le[data_in_from]->data_out;
				se[se_index_current]->data_in_v = le[data_in_from]->data_out_v;
				se[se_index_current]->data_in_tag = le[data_in_from]->data_out_tag;
			}
		}

		outfile << "SE[" << se_current->config_reg.front()[1] << "]的输入是：addr: " << se_current->addr << " addr_v: " << se_current->addr_v << " addr_tag: " << se_current->addr_tag << endl;
		outfile << "                                        " << "data_in: " << se_current->data_in << " data_in_v: " << se_current->data_in_v << " data_in_tag: " << se_current->data_in_tag << endl;

		//for debug
		outfile2 << endl;
		outfile2 << "SE[ " << se_index_current << " ]" << endl;
		outfile2 << "--------------------------------------" << endl;
		outfile2 << "SE[" << se_index_current << "]输入值" << endl;
		outfile2 << setw(15) << "addr_t" << setw(15) << "addr_v" << setw(15) << "addr" << setw(15) << "data_t" << setw(15) << "data_v" << setw(15) << "data" << endl;
		outfile2 << setw(15) << se_current->addr_tag << setw(15) << se_current->addr_v << setw(15) << se_current->addr << setw(15) << se_current->data_in_tag << setw(15)
			<< se_current->data_in_v << setw(15) << se_current->data_in << endl;
		//ack clear
		se_current->ack2addr_source_node = 0;
		se_current->ack2data_source_node = 0;
		//入数、出数
		se_current->addr_in_table();
		se_current->data_in_table();
		outfile << "inTableBuffer中的内容" << endl;
		outfile << "--------------------------------" << endl;
		outfile.width(10);
		outfile << "tag";
		outfile.width(10);
		outfile << "valid1";
		outfile.width(10);
		outfile << "ADDR";
		outfile.width(10);
		outfile << "valid2";
		outfile.width(10);
		outfile << "data";
		outfile.width(10);
		outfile << "lineOK" << endl;
		for (int i = 0; i < intablebuffer_depth; i++)
		{
			outfile.width(10);
			outfile << se_current->se_table_buffer[i].tag;
			outfile.width(10);
			outfile << se_current->se_table_buffer[i].valid1;
			outfile.width(10);
			outfile << se_current->se_table_buffer[i].addr;
			outfile.width(10);
			outfile << se_current->se_table_buffer[i].valid2;
			outfile.width(10);
			outfile << se_current->se_table_buffer[i].data;
			outfile.width(10);
			outfile << se_current->se_table_buffer[i].lineOK;
			outfile << endl;
		}
		//数据入表成功，清零
		//统计来源的扇出，若扇出大于1则不去清上一级的数据 
		if (1)                     //SE的din可能来自于LE和PE
		{
			int port1_fanout_num, port2_fanout_num, port3_fanout_num;//--这三个变量是该PE的上一级PE输出端口的扇出
			for (int i = 0; i < (int)port_fanout.size(); i++)
			{
				if (addr_in_from == port_fanout[i][1] && addr_in_flag == port_fanout[i][0])     //来自PE或LE
				{
					if (addr_in_port == 0)//port1
					{
						port1_fanout_num = port_fanout[i][2];
					}
					else if (addr_in_port == 1)//port2
					{
						port2_fanout_num = port_fanout[i][3];
					}
					else if (addr_in_port == 2)//port3
					{
						port3_fanout_num = port_fanout[i][4];
					}
				}
			}
			//开始清空相应的端口数据
			if (addr_in_port == 0)//port1
			{
				if (port1_fanout_num <= 1)//一对一的情况
				{
					if (se_current->ack2addr_source_node)
					{
						se_current->addr_v = 0;
						pe[addr_in_from]->outTableBuffer1.buffer_clear();
						pe[addr_in_from]->dout1_v = 0;
						pe[addr_in_from]->ack_outbuffer12alu = 1;
					}
				}
			}
			else if (addr_in_port == 1)//port2
			{
				if (port2_fanout_num <= 1)//一对一的情况
				{
					if (se_current->ack2addr_source_node)
					{
						se_current->addr_v = 0;
						pe[addr_in_from]->outTableBuffer2.buffer_clear();
						pe[addr_in_from]->dout2_v = 0;
						pe[addr_in_from]->ack_outbuffer22alu = 1;
					}
				}
			}

			for (int i = 0; i < (int)port_fanout.size(); i++)
			{
				if (data_in_from == port_fanout[i][1] && data_in_flag == port_fanout[i][0])     //来自PE或LE
				{
					if (data_in_port == 0)//port1
					{
						port1_fanout_num = port_fanout[i][2];
					}
					else if (data_in_port == 1)//port2
					{
						port2_fanout_num = port_fanout[i][3];
					}
					else if (data_in_port == 2)//port3
					{
						port3_fanout_num = port_fanout[i][4];
					}
				}
			}
			//开始清空相应的端口数据
			if (data_in_port == 0)//port1
			{
				if (port1_fanout_num <= 1)//一对一的情况
				{
					if (se_current->ack2data_source_node)
					{
						se_current->data_in_v = 0;
						pe[data_in_from]->outTableBuffer1.buffer_clear();
						pe[data_in_from]->dout1_v = 0;
						pe[data_in_from]->ack_outbuffer12alu = 1;
					}
				}
			}
			else if (data_in_port == 1)//port2
			{
				if (port2_fanout_num <= 1)//一对一的情况
				{
					if (se_current->ack2data_source_node)
					{
						se_current->data_in_v = 0;
						pe[data_in_from]->outTableBuffer2.buffer_clear();
						pe[data_in_from]->dout2_v = 0;
						pe[data_in_from]->ack_outbuffer22alu = 1;
					}
				}
			}

		}
		if (se_current->ack2data_source_node)
			se_current->data_in_v = 0;
		if (se_current->ack2addr_source_node)
			se_current->addr_v = 0;

		se_current->dataOut();
		outfile2 << "SE[" << se_index_current << "]输出值" << endl;
		outfile2 << setw(15) << "ib1_out_t" << setw(15) << "ib1_out_v" << setw(15) << "ib1_out" << setw(15) << "ib2_out_t" << setw(15) << "ib2_out_v" << setw(15) << "ib2_out" << endl;
		outfile2 << setw(15) << se_current->addr_out_tag << setw(15) << se_current->addr_out_v << setw(15) << se_current->addr_out << setw(15) << se_current->data_out_tag << setw(15) << se_current->data_out_v << setw(15)
			<< se_current->data_out << endl;
		//开始write
		if (se_current->addr_out_v && se_current->data_out_v)
		{
			se_current->write2men(memory2, se_current->addr_out, se_current->data_out);
			//write cycle count
			//se_current->write_latency();
			se_current->se_extra_out_for_end = 1;
			se_current->se_extra_out_for_end_v = 1;
			//se操作完成，入口处的valid值清零
			se_current->addr_out_v = 0;
			se_current->data_out_v = 0;
			//SE buffer clear
			se_current->buffer_clear();
		}
	}
	else
	{
		//不进行tag处理
		//addr fetch,default config is PE
		if (addr_in_flag == 0)//from pe
		{
			if (addr_in_port == 0)//port1
			{
				pe_outbuffer_out_no_tag(pe[addr_in_from], 0);
				if (pe[addr_in_from]->dout1_v)
				{

					se[se_index_current]->addr = pe[addr_in_from]->dout1;
					se[se_index_current]->addr_v = pe[addr_in_from]->dout1_v;

				}
				else
				{
					cout << "SE需要的地址还没有准备好。" << endl;
				}
			}
			else if (addr_in_port == 1)//port2
			{
				pe_outbuffer_out_no_tag(pe[addr_in_from], 1);
				if (pe[addr_in_from]->dout2_v)
				{
					se[se_index_current]->addr = pe[addr_in_from]->dout2;
					se[se_index_current]->addr_v = pe[addr_in_from]->dout2_v;
				}
				else
				{
					cout << "SE需要的地址还没有准备好。" << endl;
				}
			}
			else
			{
			}
		}
		else if (addr_in_flag == 1)//from le
		{
			if (le[addr_in_from]->data_out_v)
			{
				se[se_index_current]->addr = le[addr_in_from]->data_out;
				se[se_index_current]->addr_v = le[addr_in_from]->data_out_v;
			}
		}


		//data fetch
		if (data_in_flag == 0)//from pe
		{
			if (data_in_port == 0)//port1
			{
				pe_outbuffer_out_no_tag(pe[data_in_from], 0);
				if (pe[data_in_from]->dout1_v)
				{
					se[se_index_current]->data_in = pe[data_in_from]->dout1;
					se[se_index_current]->data_in_v = pe[data_in_from]->dout1_v;
				}
				//else
					//cout << "SE需要的数据还没有准备好。" << endl;
			}
			else if (data_in_port == 1)//port2
			{
				pe_outbuffer_out_no_tag(pe[data_in_from], 1);
				if (pe[data_in_from]->dout2_v)
				{
					se[se_index_current]->data_in = pe[data_in_from]->dout2;
					se[se_index_current]->data_in_v = pe[data_in_from]->dout2_v;
				}
				//else
					//cout << "SE需要的数据还没有准备好。" << endl;
			}
			//else
				//cout << "se data_in_from_port is out of range." << endl;
		}
		else if (data_in_flag == 1)//from le
		{
			if (le[data_in_from]->data_out_v)
			{
				se[se_index_current]->data_in = le[data_in_from]->data_out;
				se[se_index_current]->data_in_v = le[data_in_from]->data_out_v;
			}
		}


		//for debug
		outfile2 << endl;
		outfile2 << "SE[ " << se_index_current << " ]" << endl;
		outfile2 << "--------------------------------------" << endl;
		outfile2 << "SE[" << se_index_current << "]输入值" << endl;
		outfile2 << setw(15) << "addr_t" << setw(15) << "addr_v" << setw(15) << "addr" << setw(15) << "data_t" << setw(15) << "data_v" << setw(15) << "data" << endl;
		outfile2 << setw(15) << se_current->addr_tag << setw(15) << se_current->addr_v << setw(15) << se_current->addr << setw(15) << se_current->data_in_tag << setw(15)
			<< se_current->data_in_v << setw(15) << se_current->data_in << endl;

		//entry check cycle count
		//se[se_index_current]->entry_check_cycle();
		//SE数据打印
		//outfile << "clock[" << cnt << "] " << endl;
		outfile << "SE[" << se_index_current << "]的输入数据是：addr_in: " << se[se_index_current]->addr << "  addr_in_v: " << se[se_index_current]->addr_v << endl;
		outfile << "               " << "data_in: " << se[se_index_current]->data_in << "  data_in_v: " << se[se_index_current]->data_in_v << endl;

		if (se[se_index_current]->addr_v && se[se_index_current]->data_in_v)
		{
			se_current->addr_out_v = se_current->addr_v;
			se_current->addr_out = se_current->addr;
			se_current->data_out_v = se_current->data_in_v;
			se_current->data_out = se_current->data_in;
			//入口数据清零
			se_current->addr_v = 0;
			se_current->data_in_v = 0;
			//开始写入
			se[se_index_current]->write2men(memory2, se_current->addr_out, se_current->data_out);
			//write cycle count
			//se[se_index_current]->write_latency();
			se[se_index_current]->se_extra_out_for_end = 1;
			se_current->se_extra_out_for_end_v = 1;
			//se操作完成，入口处的valid值清零
			se[se_index_current]->addr_v = 0;
			se[se_index_current]->data_in_v = 0;
		}
	}
}

void LbeginSimProcess(LoopBegin* lbegin_current)
{
	int lb_index_current = lbegin_current->config_reg.front()[1];
	int lb_in1_from_index = lbegin_current->config_reg.front()[2];
	int lb_in1_from_port = lbegin_current->config_reg.front()[3];
	int lb_in1_from_flag = lbegin_current->config_reg.front()[4];
	int lb_in2_from_index = lbegin_current->config_reg.front()[5];
	int lb_in2_from_port = lbegin_current->config_reg.front()[6];
	int lb_in2_from_flag = lbegin_current->config_reg.front()[7];
	//线上数据清零
	lbegin_current->begin_in = 0;
	lbegin_current->feedback_in = 0;
	lbegin_current->out_v = 0;
	lbegin_current->out = 0;
	//loopbegin的输入1取数
	if (lb_in1_from_flag == 0)//begin
	{
		lbegin_current->begin_in = begin_signal;
	}
	else if (lb_in1_from_flag == 3)//loopend
	{
		if (lb_in1_from_port == 0)//port1
		{
			lbegin_current->begin_in = lend[lb_in1_from_index]->out2back;
		}
		else if (lb_in1_from_port == 1)//port2
		{
			lbegin_current->begin_in = lend[lb_in1_from_index]->out2end;
		}
	}
	else if (lb_in1_from_flag == 1)//PE
	{
		if (lb_in1_from_port == 0)//port1
		{
			//pe ob1 出数
			if (pe[lb_in1_from_index]->config_reg.front()[19] == 1)//tag mode
			{
				pe_outbuffer_out(pe[lb_in1_from_index], 0);
			}
			else
			{
				pe_outbuffer_out_no_tag(pe[lb_in1_from_index], 0);
			}
			lbegin_current->begin_in = pe[lb_in1_from_index]->dout1_v;
		}
		else if (lb_in1_from_port == 1)//port2
		{
			//pe ob2 出数
			if (pe[lb_in1_from_index]->config_reg.front()[19] == 1)//tag mode
			{
				pe_outbuffer_out(pe[lb_in1_from_index], 1);
			}
			else
			{
				pe_outbuffer_out_no_tag(pe[lb_in1_from_index], 1);
			}
			lbegin_current->begin_in = pe[lb_in1_from_index]->dout2_v;
		}
		else if (lb_in1_from_port == 2)//port3
		{
			//pe ob3 出数
			if (pe[lb_in1_from_index]->config_reg.front()[19] == 1)//tag mode
			{
				pe_outbuffer_out(pe[lb_in1_from_index], 2);
			}
			else
			{
				pe_outbuffer_out_no_tag(pe[lb_in1_from_index], 2);
			}
			lbegin_current->begin_in = pe[lb_in1_from_index]->bout;//常规连接，begin_in接bout不是bout_v
		}
	}
	else if (lb_in1_from_flag == 7)//loopend2
	{
	}
	//in2取数
	if (lb_in2_from_flag == 0)//begin
	{
		lbegin_current->feedback_in = begin_signal;
	}
	else if (lb_in2_from_flag == 3)//loopend
	{
		if (lb_in2_from_port == 0)//port1
		{
			lbegin_current->feedback_in = lend[lb_in2_from_index]->out2back;
		}
		else if (lb_in2_from_port == 1)//port2
		{
			lbegin_current->feedback_in = lend[lb_in2_from_index]->out2end;
		}
	}
	else if (lb_in2_from_flag == 1)//PE
	{
		if (lb_in2_from_port == 0)//port1
		{
			//pe ob1 出数
			if (pe[lb_in2_from_index]->config_reg.front()[19] == 1)//tag mode
			{
				pe_outbuffer_out(pe[lb_in2_from_index], 0);
			}
			else
			{
				pe_outbuffer_out_no_tag(pe[lb_in2_from_index], 0);
			}
			lbegin_current->feedback_in = pe[lb_in2_from_index]->dout1_v;
		}
		else if (lb_in2_from_port == 1)//port2
		{
			//pe ob2 出数
			if (pe[lb_in2_from_index]->config_reg.front()[19] == 1)//tag mode
			{
				pe_outbuffer_out(pe[lb_in2_from_index], 1);
			}
			else
			{
				pe_outbuffer_out_no_tag(pe[lb_in2_from_index], 1);
			}
			lbegin_current->feedback_in = pe[lb_in2_from_index]->dout2_v;
		}
		else if (lb_in2_from_port == 2)//port3
		{
			//pe ob3 出数
			if (pe[lb_in2_from_index]->config_reg.front()[19] == 1)//tag mode
			{
				pe_outbuffer_out(pe[lb_in2_from_index], 2);
			}
			else
			{
				pe_outbuffer_out_no_tag(pe[lb_in2_from_index], 2);
			}
			lbegin_current->feedback_in = pe[lb_in2_from_index]->bout;//常规连接，begin_in接bout不是bout_v
		}
	}
	else if (lb_in2_from_flag == 7)//loopend2
	{
	}
	//for debug
	outfile2 << "lbegin[" << lb_index_current << "]输入值" << endl;
	outfile2 << setw(15) << "begin_in" << setw(15) << "feedback_in" << endl;
	outfile2 << setw(15) << lbegin_current->begin_in << setw(15) << lbegin_current->feedback_in << endl;


	/*outfile << "Lbegin[" << lb_index_current << "]的输入是：" << " begin_in: " << lbegin_current->begin_in << " feedback_in:" << lbegin_current->feedback_in << endl;*/
	lbegin_current->lbegin();
	//outfile << "Lbegin[" << lb_index_current << "]的输出是：" << " out_v: " << lbegin_current->out_v << " out:" << lbegin_current->out << endl;
	
}

void LendSimProcess(LoopEnd* lend_current)
{
	int lend_index_current = lend_current->config_reg.front()[1];
	int lend_in1_from_index = lend_current->config_reg.front()[2];
	int lend_in1_from_port = lend_current->config_reg.front()[3];
	int lend_in1_from_flag = lend_current->config_reg.front()[4];
	int lend_in2_from_index = lend_current->config_reg.front()[5];
	int lend_in2_from_port = lend_current->config_reg.front()[6];
	int lend_in2_from_flag = lend_current->config_reg.front()[7];
	//loop end的 2bit输入端口要有keep and wait的功能，所以开始时不清零，但是另外一个端口是需要清零的
	lend_current->in2 = 0;
	//in1取数
	if (lend_in1_from_flag == 1)//pe
	{
		if (lend_in1_from_port == 0)//port1
		{
			//
		}
		else if (lend_in1_from_port == 1)//port2
		{
			//
		}
		else if (lend_in1_from_port == 2)//port3
		{
			pe_outbuffer_out_no_tag(pe[lend_in1_from_index], 2);
			if (pe[lend_in1_from_index]->bout_v)
			{
				//有效才取数，保证keep and wait的功能
				lend_current->in1_v = pe[lend_in1_from_index]->bout_v;
				lend_current->in1 = pe[lend_in1_from_index]->bout;
			}
			
		}
	}
	else if (lend_in1_from_flag == 3)//loopend
	{
	}
	else if (lend_in1_from_flag == 4)//join
	{
	}
	//in2取数
	if (lend_in2_from_flag == 3)//loopend
	{
		if (lend_in2_from_port == 0)//port1
		{
			lend_current->in2 = lend[lend_in2_from_index]->out2back;
		}
		else if (lend_in2_from_port == 1)//port2
		{
			lend_current->in2 = lend[lend_in2_from_index]->out2end;
		}
	}
	else if (lend_in2_from_flag == 1)//from PE
	{
		if (lend_in2_from_port == 0)//port1
		{
			pe_outbuffer_out_no_tag(pe[lend_in2_from_index], 0);
			lend_current->in2 = pe[lend_in2_from_index]->dout1_v;
		}
		else if (lend_in2_from_port == 1)//port2
		{
			pe_outbuffer_out_no_tag(pe[lend_in2_from_index], 1);
			lend_current->in2 = pe[lend_in2_from_index]->dout2_v;
		}
		else if (lend_in2_from_port == 2)//port3
		{
			pe_outbuffer_out_no_tag(pe[lend_in2_from_index], 2);
			lend_current->in2 = pe[lend_in2_from_index]->bout;
		}
	}
	else if (lend_in2_from_flag == 9)//SE
	{
		lend_current->in2 = se[lend_in2_from_index]->se_extra_out_for_end_v;//使用SE的额外输出的valid为作为有效信号
	}
	//for debug	
	outfile2 << "lend[" << lend_index_current << "]输入值" << endl;
	outfile2 << setw(15) << "in1_v" << setw(15) << "in1" << setw(15) << "in2" << endl;
	outfile2 << setw(15) << lend_current->in1_v << setw(15) << lend_current->in1 << setw(15) <<lend_current->in2 << endl;

	//outfile << "Lend[" << lend_index_current << "]的输入是：" << " in1: " << lend_current->in1 << " in1_v:" << lend_current->in1_v << " in2: " << lend_current->in2 << endl;
	//
	lend_current->lend();
	//
	//outfile << "Lend[" << lend_index_current << "]的输出是：" << " out2back: " << lend_current->out2back << " out2end:" << lend_current->out2end << endl;

	
}

void JoinSimProcess(Join* join_current)
{
	auto inport_num = (join_current->config_reg.front().size() - 3) / 3;
	//resize inputCollect
	join_current->inputCollect.resize(inport_num);

	auto join_index_current = join_current->config_reg.front()[1];
	//data fetch
	for (int i = 0; i < (int)inport_num; i++)
	{
		if (join_current->config_reg.front()[4 + i * 3] == 1)//from PE
		{
			if (join_current->config_reg.front()[3 + i * 3] == 2)//PE的port3，也是默认值
			{
				auto in_from_index = join_current->config_reg.front()[2 + i * 3];
				if (pe[in_from_index]->bout_v)
				{
					join_current->inputCollect[i] = pe[in_from_index]->bout;
				}
			}
			else if (join_current->config_reg.front()[3 + i * 3] == 0)//port1
			{
			}
			else if (join_current->config_reg.front()[3 + i * 3] == 1)//port2
			{
			}
		}
	}
	//join opration
	join_current->join();
	//outfile << "Join[" << join_index_current << "]的输出是：" << join_current->out << endl;
	//都是1激活join操作之后，清空
	if (join_current->out)
	{
		join_current->inputCollect.clear();
	}
}

void JoinBpSimProcess(JoinBp* joinbp_current)
{
	auto inport_num = (joinbp_current->config_reg.front().size() - 3) / 3;
	//resize
	joinbp_current->inputCollect.resize(inport_num);
	auto join_index_current = joinbp_current->config_reg.front()[1];
	//开始仿真之前出口数据清零
	joinbp_current->out = 0;
	//data fetch
	for (int i = 0; i < (int)inport_num; i++)
	{
		//from pe
		if (joinbp_current->config_reg.front()[4 + i * 3] == 1)//from PE
		{
			auto in_from_index = joinbp_current->config_reg.front()[2 + i * 3];
			if (joinbp_current->config_reg.front()[3 + i * 3] == 2)//PE的port3，也是默认值
			{
				if (pe[in_from_index]->ack2in3port)//ack值为1，取数；else 不取
				{
					joinbp_current->inputCollect[i] = pe[in_from_index]->ack2in3port;
				}
			}
			else if (joinbp_current->config_reg.front()[3 + i * 3] == 0)//port1
			{
				if (pe[in_from_index]->ack2in1port)
				{
					joinbp_current->inputCollect[i] = pe[in_from_index]->ack2in1port;
				}
			}
			else if (joinbp_current->config_reg.front()[3 + i * 3] == 1)//port2
			{
				if (pe[in_from_index]->ack2in2port)
				{
					joinbp_current->inputCollect[i] = pe[in_from_index]->ack2in2port;
				}
			}
		}
		//from se
		else if (joinbp_current->config_reg.front()[4 + i * 3] == 9)
		{
			auto in_from_index = joinbp_current->config_reg.front()[2 + i * 3];
			if (joinbp_current->config_reg.front()[3 + i * 3] == 0)//port1
			{
				if (se[in_from_index]->ack2addr_source_node)
				{
					joinbp_current->inputCollect[i] = se[in_from_index]->ack2addr_source_node;
				}
			}
			else if (joinbp_current->config_reg.front()[3 + i * 3] == 1)//port2
			{
				if (se[in_from_index]->ack2data_source_node)
				{
					joinbp_current->inputCollect[i] = se[in_from_index]->ack2data_source_node;
				}
			}
		}
		//from le
		else if (joinbp_current->config_reg.front()[4 + i * 3] == 10)
		{
			auto in_from_index = joinbp_current->config_reg.front()[2 + i * 3];
			if (joinbp_current->config_reg.front()[3 + i * 3] == 0)//port1
			{
				if (le[in_from_index]->ack2addrgen)
				{
					joinbp_current->inputCollect[i] = le[in_from_index]->ack2addrgen;
				}
			}
		}
	}
	//join opration
	joinbp_current->join();
	//清除发送端数据
	//joinbp输出信号为1时，使用buffer_clear()清空上一级的buffer并且将出口处的数据置0
	if (joinbp_current->out)                       //注意joinbp的配置数字规则与PE不同
	{
		auto joinbp_in_from = joinbp_current->config_reg.front()[2];//index
		auto joinbp_in_port = joinbp_current->config_reg.front()[3];//port
		auto joinbp_in_flag = joinbp_current->config_reg.front()[4];
		int joinbp_in_from_from_index, joinbp_in_from_from_port, joinbp_in_from_from_flag;
		//get source node under the circumstances of 1 vs n
		if (joinbp_in_flag == 1)    //joinbp port1 from pe
		{
			if (joinbp_in_port == 0)//port1
			{
				joinbp_in_from_from_index = pe[joinbp_in_from]->config_reg.front()[3];
				joinbp_in_from_from_port = pe[joinbp_in_from]->config_reg.front()[4];
				joinbp_in_from_from_flag = pe[joinbp_in_from]->config_reg.front()[5];
			}
			else if (joinbp_in_port == 1)//port2
			{
				joinbp_in_from_from_index = pe[joinbp_in_from]->config_reg.front()[6];
				joinbp_in_from_from_port = pe[joinbp_in_from]->config_reg.front()[7];
				joinbp_in_from_from_flag = pe[joinbp_in_from]->config_reg.front()[8];
			}
			else if (joinbp_in_port == 2)//port3
			{
				joinbp_in_from_from_index = pe[joinbp_in_from]->config_reg.front()[9];
				joinbp_in_from_from_port = pe[joinbp_in_from]->config_reg.front()[10];
				joinbp_in_from_from_flag = pe[joinbp_in_from]->config_reg.front()[11];
			}
		}
		else if (joinbp_in_flag == 9)     //joinbp port1 from se
		{
			if (joinbp_in_port == 0)
			{
				joinbp_in_from_from_index = se[joinbp_in_from]->config_reg.front()[2];
				joinbp_in_from_from_port = se[joinbp_in_from]->config_reg.front()[3];
				if (se[joinbp_in_from]->config_reg.front()[4] == 0)           //pe
					joinbp_in_from_from_flag = 2;
				else if (se[joinbp_in_from]->config_reg.front()[4] == 1)      //le
					joinbp_in_from_from_flag = 1;
				else
					cout << "error occurs in JoinBpSimProcess!" << endl;
			}
			else if (joinbp_in_port == 1)
			{
				joinbp_in_from_from_index = se[joinbp_in_from]->config_reg.front()[5];
				joinbp_in_from_from_port = se[joinbp_in_from]->config_reg.front()[6];
				if (se[joinbp_in_from]->config_reg.front()[7] == 0)           //pe
					joinbp_in_from_from_flag = 2;
				else if (se[joinbp_in_from]->config_reg.front()[7] == 1)      //le
					joinbp_in_from_from_flag = 1;
				else
					cout << "error occurs in JoinBpSimProcess!" << endl;
			}
		}
		else if (joinbp_in_flag == 10)
		{
			//le only have one port
			if (joinbp_in_port == 0)
			{
				joinbp_in_from_from_index = le[joinbp_in_from]->config_reg.front()[2];
				joinbp_in_from_from_port = le[joinbp_in_from]->config_reg.front()[3];
				joinbp_in_from_from_flag = 2;         //le的来源只为pe
			}
		}
		//start clear
		if (*(joinbp_current->config_reg.front().rbegin()))    //tagmode,负责清除ooutbuffer
		{
			if (joinbp_in_from_from_flag == 2)//PE
			{
				if (joinbp_in_from_from_port == 0)//port1
				{
					pe[joinbp_in_from_from_index]->outTableBuffer1.buffer_clear();
					pe[joinbp_in_from_from_index]->ack_outbuffer12alu = 1;
					//pe[joinbp_in_from_from_index]->dout1_v = 0;
				}
				else if (joinbp_in_from_from_port == 1)//port2
				{
					pe[joinbp_in_from_from_index]->outTableBuffer2.buffer_clear();
					pe[joinbp_in_from_from_index]->ack_outbuffer22alu = 1;
					//pe[joinbp_in_from_from_index]->dout2_v = 0;
				}
				else if (joinbp_in_from_from_port == 2)//port3
				{
					pe[joinbp_in_from_from_index]->outbuffer3_v = 0;
					pe[joinbp_in_from_from_index]->outbuffer3_tag = 0;
					pe[joinbp_in_from_from_index]->outbuffer3 = 0;
					//pe[joinbp_in_from_from_index]->bout_v = 0;
				}
			}
			else if (joinbp_in_from_from_flag == 1)//LE
			{
				if (joinbp_in_from_from_port == 0)
				{
					//清joinbp目标的LE的输出
				}
				else
					cout << "error occurs in JoinBpSimProcess for LE only have one port!" << endl;
			}
			else if (joinbp_in_from_from_flag == 9)//Ti
			{
				ti->out_v = 0;
			}	
		}
		else                                         //静态时joinbp负责出数和清ob
		{
			if (joinbp_in_from_from_flag == 2)//PE
			{
				if (joinbp_in_from_from_port == 0)//port1
				{
					if (pe[joinbp_in_from_from_index]->outBuffer1.outputBuffer.size() != 0)
					{
						pe[joinbp_in_from_from_index]->outBuffer1.dataOut(pe[joinbp_in_from_from_index]->dout1, pe[joinbp_in_from_from_index]->dout1_v);
						//pe[joinbp_in_from_from_index]->outBuffer1.outputBuffer.pop();
						pe[joinbp_in_from_from_index]->ack_outbuffer12alu = 1;
					}
				}
				else if (joinbp_in_from_from_port == 1)//port2
				{
					if (pe[joinbp_in_from_from_index]->outBuffer2.outputBuffer.size() != 0)
					{
						pe[joinbp_in_from_from_index]->outBuffer2.dataOut(pe[joinbp_in_from_from_index]->dout2, pe[joinbp_in_from_from_index]->dout2_v);
						//pe[joinbp_in_from_from_index]->outBuffer2.outputBuffer.pop();
						pe[joinbp_in_from_from_index]->ack_outbuffer22alu = 1;
					}
				}
				else if (joinbp_in_from_from_port == 2)//port3
				{
					pe[joinbp_in_from_from_index]->bout = pe[joinbp_in_from_from_index]->outbuffer3;
					pe[joinbp_in_from_from_index]->bout_v = pe[joinbp_in_from_from_index]->outbuffer3_v;
					pe[joinbp_in_from_from_index]->ack_outbuffer32alu = 1;
					pe[joinbp_in_from_from_index]->outbuffer3_v = 0;
				}
			
			}
			else if (joinbp_in_from_from_flag == 1)//LE
			{
				if (joinbp_in_from_from_port == 0)
				{
					//清joinbp目标的LE的输出
				}
				else
					cout << "error occurs in JoinBpSimProcess for LE only have one port!" << endl;
			}
			else if (joinbp_in_from_from_flag == 9)//Ti
			{
				ti->out_v = 0;
			}
		}

		//激活操作之后，清空vector
		joinbp_current->inputCollect.clear();
	}
}

void SwitchSimProcess(Switch* switch_current)
{
	
	int switch_index_current = switch_current->config_reg.front()[1];
	int switch_in_from_index = switch_current->config_reg.front()[2];
	int switch_in_from_flag = switch_current->config_reg.front()[4];
	int switch_in_from_port = switch_current->config_reg.front()[3];

	bool tag_mode = switch_current->config_reg.front()[5];
	
	if (tag_mode)//需要进行tag处理
	{
		outfile << "进行tag处理...." << endl;
		if (switch_in_from_flag == 1)//pe
		{
			//默认来源端口是input3
			if (pe[switch_in_from_index]->bout_v)
			{
				switch_current->in = pe[switch_in_from_index]->bout;
				switch_current->in_v = pe[switch_in_from_index]->bout_v;
				switch_current->in_tag = pe[switch_in_from_index]->bout_tag;
			}
		}
		else if (switch_in_from_flag == 2)//lbegin
		{
		}
		//开始switch
		if (switch_current->in_v)
		{
			switch_current->path_select_tag();
		}
	}
	else
	{
		//不需要进行tag处理
		switch_current->out1 = 0;
		switch_current->out2 = 0;
		if (switch_in_from_flag == 1)//pe
		{
			//默认来源端口是input3
			if (pe[switch_in_from_index]->bout_v)
			{
				switch_current->in = pe[switch_in_from_index]->bout;
				switch_current->in_v = pe[switch_in_from_index]->bout_v;
			}
		}
		else if (switch_in_from_flag == 2)//lbegin
		{
		}
		//开始switch
		if (switch_current->in_v)
		{
			switch_current->path_select();
			switch_current->in_v = 0;
		}

	}
}

void BreakSimProcess(Break* break_current)
{
	uint break_index_current = break_current->config_reg.front()[1];
	uint break_in1_from_index = break_current->config_reg.front()[2];//from break path
	uint break_in1_from_port = break_current->config_reg.front()[3];
	uint break_in1_from_flag = break_current->config_reg.front()[4];
	uint break_in2_from_index = break_current->config_reg.front()[5];//from join path
	uint break_in2_from_port = break_current->config_reg.front()[6];
	uint break_in2_from_flag = break_current->config_reg.front()[7];
	//in1 and in2 fetch data
	if (break_in1_from_flag == 1)//from PE
	{
		if (pe[break_in1_from_index]->bout_v)
		{
			break_current->from_break_path_v = pe[break_in1_from_index]->bout_v;
			break_current->from_break_path = pe[break_in1_from_index]->bout;
		}
		
	}
	else if (break_in1_from_flag == 2)//from lbegin
	{
	}
	//
	if (break_in2_from_flag == 4)//from join
	{
		break_current->from_join_path = join[break_in2_from_index]->out;
	}
	else if (break_in2_from_flag == 1)//from PE
	{
		//出现的可能性不高
	}
	//开始merge
	break_current->mergeBreak();
	//fire data
	if (break_current->from_join_path == 1)
	{
		break_current->from_break_path_v = 0;
	}

}

void LendsSimProcess(LoopEnds* lends_current)
{
	uint lends_index_current = lends_current->config_reg.front()[1];
	uint lends_in1_from_index = lends_current->config_reg.front()[2];
	uint lends_in1_from_port = lends_current->config_reg.front()[3];
	uint lends_in1_from_flag = lends_current->config_reg.front()[4];
	uint lends_in2_from_index = lends_current->config_reg.front()[5];
	uint lends_in2_from_port = lends_current->config_reg.front()[6];
	uint lends_in2_from_flag = lends_current->config_reg.front()[7];
	//input data fetch
	//in1
	if (lends_in1_from_flag == 1)//PE
	{
		if (pe[lends_in1_from_index]->bout_v)
		{
			lends_current->compare_in = pe[lends_in1_from_index]->bout;
			lends_current->compare_in_v = pe[lends_in1_from_index]->bout_v;
		}
		else
			cout << "不满足取数条件，不取数" << endl;
	}
	else if (lends_in1_from_flag == 2)//lbegin
	{
	}
	//in2
	if (lends_in2_from_flag == 6)//break
	{
		lends_current->break_in = break_[lends_in2_from_index]->out_l;
		lends_current->break_in_v = break_[lends_in2_from_index]->out_h;
	}
	else if (lends_in2_from_flag == 1)//PE
	{
	}
	//开始merge
	lends_current->loopends();
	//data fire
	if (lends_current->compare_in_v && lends_current->break_in_v)
	{
		lends_current->compare_in_v = 0;
		lends_current->break_in_v = 0;
	}
}

//void TaSimProcess(TagAttach* ta_current)
//{
//	int ta_index_current = ta_current->config_reg.front()[1];
//	int ta_in_from_index = ta_current->config_reg.front()[2];
//	int ta_in_from_port = ta_current->config_reg.front()[3];
//	int ta_in_from_flag = ta_current->config_reg.front()[4];
//	//开始仿真之前出口数据清零
//	ta_current->out_v = 0;
//	ta_current->out_tag = 0;
//	ta_current->out = 0;
//	//data fetch
//	if (ta_in_from_flag == 1)//PE
//	{
//		if (ta_in_from_port == 0)//port1
//		{
//			ta_current->in_v = pe[ta_in_from_index]->dout1_v;
//			ta_current->in = pe[ta_in_from_index]->dout1;
//
//		}
//		else if (ta_in_from_port == 1)//port2
//		{
//			ta_current->in_v = pe[ta_in_from_index]->dout2_v;
//			ta_current->in = pe[ta_in_from_index]->dout2;
//		}
//		else if (ta_in_from_port == 2)//port3
//		{
//			//不太可能
//		}
//	}
//	else if (ta_in_from_flag == 2)//from loopbegin
//	{
//	}
//	outfile << "TA[" << ta_index_current << "]的输入结果是：in_v: " << ta_current->in_v << " in: " << ta_current->in << endl;
//	//for debug
//	outfile2 << "TA[ " << ta_index_current << " ]" << endl;
//	outfile2 << "--------------------------------------" << endl;
//	outfile2 << "TA[" << ta_index_current << "]输入值" << endl;
//	outfile2 << setw(15) << "in_v" << setw(15) << "in" << endl;
//	outfile2 << setw(15) << ta_current->in_v << setw(15) << ta_current->in << endl;
//
//	//attach tag
//	ta_current->tag_bind();
//	outfile << "TA[" << ta_index_current << "]的输出结果是：out_v: " << ta_current->out_v << " out: " << ta_current->out << " out_tag: " << ta_current->out_tag << endl;
//
//}

void TiSimProcess()
{
	ti->tag_issue();
}

//扇出统计函数
void port_fanout_collect()
{
	vector<int> tmp;
	int pe_size = 0;
	int le_size = 0;
	int se_size = 0;
	int index = 0;
	auto config_size = vec_config_parsed_tmp.size();
	for (int i = 0; i < (int)config_size; i++)
	{
		if (vec_config_parsed_tmp[i][0] == 8)//pe
		{
			pe_size++;
			//port_fanout.resize(pe_size);
		}
		else if (vec_config_parsed_tmp[i][0] == 9)
		{
			le_size++;
			//port_fanout.resize(pe_size);
		}
		/*
		else if (vec_config_parsed_tmp[i][0] == 0)
		{
		se_size++;

		}
		*/
	}
	//resize
	port_fanout.resize(pe_size + le_size);
	for (int i = 0; i < (pe_size + le_size); i++)
	{
		port_fanout[i].resize(5);
	}

	for (int i = 0; i < (int)config_size; i++)
	{
		if (vec_config_parsed_tmp[i][0] == 8)//记录所有pe、le、se的index
		{
			port_fanout[index][1] = vec_config_parsed_tmp[i][1];
			port_fanout[index][0] = 0;                                       //0-pe 1-le
			index++;
		}
		else if (vec_config_parsed_tmp[i][0] == 9)
		{
			port_fanout[index][1] = vec_config_parsed_tmp[i][1];
			port_fanout[index][0] = 1;                                       //0-pe 1-le
			index++;
		}
	}

	for (int i = 0; i < (int)config_size; i++)                            //i表示扇入端，j表示扇出端
	{
		if (vec_config_parsed_tmp[i][0] == 8)//pe
		{
			if (vec_config_parsed_tmp[i][5] == 2)//in1 from pe
			{
				for (int j = 0; j < (int)port_fanout.size(); j++)
				{
					if (port_fanout[j][0] == 0)                                  //pe
					{
						if (port_fanout[j][1] == vec_config_parsed_tmp[i][3])
						{
							if (vec_config_parsed_tmp[i][4] == 0)//port1
							{
								port_fanout[j][2]++;
							}
							else if (vec_config_parsed_tmp[i][4] == 1)
							{
								port_fanout[j][3]++;
							}
							else if (vec_config_parsed_tmp[i][4] == 2)
							{
								port_fanout[j][4]++;
							}
						}
					}
				}
			}
			else if (vec_config_parsed_tmp[i][5] == 1)//in1 from le
			{
				for (int j = 0; j < (int)port_fanout.size(); j++)
				{
					if (port_fanout[j][0] == 1)
					{
						if (port_fanout[j][1] == vec_config_parsed_tmp[i][3])
						{
							if (vec_config_parsed_tmp[i][4] == 0)//port1
							{
								port_fanout[j][2]++;
							}
							else
							{
								cout << "error occurs!" << endl;
							}
						}
					}
				}
			}
			//in2 from pe
			if (vec_config_parsed_tmp[i][8] == 2)
			{
				for (int j = 0; j < (int)port_fanout.size(); j++)
				{
					if (port_fanout[j][0] == 0)
					{
						if (port_fanout[j][1] == vec_config_parsed_tmp[i][6])
						{
							if (vec_config_parsed_tmp[i][7] == 0)//port1
							{
								port_fanout[j][2]++;
							}
							else if (vec_config_parsed_tmp[i][7] == 1)
							{
								port_fanout[j][3]++;
							}
							else if (vec_config_parsed_tmp[i][7] == 2)
							{
								port_fanout[j][4]++;
							}
						}
					}
				}
			}
			else if (vec_config_parsed_tmp[i][8] == 1)//in2 from le
			{
				for (int j = 0; j < (int)port_fanout.size(); j++)
				{
					if (port_fanout[j][0] == 1)
					{
						if (port_fanout[j][1] == vec_config_parsed_tmp[i][6])
						{
							if (vec_config_parsed_tmp[i][7] == 0)//port1
							{
								port_fanout[j][2]++;
							}
							else
							{
								cout << "error occurs!" << endl;
							}
						}
					}
				}
			}
			//in3 from pe
			if (vec_config_parsed_tmp[i][11] == 2)
			{
				for (int j = 0; j < (int)port_fanout.size(); j++)
				{
					if (port_fanout[j][0] == 0)
					{
						if (port_fanout[j][1] == vec_config_parsed_tmp[i][9])
						{
							if (vec_config_parsed_tmp[i][10] == 0)//port1
							{
								port_fanout[j][2]++;
							}
							else if (vec_config_parsed_tmp[i][10] == 1)
							{
								port_fanout[j][3]++;
							}
							else if (vec_config_parsed_tmp[i][10] == 2)
							{
								port_fanout[j][4]++;
							}
						}
					}
				}

			}
			else if (vec_config_parsed_tmp[i][11] == 1)//in3 from le
			{
				for (int j = 0; j < (int)port_fanout.size(); j++)
				{
					if (port_fanout[j][0] == 1)
					{
						if (port_fanout[j][1] == vec_config_parsed_tmp[i][9])
						{
							if (vec_config_parsed_tmp[i][10] == 0)//port1
							{
								port_fanout[j][2]++;
							}
							else
							{
								cout << "error occurs!" << endl;
							}
						}
					}
				}
			}

		}

		else if (vec_config_parsed_tmp[i][0] == 9)                      //le的扇入
		{
			for (int j = 0; j < (int)port_fanout.size(); j++)
			{
				if (port_fanout[j][0] == 0)                                  //pe
				{
					if (port_fanout[j][1] == vec_config_parsed_tmp[i][2])
					{
						if (vec_config_parsed_tmp[i][3] == 0)//port1
						{
							port_fanout[j][2]++;
						}
						else if (vec_config_parsed_tmp[i][3] == 1)
						{
							port_fanout[j][3]++;
						}
						else if (vec_config_parsed_tmp[i][3] == 2)
						{
							port_fanout[j][4]++;
						}
					}
				}
			}
		}

		else if (vec_config_parsed_tmp[i][0] == 0)                      //se扇入
		{
			if (vec_config_parsed_tmp[i][4] == 0)                  //in1 from pe
			{
				for (int j = 0; j < (int)port_fanout.size(); j++)
				{
					if (port_fanout[j][0] == 0)                                  //pe
					{
						if (port_fanout[j][1] == vec_config_parsed_tmp[i][2])
						{
							if (vec_config_parsed_tmp[i][3] == 0)//port1
							{
								port_fanout[j][2]++;
							}
							else if (vec_config_parsed_tmp[i][3] == 1)
							{
								port_fanout[j][3]++;
							}
							else if (vec_config_parsed_tmp[i][3] == 2)
							{
								port_fanout[j][4]++;
							}
						}
					}
				}
			}
			else if (vec_config_parsed_tmp[i][4] == 1)                  //in1 from le
			{
				for (int j = 0; j < (int)port_fanout.size(); j++)
				{
					if (port_fanout[j][0] == 1)                                  //le
					{
						if (port_fanout[j][1] == vec_config_parsed_tmp[i][2])
						{
							if (vec_config_parsed_tmp[i][3] == 0)//port1
							{
								port_fanout[j][2]++;
							}
							else if (vec_config_parsed_tmp[i][3] == 1)
							{
								port_fanout[j][3]++;
							}
							else if (vec_config_parsed_tmp[i][3] == 2)
							{
								port_fanout[j][4]++;
							}
						}
					}
				}
			}

			if (vec_config_parsed_tmp[i][7] == 0)                  //in2 from pe
			{
				for (int j = 0; j < (int)port_fanout.size(); j++)
				{
					if (port_fanout[j][0] == 0)                                  //pe
					{
						if (port_fanout[j][1] == vec_config_parsed_tmp[i][5])
						{
							if (vec_config_parsed_tmp[i][6] == 0)//port1
							{
								port_fanout[j][2]++;
							}
							else if (vec_config_parsed_tmp[i][6] == 1)
							{
								port_fanout[j][3]++;
							}
							else if (vec_config_parsed_tmp[i][6] == 2)
							{
								port_fanout[j][4]++;
							}
						}
					}
				}
			}
			else if (vec_config_parsed_tmp[i][7] == 1)                  //in2 from le
			{
				for (int j = 0; j < (int)port_fanout.size(); j++)
				{
					if (port_fanout[j][0] == 1)                                  //le
					{
						if (port_fanout[j][1] == vec_config_parsed_tmp[i][5])
						{
							if (vec_config_parsed_tmp[i][6] == 0)//port1
							{
								port_fanout[j][2]++;
							}
							else if (vec_config_parsed_tmp[i][6] == 1)
							{
								port_fanout[j][3]++;
							}
							else if (vec_config_parsed_tmp[i][6] == 2)
							{
								port_fanout[j][4]++;
							}
						}
					}
				}
			}
		}
	}
	/*
	for (int k = 0; k < pe_size + se_size; k++)
	{
		cout << "fanout[" << k << "] = ";
		for (int m = 0; m < 5; m++)
		{
			cout << port_fanout[k][m];
		}
		cout << endl;
	}*/
}

/*
void pe_sim_step1_new(ProcessingElement* pe_current)
{
	int pe_index_current = pe_current->config_reg.front()[1];//当前正在处理的PE标号
	int din1_from_index = pe_current->config_reg.front()[3];
	int din2_from_index = pe_current->config_reg.front()[6];
	int bin_from_index = pe_current->config_reg.front()[9];
	//in_port
	int din1_from_port = pe_current->config_reg.front()[4];
	int din2_from_port = pe_current->config_reg.front()[7];
	int bin_from_port = pe_current->config_reg.front()[10];
	//in_flag
	int din1_from_flag = pe_current->config_reg.front()[5];
	int din2_from_flag = pe_current->config_reg.front()[8];
	int bin_from_flag = pe_current->config_reg.front()[11];
	//判断是否悬空
	bool is_din1_float = din1_from_flag;//0--悬空；非0--非悬空
	bool is_din2_float = din2_from_flag;
	bool is_bin_float = bin_from_flag;
	
	//last PE出数
	if (is_din1_float)//din1非悬空
	{
		//din1 from PE ob出数
		pe_sim_step3(pe[din1_from_index], din1_from_port);
	}
	if (is_din2_float)//din2非悬空
	{
		//din2 from PE ob出数
		pe_sim_step3(pe[din2_from_index], din2_from_port);
	}
	if (is_bin_float)//bin非悬空
	{
		//bin from PE ob出数
		pe_sim_step3(pe[bin_from_index], bin_from_index);
	}
	//data trans
	pe_sim_step_connect(pe_current);
	//data store
	pe_sim_step1(pe_current);

}

void pe_sim_step1_new_no_tag(ProcessingElement* pe_current)
{
	int pe_index_current = pe_current->config_reg.front()[1];//当前正在处理的PE标号
	int din1_from_index = pe_current->config_reg.front()[3];
	int din2_from_index = pe_current->config_reg.front()[6];
	int bin_from_index = pe_current->config_reg.front()[9];
	//in_port
	int din1_from_port = pe_current->config_reg.front()[4];
	int din2_from_port = pe_current->config_reg.front()[7];
	int bin_from_port = pe_current->config_reg.front()[10];
	//in_flag
	int din1_from_flag = pe_current->config_reg.front()[5];
	int din2_from_flag = pe_current->config_reg.front()[8];
	int bin_from_flag = pe_current->config_reg.front()[11];
	//判断是否悬空
	bool is_din1_float = din1_from_flag;//0--悬空；非0--非悬空
	bool is_din2_float = din2_from_flag;
	bool is_bin_float = bin_from_flag;
	//last PE ob出数
	//last PE出数
	if (is_din1_float)//din1非悬空
	{
		//din1 from PE ob出数
		pe_sim_step3_no_tag(pe[din1_from_index], din1_from_port);
	}

	if (is_din2_float)//din2非悬空
	{
		//din2 from PE ob出数
		pe_sim_step3_no_tag(pe[din2_from_index], din2_from_port);
	}

	if (is_bin_float)//bin非悬空
	{
		//bin from PE ob出数
		pe_sim_step3_no_tag(pe[bin_from_index], bin_from_index);
	}
	//data transmit
	pe_sim_step_connect(pe_current);
	//data get into buffer
	pe_sim_step1_no_tag(pe_current);

}
*/