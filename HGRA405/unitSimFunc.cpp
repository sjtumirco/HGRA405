#include "unitSimFunc.h"

typedef unsigned int uint;



void PeSimProcess(ProcessingElement* pe_current)
{
	int opcode = pe_current->config_reg.front()[2];
	int pe_index_current = pe_current->config_reg.front()[1];//��ǰ���ڴ����PE���
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
	//�ж��Ƿ�����
	bool is_din1_float = din1_from_flag;//0--���գ���0--������
	bool is_din2_float = din2_from_flag;
	bool is_bin_float = bin_from_flag;

	//outfile2 << "CLOCK" << cnt << "-PE[" << vec_config_parsed_tmp[i][1] << "]���ֵ" << endl;
	outfile2 << setw(15) << "dout1_t" << setw(15) << "dout1_v" << setw(15) << "dout1" << setw(15) << "dout2_t" << setw(15) << "dout2_v"
		<< setw(15) << "dout" << setw(15) << "bout_t" << setw(15) << "bout_v" << setw(15) << "bout" << endl;
	outfile2 << setw(15) << pe_current->dout1_tag << setw(15) << pe_current->dout1_v
		<< setw(15) << pe_current->dout1 << setw(15) << pe_current->dout2_tag
		<< setw(15) << pe_current->dout2_v << setw(15) << pe_current->dout2
		<< setw(15) << pe_current->bout_tag << setw(15) << pe_current->bout_v
		<< setw(15) << pe_current->bout << endl;
	//outfile2 << "CLOCK" << cnt << "-PE" << vec_config_parsed_tmp[i][1] << endl;
	
	//��ʼ����ǰ����˿���������

	pe_current->dout1_v = 0;
	pe_current->dout2_v = 0;
	pe_current->bout_v = 0;
	pe_current->dout1 = 0;
	pe_current->dout2 = 0;
	pe_current->bout = 0;

	bool tag_mode = pe_current->config_reg.front()[19];
	uint mode = pe_current->config_reg.front()[18];
	if (tag_mode)//��Ҫ����tag�Ĵ���
	{
		outfile << "PE��Ҫtag����...." << endl;
		if (mode == 0)//bypass
		{
			outfile << "pe[" << pe_index_current << "]������·ģʽ...." << endl;
			pe_sim_step3(pe_current);//�ų���LE,SE�Ȳ���Ҫob�����Ĳ�����PE������·����תģʽ�µ�ob��������
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
			//bypass��ɣ����validֵ
			pe_current->din1_v = 0; pe_current->din1 = 0; pe_current->din1_tag = 0;						
			pe_current->din2_v = 0; pe_current->din2 = 0; pe_current->din2_tag = 0;
			pe_current->bin_v = 0; pe_current->bin = 0; pe_current->bin_tag = 0;
			
			
			outfile << "pe[" << pe_index_current << "]����·ģʽ" << endl;
			outfile << "PE[" << pe_current->config_reg.front()[1] << "]������ǣ�dout1: " << pe_current->dout1 << " dout1_v: " << pe_current->dout1_v << " dout1_tag: " << pe_current->dout1_tag << endl;
			outfile << "                                        " << "dout2: " << pe_current->dout2 << " dout2_v: " << pe_current->dout2_v << " dout2_tag: " << pe_current->dout2_tag << endl;
			//outfile << "                                        " << "bout: " << pe_current->bout << " bout_v: " << pe_current->bout_v << " bout_tag: " << pe_current->bout_tag << endl;
		}
		else if (mode == 1)//transmit
		{
			//Ĭ��ֻʹ��din1��dout1
			//��Ҫ��tagģʽ�¹���
			
			//����
			outfile << "pe[" << pe_index_current << "]������תģʽ...." << endl;
			pe_sim_step3(pe_current);//�ų���LE,SE�Ȳ���Ҫob�����Ĳ�����PE������·����תģʽ�µ�ob��������
			pe_sim_step_connect(pe_current);
			if (pe_current->bin_v & pe_current->loc_reg_v)
			{
				if (pe_current->bin)
				{
					pe_current->outbuffer1_in = pe_current->loc_reg;
					pe_current->outbuffer1_in_v = pe_current->loc_reg_v;
					//Ĭ��ʹ��ob1
					(pe_current->outBuffer1).dataIn(pe_current->outbuffer1_in, pe_current->outbuffer1_in_v);
					
					if (port_fanout[pe_index_current][2] == 1)   //һ��һ
					{
						(pe_current->outBuffer1).dataOut(pe_current->dout1, pe_current->dout1_v);
						//(pe_current->outBuffer1).outputBuffer.pop();
					}
				}
			}
			
			//����			
			if (pe_current->din1_v)
			{
				pe_current->loc_reg = pe_current->din1;
				//pe_current->loc_reg_tag = pe_current->din1_tag;
				pe_current->loc_reg_v = pe_current->din1_v;
			}
			
			outfile << "��תģʽ��ֻ�����1�н��" << endl;
			outfile << "pe[" << pe_index_current << "]��������: " << "dout1: " << pe_current->dout1 << "dout1_v: " << pe_current->dout1_v << endl;
		}
		else if (mode == 2)//calculate
		{
			outfile << "pe[" << pe_index_current << "]���ڼ���ģʽ...." << endl;

			pe_sim_step2(pe_current);

			pe_sim_step3(pe_current);
			pe_sim_step_connect(pe_current);
			pe_sim_step1(pe_current);
			
			/*
			if (1)
			{
				if (is_din1_float != 0 && is_din2_float != 0 && is_bin_float != 0)// 111
				{
					//last PE����
					if (is_din1_float)//din1������
					{
						//din1 from PE ob����
						pe_sim_step3(pe[din1_from_index], din1_from_port);
					}
					if (is_din2_float)//din2������
					{
						//din2 from PE ob����
						pe_sim_step3(pe[din2_from_index], din2_from_port);
					}
					if (is_bin_float)//bin������
					{
						//bin from PE ob����
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
					//�����ǣ�bin����һ�������źţ�loc_reg�ڼ����ź������һ����ֵ


				}
				else if (is_din1_float != 0 && is_din2_float != 0 && is_bin_float == 0)//110
				{
					//last PE����
					if (is_din1_float)//din1������
					{
						//din1 from PE ob����
						pe_sim_step3(pe[din1_from_index], din1_from_port);
					}
					if (is_din2_float)//din2������
					{
						//din2 from PE ob����
						pe_sim_step3(pe[din2_from_index], din2_from_port);
					}
					if (is_bin_float)//bin������
					{
						//bin from PE ob����
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
					//last PE����
					if (is_din1_float)//din1������
					{
						//din1 from PE ob����
						pe_sim_step3(pe[din1_from_index], din1_from_port);
					}
					if (is_din2_float)//din2������
					{
						//din2 from PE ob����
						pe_sim_step3(pe[din2_from_index], din2_from_port);
					}
					if (is_bin_float)//bin������
					{
						//bin from PE ob����
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
					//last PE����
					if (is_din1_float)//din1������
					{
						//din1 from PE ob����
						pe_sim_step3(pe[din1_from_index], din1_from_port);
					}
					if (is_din2_float)//din2������
					{
						//din2 from PE ob����
						pe_sim_step3(pe[din2_from_index], din2_from_port);
					}
					if (is_bin_float)//bin������
					{
						//bin from PE ob����
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
					//last PE����
					if (is_din1_float)//din1������
					{
						//din1 from PE ob����
						pe_sim_step3(pe[din1_from_index], din1_from_port);
					}
					if (is_din2_float)//din2������
					{
						//din2 from PE ob����
						pe_sim_step3(pe[din2_from_index], din2_from_port);
					}
					if (is_bin_float)//bin������
					{
						//bin from PE ob����
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
					//last PE����
					if (is_din1_float)//din1������
					{
						//din1 from PE ob����
						pe_sim_step3(pe[din1_from_index], din1_from_port);
					}
					if (is_din2_float)//din2������
					{
						//din2 from PE ob����
						pe_sim_step3(pe[din2_from_index], din2_from_port);
					}
					if (is_bin_float)//bin������
					{
						//bin from PE ob����
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
			outfile << "pe[" << pe_index_current << "]���ڼ���+��תģʽ...." << endl;
			if (is_din1_float != 0 && is_din2_float != 0 && is_bin_float != 0)// 111
			{	
				pe_sim_step2(pe_current);

				pe_sim_step3(pe_current);
				pe_sim_step_connect(pe_current);
				//��ת����
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
				//��ת����
				if (pe_current->din2_v)
				{
					pe_current->loc_reg = pe_current->din2;
				}
				pe_sim_step1(pe_current);
			}
		}
		/*
		else if (mode == 5)//tag ��ģʽ
		{
			//tag��ģʽĬ������1��tagֵ������2��dataֵ
			struct tmp { bool valid; int data; };
			outfile << "pe[" << pe_index_current << "]����tag��ģʽ...." << endl;
			//ack����
			pe_current->ack2in1port = 0;
			pe_current->ack2in2port = 0;
			//������������
			pe_current->dout1_v = 0;
			pe_current->dout2_v = 0;
			//������FIFO
			if (pe_current->din1_v && pe_current->din2_v)
			{

				 TableLine4Fifo tmp1,tmp2;
				if (!pe_current->tableBuffer_fifo_full1())
				{
					tmp1.valid = pe_current->din1_v;
					tmp1.data = pe_current->din1;
					pe_current->tableBuffer_fifo1.push(tmp1);
					pe_current->ack2in1port = 1;
					//fifo1������ɣ������������
					pe_current->din1_v = 0;
				}
				if (!pe_current->tableBuffer_fifo_full2())
				{
					tmp2.valid = pe_current->din2_v;
					tmp2.data = pe_current->din2;
					pe_current->tableBuffer_fifo2.push(tmp2);
					pe_current->ack2in2port = 0;
					//fifo2������ɣ������������
					pe_current->din2_v = 0;
				}
				
			}

			//ƥ��������-���
			if (!pe_current->tableBuffer_fifo1.empty() && !pe_current->tableBuffer_fifo2.empty())
			{
				pe_current->outbuffer1_in = pe_current->tableBuffer_fifo2.front().data;
				pe_current->outbuffer1_in_tag = pe_current->tableBuffer_fifo1.front().data;
				pe_current->outbuffer1_in_v = pe_current->tableBuffer_fifo1.front().valid;
				pe_current->outTableBuffer1.dataIn();
				//pe_current->outTableBuffer1.dataOut();//ֻ�Ǵ�ob���ó����ݷ���������˿ڣ�û�����ob�е�����
			}
		}
		*/
		
	}
	else
	{
		//����Ҫ����tag�Ĵ���
		if (mode == 0)//bypass
		{
			outfile2 << "��ǰPE������·ģʽ" << endl;
			pe_sim_step_connect(pe_current);
			if (pe_current->din1_v)
			{
				pe_current->outbuffer1_in = pe_current->din1;
				pe_current->outbuffer1_in_v = 1;
				if (pe_current->outBuffer1.dataIn(pe_current->outbuffer1_in, pe_current->outbuffer1_in_v))
				{
					//out
					if (port_fanout[pe_index_current][2] == 1)   //һ��һ
					{
						pe_current->outBuffer1.dataOut(pe_current->dout1, pe_current->dout1_v);
						//pe_current->outBuffer1.outputBuffer.pop();
						pe_current->ack_outbuffer12alu = 1;
					}
				}

				//������ɣ������������
				pe_current->din1_v = 0;

			}
			if (pe_current->din2_v)
			{
				pe_current->outbuffer2_in = pe_current->din2;
				pe_current->outbuffer2_in_v = 1;
				if (pe_current->outBuffer2.dataIn(pe_current->outbuffer2_in, pe_current->outbuffer2_in_v))
				{
					//out
					if (port_fanout[pe_index_current][3] == 1)   //һ��һ
					{
						pe_current->outBuffer2.dataOut(pe_current->dout2, pe_current->dout2_v);
						//(pe_current->outBuffer2).outputBuffer.pop();
						pe_current->ack_outbuffer22alu = 1;
					}
				}

				//������ɣ������������
				pe_current->din2_v = 0;
			}
			outfile2 << "PE[" << pe_index_current << "]���ֵ" << endl;
			outfile2 << setw(15) << "out1_v" << setw(15) << "out1" << endl;
			outfile2 << setw(15) << pe_current->dout1_v << setw(15) << pe_current->dout1 << endl;
			outfile2 << setw(15) << "out2_v" << setw(15) << "out2" << endl;
			outfile2 << setw(15) << pe_current->dout2_v << setw(15) << pe_current->dout2 << endl;
			outfile2 << setw(15) << "out3_v" << setw(15) << "out3" << endl;
			outfile2 << setw(15) << pe_current->bout_v << setw(15) << pe_current->bout << endl;
		}
		else if (mode == 1)//transmit     ֻ��outbuffer��˳��Ӧ��Ϊ3c12
		{
			//��ʼ����ǰ����ģ�������������
			pe_current->outbuffer1_in_v = 0; pe_current->outbuffer2_in_v = 0; pe_current->outbuffer3_in_v = 0;

			//��תģʽֻ������תһ����������Ĭ��ʹ��din1
			pe_sim_step3_no_tag(pe_current);
			pe_sim_step_connect(pe_current);
			//����
			if (pe_current->din1_v)
			{
				pe_current->loc_reg = pe_current->din1;
				pe_current->loc_reg_v = pe_current->din1_v;
				//������ɣ������������
				pe_current->din1_v = 0;
			}

			
			//����outbuffer
			if (pe_current->config_reg.front()[15])//ob1_from ������
			{
				if (pe_current->config_reg.front()[15] == 2)//ob1 from lr
				{
					if (pe_current->bin_v)
					{
						if (pe_current->bin & pe_current->loc_reg_v)
						{
							pe_current->outbuffer1_in = pe_current->loc_reg;
							pe_current->outbuffer1_in_v = pe_current->loc_reg_v;
							//������ɣ������������
							pe_current->bin_v = 0;
							pe_current->bin = 0;
						}
					}
				}
			}
			//outbuffer����
			if (pe_current->outbuffer1_in_v)
			{
				pe_current->outBuffer1.dataIn(pe_current->outbuffer1_in, pe_current->outbuffer1_in_v);
				if (pe_current->outBuffer1.isOutBufferFull())
				{
					pe_current->ack_outbuffer12alu = 0;
					pe_current->ack2in3port = 0;   //��ת�ڵ㲻��inbuffer��inbuffer��bp��Ϊoutbuffer��bp��ע����һ��PE��step3����Ҫͬʱ��������bp�źţ�
					//pe_current->ack2in1port = 0;   ����in1����buffer���ǽ�locreg���費��Ҫ��ס��
				}
				else
				{
					pe_current->ack_outbuffer12alu = 1;
					pe_current->ack2in3port = 1;   //��ת�ڵ㲻��inbuffer��inbuffer��bp��Ϊoutbuffer��bp
					//pe_current->ack2in1port = 0;   ����in1����buffer���ǽ�locreg���費��Ҫ��ס��
				}

			}

			//ob��ӡ
			//�м����
			queue<OutBuffer_no_tag> ob1_tmp = pe_current->outBuffer1.outputBuffer;
			queue<OutBuffer_no_tag> ob2_tmp = pe_current->outBuffer2.outputBuffer;
			if (!ob1_tmp.empty())
			{
				outfile << "outbuffer1ֵ��ӡ" << endl;
				outfile << setw(15) << "outbuffer1_v" << setw(15) << "outbuffer1" << endl;
				outfile << setw(15) << (ob1_tmp.front()).valid << setw(15) << (ob1_tmp.front()).data << endl;
			}

			if (!ob2_tmp.empty())
			{
				outfile << "outbuffer2ֵ��ӡ" << endl;
				outfile << setw(15) << "outbuffer2_v" << setw(15) << "outbuffer2" << endl;
				outfile << setw(15) << (ob2_tmp.front()).valid << setw(15) << (ob2_tmp.front()).data << endl;
			}

			//��ӡoutbuffer�е�����
			//outfile << "outbufferֵ��ӡ" << endl;
			if (pe_current->outbuffer3_v)
			{
				outfile << "outbuffer3ֵ��ӡ" << endl;
				outfile << setw(15) << "outbuffer3_v" << setw(15) << "outbuffer3" << endl;
				outfile << setw(15) << pe_current->outbuffer3_v << setw(15) << pe_current->outbuffer3 << endl;
			}
			

			/*
			if (pe_current->bin_v)
			{
				if (pe_current->bin & pe_current->loc_reg_v)//boolֵ��Ч�����
				{
					pe_current->dout1 = pe_current->loc_reg;
					pe_current->dout1_v = pe_current->loc_reg_v;
					//pe_current->outBuffer1.isOutBufferFull();
					//pe_current->outBuffer1In();
					//pe_current->peExportCtr1();
					//������ɣ������������
					pe_current->bin_v = 0;
					pe_current->bin = 0;
				}
			}	
			*/
			
			outfile2 << "PE[" << pe_index_current << "]���ֵ" << endl;
			outfile2 << setw(15) << "dout1_v" << setw(15) << "dout1" << endl;
			outfile2 << setw(15) << pe_current->dout1_v << setw(15) << pe_current->dout1 << endl;

		}
		else if (mode == 2)//calculate
		{

			//if (pe_current->config_reg.front()[24] == 1)//˳��������stage
			//{				
			//	pe_sim_step3_no_tag(pe_current);
			//	pe_sim_step_connect(pe_current);
			//	pe_sim_step1_no_tag(pe_current);

			//	pe_sim_step2_no_tag(pe_current);

			//}
			/*
			if (pe_current->config_reg.front()[1] == 2)//pe2��ʼ�����·��
			{
				//pe2->pe3->lb1
				pe_sim_step3_no_tag(pe[3]);
				pe_sim_step_connect(pe[3]);
				pe_sim_step1_no_tag(pe[3]);

				pe_sim_step2_no_tag(pe[3]);

				LbeginSimProcess(lbegin[1]);

				pe_sim_step3_no_tag(pe[2]);
				pe_sim_step_connect(pe[2]);
				pe_sim_step1_no_tag(pe[2]);

				pe_sim_step2_no_tag(pe[2]);
				//pe18
				pe_sim_step3_no_tag(pe[18]);
				pe_sim_step_connect(pe[18]);
				pe_sim_step1_no_tag(pe[18]);
				pe_sim_step2_no_tag(pe[18]);
			}
			else
			*/
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
		else if (mode == 5)//tag ��ģʽ
		{
			//tag��ģʽĬ������1��tagֵ������2��dataֵ
			outfile << "pe[" << pe_index_current << "]����tag��ģʽ...." << endl;
			outfile2 << "Tag Bing Mode" << endl;
			//��ʼ����ǰ����˿�����
			pe_current->dout1_v = 0; pe_current->dout2_v = 0; pe_current->bout_v = 0;
			pe_current->dout1_tag = 0; pe_current->dout2_tag = 0; pe_current->bout_tag = 0;
			//s3->sc

			//fifo���
			outfile << "PE[" << pe_index_current << "]infifo1�Ĵ�С" << endl;
			outfile << pe_current->tableBuffer_fifo1.size() << endl;
			outfile << "PE[" << pe_index_current << "]infifo2�Ĵ�С" << endl;
			outfile << pe_current->tableBuffer_fifo2.size() << endl;

			pe_sim_step3_no_tag(pe_current);
			pe_sim_step_connect(pe_current);

			if (pe_current->din1 == pe_current->last_t4bind)
			{	//����ͬһ��ti�ڵ������
				pe_current->din1_v = 0;
			}
			//ack����
			//pe_current->ack2in1port = 0;
			//pe_current->ack2in2port = 0;
			//������������
			pe_current->dout1_v = 0;
			pe_current->dout2_v = 0;
			//������FIFO			
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
					//fifo1������ɣ������������
					pe_current->din1_v = 0;
					//last_t4bind����
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
					//fifo2������ɣ������������
					pe_current->din2_v = 0;

				}
			}

			//ƥ��������-���
			if (!pe_current->tableBuffer_fifo1.empty() && !pe_current->tableBuffer_fifo2.empty())
			{
				pe_current->outbuffer1_in = pe_current->tableBuffer_fifo2.front().data;
				pe_current->outbuffer1_in_tag = pe_current->tableBuffer_fifo1.front().data;
				pe_current->outbuffer1_in_v = pe_current->tableBuffer_fifo1.front().valid;				
				//���ݵ��뵽outTableBuffer����ȥ
				pe_current->outTableBuffer1.in = pe_current->outbuffer1_in;
				pe_current->outTableBuffer1.in_v = pe_current->outbuffer1_in_v;
				pe_current->outTableBuffer1.in_tag = pe_current->outbuffer1_in_tag;

				pe_current->outTableBuffer1.dataIn();
				//���������FIFO����һ������
				pe_current->tableBuffer_fifo1.pop();
				pe_current->tableBuffer_fifo2.pop();
				//������fifo����
				pe_current->ack2in1port = 1;
				pe_current->ack2in2port = 1;
			}

			outfile2 << "PE[" << pe_index_current << "]FIFO���ֵ" << endl;
			outfile2 << setw(15) << "ob1_in_t" << setw(15) << "ob1_in_v" << setw(15) << "ob1_in" << endl;
			outfile2 << setw(15) << pe_current->outbuffer1_in_tag << setw(15) << pe_current->outbuffer1_in_v
				<< setw(15) << pe_current->outbuffer1_in << endl;
			/*outfile2 << "tag bind mode�����" << endl;
			outfile2 << setw(15) << "dout1_t" << setw(15) << "dout1_v" << setw(15) << "dout1" << endl;
			outfile2 << setw(15) << pe_current->dout1_tag << setw(15) << pe_current->dout1_v << setw(15) << pe_current->dout1 << endl;*/
		}
	}
	
}


