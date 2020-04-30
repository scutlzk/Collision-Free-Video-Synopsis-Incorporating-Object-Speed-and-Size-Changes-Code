#pragma once
#include<iostream>
#include<string>
#include<fstream>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include"static_const.h"
using namespace std;
using namespace cv;

class Tube
{
public:
	static int **v_translate;//����ٶ�ת����
	int id;//tube id
	int ts, te;//��ժҪ��Ƶ�е���ʼ֡�ͽ���֡
	int src_ts, src_te;//��ԭ��Ƶ�е���ʼ֡�ͽ���֡
	int *v;//�ٶ� int��ʾ
	double *constframevalue;//tubeÿһ֡�뱳���Ĳ�ֵ  ��tubuvalue.h�ļ�����
	Rect ** constframenode;//tubeÿһ֡�ľ���
	int length;//tube����
	int paragraph_length;//tube�Ķγ���

	double *ratio_v;//�ٶ� double��ʾ
	double *ratio_size;//�ߴ�
	int *min_size;//ÿһ�ε���С����������������

	static void v_translating(int v_tranlong = 4 * lengthunit+1) {
		v_translate = new int*[v_tranlong];
		for (int i = 0; i < v_tranlong; i++)
		{
			v_translate[i] = new int[v_tranlong];
		}
		for (int i = 1; i < v_tranlong; i++)
		{
			for (int j = 0; j < i; j++)
			{
				v_translate[i][j] = j*(double(lengthunit) / i);
			}
		}
	}

	bool initialization(int id, int src_ts, int src_te)
	{
		v_translating();
		this->id = id;
		this->src_ts = src_ts;
		this->src_te = src_te;
		this->length = src_te - src_ts + 1;
		this->paragraph_length = length / lengthunit;
		if (length%lengthunit != 0)paragraph_length += 1;

		min_size = new int[paragraph_length];
		v = new int[paragraph_length];
		ratio_v = new double[paragraph_length];
		ratio_size = new double[paragraph_length];

		for (int i = 0; i < paragraph_length; ++i)
		{
			v[i] = lengthunit;
			ratio_v[i] = 1;
			ratio_size[i] = 1;
			min_size[i] = INT_MAX;
		}

		ts = -1;
		te = -1;

		constframevalue = nullptr;
		constframenode = nullptr;
		return 1;
	}

	Tube()
	{
		this->id = 0;
		this->src_ts = 0;
		this->src_te = 0;
		this->length = 0;

		v = 0;
		ts = 0;
		te = 0;
		constframevalue = nullptr;
		constframenode = nullptr;
		paragraph_length = 0;
		ratio_v = nullptr;
		ratio_size = nullptr;
	}
	inline double get_ratio_v(int v) 
	{
		return double(lengthunit) / v;
	}

	void read_constframevalue(string mark)//��ȡ constframevalue constframevalue��Ԥ����õģ�ÿ��tube�ļ����ڵ�value.txt
	{
		constframevalue = new double[length+lengthunit*3];
		for (int i = 0; i < length + lengthunit * 3; ++i)
			constframevalue[i] = 0;
		fstream f;
		f.open(mark + "\\" + to_string(id) + "\\" + to_string(id) + "value.txt", ios::in);
		if (!f) 
			cerr << "read_constframevalue�ļ��򿪴���" << endl;

		int g;
		for (int i = src_ts, j = 0; i < src_te + 1; i++, j++)
		{
			f >> g;
			if (g != i)
				cerr << "read_constframevalue value.txtֵ������" << endl;
			f >> constframevalue[j];
		}
		f.close();
	}

	void read_constframenode(string mark)//��ȡ constframenode constframenode��Ԥ����õģ�ÿ��tube�ļ����ڵ�node.txt
	{
		constframenode = new Rect*[length];
		for (int i = 0; i < length; ++i)
		{
			constframenode[i] = 0;
		}
		int node[4];
		fstream f;
		int g;
		f.open(mark + "\\" + to_string(id) + "\\" + to_string(id) + "node.txt", ios::in);
		if (!f) 
			cerr << "read_constframenode�ļ��򿪴���" << endl;

		for (int i = src_ts, j = 0; i < src_te + 1; i++, j++)
		{
			f >> g; 
			if (g != i)
				cerr << "read_constframenode node.txtֵ������" << endl;
			f >> node[0] >> node[1] >> node[2] >> node[3];
			constframenode[j] = new Rect(node[0], node[1], node[2], node[3]);
		}
		f.close();
	}

