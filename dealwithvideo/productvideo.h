#pragma once
#include"tubemanager.h"
#include "opencv2/photo.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"  
#include "iostream"  
#include "string"  
#include<Windows.h>
#include<ctime>
#include<fstream>
#include"static_const.h"
using namespace std;
using namespace cv;
void tubepossion(int id, const tubemanagers &manager, int bfe, string tubefilmname, string  markfilmname, Mat **video, string backgroundfilmname, double *v, int paragraph_length);//每多少帧抽一帧  0：不抽 /1：补帧;
void png_to_avi(Mat **video, string backgroundfilmname, int fps = 30, int isColor = 1, string videoname = "test_yiqi");
void possion(Mat *dst, double ratio, int x, int y, string tubefilename, string markfilename);


int productvideo(const tubemanagers &manager, string srcfile = "C:\\outputs\\input\\")//生成视频
{
	Mat **video = new Mat*[frameall];
	for (int i = 0; i < frameall; i++)
		video[i] = 0;
	for (int i = 1; i < manager.ide + 1; ++i)//把每个管道处理到摘要视频中的对应帧
	{
		cout << manager.data[i].src_ts << "*****" << manager.data[i].src_te << manager.best_result[i][0] << "  " << manager.best_result[i][1] << endl;
		tubepossion(i, manager, manager.best_result[i][0],
			srcfile,
			manager.markfile + "\\" + to_string(i) + "\\",
			video,
			manager.markfile + "\\0.png", manager.best_result[i], manager.data[i].paragraph_length);
	}
	png_to_avi(video, manager.markfile + "\\0.png",25, 1, manager.time + ".avi");
	return 0;
}

void possion(Mat *dst, double ratio, int x, int y, string tubefilename, string markfilename)//泊松图像编辑，把管道缝合进背景
{
	Mat src = imread(tubefilename, 3);
	Mat mark = imread(markfilename, 0);
	resize(src, src, Size(), ratio, ratio);
	resize(mark, mark, Size(), ratio, ratio);
	Rect roi_rect = cv::Rect(x*(1 - ratio), y*(1 - ratio), src.cols, src.rows);
	src.copyTo((*dst)(roi_rect), mark);
}

Rect rectCenterScale(Rect rect, double t)
{
	rect.x = rect.x + 0.5*rect.width - 0.5*rect.width*t;
	rect.y = rect.y + 0.5*rect.height - 0.5*rect.height*t;
	rect.width *= t;
	rect.height *= t;
	return (rect);
}

