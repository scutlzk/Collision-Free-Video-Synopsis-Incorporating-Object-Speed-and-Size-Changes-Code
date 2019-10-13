#pragma once
#include"tube.h"
using namespace std;
#include "static_const.h"
#include "Eigen/Sparse"
#include <Windows.h>
#include <ctime>
#include <algorithm> 

using namespace Eigen;
class tubemanagers
{
public:
	double **best_result ;//最优结果  记录每个tube的起始帧，及每个tube的每一段的速度
	int M;
	int ide;//tube个数
	Tube*data;
	int videos;
	int videoe;
	double *activity_e;//activity_e[0]为总能量
	double *balance_e;//balance_e[0]为总能量
	double **time_e;//time_e[0]为总能量
	double **collision_e;//collision_e[0]为总能量   collision_e[x][y]为x跟y之间的碰撞能量

	string markfile;
	double enemgy;

	double beforeactivity_e;
	double beforebalance_e;
	double **beforetime_e;//beforetime_e[0]为总能量
	double *beforecollision_e;//beforecollision_e[0]为总能量   beforecollision_e[x][y]为x跟y之间的碰撞能量
	int fps;//原视频帧率
	int f[1000][2];
	int cp;
	string time;
	tubemanagers(int videoe, string markfile, int fps, int videos = 0)
	{
		M = 0;
		time = getTime();
		this->videoe = videoe;//摘要视频的结束帧，可以理解为长度
		this->videos = videos;
		this->markfile = markfile;
		this->fps = fps;

		fstream f;
		f.open(markfile + "\\" + "tubeframe.txt");

		int a, b, c;

		f >> this->ide;

		enemgy = 0;
		data = new Tube[ide + 1];
		collision_e = new double*[ide + 1];
		time_e = new double*[ide + 1];
		activity_e = new double[ide + 1];
		balance_e = new double[ide + 1];
		beforecollision_e = new double[ide + 1];
		beforetime_e = new double*[2];
		beforeactivity_e = 0;
		beforebalance_e = 0;
		best_result = new double *[ide + 1];

		for (int i = 1; i < ide + 1; ++i)
		{
			f >> a >> b >> c;
			if (a != i)
				cerr << "if (a != i)" << endl;

			data[i].initialization(a, b, c);
			data[i].read_constframenode(markfile);
			data[i].read_constframevalue(markfile);

			best_result[i] = new double[1 + 2 * data[i].paragraph_length];

			for (int j = 0; j < 1 + 2 * data[i].paragraph_length; ++j)
				best_result[i][j] = 0;
			data[i].ts = (i - 1) *70;

		}

		compute_M();

		beforetime_e[0] = new double[ide + 1];
		beforetime_e[1] = new double[ide + 1];
		for (int i = 0; i < ide + 1; ++i)
		{
			collision_e[i] = new double[ide + 1];
			time_e[i] = new double[ide + 1];
			beforecollision_e[i] = 0;

			for (int j = 0; j < ide + 1; j++)
			{
				collision_e[i][j] = 0;
				time_e[i][j] = 0;
				beforetime_e[0][j] = 0;
				beforetime_e[1][j] = 0;
			}
			activity_e[i] = 0;
			balance_e[i] = 0;
		}

	}
	string getTime()
	{
		time_t timep;
		std::time(&timep);
		char tmp[64];
		strftime(tmp, sizeof(tmp), "%Y_%m_%d_%H_%M_%S", localtime(&timep));
		return tmp;
	}
	void compute_M() //参考论文里的 the increasing rate is determined by ξ that is automatically determined as follows
	{
		int *per_2 = new int[ide - 2];
		for (int i = 0; i < ide - 2; i++)
		{
			per_2[i] = 0;
			per_2[i] = abs(data[i + 2].src_ts - data[i].src_ts);
		}
		sort(per_2, per_2 + ide - 2);

		M = per_2[(ide - 2) / 2];
		cout << M << endl;

	}
	int DecideOverlap(const Rect &r1, const Rect &r2, const double &ratio_size1, const double &ratio_size2)//计算矩形的重叠面积即碰撞面积
	{
		int x1 = r1.x + (r1.width / 2 * (1 - ratio_size1));
		int y1 = r1.y + (r1.height / 2 * (1 - ratio_size1));
		int width1 = r1.width*ratio_size1;
		int height1 = r1.height*ratio_size1;

		int x2 = r2.x + (r2.width / 2 * (1 - ratio_size2));
		int y2 = r2.y + (r2.height / 2 * (1 - ratio_size2));
		int width2 = r2.width*ratio_size2;
		int height2 = r2.height*ratio_size2;

		int endx = max(x1 + width1, x2 + width2);
		int startx = min(x1, x2);
		int width = width1 + width2 - (endx - startx);

		int endy = max(y1 + height1, y2 + height2);
		int starty = min(y1, y2);
		int height = height1 + height2 - (endy - starty);
		if (width < 0 || height < 0)
			return 0;
		else
			return abs(width*height);

	}
	int computecollision(int id1, int id2)//计算tube id1和tube id2 的碰撞面积
	{
		int ae = 0;
		if (id1 == id2)return 0;
		if (data[id1].ts > (data[id2].ts + data[id2].tubematbelong()))
			return 0;
		if (data[id2].ts > (data[id1].ts + data[id1].tubematbelong()))
			return 0;

		int frame1_i = 0;
		int frame1j = 0;
		int frame2_i = 0;
		int frame2j = 0;
		int tube1b = data[id1].ts;//摘要视频里的
		int tube2b = data[id2].ts;

		int num2 = 0;
		int num1 = 0;
		if (data[id1].ts > data[id2].ts)
			if (computecollision_lock_in_time(id2, id1, frame2_i, frame2j, tube2b, tube1b, num2))
			{
				return 0;
			}

		if (data[id1].ts < data[id2].ts)
			if (computecollision_lock_in_time(id1, id2, frame1_i, frame1j, tube1b, tube2b, num1))
			{
				return 0;
			}

		int frame1b = frame1_i*lengthunit + data[id1].v_translate[data[id1].v[frame1_i]][frame1j];
		int frame2b = frame2_i*lengthunit + data[id2].v_translate[data[id2].v[frame2_i]][frame2j];

		for (; ((data[id1].length > frame1b) && (data[id2].length > frame2b)); ++tube1b)
		{

			if (tube1b >= videos&&tube1b <= videoe)
			{
				int q = DecideOverlap(*data[id1].constframenode[frame1b], *data[id2].constframenode[frame2b], data[id1].ratio_size[frame1_i], data[id2].ratio_size[frame2_i]);

				if (q > 0)
					q = DecideOverlap(*data[id1].constframenode[frame1b], *data[id2].constframenode[frame2b], 1, 1);

				ae += q;
			}
			++frame1j;
			if (frame1j == data[id1].v[frame1_i])
			{
				++frame1_i;
				if (frame1_i == data[id1].paragraph_length)
					break;
				frame1j = 0;
			}
			++frame2j;
			if (frame2j == data[id2].v[frame2_i])
			{
				++frame2_i;
				if (frame2_i == data[id2].paragraph_length)
					break;
				frame2j = 0;
			}
			frame1b = frame1_i*lengthunit + data[id1].v_translate[data[id1].v[frame1_i]][frame1j];

			frame2b = frame2_i*lengthunit + data[id2].v_translate[data[id2].v[frame2_i]][frame2j];
		}
		return ae;
	}
	double computetime(int idx, int idy)//计算tube x 和 tube y的时间能量
	{

		double a = 0;
		int i = idx;
		int j = idy;
		if (i == j)return 0;

		if (data[i].src_ts > data[j].src_ts &&data[i].ts < data[j].ts)
		{
			int mark;
			int this_paragraph_length;
			int begin = data[i].ts;
			for (int k = 0; k < data[i].paragraph_length; k++)
			{
				mark = k*lengthunit;
				for (int t0 = 0; t0 < data[i].v[k]; t0++)
				{
					if (begin < data[j].ts)
						a += data[i].constframevalue[mark + data[i].v_translate[data[i].v[k]][t0]] * data[i].ratio_v[k];
					begin++;
				}
			}

		}
		double q = (1.0 / (1 - pow((min(abs(data[i].src_ts - data[j].src_ts), M)
			/ double(M)), 5) + 0.000000001)) - 1.0;
		return a*q;
	}
	double compute_enemgy()//计算能量
	{
		activity_e[0] = 0;
		balance_e[0] = 0;
		collision_e[0][0] = 0;
		time_e[0][0] = 0;

		for (int i = 1; i < ide + 1; ++i)
		{
			activity_e[i] = data[i].activityenemgy(videos, videoe);
			activity_e[0] += activity_e[i];
			collision_e[i][0] = 0;
			balance_e[i] = data[i].benemgy();
			balance_e[0] += balance_e[i];

			for (int j = i; j < ide + 1; ++j)
			{
				if (i == j)continue;
				collision_e[i][j] = computecollision(i, j);
				collision_e[j][i] = collision_e[i][j];
				collision_e[0][0] += collision_e[i][j];
				collision_e[i][0] += collision_e[i][j];

				time_e[i][j] = computetime(i, j);
				time_e[j][i] = computetime(j, i);
				time_e[0][0] += (time_e[i][j] + time_e[j][i]);
			}
		}

		enemgy = actfactor*activity_e[0] + balfactor*balance_e[0] + colfactor* collision_e[0][0] + timefactor* time_e[0][0];
		cout << "****" << actfactor* activity_e[0] << "****" << balfactor* balance_e[0] << "****" << colfactor* collision_e[0][0] << "****" << timefactor*time_e[0][0] << endl;
		return enemgy;
	}

