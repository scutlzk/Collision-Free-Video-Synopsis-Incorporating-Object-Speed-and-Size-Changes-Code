#include"static_const.h"
const  int lengthunit = 48;//段长度64
const  int benem2 = 10000;//Ed里速度的
const  int benem3= 10000;//Ed里尺寸的
const  double benem3_a = 2;//σ where σ is empirically set as 2, 
const double benem3_b = 100;//scaling down 给的是 0 10 100
const double benem3_c = 0.01;//Es里面速度的
const double benem3_d = 0.01;//Es里面尺寸的

//E(F, V, R) = Ea(F, V) + ωcEc(F, V, R) + ωtEt(F) + ωsEs(V, R) + ωdEd(V, R),
const  double  actfactor = 1;//ωa
const  double balfactor = 1;//ωs    ωd
const  double colfactor = 100;//ωc
const  double timefactor = 0.01;//ωt

const  int timelong =375;//摘要视频长度
const  int frameall = 12100;//原视频长度+2
const  double benem2_a=2;// σ where σ is empirically set as 2, 





 