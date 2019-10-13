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

	tubemanagers manager(timelong/*生成的摘要视频最长长度 帧*/, file/*预处理后的管道文件夹*/, 15/*原视频的帧率*/);//初始化

	computemcmc(manager, 0.3/*速度下限*/, 2.001/*速度上限*/, 0.5/*面积下限*/);//进行mcmc，得到最优结果

	double  end = GetTickCount();
	cout << "Count:" << end - start << endl;

	productvideo(manager, file + "\\outputs\\input\\"/*原视频的帧图*/);//生成视频

	end = GetTickCount();
	cout << "GetTickCount:" << end - start << endl;
}