	int computecollision_lock_in_time(const int& slowid, const  int& fastid, int &slowi, int &slowj, int &slowtubeb, const int &fasttubeb, int &num)const//1失败 0成功  
		//又臭又长，大概就是同步两个tube在摘要视频某一帧的帧，从而计算碰撞面积
	{
		for (int i = 0; i < data[slowid].paragraph_length; ++i)
		{
			if (slowtubeb + data[slowid].v[i] >= fasttubeb)
			{
				for (int j = 0; j < data[slowid].v[i]; ++j)
				{
					++slowtubeb;
					if (slowtubeb == fasttubeb)
					{
						if (i*lengthunit + data[slowid].v_translate[data[slowid].v[i]][j] < data[slowid].length)
						{
							slowi = i;
							slowj = j;
							return 0;
						}
						else
							return 1;
					}
				}
			}
			else
				slowtubeb += data[slowid].v[i];

		}
		return 1;
	}
	int computecollision(const int& id1, const int& id2, const int& id1tts, const int& para, const int& id1vv, const double& sizee)
	{
		int temp1 = data[id1].ts;
		int temp2 = data[id1].v[para];
		double temp3 = data[id1].ratio_size[para];
		double temp4 = data[id1].ratio_v[para];

		data[id1].ts = id1tts;
		data[id1].v[para] = id1vv;
		data[id1].ratio_size[para] = sizee;
		data[id1].ratio_v[para] = data[id1].get_ratio_v(id1vv);

		double ae = computecollision(id1, id2);

		data[id1].ts = temp1;
		data[id1].v[para] = temp2;
		data[id1].ratio_size[para] = temp3;
		data[id1].ratio_v[para] = temp4;
		return ae;
	}
	double computetime(int idx, int idy, int idxtts, int para, int idxvv)
	{
		//修改值
		int tss = data[idx].ts;
		int temp1 = data[idx].v[para];
		double temp = data[idx].ratio_v[para];
		data[idx].ts = idxtts;
		data[idx].v[para] = idxvv;
		data[idx].ratio_v[para] = data[idx].get_ratio_v(idxvv);

		double a = computetime(idx, idy);
		//还原
		data[idx].ratio_v[para] = temp;
		data[idx].v[para] = temp1;
		data[idx].ts = tss;
		return a;
	}
	double computetime(int idx, int idy, int idyts, int para, int idyv, int flag)
	{
		//修改值
		int tss = data[idy].ts;
		int temp1 = data[idy].v[para];
		double temp = data[idy].ratio_v[para];
		data[idy].ts = idyts;
		data[idy].v[para] = idyv;
		data[idy].ratio_v[para] = data[idy].get_ratio_v(idyv);

		double a = computetime(idx, idy);
		//还原
		data[idy].ratio_v[para] = temp;
		data[idy].v[para] = temp1;
		data[idy].ts = tss;
		return a;
	}