void pe_outbuffer_out_no_tag(ProcessingElement* pe_current,uint32_t ob_index)
{
	int pe_index_current = pe_current->config_reg.front()[1];//��ǰ���ڴ����PE���

	//step3---output buffer��������
	//��ʼ����֮ǰ���ڴ�����������
	/*
	pe_current->dout1_v = 0;
	pe_current->dout1 = 0;
	pe_current->dout2_v = 0;
	pe_current->dout2 = 0;
	pe_current->bout_v = 0;
	pe_current->bout = 0;
	*/

	if (ob_index == 0)//ob1��Ҫ����
	{
		//ob1����
		if (pe_current->outBuffer1.outputBuffer.size() != 0)
		{		
			if (port_fanout[pe_index_current][2] <= 1)   //һ��һ
			{
				pe_current->outBuffer1.dataOut(pe_current->dout1, pe_current->dout1_v);
				//pe_current->outBuffer1.outputBuffer.pop();
				pe_current->ack_outbuffer12alu = 1;

				//��Ϊ��תģʽ��inbuffer��bpҲ����obuffer��bp�ı�
				uint mode = pe_current->config_reg.front()[18];
				if (mode == 1)    //��תģʽ��no_tag
				{
					//pe_current->ack2in1port = 1;   
					pe_current->ack2in3port = 1;    
				}
			}
		}
	}	
	else if (ob_index == 1)//ob2��Ҫ����
	{
		//ob2����
		if (pe_current->outBuffer2.outputBuffer.size() != 0)
		{			
			if (port_fanout[pe_index_current][3] <= 1)   //һ��һ
			{
				pe_current->outBuffer2.dataOut(pe_current->dout2, pe_current->dout2_v);
				//pe_current->outBuffer2.outputBuffer.pop();
				pe_current->ack_outbuffer22alu = 1;
				//��Ϊ��תģʽ��inbuffer��bpҲ����obuffer��bp�ı�
				uint mode = pe_current->config_reg.front()[18];
				if (mode == 1)    //��תģʽ��no_tag
				{
					//pe_current->ack2in2port = 1;
					pe_current->ack2in3port = 1;
				}
			}
		}
	}
	
	else if (ob_index == 2)//ob3��Ҫ����
	{
		//ob3����
		if (pe_current->outbuffer3_v)
		{
			if (port_fanout[pe_index_current][4] <= 1)   //һ��һ
			{
				pe_current->bout = pe_current->outbuffer3;
				pe_current->bout_v = pe_current->outbuffer3_v;
				pe_current->ack_outbuffer32alu = 1;
				pe_current->outbuffer3_v = 0;
			}
		}
	}
	

	//����Ƿ���end�ڵ�
	if (pe_current->config_reg.front()[17])
	{
		if (pe_current->dout1_v | pe_current->dout2_v)
		{
			pe_current->bout = 1;
			pe_current->bout_v = 1;//ʹ��out3_v������Ϊend�ڵ��źŵĳ��ڣ�Ҫ��out3��û�б�ʹ�õ�����²��ܳ�����
		}

	}
}

