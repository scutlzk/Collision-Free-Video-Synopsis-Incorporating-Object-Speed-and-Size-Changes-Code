//by scut lzk 
#include <windows.h> 
#include"mcmc.h"
#include"tubemanager.h"
#include<iostream>
#include<string>
#include<fstream>
#include <opencv2\highgui\highgui.hpp>
#include"productvideo.h"
#include"static_const.h"
int main() {
	double start = GetTickCount();
	string file = "Z:\\video4";

	tubemanagers manager(timelong/*���ɵ�ժҪ��Ƶ����� ֡*/, file/*Ԥ�����Ĺܵ��ļ���*/, 15/*ԭ��Ƶ��֡��*/);//��ʼ��

	computemcmc(manager, 0.3/*�ٶ�����*/, 2.001/*�ٶ�����*/, 0.5/*�������*/);//����mcmc���õ����Ž��

	double  end = GetTickCount();
	cout << "Count:" << end - start << endl;

	productvideo(manager, file + "\\outputs\\input\\"/*ԭ��Ƶ��֡ͼ*/);//������Ƶ

	end = GetTickCount();
	cout << "GetTickCount:" << end - start << endl;
}