	double compute_enemgy(int id, int tts, int para, int vv, double sizee)
	{
		beforeactivity_e = 0;
		beforebalance_e = 0;
		for (int i = 0; i < ide + 1; i++)
		{
			beforecollision_e[i] = 0;
			beforetime_e[0][i] = 0;
			beforetime_e[1][i] = 0;
		}
		if (tts == INT_MAX)
			tts = data[id].ts;
		if (vv == INT_MAX)
			vv = data[id].v[para];
		if (sizee == 0)
			sizee = data[id].ratio_size[para];
		beforeactivity_e = data[id].activityenemgy(videos, videoe, tts, para, vv);
		beforebalance_e = data[id].benemgy(tts, para, vv, sizee);

		time_e[id][0] = 0;
		for (int i = 1; i < ide + 1; ++i)
		{
			if (i == id)continue;
			beforecollision_e[i] = computecollision(id, i, tts, para, vv, sizee);
			beforecollision_e[0] += beforecollision_e[i];

			beforetime_e[0][i] = computetime(id, i, tts, para, vv);
			beforetime_e[1][i] = computetime(i, id, tts, para, vv, 1);
			beforetime_e[0][0] += beforetime_e[0][i];
			beforetime_e[1][0] += beforetime_e[1][i];

			time_e[id][0] += (time_e[id][i] + time_e[i][id]);

		}
		for (int i = 1; i < ide + 1; i++)
		{
			collision_e[i][0] = 0;

			for (int j = 1; j < ide + 1; ++j)
			{
				collision_e[i][0] += collision_e[i][j];
			}
		}

		beforetime_e[0][0] += beforetime_e[1][0];
		double p = actfactor*(activity_e[0] + beforeactivity_e - activity_e[id]) +
			balfactor*(balance_e[0] + beforebalance_e - balance_e[id]) +
			colfactor*(collision_e[0][0] + beforecollision_e[0] - collision_e[id][0]) +
			timefactor*(time_e[0][0] + beforetime_e[0][0] - time_e[id][0]);
		return  p;
	}