	double activityenemgy(int videob/*ժҪ��Ƶ��ʼ֡*/, int videoe/*ժҪ��Ƶ����֡*/)//����activity���� 
	{
		double a = 0;
		int j = ts;
		int mark = 0;
		int this_paragraph_length;
		for (int i = 0; i < paragraph_length; i++)
		{
			mark = i*lengthunit;
			this_paragraph_length = lengthunit;
			if (i == (paragraph_length - 1))
			{
				if (!(this_paragraph_length = length%lengthunit))
					this_paragraph_length = lengthunit;
			}
			for (int k = 0; k < v[i]; ++k)
			{
				if (j<videob || j>videoe)
				{
					a += constframevalue[mark + v_translate[v[i]][k]] * ratio_v[i];
				}
				++j;
			}
		}
		return a;
	}
	void compute_ratio_v()
	{
		for (int i = 0; i < paragraph_length; ++i)
			ratio_v[i] = get_ratio_v(v[i]);
	}
	double benemgy()//����smooth����
	{
		double a = 0;
		for (int i = 0; i < paragraph_length; ++i)
		{
			if (ratio_v[i] > 1)
				a += benem2* exp(benem2_a*ratio_v[i]);// *exp(benem2_b / length);
			else
			{
				a += benem2* exp(benem2_a / ratio_v[i]);// *exp(benem2_b / length);
			}
			a += benem3*exp(benem3_a / ratio_size[i]);// *exp(benem3_b / min_size[i]);

		}
		for (int i = 1; i < paragraph_length - 1; ++i)
		{
			a += benem3_c*exp(2 * ratio_v[i] - ratio_v[i - 1] - ratio_v[i + 1]);
			a += benem3_d*exp(2 * ratio_size[i] - ratio_size[i - 1] - ratio_size[i + 1]);
		}
		return a/ paragraph_length;
	}
	double  benemgy(int tts, int para, int vv, double sizee)//����smooth����
	{
		//�޸�ֵ
		int temp = v[para];
		v[para] = vv;
		double t = ratio_v[para];
		double temp4 = ratio_size[para];
		ratio_size[para] = sizee;
		double a = 0;
		ratio_v[para] = get_ratio_v(vv);
		a += benemgy();
		//��ԭ
		v[para] = temp;
		ratio_size[para] = temp4;
		ratio_v[para] = t;
		return a;
	}

	double
		activityenemgy(int videob, int videoe, int tts, int para, int vv)////����activity���� 
	{
		//�޸�ֵ
		int temp = v[para];
		v[para] = vv;
		double temp1 = ratio_v[para];
		ratio_v[para] = get_ratio_v(vv);
		int temp2 = ts;
		ts = tts;

		double a=activityenemgy(videob, videoe);
		//��ԭ
		ratio_v[para] = temp1;
		v[para] = temp;
		ts = temp2;
		return a;
	}


	int tubematbelong()//tube�ľ����ٶȸı��Ŀ��ܳ���
	{
		int tubelong = 0;
		for (int i = 0; i < paragraph_length-1; ++i)
		{
			tubelong += v[i];
		}
		if (!(length%lengthunit))
		{
			tubelong += v[paragraph_length - 1];
		}
		else
			tubelong += (length%lengthunit*(v[paragraph_length - 1] / double(lengthunit)) + 1.0);
		return tubelong;
	}

	int tubematbelong(int para, int vv)
	{
		//�ı�ֵ
		int q = v[para];
		v[para] = vv;

		int t = tubematbelong();
		//��ԭ
		v[para] = q;
		return t;
	}
};
int** Tube::v_translate = nullptr;