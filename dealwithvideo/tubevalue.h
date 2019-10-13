#pragma once
//管道预处理  输出 x，y  跟背景的差值  rgb to int
#include "opencv2/photo.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include "iostream"
#include "string"
#include<fstream>
#include<cmath>
using namespace std;
using namespace cv;
void dealwithtube(int Max_frame,
	string backgroundfile = "C:\\Users\\l1005\\Desktop\\dealwithvideo\\0422\\0.png", 
	string tubeinfotxt= "C:\\Users\\l1005\\Desktop\\dealwithvideo\\0422\\tubeframe.txt", 
	string tubefile= "C:\\Users\\l1005\\Desktop\\dealwithvideo\\0422", 
	string inputfile="C:\\Users\\l1005\\Desktop\\视频摘要\\outputs\\input");
int main() { dealwithtube(10220); }

void dealwithtube(int Max_frame, string backgroundfile, string tubeinfotxt, string tubefile, string inputfile)
{
	fstream t;
	t.open(tubeinfotxt, ios::in);
	fstream store;
	int tubenum;
	t >> tubenum;
	int **tubedata = new int*[tubenum + 1];
	Mat background = imread(backgroundfile, 3);
	Mat **b = new Mat*[Max_frame];
	for (int i = 0; i < Max_frame + 1; ++i)
		b[i] = 0;
	for (int i = 1; i < tubenum + 1; ++i)
	{
		cout << i << endl;
		tubedata[i] = new int[2];
		int g;
		t >> g;
		if (g != i)
			cerr << "dealwithtube_tubevaalue.h_36line" << endl;
		t >> tubedata[i][0] >> tubedata[i][1];
		store.open((tubefile)+"\\" + to_string(i) + "\\" + to_string(i) + "value.txt", ios::out | ios::trunc);
		
		for (int j = tubedata[i][0]; j < tubedata[i][1] + 1; ++j)
		{
			Mat a = imread((tubefile)+"\\" + to_string(i) + "\\" + to_string(j) + ".png", 0);
			if (!b[j])
				b[j] = new Mat(imread(inputfile + "\\" + to_string(j) + ".png", 3));
			double aa = 0;
			for (int k = 0; k < a.rows; k++)
				for (int p = 0; p < a.cols; p++)
				{
					if (a.at<uchar>(k, p) == 255)
					{
						aa += pow(b[j]->at<Vec3b>(k, p)[0] - background.at<Vec3b>(k, p)[0], 2);
						aa += (pow(b[j]->at<Vec3b>(k, p)[1] - background.at<Vec3b>(k, p)[1], 2) + pow(b[j]->at<Vec3b>(k, p)[2] - background.at<Vec3b>(k, p)[2], 2));					
						//int rgb = (b[j]->at<Vec3b>(k, p)[2] << 16) | (b[j]->at<Vec3b>(k, p)[1] << 8) | b[j]->at<Vec3b>(k, p)[0];	
					}
				}
			int t = aa;
			store << j << " " << t << endl;
		}
		store.close();
	}
}