	void set(int id, int ts, int para, int v, double sizee)
	{
		if (v != INT_MAX)
		{
			data[id].v[para] = v;

			data[id].ratio_v[para] = data[id].get_ratio_v(v);
		}
		if (ts != INT_MAX)
			data[id].ts = ts;
		if (sizee != 0)
		{
			data[id].ratio_size[para] = sizee;
		}

		activity_e[0] -= activity_e[id];
		activity_e[id] = beforeactivity_e;
		activity_e[0] += activity_e[id];

		if (ts == INT_MAX)
		{
			balance_e[0] -= balance_e[id];
			balance_e[id] = beforebalance_e;
			balance_e[0] += balance_e[id];
		}
		collision_e[0][0] -= collision_e[id][0];
		collision_e[id][0] = beforecollision_e[0];
		collision_e[0][0] += collision_e[id][0];

		time_e[0][0] -= time_e[id][0];
		time_e[id][0] = beforetime_e[0][0];
		time_e[0][0] += time_e[id][0];
		for (int i = 1; i < ide + 1; ++i)
		{
			collision_e[id][i] = beforecollision_e[i];
			collision_e[i][id] = beforecollision_e[i];
			time_e[id][i] = beforetime_e[0][i];
			time_e[i][id] = beforetime_e[1][i];
			collision_e[i][0] = 0;
		}
		for (int i = 1; i < ide + 1; ++i)
			for (int j = 1; j < ide + 1; ++j)
				collision_e[i][0] += collision_e[i][j];
		enemgy = actfactor*activity_e[0] + balfactor*balance_e[0] + colfactor*collision_e[0][0] + timefactor*time_e[0][0];

	}


