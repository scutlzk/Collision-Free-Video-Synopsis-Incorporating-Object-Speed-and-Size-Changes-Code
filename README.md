Collision-Free Video Synopsis Incorporating Object Speed and Size Changes Code

If this project has been helpful to you, consider giving it a star! Your support would make me very happy and encourage me to continue improving it.

1、opencv3.1.1 64bit

video4.7z download url

	https://download.csdn.net/download/li_haoren/11857087
	
	https://download.csdn.net/download/li_haoren/11857092
	
	https://download.csdn.net/download/li_haoren/11857083
	
	or  https://drive.google.com/file/d/1S1Gvpah_gjk913YcKlpJofbCO0z3327V/view?usp=sharing

 	more video and tube https://drive.google.com/drive/folders/10Kmm-iq25r8F-On-pwZ3wQNawD96w6Vk?usp=drive_link
先解压video4.7z， 这个是样例管道。
然后在 main.cpp 里 把file改为这个文件夹目录
运行就能生成摘要视频

video4里有

1-14，14个文件夹分别是 14个管道。

	管道文件夹里有value.txt 这个由tubevalue.h 生成。
	
	管道文件夹里有node.txt 这个是框住管道帧里面的矩形
	
	管道文件夹里还有管道的每一帧图片
	
0.png是视频的背景

tubeframe.txt 是这个视频的管道信息记录了每个管道在原始视频的起始帧和结束帧

outputs 文件夹里是原视频的每一帧帧图，用于生成摘要视频

static_const.cpp里调整各个参数


0.png is the background of the video

tubeframe.txt contain the tube information of this video, record the start frame and end frame of each tube in the original video

outputs folder have picture of each frame of the original video，used to generate synopsis video

static_const.cpp used to adjust various parameters

-------------------------------------
if you have any problem in running this code please contact me by l1005152@gmail.com

please visit http://nieyongwei.net for more information

please cite our paper:


	Yongwei Nie ; Zhenkai Li ; Zhensong Zhang ; Qing Zhanga ; Tiezheng Ma ; Hanqiu Sun  Collision-Free Video Synopsis Incorporating Object Speed and Size Changes
	
	DOI: 10.1109/TIP.2019.2942543
	
	Keywords:  Electron tubes,Optimization,Surveillance,Image coding,Computer science,Sun,Indexes
	
	Author Keywords:   Surveillance video synopsis,collision avoiding,speed/size change,Metropolis sampling
	
	URL: https://ieeexplore.ieee.org/document/8848836
	
	 

