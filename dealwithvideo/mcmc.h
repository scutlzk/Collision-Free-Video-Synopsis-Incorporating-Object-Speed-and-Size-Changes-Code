#pragma once
#include<iostream>
#include"tubemanager.h"
#include <opencv2\highgui\highgui.hpp>
#include"effolkronium/random.hpp"
#include"static_const.h"
#include"productvideo.h"
using namespace cv;
using Random = effolkronium::random_static;
using namespace std;

int computemcmc(tubemanagers &manager, double vlow, double vup, double rlow)
{
	fstream f;
	f.open(manager.time + ".txt", ios::out);
	if (!f)cout << "打开result.txt出错" << endl;

	int id;//改变的物体id
	int flag;
	int framevalue = INT_MAX;//起始帧
	int vvalue = INT_MAX;//速度；
	double sizee = 0;//尺寸；
	int para = 0;
	double alpha;
	double cc;
	int best_cp = 0;
	double gamma;
	double qsd = manager.compute_enemgy();
	gamma = (1 / qsd)*600;
	double y1 = exp(-1 * gamma*manager.compute_enemgy());//初始化
	cout << manager.enemgy << endl;
	cout << y1 << endl;
	double y2;
	double best_q = qsd;
	long long times = 30000000;
	int theta1 = 1;

	//进行MCMC
	for (long long i = 0; i < times; i++)
	{

		id = Random::get<Random::common>(1, manager.ide);

		flag = Random::get<bool>(0.75);

		if (flag)
		{
			framevalue = (Random::get<Random::common>(0, manager.videoe) /5) * 5;

			vvalue = INT_MAX;
			sizee = 0;
			para = 0;
		}
		else
		{
			framevalue = INT_MAX;
			para = Random::get<Random::common>(0, (manager.data[id].paragraph_length - 1));
			if (i%4 == 0)
			{
				sizee = Random::get<Random::common>(rlow, 1.0);//???
				vvalue = INT_MAX;
			}
			else
			{
				vvalue = Random::get<Random::common>(cvRound(lengthunit/vup), cvRound(lengthunit/vlow));   //31
				sizee = 0;
			}
		}
		double q = manager.compute_enemgy(id, framevalue, para, vvalue, sizee);//计算能量
		y2 = exp(-1 * gamma*q);

		//存储最好的结果   最好的结果不一定是最后的结果
		if (best_q > q)
		{
			best_cp = i;
			y2 = exp(-1 * gamma*q);
			cout << endl << best_q << "  < " << q  <<"  "<<y2<< endl;
			best_q = q;
			for (int ii = 1; ii < manager.ide + 1; ++ii)
			{
				manager.best_result[ii][0] = manager.data[ii].ts;
				for (int j = 1; j < 1 + manager.data[ii].paragraph_length; ++j)
				{
					manager.best_result[ii][j] = manager.data[ii].v[(j - 1)];
					manager.best_result[ii][j + manager.data[ii].paragraph_length] = manager.data[ii].ratio_size[(j - 1)];
				}
			}
			if (vvalue != INT_MAX)
				manager.best_result[id][para + 1] = vvalue;
			if (framevalue != INT_MAX)
				manager.best_result[id][0] = framevalue;
			if (sizee != 0)
			{
				manager.best_result[id][para + 1 + manager.data[id].paragraph_length] = sizee;
			}
			cout << "****   " << actfactor* manager.activity_e[0] << "****   " << balfactor* manager.balance_e[0] << "*****   " << colfactor* manager.collision_e[0][0] << "  " << manager.collision_e[0][0] << "------*****" << timefactor*manager.time_e[0][0] << endl;
		}
		alpha = min(1.0, y2 / y1);
		cc = Random::get<Random::common>(0.0, 1.0);
		if (cc < alpha)
		{
			if (y1>0.05)
			{
				best_cp = i;
				gamma = (1 / q) * 600;
				y2 = exp(-1 * gamma*q);
			}
			y1  =y2;
			//接受修改
			manager.set(id, framevalue, para, vvalue, sizee);
		}

	}
	//重置为最好的结果
	for (int ii = 1; ii < manager.ide + 1; ++ii)
	{
		manager.data[ii].ts = manager.best_result[ii][0];
		for (int j = 1; j < 1 + manager.data[ii].paragraph_length; ++j)
		{
			manager.data[ii].v[(j - 1)] = manager.best_result[ii][j];
			
			manager.data[ii].ratio_size[(j - 1)] = manager.best_result[ii][j + manager.data[ii].paragraph_length];
		}
		manager.data[ii].compute_ratio_v();
	}

	
	manager.v_reset();//速度重置
	manager.size_reset();//size 重置
	manager.size_smooth();//size 平滑


	//生成结果数据
	for (int ii = 1; ii < manager.ide + 1; ++ii)
	{
		f << ii << " " <<
			manager.best_result[ii][0] << "   ";
		cout << ii << " " <<
			manager.best_result[ii][0] << "   ";
		for (int j = 1; j < 1 + manager.data[ii].paragraph_length; ++j)
		{
			f << j << " " << manager.best_result[ii][j] << " " << manager.best_result[ii][j + manager.data[ii].paragraph_length] << " " << "   ";
			cout << j << "*" << manager.best_result[ii][j] << "*" << manager.best_result[ii][j + manager.data[ii].paragraph_length] << "*" << "   ";
		}
		f << endl;
		cout << endl;
	}

	int g = 0;
	int qe_1 = manager.collsion_sum(g);
	f << qe_1 << " ";
	f << g<<" ";
	double asd = 0;
	double activit = 0;
	for (int i = 1; i < manager.ide + 1; i++)
	{
		for (int j = 1; j < manager.ide + 1; j++)
			asd += manager.computetime(i, j);
		activit += manager.data[i].activityenemgy(manager.videos, manager.videoe);
	}
	f<<asd<< " ";
	double qe_2 = manager.compute_enemgy();
	f << qe_2 << " ";;

	double b = 0;
	double c = 0;
	double dd = 0;
	double ss = 0;
	double tt = 0;
	double v_e = 0;
	double s_e = 0;
	double smooth_e = 0;
	int cp3 = 0;
	int cp1 = 0, cp2 = 0;
	for (int j = 1; j < manager.ide + 1; j++)
	{
		double v_ee = 0;
		double s_ee = 0;
		double smooth_ee = 0;
		for (int i = 0; i < manager.data[j].paragraph_length; ++i)
		{
			cp3++;
			s_ee += exp(benem3_a / manager.data[j].ratio_size[i]);// *exp(benem3_b / manager.data[j].min_size[i]);

			if (manager.data[j].ratio_v[i] >= 1)
			{
				cp1++;
				dd += manager.data[j].ratio_v[i];
				v_ee += exp(benem2_a*manager.data[j].ratio_v[i]);// *exp(benem2_b / manager.data[j].length);
			}
			else
			{
				v_ee += exp(benem2_a / manager.data[j].ratio_v[i]);// *exp(benem2_b / manager.data[j].length);
			}
			if (manager.data[j].ratio_v[i] <= 1)
				{
					ss += manager.data[j].ratio_v[i] ;
					cp2++;
				}
			b += manager.data[j].ratio_size[i]/double(manager.data[j].paragraph_length);
			double qqq=manager.data[j].v[i] / double(lengthunit)/double( manager.data[j].paragraph_length);
			c += qqq;
		}
		for (int i = 1; i < manager.data[j].paragraph_length - 1; ++i)
		{
			smooth_ee += exp(2 * manager.data[j].ratio_v[i] - manager.data[j].ratio_v[i - 1] - manager.data[j].ratio_v[i + 1]);
			smooth_ee += exp(2 * manager.data[j].ratio_size[i] - manager.data[j].ratio_size[i - 1] - manager.data[j].ratio_size[i + 1]);
		}
		s_e += s_ee / manager.data[j].paragraph_length;
		smooth_e += smooth_ee / manager.data[j].paragraph_length;
		v_e += v_ee / manager.data[j].paragraph_length;;
	}
	c /= manager.ide;
	b/= manager.ide;
	if (cp1 == 0)dd = 0;
	else dd /=cp1;
	if (cp2 == 0) ss = 0;
	else ss/=cp2;
	if (dd < 0)dd = 1.0 / dd;
	f << " " << b << " " << dd << " " << ss << " " << v_e << " " << s_e << " " << smooth_e <<" "<< activit<< endl;
	fstream fgh;
	fgh.open(  to_string(vlow)+".txt", ios::out | ios::app);
	fgh << qe_1 << " ";
	fgh << g << " ";
	fgh << asd << " ";
	fgh << qe_2 << " ";;
	fgh << " " << b << " " << dd << " " << ss << " " << v_e << " " << s_e << " " << smooth_e << " " << activit << endl;
	fgh.close();
	f.close();
	return 0;
}