	void size_reset() //重置所有tube的段的size为1，如果size为1时没有发生碰撞，则为1
	{
		for (int ii = 1; ii < ide + 1; ++ii)
		{
			data[ii].ts = best_result[ii][0];
			for (int j = 1; j < 1 + data[ii].paragraph_length; ++j)
			{
				data[ii].v[(j - 1)] = best_result[ii][j];

				data[ii].ratio_size[(j - 1)] = 1;
			}
			data[ii].compute_ratio_v();
		}

		for (int id1 = 1; id1 < ide + 1; ++id1)
		{
			double *ae = new double[data[id1].paragraph_length];
			for (int i = 0; i < data[id1].paragraph_length; i++)
				ae[i] = 0;
			for (int id2 = 1; id2 < ide + 1; ++id2)
			{
				if (id1 == id2)continue;
				if (data[id1].ts > (data[id2].ts + data[id2].tubematbelong()))
					continue;
				if (data[id2].ts > (data[id1].ts + data[id1].tubematbelong()))
					continue;

				int frame1_i = 0;
				int frame1j = 0;
				int frame2_i = 0;
				int frame2j = 0;
				int tube1b = data[id1].ts;//摘要视频里的
				int tube2b = data[id2].ts;

				int num2 = 0;
				int num1 = 0;
				if (data[id1].ts > data[id2].ts)
					if (computecollision_lock_in_time(id2, id1, frame2_i, frame2j, tube2b, tube1b, num2))
					{
						continue;
					}

				if (data[id1].ts < data[id2].ts)
					if (computecollision_lock_in_time(id1, id2, frame1_i, frame1j, tube1b, tube2b, num1))
					{
						continue;
					}


				int frame1b = frame1_i*lengthunit + data[id1].v_translate[data[id1].v[frame1_i]][frame1j];
				int frame2b = frame2_i*lengthunit + data[id2].v_translate[data[id2].v[frame2_i]][frame2j];

				for (; ((data[id1].length > frame1b) && (data[id2].length > frame2b)); ++tube1b)
				{

					if (tube1b >= videos&&tube1b <= videoe)
					{
						int q = DecideOverlap(*(data[id1].constframenode[frame1b]), *(data[id2].constframenode[frame2b]), data[id1].ratio_size[frame1_i], data[id2].ratio_size[frame2_i]);
						if (q > 0)
							q = DecideOverlap(*data[id1].constframenode[frame1b], *data[id2].constframenode[frame2b], 1, 1);

						ae[frame1_i] += q;

					}
					++frame1j;
					if (frame1j == data[id1].v[frame1_i])
					{
						++frame1_i;
						if (frame1_i == data[id1].paragraph_length)
							break;
						frame1j = 0;
					}
					++frame2j;
					if (frame2j == data[id2].v[frame2_i])
					{
						++frame2_i;
						if (frame2_i == data[id2].paragraph_length)
							break;
						frame2j = 0;
					}

					frame1b = frame1_i*lengthunit + data[id1].v_translate[data[id1].v[frame1_i]][frame1j];
					frame2b = frame2_i*lengthunit + data[id2].v_translate[data[id2].v[frame2_i]][frame2j];
				}

			}
			for (int para = 0; para < data[id1].paragraph_length; para++)
			{
				if (ae[para] > 10)
				{
					data[id1].ratio_size[para] = (best_result[id1][para + 1 + data[id1].paragraph_length]);


				}
				else
				{
					best_result[id1][para + 1 + data[id1].paragraph_length] = 1;
				}
			}

		}
	}