void tubepossion(int id, const tubemanagers &manager, int bfe, string tubefilmname, string  markfilmname, Mat **video, string backgroundfilmname, double *v, int paragraph_length)//生成图片
{
	int i = 0;
	for (int ii = 0; ii < manager.data[id].paragraph_length; ii++)
	{
		for (int j = 0; j < manager.data[id].v[ii]; ++j)
		{
			i = ii*lengthunit + manager.data[id].v_translate[manager.data[id].v[ii]][j];
			if (i < 0)i =0;
			if (i >= manager.data[id].length)return;
			if (video[bfe] == 0)
			{
				video[bfe] = new Mat(imread(backgroundfilmname, 3));
			}
			possion(video[bfe],
				manager.best_result[id][ii+1 + manager.data[id].paragraph_length],
				manager.data[id].constframenode[i]->x + 0.5*manager.data[id].constframenode[i ]->width,
				manager.data[id].constframenode[i ]->y + 0.5*manager.data[id].constframenode[i ]->height,
				tubefilmname + to_string(i+ manager.data[id].src_ts) + ".png",//
				markfilmname + to_string(i+manager.data[id].src_ts) + ".png");

			rectangle(*video[bfe], rectCenterScale(*manager.data[id].constframenode[i ], manager.best_result[id][ii +1+ manager.data[id].paragraph_length])
				, Scalar(0, 0, 255));

			int times = (i + manager.data[id].src_ts) / manager.fps;
			Point textp = Point(manager.data[id].constframenode[i]->x + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])* 0.5*manager.data[id].constframenode[i]->width,
				manager.data[id].constframenode[i]->y + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])*0.5*manager.data[id].constframenode[i]->height);
			putText(*video[bfe], // 待绘制的图像
				to_string(times / 3600) + ":" + to_string((times % 3600) / 60) + ":" + to_string(times % 60),
				textp, // 文本框的左下角
				cv::FONT_HERSHEY_PLAIN, // 字体 (如cv::FONT_HERSHEY_PLAIN)
				1, // 尺寸因子，值越大文字越大
				cv::Scalar(0, 0, 0)

			);

			if (manager.data[id].v[ii] > lengthunit)
			{
				Point root_points[1][4];
				root_points[0][0] = Point(manager.data[id].constframenode[i]->x + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])* 0.5*manager.data[id].constframenode[i]->width,
					manager.data[id].constframenode[i]->y + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])*0.5*manager.data[id].constframenode[i]->height - 11);
				root_points[0][1] = Point(manager.data[id].constframenode[i]->x + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])* 0.5*manager.data[id].constframenode[i]->width+ (manager.data[id].v[ii]/double(lengthunit)-1)*10,
					manager.data[id].constframenode[i]->y + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])*0.5*manager.data[id].constframenode[i]->height - 11);
				root_points[0][2] = Point(manager.data[id].constframenode[i]->x + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])* 0.5*manager.data[id].constframenode[i]->width + (manager.data[id].v[ii] /double( lengthunit) - 1)*10,
					manager.data[id].constframenode[i]->y + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])*0.5*manager.data[id].constframenode[i]->height -15);
				root_points[0][3] = Point(manager.data[id].constframenode[i]->x + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])* 0.5*manager.data[id].constframenode[i]->width,
					manager.data[id].constframenode[i]->y + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])*0.5*manager.data[id].constframenode[i]->height - 15 );


				const Point* ppt[1] = { root_points[0] };
				int npt[] = { 4 };
				fillPoly(*video[bfe], ppt, npt, 1, Scalar(236,76,0));

			}
			else
			{
				if (manager.data[id].v[ii] == lengthunit);
				else {
					Point root_points[1][4];
					root_points[0][0] = Point(manager.data[id].constframenode[i]->x + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])* 0.5*manager.data[id].constframenode[i]->width,
						manager.data[id].constframenode[i]->y + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])*0.5*manager.data[id].constframenode[i]->height - 11);
					root_points[0][1] = Point(manager.data[id].constframenode[i]->x + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])* 0.5*manager.data[id].constframenode[i]->width + (lengthunit / double(manager.data[id].v[ii])-1) * 10,
						manager.data[id].constframenode[i]->y + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])*0.5*manager.data[id].constframenode[i]->height - 11);
					root_points[0][2] = Point(manager.data[id].constframenode[i]->x + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])* 0.5*manager.data[id].constframenode[i]->width + (lengthunit / double(manager.data[id].v[ii])-1) * 10,
						manager.data[id].constframenode[i]->y + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])*0.5*manager.data[id].constframenode[i]->height - 15);
					root_points[0][3] = Point(manager.data[id].constframenode[i]->x + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])* 0.5*manager.data[id].constframenode[i]->width,
						manager.data[id].constframenode[i]->y + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])*0.5*manager.data[id].constframenode[i]->height - 15);

					const Point* ppt[1] = { root_points[0] };
					int npt[] = { 4 };
					fillPoly(*video[bfe], ppt, npt, 1, Scalar(0, 255, 255));

				}
			}
			rectangle(*video[bfe], Point(manager.data[id].constframenode[i]->x + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])* 0.5*manager.data[id].constframenode[i]->width,
				manager.data[id].constframenode[i]->y + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])*0.5*manager.data[id].constframenode[i]->height - 11),
				Point(manager.data[id].constframenode[i]->x + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])* 0.5*manager.data[id].constframenode[i]->width + 27,
					manager.data[id].constframenode[i]->y + (1 - manager.best_result[id][ii + 1 + manager.data[id].paragraph_length])*0.5*manager.data[id].constframenode[i]->height - 15)
				, Scalar(0, 0, 255));
			++bfe;
		}
	}
}
void png_to_avi(Mat **video, string backgroundfilmname, int fps, int isColor, string videoname)//把序列帧弄成视频
{
	Mat d = imread(backgroundfilmname, 3);
	VideoWriter writer(videoname, VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, Size(d.cols, d.rows), isColor);//('D', 'I', 'V', 'X')
	int k = 0;
	for (int i = 0; i < frameall; ++i)
	{
		if (!video[i])
		{
			video[i] = new Mat(imread(backgroundfilmname, 3));
		}
		d = *video[i];
		if (++k > timelong)
			break;
		writer.write(d);
	}
	Mat g = imread(backgroundfilmname, 3);
	writer.write(g);
}