void pe_sim_step3_no_tag(ProcessingElement* pe_current)
{
	int pe_index_current = pe_current->config_reg.front()[1];//��ǰ���ڴ����PE���
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
	//�ж��Ƿ�����
	bool is_din1_float = din1_from_flag;//0--���գ���0--������
	bool is_din2_float = din2_from_flag;
	bool is_bin_float = bin_from_flag;
	//�жϵ�ǰPE�����������Դ���
	//in1port
	if (din1_from_flag == 2)//PE
	{
		if (pe_current->ack2in1port == 1)        //��ǰPE inbufferδ��ʱ����һPE���Գ���
		{
			if (pe[din1_from_index]->config_reg.front()[18] == 0)
			{
				//��һ��PE������·ģʽ����Ҫ�ٶ����ob����,��תģʽ����Ҫ��������
			}
			else
			{
				pe_outbuffer_out_no_tag(pe[din1_from_index], din1_from_port);
				/*
				if (din1_from_port == 0)         //������Դ��PE����˿�ά����Ӧ��obuffer��bp��ע��PE�Ķ˿ڽ���ʱ����1
				{
					pe[din1_from_index]->ack_outbuffer12alu = 1;    //��������
				}
				else if (din1_from_port == 1)
				{
					pe[din1_from_index]->ack_outbuffer22alu = 1;
				}
				else if (din1_from_port == 2)                 //�������Ӧ�ò��ᷢ��
				{
					pe[din1_from_index]->ack_outbuffer32alu = 1;
				}
				*/
			}
		}
		
	}
	else if (din1_from_flag == 1)//LE
	{
		//����Ҫ��������LE��outbuffer��������
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
		if (pe_current->ack2in2port == 1)   //�鱾��PE��inbuffer�Ƿ�����
		{
			if (pe[din2_from_index]->config_reg.front()[18] == 0)
			{
				//��һ��PE������·ģʽ����Ҫ�ٶ����ob����,��תģʽ����Ҫ��������
			}
			else
			{
				pe_outbuffer_out_no_tag(pe[din2_from_index], din2_from_port);
				/*
				if (din2_from_port == 0)         //������Դ��PE����˿�ά����Ӧ��obuffer��bp��ע��PE�Ķ˿ڽ���ʱ����1
				{
					pe[din2_from_index]->ack_outbuffer12alu = 1;    //��������
				}
				else if (din2_from_port == 1)
				{
					pe[din2_from_index]->ack_outbuffer22alu = 1;
				}
				else if (din2_from_port == 2)                 //�������Ӧ�ò��ᷢ��
				{
					pe[din2_from_index]->ack_outbuffer32alu = 1;
				}
				*/
			}
		}
		
	}
	else if (din2_from_flag == 1)//LE
	{
		//����Ҫ��������LE��outbuffer��������
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
		if (pe_current->ack2in3port == 1)   //��תģʽ��Ӧ�ú�obuffer��bp��ͬ
		{
			if (pe[bin_from_index]->config_reg.front()[18] == 0 | pe[bin_from_index]->config_reg.front()[18] == 1)
			{
				//��һ��PE������ת����·ģʽ����Ҫ�ٶ����ob����
			}
			else
			{
				pe_outbuffer_out_no_tag(pe[bin_from_index], bin_from_port);
				/*
				if (bin_from_port == 0)         //������Դ��PE����˿�ά����Ӧ��obuffer��bp��ע��PE�Ķ˿ڽ���ʱ����1
				{
					pe[bin_from_index]->ack_outbuffer12alu = 1;    //��������
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
		//����Ҫ��������LE��outbuffer��������
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
	//step2---ib����->ALU����->ob����
	//��ʼ����ǰ����ģ�������������
	pe_current->alu_out_v = 0;pe_current->alu_out_b_v = 0;
	pe_current->inbuffer1_out_v = 0; pe_current->inbuffer2_out_v = 0; pe_current->inbuffer3_out_v = 0;
	pe_current->outbuffer1_in_v = 0; pe_current->outbuffer2_in_v = 0; pe_current->outbuffer3_in_v = 0;
	//��ʼǰob�е��������㣬ֻ��ob�еİ����ݱ���һ�����ڵ���������
	//pe_current->outbuffer1_v = 0; pe_current->outbuffer2_v = 0; pe_current->outbuffer3_v = 0;

	//ib1 out 
	//���Ȳ�ѯobuffer��bp�Ƿ�Ϊ0��0��ʾobuffer����
	//inbuffer������Ҳ��Ҫά��inbuffer��bp
	if (pe_current->ack_outbuffer12alu == 1)     //��ѯobuffer�Ƿ�����
	{
		if (pe_current->inBuffer1.inputBuffer.size() != 0)
		{
			pe_current->inBuffer1.dataOut(pe_current->inbuffer1_out, pe_current->inbuffer1_out_v);
			//pe_current->inbuffer1_out = pe_current->inBuffer1.inputBuffer.front().data;
			//pe_current->inbuffer1_out_v = pe_current->inBuffer1.inputBuffer.front().valid;
			pe_current->ack2in1port = 1;                                                  //inbuffer����������inbufferδ����bp=1
		}
		//pe_current->inBuffer1.dataIn(pe_current->inbuffer1_out, pe_current->inbuffer1_out_v);   ���õı�﷽ʽ��
	}
		//ib2 out
	if (pe_current->ack_outbuffer22alu == 1)     //δ��
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
	if (pe_current->ack_outbuffer32alu == 1)     //δ��
	{
		if (pe_current->inbuffer3_v)
		{
			pe_current->inbuffer3_out = pe_current->inbuffer3;
			pe_current->inbuffer3_out_v = pe_current->inbuffer3_v;
			pe_current->ack2in3port = 1;
			pe_current->inbuffer3_v = 0;     //��inbuffer3
		}
	}
	
	//��ӡ�м�ֵ1
	outfile2 << "PE[" << pe_current->config_reg.front()[1] << "]�м�ֵ1" << endl;
	outfile2 << setw(15) << "ib1_out_t" << setw(15) << "ib1_out_v" << setw(15) << "ib1_out" << setw(15) << "ib2_out_t" << setw(15) << "ib2_out_v"
		<< setw(15) << "ib2_out" << setw(15) << "ib3_out_t" << setw(15) << "ib3_out_v" << setw(15) << "ib3_out" << endl;
	outfile2 << setw(15) << 0 << setw(15) << pe_current->inbuffer1_out_v
		<< setw(15) << pe_current->inbuffer1_out << setw(15) << 0
		<< setw(15) << pe_current->inbuffer2_out_v << setw(15) << pe_current->inbuffer2_out
		<< setw(15) << 0 << setw(15) << pe_current->inbuffer3_out_v
		<< setw(15) << pe_current->inbuffer3_out << endl;
	

	//ALU����
	//ȡ�����˿�
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
	//alu����
	//alu������������ģ��������ݲ������㣬��һ��������ǰ����û�м�����㣬Ҫ�ڳ��ڴ�keepס
	if (pe_current->config_reg.front()[2] == 8)//mux����������Դ�
	{
		if (pe_current->inbuffer3_out_v)
		{
			if (pe_current->inbuffer3_out == 0)//ѡ��in2
			{
				if (pe_current->alu_in2_v)//din2����������Ч�Ϳ��Խ���mux����
				{
					pe_current->ALU(pe_current->config_reg.front()[2], pe_current->alu_in1, pe_current->alu_in2, pe_current->inbuffer3_out);					
					//alu������ɣ������������
					pe_current->alu_in1_v = 0;
					pe_current->alu_in2_v = 0;
					pe_current->inbuffer1_out_v = 0;
					pe_current->inbuffer2_out_v = 0;
					pe_current->inbuffer3_out_v = 0;
					//inBuffer���ݵ������벹��
					//pe_current->inbuffer1_v = 0; pe_current->inbuffer2_v = 0; pe_current->inbuffer3_v = 0;					
				}

			}
			else if (pe_current->inbuffer3_out == 1)//ѡ��in1
			{
				if (pe_current->alu_in1_v)//din1��Ч�Ϳ��Խ���mux����
				{
					pe_current->ALU(pe_current->config_reg.front()[2], pe_current->alu_in1, pe_current->alu_in2, pe_current->inbuffer3_out);
					//������ɣ������������
					pe_current->alu_in1_v = 0;
					pe_current->alu_in2_v = 0;
					pe_current->inbuffer1_out_v = 0;
					pe_current->inbuffer2_out_v = 0;
					pe_current->inbuffer3_out_v = 0;
					//inBuffer���ݵ������벹��
					//pe_current->inbuffer1_v = 0; pe_current->inbuffer2_v = 0; pe_current->inbuffer3_v = 0;					
				}				
			}

		}
	}
	else
	{
		//��mux����
		if (pe_current->alu_in1_v & pe_current->alu_in2_v)//��������������Ч�ſ�ʼ����
		{
			//alu ����
			pe_current->ALU(pe_current->config_reg.front()[2], pe_current->alu_in1, pe_current->alu_in2, pe_current->inbuffer3_out);
			//������ɣ������������
			pe_current->alu_in1_v = 0;
			pe_current->alu_in2_v = 0;
			pe_current->inbuffer1_out_v = 0;             //alu��ʼ�������inbuffer��valid
			pe_current->inbuffer2_out_v = 0;
			pe_current->inbuffer3_out_v = 0;
			//inBuffer���ݵ������벹��
			//pe_current->inbuffer1_v = 0; pe_current->inbuffer2_v = 0; pe_current->inbuffer3_v = 0;			
		}
	}

	//ob����
	//���������Ƿ�Ҫ�ı�bp
	if (1)
	{
		if (pe_current->config_reg.front()[15])//ob1_from ������
		{
			//����fetch���˿�
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
			//����
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

		if (pe_current->config_reg.front()[16])//ob2������
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
			//����
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

		if (pe_current->config_reg.front()[15] == 0 && pe_current->config_reg.front()[16] == 0)//ob3�����գ�������alu
		{
			if (pe_current->alu_out_b_v)
			{
				pe_current->outbuffer3 = pe_current->alu_out_b;//ob3����		
				pe_current->outbuffer3_v = pe_current->alu_out_b_v;
				pe_current->ack_outbuffer32alu = 0;
				pe_current->alu_out_b_v = 0;
			}
		}
	}
	
	

	//��ӡ�м�ֵ2
	outfile2 << "PE[" << pe_current->config_reg.front()[1] << "]�м�ֵ2" << endl;
	outfile2 << setw(15) << "alu_out_v" << setw(15) << "alu_out" << setw(15) << "alu_out_b_v" << setw(15) << "alu_out_b" << endl;
	outfile2 << setw(15) << pe_current->alu_out_v << setw(15) << pe_current->alu_out
		<< setw(15) << pe_current->alu_out_b_v << setw(15) << pe_current->alu_out_b << endl;

	//�м����
	queue<OutBuffer_no_tag> ob1_tmp = pe_current->outBuffer1.outputBuffer;
	queue<OutBuffer_no_tag> ob2_tmp = pe_current->outBuffer2.outputBuffer;

	

	if (!ob1_tmp.empty())
	{
		outfile << "outbuffer1ֵ��ӡ" << endl;
		outfile << setw(15) << "outbuffer1_v" << setw(15) << "outbuffer1" << endl;
		outfile << setw(15) << (ob1_tmp.front()).valid << setw(15) << (ob1_tmp.front()).data << endl;
	}

	if (!ob2_tmp.empty())
	{
		outfile << "outbuffer2ֵ��ӡ" << endl;
		outfile << setw(15) << "outbuffer2_v" << setw(15) << "outbuffer2" << endl;
		outfile << setw(15) << (ob2_tmp.front()).valid << setw(15) << (ob2_tmp.front()).data << endl;
	}

	//��ӡoutbuffer�е�����
	//outfile << "outbufferֵ��ӡ" << endl;
	if (pe_current->outbuffer3_v)
	{
		outfile << "outbuffer3ֵ��ӡ" << endl;
		outfile << setw(15) << "outbuffer3_v" << setw(15) << "outbuffer3" << endl;
		outfile << setw(15) << pe_current->outbuffer3_v << setw(15) << pe_current->outbuffer3 << endl;
	}
	
	//lr ����
	if (pe_current->config_reg.front()[12] == 1)	//lr from alu
	{
		if (pe_current->alu_out_v)
		{
			pe_current->loc_reg = pe_current->alu_out;
			pe_current->loc_reg_v = 1;
		}
		//�������ӵ�loc reg ����Ĺ���
		if (pe_current->loc_reg == THRESHOLD)
		{
			pe_current->loc_reg = 0;
		}

	}
	else if (pe_current->config_reg.front()[12] == 2)	//lr from in1 for ��ת
	{
	}
	else if (pe_current->config_reg.front()[12] == 3)	//lr from in2 for��ת
	{
	}			
}

void pe_sim_step1_no_tag(ProcessingElement* pe_current)
{		
	//step1---inbuffer��������
	//��ʼ����ǰ����ģ��������ݲ������㣬��һ��������ǰ����Ҫ�ڳ��ڴ�keepס
	//���ھ�̬ʱbp��һ�������Ե��źţ�����Ҫ����bp�ĳ�ʼ��
	//pe_current->ack2in1port = 0; pe_current->ack2in2port = 0; pe_current->ack2in3port = 0;

	//inbuffer1
	if (pe_current->config_reg.front()[20] == 1)//�������ӵ���inbuffer��
	{
		//����
		if (pe_current->din1_v)
		{
			if (pe_current->inBuffer1.dataIn(pe_current->din1, pe_current->din1_v))
				pe_current->din1_v = 0;                   //�����ɹ�����valid
			//��buffer�ɹ�������������bp�ź�
			if (pe_current->inBuffer1.isInBufferFull())   //��������buffer������bp�ź�����
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
		cout << "��������û�н��뵽inbuffer��" << endl;
	}
		
	//inbuffer2
	if(pe_current->config_reg.front()[21] == 1)//�������ӵ���inbuffer��
	{
		//����
		if (pe_current->din2_v)
		{
			if (pe_current->inBuffer2.dataIn(pe_current->din2, pe_current->din2_v))
				pe_current->din2_v = 0;

			//��buffer�ɹ����ı�bp�ź�
			if (pe_current->inBuffer2.isInBufferFull())   //��������buffer������bp�ź�����
			{
				pe_current->ack2in2port = 0;
			}
			else
				pe_current->ack2in2port = 1;
			
			/*    ��̬PE������������
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
		cout << "��������û�н��뵽inbuffer��" << endl;

	//inbuffer3
	if (pe_current->config_reg.front()[22] == 1)//�������ӵ���inbuffer��
	{
		//����
		if (pe_current->bin_v)
		{
			pe_current->inbuffer3 = pe_current->bin;
			pe_current->inbuffer3_v = pe_current->bin_v;
			//��buffer�ɹ�����bp�ź�
			pe_current->ack2in3port = 0;
			//������
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
		cout << "��������û�н��뵽inbuffer��" << endl;
	//inbuffer����������ɣ���յ�ǰPE�������
	pe_current->din1_v = 0; pe_current->din2_v = 0; pe_current->bin_v = 0;
	//���last PE�ĳ�������

	//�м����
	queue<OutBuffer_no_tag> ib1_tmp, ib2_tmp;
	ib1_tmp = pe_current->inBuffer1.inputBuffer;
	ib2_tmp = pe_current->inBuffer2.inputBuffer;

	if (!ib1_tmp.empty())
	{
		//��ӡinbuffer1�е�����
		outfile << "inbuffer1ֵ��ӡ" << endl;
		outfile << setw(15) << "inbuffer1_v" << setw(15) << "inbuffer1"  << endl;
		outfile << setw(15) << (ib1_tmp.front()).valid << setw(15) << (ib1_tmp.front()).data  << endl;
	}
	if (!ib2_tmp.empty())
	{
		//��ӡinbuffer2�е�����
		outfile << "inbuffer2ֵ��ӡ" << endl;
		outfile << setw(15) << "inbuffer2_v" << setw(15) << "inbuffer2" << endl;
		outfile << setw(15) << (ib2_tmp.front()).valid << setw(15) << (ib2_tmp.front()).data << endl;
	}
	if (pe_current->inbuffer3_v)
	{
		outfile << "inbuffer3ֵ��ӡ" << endl;
		outfile << setw(15) << "inbuffer3_v" << setw(15) << "inbuffer3" << endl;
		outfile << setw(15) << pe_current->inbuffer3_v << setw(15) << pe_current->inbuffer3 << endl;
	}
	

}

void pe_sim_step_connect(ProcessingElement* pe_current)
{
	int pe_index_current = pe_current->config_reg.front()[1];//��ǰ���ڴ����PE���
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
	//�ж��Ƿ�����
	bool is_din1_float = din1_from_flag;//0--���գ���0--������
	bool is_din2_float = din2_from_flag;
	bool is_bin_float = bin_from_flag;
	
	//din1 ������
	if (is_din1_float) //din1 ������
	{
		if (din1_from_flag == 2) //from PE
		{
			if (din1_from_port == 0)//PE��port1
			{
				if (pe[din1_from_index]->dout1_v)//������Ч��ȡ��
				{
					pe_current->din1 = pe[din1_from_index]->dout1;
					pe_current->din1_v = pe[din1_from_index]->dout1_v;
					pe_current->din1_tag = pe[din1_from_index]->dout1_tag;
				}
				else
				{
					cout << "��һ��PE�������û��valid." << endl;
					//pe_current->in1_v = 0;
				}
			}
			else if (din1_from_port == 1)//��һ��PE��port2
			{
				if (pe[din1_from_index]->dout2_v)//data valid,fetch
				{
					pe_current->din1 = pe[din1_from_index]->dout2;
					pe_current->din1_v = pe[din1_from_index]->dout2_v;
					pe_current->din1_tag = pe[din1_from_index]->dout2_tag;
				}
				else
				{
					cout << "��һ��PE�������û��valid." << endl;
					//pe_current->in1_v = 0;
				}

			}
		}
		else if (din1_from_flag == 1) //����������LE
		{
			if (le[din1_from_index]->data_out_v)//data valid,fetch
			{
				pe_current->din1 = le[din1_from_index]->data_out;
				pe_current->din1_v = le[din1_from_index]->data_out_v;
				pe_current->din1_tag = le[din1_from_index]->data_out_tag;
			}
			else
			{
				cout << "��һ��LE���������û��valid" << endl;
				//pe_current->in1_v = 0;

			}
		}
		else if (din1_from_flag == 3) //����������lbegin
		{
			if (lbegin[din1_from_index]->out_v)//������Ч��ȡ��
			{
				pe_current->din1 = lbegin[din1_from_index]->out;
				pe_current->din1_v = lbegin[din1_from_index]->out_v;
				pe_current->din1_tag = 0;
			}
		}
		else if (din1_from_flag == 9) //����������ti
		{
			if (ti->out_v)//����׼���ã�ȡ��
			{
				pe_current->din1 = ti->out;
				pe_current->din1_v = ti->out_v;
				pe_current->din1_tag = ti->out_t;
			}
		}
	}
	else
	{
		cout << "PE[" << pe_index_current << "]������1����" << endl;
		pe_current->din1 = 0;
		pe_current->din1_v = 0;
		pe_current->din1_tag = 0;

	}
	//din2������
	if (is_din2_float)//in2������
	{
		if (din2_from_flag == 2) //����2������PE
		{
			if (din2_from_port == 0)//��һ��PE��port1
			{
				if (pe[din2_from_index]->dout1_v)//data valid,fetch
				{
					pe_current->din2 = pe[din2_from_index]->dout1;
					pe_current->din2_v = pe[din2_from_index]->dout1_v;
					pe_current->din2_tag = pe[din2_from_index]->dout1_tag;
				}
				else
				{
					cout << "��һ��PE�������û��valid." << endl;
					//pe_current->in2_v = 0;
				}

			}
			else if (din1_from_port == 1)//��һ��PE��port2
			{
				if (pe[din2_from_index]->dout2_v)//data valid,fetch
				{
					pe_current->din2 = pe[din2_from_index]->dout2;
					pe_current->din2_v = pe[din2_from_index]->dout2_v;
					pe_current->din2_tag = pe[din2_from_index]->dout2_tag;
				}
				else
				{
					cout << "��һ��PE�������û��valid." << endl;
					//pe_current->in2_v = 0;
				}
			}
		}
		else if (din2_from_flag == 1) //����2������LE
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
				cout << "��һ��LE���������û��valid" << endl;
			}
		}
		else if (din2_from_flag == 3) //����������lbegin
		{
			if (lbegin[din2_from_index]->out_v)//������Ч��ȡ��
			{
				pe_current->din2 = lbegin[din2_from_index]->out;
				pe_current->din2_v = lbegin[din2_from_index]->out_v;
				pe_current->din2_tag = 0;
			}
		}
	}
	else
	{
		cout << "PE������2����" << endl;
		pe_current->din2 = 0;
		pe_current->din2_v = 0;
	}
	//bin������
	if (is_bin_float)//in3������
	{
		if (bin_from_flag == 2)//����3������PE
		{
			if (bin_from_port == 2)//port3
			{
				if (pe[bin_from_index]->bout_v)
				{
					pe_current->bin = pe[bin_from_index]->bout;
					pe_current->bin_v = pe[bin_from_index]->bout_v;
				}
				else
				{
					cout << "��һ��PE���������û��valid" << endl;
					//pe_current->in3_v = 0;
				}

			}
			else if (bin_from_port == 0)//port1
			{
				if (pe[bin_from_index]->dout1_v)
				{
					pe_current->bin_v = pe[bin_from_index]->dout1_v;//Ĭ��ֻ��valid�źŴ����bin_v��bin
					pe_current->bin = pe[bin_from_index]->dout1_v;
				}
			}
			else if (bin_from_port == 1)//port2
			{
				if (pe[bin_from_index]->dout2_v)
				{
					pe_current->bin_v = pe[bin_from_index]->dout2_v;//Ĭ��ֻ��valid�źŴ����bin_v��bin
					pe_current->bin = pe[bin_from_index]->dout2_v;
				}
			}
			else
				cout << "error" << endl;
		}
		else if (bin_from_flag == 1)//����3��Դ��LE
		{
			cout << "PE������3������������LE�����ñ�д���������." << endl;
		}
		else if (bin_from_flag == 10)//from SE
		{
			pe_current->bin_v = se[bin_from_index]->se_extra_out_for_end_v;
			pe_current->bin = se[bin_from_index]->se_extra_out_for_end;
		}
		else if (bin_from_flag == 3)//����3��Դ��lbegin
		{
			pe_current->bin_v = lbegin[bin_from_index]->out_v;
			pe_current->bin = lbegin[bin_from_index]->out;

		}
		else if (bin_from_flag == 4)//����3��Դ��lend
		{
			if (bin_from_port == 0)//lend��port2back
			{

				pe_current->bin_v = lend[bin_from_index]->out2back;//ʹ���źŴ�valid�ڽ���																					   											
			}
			else if (bin_from_port == 1)//FG2��port2end
			{
				pe_current->bin_v = lend[bin_from_index]->out2end;//ʹ���źŴ�valid�ڽ���																					  												
			}
			else
				cout << "port �����˷�Χ" << endl;
		}

		else if (bin_from_flag == 5)//����3��Դ��join
		{
			//
		}
		else if (bin_from_flag == 10)//����3��Դ��begin
		{
			//pe_current->bin_v = begin_signal;
		}
		else if (bin_from_flag == 6)//����3��Դ��switch
		{
			if (bin_from_port == 0)//switch��port1(left)
			{

				pe_current->bin_v = switch_[bin_from_index]->out1;
			}
			else if (bin_from_port == 1)//switch��port2(right)
			{
				pe_current->bin_v = switch_[bin_from_index]->out2;
			}
			else
				cout << "port �����˷�Χ" << endl;
		}
		else if (bin_from_flag == 7)//����3��Դ��break
		{
			//
		}

		//......
		else
		{
			cout << "�����˲�����Χ" << endl;
		}
	}
	else
	{
		pe_current->bin = 0;
		pe_current->bin_v = 0;
	}
	//cycle count
	//pe_current->cycle_entryLogic();

	////��ڴ����ݴ�ӡ
	//outfile << "PE[" << pe_index_current << "]�������ǣ� din1: " << pe_current->din1 << "  din1_v: " << pe_current->din1_v << "  din1_tag: " << pe_current->din1_tag << endl;
	//outfile << "                    din2: " << pe_current->din2 << "  din2_v: " << pe_current->din2_v << "  din2_tag: " << pe_current->din2_tag << endl;
	//outfile << "                    bin: " << pe_current->bin << "  bin_v: " << pe_current->bin_v << "  bin_tag: " << pe_current->bin_tag << endl;
	//for debug
	if (1)//��ӡ����
	{
		outfile2 << endl;
		//outfile2 << "PE[ " << pe_index_current << " ]" << endl;
		
		outfile2 << "PE[" << pe_index_current << "]����ֵ" << endl;
		outfile2 << setw(15) << "din1_t" << setw(15) << "din1_v" << setw(15) << "din1" << setw(15) << "din2_t" << setw(15) << "din2_v" << setw(15)
			<< "din2" << setw(15) << "bin_t" << setw(15) << "bin_v" << setw(15) << "bin" << endl;		
		outfile2 << setw(15) << pe_current->din1_tag << setw(15) << pe_current->din1_v << setw(15) << pe_current->din1 << setw(15) << pe_current->din2_tag << setw(15)
			<< pe_current->din2_v << setw(15) << pe_current->din2 << setw(15) << pe_current->bin_tag << setw(15) << pe_current->bin_v << setw(15) << pe_current->bin << endl;
		//outfile2 << "---------------------------------------------------------------" << endl;
	}
}

void pe_sim_step1(ProcessingElement* pe_current)
{
	int pe_index_current = pe_current->config_reg.front()[1];//��ǰ���ڴ����PE���
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
	//�ж��Ƿ�����
	bool is_din1_float = din1_from_flag;//0--���գ���0--������
	bool is_din2_float = din2_from_flag;
	bool is_bin_float = bin_from_flag;

	//inTableBuffer����
	//data in
	pe_current->inTableBuffer.dataIn(pe_current);
	//ack==1���������ɹ��������ڵ�����
	if (pe_current->ack2in1port)
		pe_current->din1_v = 0;
	if (pe_current->ack2in2port)
		pe_current->din2_v = 0;
	if (pe_current->ack2in3port)
		pe_current->bin_v = 0;


	
	//�ڶ�̬�������£�PE������˿ڵ���0����Ҫ�����������1����PEĳһ����˿ڵ��ȳ�Ϊ1����ô�����ݴ�����һ��PE��tableBuffer֮��Ϳ���ִ����һ��PE����˿ڵ���0����
												//  2����PEĳһ����˿ڵ��ȳ�>1����ô�����ݴ���tableBuffer֮������Ӧ��joinbpִ����0����
	//��ȡ��ԴPE 1,2,3�˿ڵ��ȳ�,�����������Ӧ�˿ڵ�����
	if (1)                                        //��ǰ������
	{
		int port1_fanout_num = 0, port2_fanout_num = 0, port3_fanout_num = 0;//--�����������Ǹ�PE����һ��PE����˿ڵ��ȳ�

		if (is_din1_float)   //��din1������ʱͳ�����ȳ�
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
			//��ʼ�����Ӧ�Ķ˿�����
			//���ݽ���tableBuffer֮��ſ�ʼ������
			if (din1_from_port == 0)//port1
			{
				if (port1_fanout_num <= 1)//һ��һ�����
				{
					if (pe_current->ack2in1port)
					{
						if (din1_from_flag == 2)//from PE
						{
							pe[din1_from_index]->outTableBuffer1.buffer_clear();
							pe[din1_from_index]->dout1_v = 0;
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
							pe[din1_from_index]->dout2_v = 0;
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
		//in2 from�Ķ˿�����
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
							pe[din2_from_index]->dout1_v = 0;
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
							pe[din2_from_index]->dout2_v = 0;
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
		//in3 from�Ķ˿�����
		//in3 from���˿ڵ��ȳ�
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
			//in3 from���˿�����
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

	//PE inTableBuffer�е����ݴ�ӡ
	if (1)
	{
		outfile << "inTableBuffer�е�����" << endl;
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
	int pe_index_current = pe_current->config_reg.front()[1];//��ǰ���ڴ����PE���
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
	//�ж��Ƿ�����
	//bool is_din1_float = din1_from_flag;//0--���գ���0--������
	//bool is_din2_float = din2_from_flag;
	//bool is_bin_float = bin_from_flag;
	//��ʼ����֮ǰ��ģ������˿���������
	pe_current->alu_out_v = 0;
	pe_current->alu_out = 0;
	pe_current->alu_out_b_v = 0;
	pe_current->alu_out_b = 0;
	

	//ack clear
	pe_current->ack_alu2ib1 = 0;
	pe_current->ack_alu2ib2 = 0;

	//inTableBuffer���
	//data out
	pe_current->inTableBuffer.dataOut(pe_current);
	outfile2 << "PE[" << pe_current->config_reg.front()[1] << "]�м�ֵ1" << endl;
	outfile2 << setw(15) << "ib1_out_t" << setw(15) << "ib1_out_v" << setw(15) << "ib1_out" << setw(15) << "ib2_out_t" << setw(15) << "ib2_out_v"
		<< setw(15) << "ib2_out" << setw(15) << "ib3_out_t" << setw(15) << "ib3_out_v" << setw(15) << "ib3_out" << endl;
	outfile2 << setw(15) << pe_current->inbuffer1_out_tag << setw(15) << pe_current->inbuffer1_out_v
		<< setw(15) << pe_current->inbuffer1_out << setw(15) << pe_current->inbuffer2_out_tag
		<< setw(15) << pe_current->inbuffer2_out_v << setw(15) << pe_current->inbuffer2_out
		<< setw(15) << pe_current->inbuffer3_out_tag << setw(15) << pe_current->inbuffer3_out_v
		<< setw(15) << pe_current->inbuffer3_out << endl;
	outfile2 << endl;
	
	//tagĬ����ǰ���䵽ob��ڴ�
	if (pe_current->config_reg.front()[15])//ob1_from ������
	{
		pe_current->outbuffer1_in_tag = pe_current->inbuffer1_out_tag;
	}
	if (pe_current->config_reg.front()[16])//ob2_from ������
	{
		pe_current->outbuffer2_in_tag = pe_current->inbuffer2_out_tag;
	}

	pe_current->outbuffer3_tag = pe_current->inbuffer3_out_tag;

	//ALU�˿�ȡ��
	//alu_in1
	if (pe_current->config_reg.front()[13] == 0)//alu_in1_from ib1
	{
		if (pe_current->inbuffer1_out_v)
		{
			pe_current->alu_in1 = pe_current->inbuffer1_out;
			pe_current->alu_in1_v = pe_current->inbuffer1_out_v;
			//��ack�ź�
			pe_current->ack_alu2ib1 = 1;
		}

	}
	else if (pe_current->config_reg.front()[13] == 1) //alu_in1_from lr
	{
		if (pe_current->loc_reg_v)
		{
			pe_current->alu_in1 = pe_current->loc_reg;
			pe_current->alu_in1_v = pe_current->loc_reg_v;
			//��ack�ź�
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
			//��ack�ź�
			pe_current->ack_alu2ib2 = 1;
		}

	}
	else if (pe_current->config_reg.front()[14] == 1)  //alu_in2_from lr
	{
		if (pe_current->loc_reg_v)
		{
			pe_current->alu_in2 = pe_current->loc_reg;
			pe_current->alu_in2_v = pe_current->loc_reg_v;
			//��ack�ź�
			pe_current->ack_alu2ib2 = 1;
		}

	}	
	/*outfile << "PE[" << pe_current->config_reg.front()[1] << "] ALU�������ǣ�in1: " << pe_current->alu_in1 << " in2: " << pe_current->alu_in2 << endl;*/
	//ִ�м���
	if (pe_current->alu_in1_v & pe_current->alu_in2_v)
	{
		pe_current->ALU(pe_current->config_reg.front()[2], pe_current->alu_in1, pe_current->alu_in2, pe_current->inbuffer3_out);
		//�������֮�����inTableBuffer��ĳһ�в������request����
		pe_current->alu_in1_v = 0;
		pe_current->alu_in2_v = 0;
		pe_current->inbuffer1_out_v = 0;
		pe_current->inbuffer2_out_v = 0;
		pe_current->inbuffer3_out_v = 0;	
		pe_current->inTableBuffer.buffer_clear();
	}
	
	outfile2 << "PE[" << pe_current->config_reg.front()[1] << "]�м�ֵ2" << endl;
	outfile2 << setw(15) << "alu_out_v" << setw(15) << "alu_out" << setw(15) << "alu_out_b_v" << setw(15) << "alu_out_b" << endl;
	outfile2 << setw(15) << pe_current->alu_out_v << setw(15) << pe_current->alu_out
		<< setw(15) << pe_current->alu_out_b_v << setw(15) << pe_current->alu_out_b << endl;

	//alu_out get into outTableBuffer
	//ȡ�����˿�
	if (pe_current->config_reg.front()[15])//ob1_from ������
	{
		if (pe_current->config_reg.front()[15] == 1)//ob1 from alu
		{
			if (pe_current->alu_out_v)
			{
				pe_current->outbuffer1_in = pe_current->alu_out;
				pe_current->outbuffer1_in_v = pe_current->alu_out_v;
				if (pe_current->outBuffer1.isOutBufferFull())
				{
					pe_current->ack_outbuffer12alu = 0;
				}
				else
					pe_current->ack_outbuffer12alu = 1;
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
				if (pe_current->outBuffer1.isOutBufferFull())
				{
					pe_current->ack_outbuffer12alu = 0;
				}
				else
					pe_current->ack_outbuffer12alu = 1;
			}
		}
		//pe_current->outbuffer1_in_tag = pe_current->inbuffer1_out_tag;
		//pe_current->outbuffer1_in_v = 1;
		//��Ч��ʼ����
		if (pe_current->outbuffer1_in_v)
		{
			//���ݵ��뵽outTableBuffer����ȥ
			pe_current->outTableBuffer1.in = pe_current->outbuffer1_in;
			pe_current->outTableBuffer1.in_v = pe_current->outbuffer1_in_v;
			pe_current->outTableBuffer1.in_tag = pe_current->outbuffer1_in_tag;
			//����
			pe_current->outTableBuffer1.dataIn();		
			//pe������ɣ���ģ����ڴ���validֵ����
			pe_current->outbuffer1_in_v = 0;
			pe_current->alu_out_v = 0;
		}

	}

	if (pe_current->config_reg.front()[16])//ob2������
	{
		pe_current->outbuffer2_in_tag = pe_current->inbuffer1_out_tag;
		if (pe_current->config_reg.front()[16] == 1)//ob2 from alu
		{
			pe_current->outbuffer2_in = pe_current->alu_out;
			pe_current->outbuffer2_in_v = pe_current->alu_out_v;
			if (pe_current->outBuffer2.isOutBufferFull())
			{
				pe_current->ack_outbuffer22alu = 0;
			}
			else
				pe_current->ack_outbuffer22alu = 1;
		}

		else if (pe_current->config_reg.front()[16] == 3)//ob2 from in2
		{
		}
		else if (pe_current->config_reg.front()[16] == 2)//ob2 from lr
		{
			pe_current->outbuffer2_in = pe_current->loc_reg;
			pe_current->outbuffer2_in_v = pe_current->loc_reg_v;
			if (pe_current->outBuffer2.isOutBufferFull())
			{
				pe_current->ack_outbuffer22alu = 0;
			}
			else
				pe_current->ack_outbuffer22alu = 1;
		}


		//pe_current->outbuffer2_in_v = 1;
		//��Ч��ʼ����
		if (pe_current->outbuffer2_in_v)
		{
			//���ݵ��뵽outTableBuffer����ȥ
			pe_current->outTableBuffer2.in = pe_current->outbuffer2_in;
			pe_current->outTableBuffer2.in_v = pe_current->outbuffer2_in_v;
			pe_current->outTableBuffer2.in_tag = pe_current->outbuffer2_in_tag;
			//����
			pe_current->outTableBuffer2.dataIn();
			//pe������ɣ���ģ����ڴ���validֵ����
			pe_current->outbuffer2_in_v = 0;
		}

	}

	if (pe_current->config_reg.front()[15] == 0 && pe_current->config_reg.front()[16] == 0)//ob3�����գ�������alu_out_b
	{
		pe_current->outbuffer3_tag = pe_current->inbuffer1_out_tag;
		if (pe_current->alu_out_b_v)
		{
			pe_current->outbuffer3 = pe_current->alu_out_b;//ob3����			
			//pe������ɣ���ģ����ڴ���validֵ����
			pe_current->outbuffer3_in_v = 0;
			pe_current->ack_outbuffer32alu = 0;
		}

	}
	//ob����Ԫ�ش�ӡ
	if (1)
	{
		if (!pe_current->outTableBuffer1.outBuffer.empty())
		{
			outfile << "PE[" << pe_index_current << "]ob1����Ԫ��ֵ" << endl;
			outfile << setw(10) << "ob1_t" << setw(10) << "ob1_v" << setw(10) << "ob1_d" << endl;
			outfile << setw(10) << pe_current->outTableBuffer1.outBuffer.front().tag << setw(10) << pe_current->outTableBuffer1.outBuffer.front().valid
				<< setw(10) << pe_current->outTableBuffer1.outBuffer.front().data << endl;
		}
		if (!pe_current->outTableBuffer2.outBuffer.empty())
		{	
			outfile << "PE[" << pe_index_current << "]ob2����Ԫ��ֵ" << endl;
			outfile << setw(10) << "ob2_t" << setw(10) << "ob2_v" << setw(10) << "ob2_d" << endl;
			outfile << setw(10) << pe_current->outTableBuffer2.outBuffer.front().tag << setw(10) << pe_current->outTableBuffer2.outBuffer.front().valid 
				<< setw(10) << pe_current->outTableBuffer2.outBuffer.front().data << endl;
		}
		

	}
	//lr ����
	if (pe_current->config_reg.front()[12] == 1)//from ALU
	{
		if (pe_current->alu_out_v)
		{
			pe_current->loc_reg_v = pe_current->alu_out_v;
			pe_current->loc_reg = pe_current->alu_out;
		}
		////�������ӵ�loc reg ����Ĺ���
		//if (pe_current->loc_reg == THRESHOLD)
		//{
		//	pe_current->loc_reg = 0;
		//}
	}
}

void pe_sim_step3(ProcessingElement* pe_current)
{
	int pe_index_current = pe_current->config_reg.front()[1];//��ǰ���ڴ����PE���
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
	//�ж��Ƿ�����
	bool is_din1_float = din1_from_flag;//0--���գ���0--������
	bool is_din2_float = din2_from_flag;
	bool is_bin_float = bin_from_flag;
	//�жϵ�ǰPE�����������Դ���
	//in1port
	if (din1_from_flag == 2)//PE
	{
		if (pe_current->config_reg.front()[18] == 0 | pe_current->config_reg.front()[18] == 1)
		{
			//��·ģʽ����Ҫ�ٶ���ִ��ob��������
		}
		else
		{
			//����Ҫ������һ��PE�ǳ��ھ�̬���Ƕ�̬��tag�󶨵�PE��Ȼ���ھ�̬���������ǰ󶨵����������뵽outTableBuffer��ȥ��
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
		//����Ҫ��������LE��outbuffer��������
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
			//��·ģʽ����Ҫ�ٶ����ob����
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
		//����Ҫ��������LE��outbuffer��������
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
			//��·ģʽ����Ҫ�ٶ����ob����
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
		//����Ҫ��������LE��outbuffer��������
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
	int pe_index_current = pe_current->config_reg.front()[1];//��ǰ���ڴ����PE���
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
	//�ж��Ƿ�����
	//bool is_din1_float = din1_from_flag;//0--���գ���0--������
	//bool is_din2_float = din2_from_flag;
	//bool is_bin_float = bin_from_flag;

	//��ģ����ڴ����ź�ֵ����
	pe_current->dout1_v = 0;
	pe_current->dout1 = 0;
	pe_current->dout1_tag = 0;
	pe_current->dout2 = 0;
	pe_current->dout2_v = 0;
	pe_current->dout2_tag = 0;
	pe_current->bout_v = 0;
	pe_current->bout = 0;
	pe_current->bout_tag = 0;
	//step3�ĳ���������ֻҪob table���ǿ�״̬���Ϳ��Գ���
	//ob1 out
	if (ob_index == 0)//ob1��Ҫ����
	{
		if (!pe_current->outTableBuffer1.outBuffer.empty())
		{
			if (pe_current->outTableBuffer1.outBuffer.front().valid)
			{
				pe_current->outTableBuffer1.dataOut();
				//���ݴ����е�����PE
				pe_current->dout1 = pe_current->outTableBuffer1.out;
				pe_current->dout1_v = pe_current->outTableBuffer1.out_v;
				pe_current->dout1_tag = pe_current->outTableBuffer1.out_tag;
			}
			
		}
	}
	
	else if (ob_index == 1)//ob2��Ҫ����
	{
		//ob2 out
		if (!pe_current->outTableBuffer2.outBuffer.empty())
		{
			if (pe_current->outTableBuffer2.outBuffer.front().valid)
			{
				pe_current->outTableBuffer2.dataOut();
				//���ݴ����е�����PE
				pe_current->dout2 = pe_current->outTableBuffer2.out;
				pe_current->dout2_v = pe_current->outTableBuffer2.out_v;
				pe_current->dout2_tag = pe_current->outTableBuffer2.out_tag;
			}			
		}
	}
	else if (ob_index == 2)//ob3��Ҫ����
	{
		//ob3 out
		if (pe_current->outbuffer3_v)
		{
			pe_current->bout = pe_current->outbuffer3;//ob3����
			pe_current->bout_v = pe_current->outbuffer3_v;
			pe_current->bout_tag = pe_current->inbuffer1_out_tag;
		}
	}						
	

	//����Ƿ���end�ڵ�
	if (pe_current->config_reg.front()[17])
	{
		if (pe_current->dout1_v | pe_current->dout2_v )
		{
			pe_current->bout = 1;
			pe_current->bout_v = 1;//ʹ��out3_v��Ϊend�źŵ������ֻ����output3û�б�ʹ�õ�����²ſ���ʹ��
		}
		

	}
}

/*
void pe_sim_step1_mux(ProcessingElement* pe_current)
{
	pe_current->inTableBuffer.dataIn(pe_current);
	//PE inTableBuffer�е�����
	outfile << "inTableBuffer�е�����" << endl;
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
	if (pe_current->config_reg.front()[15])//ob1_from ������
	{
		pe_current->outbuffer1_in_tag = pe_current->inbuffer1_out_tag;
	}
	if (pe_current->config_reg.front()[16])//ob2_from ������
	{
		pe_current->outbuffer2_in_tag = pe_current->inbuffer2_out_tag;
	}
	pe_current->outbuffer3_tag = pe_current->inbuffer3_out_tag;
}

void pe_sim_step2_mux(ProcessingElement* pe_current)
{
	//ALUȡ��
	if (pe_current->inbuffer1_out_v | pe_current->inbuffer2_out_v | pe_current->inbuffer3_out_v) //��Ч��ȡ��
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
		//ȡ��֮�����inTableBuffer��ĳһ��
		pe_current->inTableBuffer.buffer_clear();

	}
	outfile << "PE[" << pe_current->config_reg.front()[1] << "] ALU�������ǣ�in1: " << pe_current->alu_in1 << " in2: " << pe_current->alu_in2 << endl;
	if (pe_current->alu_in1_v & pe_current->alu_in2_v)
	{
		pe_current->ALU(pe_current->config_reg.front()[2], pe_current->alu_in1, pe_current->alu_in2, pe_current->inbuffer3_out);
	}
	
	//
	pe_current->inbuffer1_out_v = 0;
	pe_current->inbuffer2_out_v = 0;
	pe_current->inbuffer3_out_v = 0;

	//lr ����
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

	//��ģ����ڴ����ź�ֵ����
	pe_current->dout1_v = 0;
	pe_current->dout1 = 0;
	pe_current->dout1_tag = 0;
	pe_current->dout2 = 0;
	pe_current->dout2_v = 0;
	pe_current->dout2_tag = 0;
	pe_current->bout_v = 0;
	pe_current->bout = 0;
	pe_current->bout_tag = 0;
	//ob����������
	//ȡ���˿�
	if (pe_current->config_reg.front()[15])//ob1_from ������
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
		//��Ч��ʼ������ȡ��
		if (pe_current->outbuffer1_in_v)
		{
			//���ݵ��뵽outTableBuffer����ȥ
			pe_current->outTableBuffer1.in = pe_current->outbuffer1_in;
			pe_current->outTableBuffer1.in_v = pe_current->outbuffer1_in_v;
			pe_current->outTableBuffer1.in_tag = pe_current->outbuffer1_in_tag;
			//����������
			pe_current->outTableBuffer1.dataIn();
			pe_current->outTableBuffer1.dataOut();
			//���ݴ����е�����PE
			pe_current->dout1 = pe_current->outTableBuffer1.out;
			pe_current->dout1_v = pe_current->outTableBuffer1.out_v;
			pe_current->dout1_tag = pe_current->outTableBuffer1.out_tag;
		}
		
	}

	if (pe_current->config_reg.front()[16])//ob2������
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
			//���ݵ��뵽outTableBuffer����ȥ
			pe_current->outTableBuffer2.in = pe_current->outbuffer2_in;
			pe_current->outTableBuffer2.in_v = pe_current->outbuffer2_in_v;
			pe_current->outTableBuffer2.in_tag = pe_current->outbuffer2_in_tag;
			//����������
			pe_current->outTableBuffer2.dataIn();
			pe_current->outTableBuffer2.dataOut();
			//���ݴ����е�����PE
			pe_current->dout2 = pe_current->outTableBuffer2.out;
			pe_current->dout2_v = pe_current->outTableBuffer2.out_v;
			pe_current->dout2_tag = pe_current->outTableBuffer2.out_tag;
		}
		
	}
	outfile << "PE[" << pe_current->config_reg.front()[1] << "]������ǣ�dout1: " << pe_current->dout1 << " dout1_v: " << pe_current->dout1_v << " dout1_tag: " << pe_current->dout1_tag << endl;
	outfile << "                                        " << "dout2: " << pe_current->dout2 << " dout2_v: " << pe_current->dout2_v << " dout2_tag: " << pe_current->dout2_tag << endl;

	//if (pe_current->config_reg.front()[15] == 0 && pe_current->config_reg.front()[16] == 0)//ob3�����գ�������alu
	//{
	//	pe_current->outbuffer3 = pe_current->alu_out_b;//ob3����
	//	pe_current->bout = pe_current->outbuffer3;//ob3����
	//	pe_current->bout_v = 1;
	//	pe_current->bout_tag = pe_current->inbuffer1_out_tag;
	//}

	//����Ƿ���end�ڵ�
	if (pe_current->config_reg.front()[17])
	{
		if (pe_current->dout1_v | pe_current->dout2_v)
		{
			pe_current->bout = 1;
			pe_current->bout_v = 1;//ʹ��out3_v��Ϊend�źŵ������ֻ����output3û�б�ʹ�õ�����²ſ���ʹ��
		}


	}

	//pe������ɣ���ڴ���validֵ����
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
	if (tag_mode)//����tag����
	{
		//le���valid�ź�����
		le_current->data_out_v = 0;
		outfile << "LE[" << le_index_current << "]��Ҫ����tag����" << endl;
		//addr fetch
		if (le_infrom_port == 0)//��ַ�����Դ��PE��port1
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
		else if (le_infrom_port == 1)//��ַ������������һ��PE��port2
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
		/*outfile << "LE[" << le_index_current << "]�ĵ�ַ�����ǣ�" << " addr_in: " << le_current->addr_in << " addr_in_v: " << le_current->addr_in_v << " addr_in_tag: " << le_current->addr_in_tag << endl;*/
		//for debug		
		outfile2 << "LE[" << le_index_current << "]����ֵ" << endl;
		outfile2 << setw(15) << "addr_t" << setw(15) << "addr_v" << setw(15) << "addr" << endl;
		outfile2 << setw(15) << le_current->addr_in_tag << setw(15) << le_current->addr_in_v << setw(15) << le_current->addr_in << endl;
		


		
		//addr in buffer
		pair<unsigned int, int> addrANDdata;
		if (le_current->addr_in_v)//����������Ч��������table����
		{
			le_current->addrInTableBuffer();
			//PE->LE�����LE���PE��ob��ֻ���һ��һ�����һ�Զ�joinbp�ڵ㸺��
			if (1)
			{
				int port1_fanout_num, port2_fanout_num, port3_fanout_num;//--�����������Ǹýڵ����һ���ڵ�����˿ڵ��ȳ�
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
				//��ʼ�����Ӧ�Ķ˿�����
				if (le_infrom_port == 0)//port1
				{
					if (port1_fanout_num <= 1)//һ��һ�����
					{
						if (le_current->ack2addrgen)
						{
							pe[le_infrom_index]->outTableBuffer1.buffer_clear();
							pe[le_infrom_index]->dout1_v = 0;
						}
					}
				}
				else if (le_infrom_port == 1)//port2
				{
					if (port2_fanout_num <= 1)//һ��һ�����
					{
						if (le_current->ack2addrgen)
						{
							pe[le_infrom_index]->outTableBuffer2.buffer_clear();
							pe[le_infrom_index]->dout2_v = 0;
						}
					}
				}
				else if (le_infrom_port == 2)//port3
				{
					if (port3_fanout_num <= 1)//һ��һ�����
					{
						if (le_current->ack2addrgen)
						{
							pe[le_infrom_index]->bout_v = 0;
						}
					}
				}
			}
		}	
		//addr_out_v��λ
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
			if (ATTACH_MEMORY == 0)//������memory
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
			{	//����memory
				if (!le_current->ack)//ack==0,LSUnitû�н�������
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
		/*outfile2 << "LE[" << le_index_current << "]���ֵ" << endl;
		outfile2 << setw(15) << "data_out_t" << setw(15) << "data_out_v" << setw(15) << "data_out" << endl;
		outfile2 << setw(15) << le_current->data_out_tag << setw(15) << le_current->data_out_v << setw(15) << le_current->data_out << endl;*/
		//
	outfile << "inTableBuffer�е�����" << endl;
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
	/*outfile << "LE[" << le_index_current << "]������ǣ�" << " data_out: " << le_current->data_out << " data_out_v: " << le_current->data_out_v << " data_out_tag: " << le_current->data_out_tag << endl;*/
	
	}
	else
	{
		//����Ҫ����tag����
		//le���valid�ź�����
		le_current->data_out_v = 0;
		//addr fetch
		if (le_infrom_port == 0)//��ַ�����������һ��PE��port1
		{
			pe_outbuffer_out_no_tag(pe[le_infrom_index], 0);
			if (pe[le_infrom_index]->dout1_v)
			{
				le_current->addr_in = pe[le_infrom_index]->dout1;
				le_current->addr_in_v = pe[le_infrom_index]->dout1_v;
			}
			else
				cout << "��һ�������ַ��PE����û��valid." << endl;
		}
		else if (le_infrom_port == 1)//��ַ������������һ��PE��port2
		{
			pe_outbuffer_out_no_tag(pe[le_infrom_index], 1);
			if (pe[le_infrom_index]->dout2_v)
			{
				le_current->addr_in = pe[le_infrom_index]->dout2;
				le_current->addr_in_v = pe[le_infrom_index]->dout2_v;
			}
			else
				cout << "��һ�������ַ��PE����û��valid." << endl;
		}
		else
		{
		}
		/*outfile << "LE[" << le_index_current << "]�ĵ�ַ�����ǣ�" << " addr_in: " << le_current->addr_in << " addr_in_v: " << le_current->addr_in_v <<  endl;*/
		//������ݼ��ʱ��
		//le_current->entry_check();
		//for debug
		outfile2 << "LE[" << le_index_current << "]����ֵ" << endl;
		outfile2 << setw(15) << "addr_t" << setw(15) << "addr_v" << setw(15) << "addr" << endl;
		outfile2 << setw(15) << le_current->addr_in_tag << setw(15) << le_current->addr_in_v << setw(15) << le_current->addr_in << endl;

		if (le_current->addr_in_v)//����ַ�����ǲ���׼������
		{			
			le_current->addrInBuffer();
		}
		if (ATTACH_MEMORY == 1)//�����memory
		{
			if (le_current->addr_out_v)
			{
				if (!le_current->ack)//LSUnitû�н��յ�ַ���ݣ�������
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
			//û�����memory
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
		//le������ɣ���ڴ���valid�ź�����
		le_current->addr_in_v = 0;
		
		//outfile << "   " << endl;
		////outfile << "clock[" << cnt << "] " << endl;
		//outfile << "LE[" << le_index_current << "]����������data_out�� " << le_current->data_out << "; data_out_v: " << le_current->data_out_v << endl;

		
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
	if (tag_mode)//����tag����
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
				else
					cout << "SE��Ҫ�����ݻ�û��׼���á�" << endl;
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
				else
					cout << "SE��Ҫ�����ݻ�û��׼���á�" << endl;
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

		outfile << "SE[" << se_current->config_reg.front()[1] << "]�������ǣ�addr: " << se_current->addr << " addr_v: " << se_current->addr_v << " addr_tag: " << se_current->addr_tag << endl;
		outfile << "                                        " << "data_in: " << se_current->data_in << " data_in_v: " << se_current->data_in_v << " data_in_tag: " << se_current->data_in_tag << endl;

		//for debug
		outfile2 << endl;
		outfile2 << "SE[ " << se_index_current << " ]" << endl;
		outfile2 << "--------------------------------------" << endl;
		outfile2 << "SE[" << se_index_current << "]����ֵ" << endl;
		outfile2 << setw(15) << "addr_t" << setw(15) << "addr_v" << setw(15) << "addr" << setw(15) << "data_t" << setw(15) << "data_v" << setw(15) << "data" << endl;
		outfile2 << setw(15) << se_current->addr_tag << setw(15) << se_current->addr_v << setw(15) << se_current->addr << setw(15) << se_current->data_in_tag << setw(15)
			<< se_current->data_in_v << setw(15) << se_current->data_in << endl;
		//ack clear
		se_current->ack2addr_source_node = 0;
		se_current->ack2data_source_node = 0;
		//����������
		se_current->addr_in_table();
		se_current->data_in_table();
		outfile << "inTableBuffer�е�����" << endl;
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
		//�������ɹ�������
		//ͳ����Դ���ȳ������ȳ�����1��ȥ����һ�������� 
		if (1)                     //SE��din����������LE��PE
		{
			int port1_fanout_num, port2_fanout_num, port3_fanout_num;//--�����������Ǹ�PE����һ��PE����˿ڵ��ȳ�
			for (int i = 0; i < (int)port_fanout.size(); i++)
			{
				if (addr_in_from == port_fanout[i][1] && addr_in_flag == port_fanout[i][0])     //����PE��LE
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
			//��ʼ�����Ӧ�Ķ˿�����
			if (addr_in_port == 0)//port1
			{
				if (port1_fanout_num <= 1)//һ��һ�����
				{
					if (se_current->ack2addr_source_node)
					{
						se_current->addr_v = 0;
						pe[addr_in_from]->outTableBuffer1.buffer_clear();
						pe[addr_in_from]->dout1_v = 0;
					}
				}
			}
			else if (addr_in_port == 1)//port2
			{
				if (port2_fanout_num <= 1)//һ��һ�����
				{
					if (se_current->ack2addr_source_node)
					{
						se_current->addr_v = 0;
						pe[addr_in_from]->outTableBuffer2.buffer_clear();
						pe[addr_in_from]->dout2_v = 0;
					}
				}
			}

			for (int i = 0; i < (int)port_fanout.size(); i++)
			{
				if (data_in_from == port_fanout[i][1] && data_in_flag == port_fanout[i][0])     //����PE��LE
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
			//��ʼ�����Ӧ�Ķ˿�����
			if (data_in_port == 0)//port1
			{
				if (port1_fanout_num <= 1)//һ��һ�����
				{
					if (se_current->ack2data_source_node)
					{
						se_current->data_in_v = 0;
						pe[data_in_from]->outTableBuffer1.buffer_clear();
						pe[data_in_from]->dout1_v = 0;
					}
				}
			}
			else if (data_in_port == 1)//port2
			{
				if (port2_fanout_num <= 1)//һ��һ�����
				{
					if (se_current->ack2data_source_node)
					{
						se_current->data_in_v = 0;
						pe[data_in_from]->outTableBuffer2.buffer_clear();
						pe[data_in_from]->dout2_v = 0;
					}
				}
			}

		}
		if (se_current->ack2data_source_node)
			se_current->data_in_v = 0;
		if (se_current->ack2addr_source_node)
			se_current->addr_v = 0;

		se_current->dataOut();
		outfile2 << "SE[" << se_index_current << "]���ֵ" << endl;
		outfile2 << setw(15) << "ib1_out_t" << setw(15) << "ib1_out_v" << setw(15) << "ib1_out" << setw(15) << "ib2_out_t" << setw(15) << "ib2_out_v" << setw(15) << "ib2_out" << endl;
		outfile2 << setw(15) << se_current->addr_out_tag << setw(15) << se_current->addr_out_v << setw(15) << se_current->addr_out << setw(15) << se_current->data_out_tag << setw(15) << se_current->data_out_v << setw(15)
			<< se_current->data_out << endl;
		//��ʼwrite
		if (se_current->addr_out_v && se_current->data_out_v)
		{
			se_current->write2men(memory2, se_current->addr_out, se_current->data_out);
			//write cycle count
			//se_current->write_latency();
			se_current->se_extra_out_for_end = 1;
			se_current->se_extra_out_for_end_v = 1;
			//se������ɣ���ڴ���validֵ����
			se_current->addr_out_v = 0;
			se_current->data_out_v = 0;
			//SE buffer clear
			se_current->buffer_clear();
		}
	}
	else
	{
		//������tag����
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
					cout << "SE��Ҫ�ĵ�ַ��û��׼���á�" << endl;
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
					cout << "SE��Ҫ�ĵ�ַ��û��׼���á�" << endl;
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
				else
					cout << "SE��Ҫ�����ݻ�û��׼���á�" << endl;
			}
			else if (data_in_port == 1)//port2
			{
				pe_outbuffer_out_no_tag(pe[data_in_from], 1);
				if (pe[data_in_from]->dout2_v)
				{
					se[se_index_current]->data_in = pe[data_in_from]->dout2;
					se[se_index_current]->data_in_v = pe[data_in_from]->dout2_v;
				}
				else
					cout << "SE��Ҫ�����ݻ�û��׼���á�" << endl;
			}
			else
				cout << "se data_in_from_port is out of range." << endl;
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
		outfile2 << "SE[" << se_index_current << "]����ֵ" << endl;
		outfile2 << setw(15) << "addr_t" << setw(15) << "addr_v" << setw(15) << "addr" << setw(15) << "data_t" << setw(15) << "data_v" << setw(15) << "data" << endl;
		outfile2 << setw(15) << se_current->addr_tag << setw(15) << se_current->addr_v << setw(15) << se_current->addr << setw(15) << se_current->data_in_tag << setw(15)
			<< se_current->data_in_v << setw(15) << se_current->data_in << endl;

		//entry check cycle count
		//se[se_index_current]->entry_check_cycle();
		//SE���ݴ�ӡ
		//outfile << "clock[" << cnt << "] " << endl;
		outfile << "SE[" << se_index_current << "]�����������ǣ�addr_in: " << se[se_index_current]->addr << "  addr_in_v: " << se[se_index_current]->addr_v << endl;
		outfile << "               " << "data_in: " << se[se_index_current]->data_in << "  data_in_v: " << se[se_index_current]->data_in_v << endl;

		if (se[se_index_current]->addr_v && se[se_index_current]->data_in_v)
		{
			se_current->addr_out_v = se_current->addr_v;
			se_current->addr_out = se_current->addr;
			se_current->data_out_v = se_current->data_in_v;
			se_current->data_out = se_current->data_in;
			//�����������
			se_current->addr_v = 0;
			se_current->data_in_v = 0;
			//��ʼд��
			se[se_index_current]->write2men(memory2, se_current->addr_out, se_current->data_out);
			//write cycle count
			//se[se_index_current]->write_latency();
			se[se_index_current]->se_extra_out_for_end = 1;
			se_current->se_extra_out_for_end_v = 1;
			//se������ɣ���ڴ���validֵ����
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
	//������������
	lbegin_current->begin_in = 0;
	lbegin_current->feedback_in = 0;
	lbegin_current->out_v = 0;
	lbegin_current->out = 0;
	//loopbegin������1ȡ��
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
			//pe ob1 ����
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
			//pe ob2 ����
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
			//pe ob3 ����
			if (pe[lb_in1_from_index]->config_reg.front()[19] == 1)//tag mode
			{
				pe_outbuffer_out(pe[lb_in1_from_index], 2);
			}
			else
			{
				pe_outbuffer_out_no_tag(pe[lb_in1_from_index], 2);
			}
			lbegin_current->begin_in = pe[lb_in1_from_index]->bout;//�������ӣ�begin_in��bout����bout_v
		}
	}
	else if (lb_in1_from_flag == 7)//loopend2
	{
	}
	//in2ȡ��
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
			//pe ob1 ����
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
			//pe ob2 ����
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
			//pe ob3 ����
			if (pe[lb_in2_from_index]->config_reg.front()[19] == 1)//tag mode
			{
				pe_outbuffer_out(pe[lb_in2_from_index], 2);
			}
			else
			{
				pe_outbuffer_out_no_tag(pe[lb_in2_from_index], 2);
			}
			lbegin_current->feedback_in = pe[lb_in2_from_index]->bout;//�������ӣ�begin_in��bout����bout_v
		}
	}
	else if (lb_in2_from_flag == 7)//loopend2
	{
	}
	//for debug
	outfile2 << "lbegin[" << lb_index_current << "]����ֵ" << endl;
	outfile2 << setw(15) << "begin_in" << setw(15) << "feedback_in" << endl;
	outfile2 << setw(15) << lbegin_current->begin_in << setw(15) << lbegin_current->feedback_in << endl;


	/*outfile << "Lbegin[" << lb_index_current << "]�������ǣ�" << " begin_in: " << lbegin_current->begin_in << " feedback_in:" << lbegin_current->feedback_in << endl;*/
	lbegin_current->lbegin();
	//outfile << "Lbegin[" << lb_index_current << "]������ǣ�" << " out_v: " << lbegin_current->out_v << " out:" << lbegin_current->out << endl;
	
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
	//loop end�� 2bit����˿�Ҫ��keep and wait�Ĺ��ܣ����Կ�ʼʱ�����㣬��������һ���˿�����Ҫ�����
	lend_current->in2 = 0;
	//in1ȡ��
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
				//��Ч��ȡ������֤keep and wait�Ĺ���
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
	//in2ȡ��
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
		lend_current->in2 = se[lend_in2_from_index]->se_extra_out_for_end_v;//ʹ��SE�Ķ��������validΪ��Ϊ��Ч�ź�
	}
	//for debug	
	outfile2 << "lend[" << lend_index_current << "]����ֵ" << endl;
	outfile2 << setw(15) << "in1_v" << setw(15) << "in1" << setw(15) << "in2" << endl;
	outfile2 << setw(15) << lend_current->in1_v << setw(15) << lend_current->in1 << setw(15) <<lend_current->in2 << endl;

	//outfile << "Lend[" << lend_index_current << "]�������ǣ�" << " in1: " << lend_current->in1 << " in1_v:" << lend_current->in1_v << " in2: " << lend_current->in2 << endl;
	//
	lend_current->lend();
	//
	//outfile << "Lend[" << lend_index_current << "]������ǣ�" << " out2back: " << lend_current->out2back << " out2end:" << lend_current->out2end << endl;

	
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
			if (join_current->config_reg.front()[3 + i * 3] == 2)//PE��port3��Ҳ��Ĭ��ֵ
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
	//outfile << "Join[" << join_index_current << "]������ǣ�" << join_current->out << endl;
	//����1����join����֮�����
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
	//��ʼ����֮ǰ������������
	joinbp_current->out = 0;
	//data fetch
	for (int i = 0; i < (int)inport_num; i++)
	{
		//from pe
		if (joinbp_current->config_reg.front()[4 + i * 3] == 1)//from PE
		{
			auto in_from_index = joinbp_current->config_reg.front()[2 + i * 3];
			if (joinbp_current->config_reg.front()[3 + i * 3] == 2)//PE��port3��Ҳ��Ĭ��ֵ
			{
				if (pe[in_from_index]->ack2in3port)//ackֵΪ1��ȡ����else ��ȡ
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
	//������Ͷ�����
	//joinbp����ź�Ϊ1ʱ��ʹ��buffer_clear()�����һ����buffer���ҽ����ڴ���������0
	if (joinbp_current->out)                       //ע��joinbp���������ֹ�����PE��ͬ
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
				joinbp_in_from_from_flag = 2;         //le����ԴֻΪpe
			}
		}
		//start clear
		if (*(joinbp_current->config_reg.front().rbegin()))    //tagmode
		{
			if (joinbp_in_from_from_flag == 2)//PE
			{
				if (joinbp_in_from_from_port == 0)//port1
				{
					pe[joinbp_in_from_from_index]->outTableBuffer1.buffer_clear();
					pe[joinbp_in_from_from_index]->dout1_v = 0;
				}
				else if (joinbp_in_from_from_port == 1)//port2
				{
					pe[joinbp_in_from_from_index]->outTableBuffer2.buffer_clear();
					pe[joinbp_in_from_from_index]->dout2_v = 0;
				}
				else if (joinbp_in_from_from_port == 2)//port3
				{
					pe[joinbp_in_from_from_index]->outbuffer3_v = 0;
					pe[joinbp_in_from_from_index]->outbuffer3_tag = 0;
					pe[joinbp_in_from_from_index]->outbuffer3 = 0;
					pe[joinbp_in_from_from_index]->bout_v = 0;
				}
			}
			else if (joinbp_in_from_from_flag == 1)//LE
			{
				if (joinbp_in_from_from_port == 0)
				{
					//��joinbpĿ���LE�����
				}
				else
					cout << "error occurs in JoinBpSimProcess for LE only have one port!" << endl;
			}
			else if (joinbp_in_from_from_flag == 9)//Ti
			{
				ti->out_v = 0;
			}	
		}
		else                                         //��̬ʱjoinbp�����������ob
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
					//��joinbpĿ���LE�����
				}
				else
					cout << "error occurs in JoinBpSimProcess for LE only have one port!" << endl;
			}
			else if (joinbp_in_from_from_flag == 9)//Ti
			{
				ti->out_v = 0;
			}
		}

		//�������֮�����vector
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
	
	if (tag_mode)//��Ҫ����tag����
	{
		outfile << "����tag����...." << endl;
		if (switch_in_from_flag == 1)//pe
		{
			//Ĭ����Դ�˿���input3
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
		//��ʼswitch
		if (switch_current->in_v)
		{
			switch_current->path_select_tag();
		}
	}
	else
	{
		//����Ҫ����tag����
		switch_current->out1 = 0;
		switch_current->out2 = 0;
		if (switch_in_from_flag == 1)//pe
		{
			//Ĭ����Դ�˿���input3
			if (pe[switch_in_from_index]->bout_v)
			{
				switch_current->in = pe[switch_in_from_index]->bout;
				switch_current->in_v = pe[switch_in_from_index]->bout_v;
			}
		}
		else if (switch_in_from_flag == 2)//lbegin
		{
		}
		//��ʼswitch
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
		//���ֵĿ����Բ���
	}
	//��ʼmerge
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
			cout << "������ȡ����������ȡ��" << endl;
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
	//��ʼmerge
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
//	//��ʼ����֮ǰ������������
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
//			//��̫����
//		}
//	}
//	else if (ta_in_from_flag == 2)//from loopbegin
//	{
//	}
//	outfile << "TA[" << ta_index_current << "]���������ǣ�in_v: " << ta_current->in_v << " in: " << ta_current->in << endl;
//	//for debug
//	outfile2 << "TA[ " << ta_index_current << " ]" << endl;
//	outfile2 << "--------------------------------------" << endl;
//	outfile2 << "TA[" << ta_index_current << "]����ֵ" << endl;
//	outfile2 << setw(15) << "in_v" << setw(15) << "in" << endl;
//	outfile2 << setw(15) << ta_current->in_v << setw(15) << ta_current->in << endl;
//
//	//attach tag
//	ta_current->tag_bind();
//	outfile << "TA[" << ta_index_current << "]���������ǣ�out_v: " << ta_current->out_v << " out: " << ta_current->out << " out_tag: " << ta_current->out_tag << endl;
//
//}

void TiSimProcess()
{
	ti->tag_issue();
}

//�ȳ�ͳ�ƺ���
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
		if (vec_config_parsed_tmp[i][0] == 8)//��¼����pe��le��se��index
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

	for (int i = 0; i < (int)config_size; i++)                            //i��ʾ����ˣ�j��ʾ�ȳ���
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

		else if (vec_config_parsed_tmp[i][0] == 9)                      //le������
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

		else if (vec_config_parsed_tmp[i][0] == 0)                      //se����
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

	for (int k = 0; k < pe_size + se_size; k++)
	{
		cout << "fanout[" << k << "] = ";
		for (int m = 0; m < 5; m++)
		{
			cout << port_fanout[k][m];
		}
		cout << endl;
	}
}

/*
void pe_sim_step1_new(ProcessingElement* pe_current)
{
	int pe_index_current = pe_current->config_reg.front()[1];//��ǰ���ڴ����PE���
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
	//�ж��Ƿ�����
	bool is_din1_float = din1_from_flag;//0--���գ���0--������
	bool is_din2_float = din2_from_flag;
	bool is_bin_float = bin_from_flag;
	
	//last PE����
	if (is_din1_float)//din1������
	{
		//din1 from PE ob����
		pe_sim_step3(pe[din1_from_index], din1_from_port);
	}
	if (is_din2_float)//din2������
	{
		//din2 from PE ob����
		pe_sim_step3(pe[din2_from_index], din2_from_port);
	}
	if (is_bin_float)//bin������
	{
		//bin from PE ob����
		pe_sim_step3(pe[bin_from_index], bin_from_index);
	}
	//data trans
	pe_sim_step_connect(pe_current);
	//data store
	pe_sim_step1(pe_current);

}

void pe_sim_step1_new_no_tag(ProcessingElement* pe_current)
{
	int pe_index_current = pe_current->config_reg.front()[1];//��ǰ���ڴ����PE���
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
	//�ж��Ƿ�����
	bool is_din1_float = din1_from_flag;//0--���գ���0--������
	bool is_din2_float = din2_from_flag;
	bool is_bin_float = bin_from_flag;
	//last PE ob����
	//last PE����
	if (is_din1_float)//din1������
	{
		//din1 from PE ob����
		pe_sim_step3_no_tag(pe[din1_from_index], din1_from_port);
	}

	if (is_din2_float)//din2������
	{
		//din2 from PE ob����
		pe_sim_step3_no_tag(pe[din2_from_index], din2_from_port);
	}

	if (is_bin_float)//bin������
	{
		//bin from PE ob����
		pe_sim_step3_no_tag(pe[bin_from_index], bin_from_index);
	}
	//data transmit
	pe_sim_step_connect(pe_current);
	//data get into buffer
	pe_sim_step1_no_tag(pe_current);

}
*/