	void v_reset()//重置所有tube的段的速度为1，如果速度为1时没有发生碰撞，则为1
	{
		int *old_long = new int[ide + 1];
		for (int i = 0; i < ide + 1; i++)
			old_long[i] = 0;
		for (int ii = 1; ii < ide + 1; ++ii)
		{
			data[ii].ts = best_result[ii][0];
			for (int j = 1; j < 1 + data[ii].paragraph_length; ++j)
			{
				data[ii].v[(j - 1)] = best_result[ii][j];
				data[ii].ratio_size[(j - 1)] = best_result[ii][j + data[ii].paragraph_length];
			}
			data[ii].compute_ratio_v();
			old_long[ii] = data[ii].ts + data[ii].tubematbelong();
		}
		/**/
		int *old_collsion = new int[ide + 1];
		for (int i = 0; i < ide + 1; i++)
			old_collsion[i] = 0;

		for (int id1 = 1; id1 < ide + 1; ++id1)
		{
			for (int id2 = 1; id2 < ide + 1; ++id2)
			{
				if (id1 == id2)continue;
				if (data[id1].ts > (data[id2].ts + data[id2].tubematbelong()))
					continue;
				if (data[id2].ts > (data[id1].ts + data[id1].tubematbelong()))
					continue;

				int frame1_i = 0;
				int frame1j = 0;
				int frame2_i = 0;
				int frame2j = 0;
				int tube1b = data[id1].ts;//摘要视频里的
				int tube2b = data[id2].ts;

				int num2 = 0;
				int num1 = 0;
				if (data[id1].ts > data[id2].ts)
					if (computecollision_lock_in_time(id2, id1, frame2_i, frame2j, tube2b, tube1b, num2))
					{
						continue;
					}

				if (data[id1].ts < data[id2].ts)
					if (computecollision_lock_in_time(id1, id2, frame1_i, frame1j, tube1b, tube2b, num1))
					{
						continue;
					}

				int frame1b = frame1_i*lengthunit + data[id1].v_translate[data[id1].v[frame1_i]][frame1j];
				int frame2b = frame2_i*lengthunit + data[id2].v_translate[data[id2].v[frame2_i]][frame2j];





				for (; ((data[id1].length > frame1b) && (data[id2].length > frame2b)); ++tube1b)
				{

					if (tube1b >= videos&&tube1b <= videoe)
					{
						int q = DecideOverlap(*data[id1].constframenode[frame1b], *data[id2].constframenode[frame2b], data[id1].ratio_size[frame1_i], data[id2].ratio_size[frame2_i]);

						old_collsion[id1] += q;

					}
					++frame1j;
					if (frame1j == data[id1].v[frame1_i])
					{
						++frame1_i;

						if (frame1_i == data[id1].paragraph_length)
							break;
						frame1j = 0;
					}
					++frame2j;
					if (frame2j == data[id2].v[frame2_i])
					{
						++frame2_i;
						if (frame2_i == data[id2].paragraph_length)
							break;
						frame2j = 0;
					}

					frame1b = frame1_i*lengthunit + data[id1].v_translate[data[id1].v[frame1_i]][frame1j];
					frame2b = frame2_i*lengthunit + data[id2].v_translate[data[id2].v[frame2_i]][frame2j];
				}



			}


		}

		/**/
		for (int id1 = 1; id1 < ide + 1; ++id1)
		{
			for (int asd = 0; asd < data[id1].paragraph_length; asd++)
			{
				double ae = 0;
				data[id1].v[asd] = lengthunit;
				data[id1].ratio_v[asd] = 1;
				for (int id2 = 1; id2 < ide + 1; ++id2)
				{
					if (id1 == id2)continue;
					if (data[id1].ts > (data[id2].ts + data[id2].tubematbelong()))
						continue;
					if (data[id2].ts > (data[id1].ts + data[id1].tubematbelong()))
						continue;

					int frame1_i = 0;
					int frame1j = 0;
					int frame2_i = 0;
					int frame2j = 0;
					int tube1b = data[id1].ts;//摘要视频里的
					int tube2b = data[id2].ts;

					int num2 = 0;
					int num1 = 0;
					if (data[id1].ts > data[id2].ts)
						if (computecollision_lock_in_time(id2, id1, frame2_i, frame2j, tube2b, tube1b, num2))
						{
							continue;
						}

					if (data[id1].ts < data[id2].ts)
						if (computecollision_lock_in_time(id1, id2, frame1_i, frame1j, tube1b, tube2b, num1))
						{
							continue;
						}

					int frame1b = frame1_i*lengthunit + data[id1].v_translate[data[id1].v[frame1_i]][frame1j];
					int frame2b = frame2_i*lengthunit + data[id2].v_translate[data[id2].v[frame2_i]][frame2j];





					for (; ((data[id1].length > frame1b) && (data[id2].length > frame2b)); ++tube1b)
					{

						if (tube1b >= videos&&tube1b <= videoe)
						{
							int q = DecideOverlap(*data[id1].constframenode[frame1b], *data[id2].constframenode[frame2b], data[id1].ratio_size[frame1_i], data[id2].ratio_size[frame2_i]);

							ae += q;

						}
						++frame1j;
						if (frame1j == data[id1].v[frame1_i])
						{
							++frame1_i;

							if (frame1_i == data[id1].paragraph_length)
								break;
							frame1j = 0;
						}
						++frame2j;
						if (frame2j == data[id2].v[frame2_i])
						{
							++frame2_i;
							if (frame2_i == data[id2].paragraph_length)
								break;
							frame2j = 0;
						}

						frame1b = frame1_i*lengthunit + data[id1].v_translate[data[id1].v[frame1_i]][frame1j];
						frame2b = frame2_i*lengthunit + data[id2].v_translate[data[id2].v[frame2_i]][frame2j];
					}



				}
				if (ae > old_collsion[id1]|(data[id1].ts+data[id1].tubematbelong()>old_long[id1]&& data[id1].ts + data[id1].tubematbelong()>videoe))
				{
					data[id1].v[asd] = (best_result[id1][asd + 1]);
					data[id1].ratio_v[asd] = data[id1].get_ratio_v(data[id1].v[asd]);
				}
				else

					best_result[id1][asd + 1] = lengthunit;

			}


		}
	}


	void size_smooth() //平滑size
	{

		for (int i = 0; i < 1000; i++)
			for (int j = 0; j < 2; ++j)
				f[i][j] = 0;
		cp = 0;
		for (int i = 1; i < ide + 1; ++i)
		{


			SparseMatrix< double, ColMajor> a(2 * data[i].paragraph_length - 2, data[i].paragraph_length);
			VectorXd b(data[i].paragraph_length);
			if (data[i].paragraph_length == 1)
				continue;
			for (int j = 0; j < data[i].paragraph_length; ++j)
			{

				b(j) = (best_result[i][1 + data[i].paragraph_length + j]);
				a.insert(j, j) = 1;
			}
			for (int j = data[i].paragraph_length; j < 2 * data[i].paragraph_length - 2; j++)
			{
				a.insert(j, j - data[i].paragraph_length) = -1;
				a.insert(j, j - data[i].paragraph_length + 1) = 2;
				a.insert(j, j - data[i].paragraph_length + 2) = -1;
			}
			VectorXd x;

			SparseQR <SparseMatrix <double>, COLAMDOrdering< int >>  solver;
			solver.compute(a.transpose()*a);
			solver.analyzePattern(a.transpose()*a);
			solver.factorize(a.transpose()*a);
			x = solver.solve(b);

			for (int j = 0; j < data[i].paragraph_length; ++j)
			{
				best_result[i][1 + data[i].paragraph_length + j] = min(best_result[i][1 + data[i].paragraph_length + j], x(j));
				if (best_result[i][1 + data[i].paragraph_length + j] < 0.5)best_result[i][1 + data[i].paragraph_length + j] = 0.5;
				data[i].ratio_size[j] = best_result[i][1 + data[i].paragraph_length + j];
			}

		}
		cout << "********" << endl;
	}




	int collsion_sum(int &g) 
	{
		int	ae = 0;
		for (int id1 = 1; id1 < ide + 1; ++id1)
		{

			for (int id2 = 1; id2 < ide + 1; ++id2)
			{
				if (id1 == id2)continue;
				if (data[id1].ts > (data[id2].ts + data[id2].tubematbelong()))
					continue;
				if (data[id2].ts > (data[id1].ts + data[id1].tubematbelong()))
					continue;
				int frame1_i = 0;
				int frame1j = 0;
				int frame2_i = 0;
				int frame2j = 0;
				int tube1b = data[id1].ts;//摘要视频里的
				int tube2b = data[id2].ts;

				int num2 = 0;
				int num1 = 0;
				if (data[id1].ts > data[id2].ts)
					if (computecollision_lock_in_time(id2, id1, frame2_i, frame2j, tube2b, tube1b, num2))
					{
						continue;
					}

				if (data[id1].ts < data[id2].ts)
					if (computecollision_lock_in_time(id1, id2, frame1_i, frame1j, tube1b, tube2b, num1))
					{
						continue;
					}


				int frame1b = frame1_i*lengthunit + data[id1].v_translate[data[id1].v[frame1_i]][frame1j];
				int frame2b = frame2_i*lengthunit + data[id2].v_translate[data[id2].v[frame2_i]][frame2j];


				for (; ((data[id1].length > frame1b) && (data[id2].length > frame2b)); ++tube1b)
				{

					if (tube1b >= videos&&tube1b <= videoe)
					{


						int q = DecideOverlap(*data[id1].constframenode[frame1b], *data[id2].constframenode[frame2b], data[id1].ratio_size[frame1_i], data[id2].ratio_size[frame2_i]);

						if (q > 5)q = DecideOverlap(*data[id1].constframenode[frame1b], *data[id2].constframenode[frame2b], 1, 1);
						if (q > 10)g += 1;
						ae += q;



						if(q>0)cout << id1 << " " << id2 << " " << frame1_i << "  "<<frame2_i << "  " << q <<"   "<< (*data[id1].constframenode[frame1b]& *data[id2].constframenode[frame2b]).area()<<endl;
					}
					++frame1j;
					if (frame1j == data[id1].v[frame1_i])
					{
						++frame1_i;
						if (frame1_i == data[id1].paragraph_length)
							break;
						frame1j = 0;
					}
					++frame2j;
					if (frame2j == data[id2].v[frame2_i])
					{
						++frame2_i;
						if (frame2_i == data[id2].paragraph_length)
							break;
						frame2j = 0;
					}

					frame1b = frame1_i*lengthunit + data[id1].v_translate[data[id1].v[frame1_i]][frame1j];
					frame2b = frame2_i*lengthunit + data[id2].v_translate[data[id2].v[frame2_i]][frame2j];
				}

			}


		}
		cout << g << "---------------------------------" << ae << endl; return ae;
	}
};