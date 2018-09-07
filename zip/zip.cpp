#include <stdio.h>     //定义输入／输出函数
#include <stdlib.h>    //定义杂项函数及内存分配函数
#include <ctime>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
using namespace std;
#define M_PI 3.14159265358979323846
#define DMax 30.0
 
long double gps[3150][2];
int gps_visit[3150] = {0};
 
/*  The code of GeoDistance function:
Input: Two coordination {Latitude1, Longitude1, Latitude2, Longitude2 } (type:double)
Output: Distance(Unit: m) (type:double)
*/
long double Rad(long double d){
        return d * M_PI / 180.0;
}
//经度 longitude        纬度latitude
long double Geodist(long double  lon1, long double lat1, long double lon2, long double lat2){
        long double radLat1 = Rad(lat1);
        long double radLat2 = Rad(lat2);
        long double delta_lon = Rad(lon2 - lon1);
        long double top_1 = cos(radLat2) * sin(delta_lon);
        long double top_2 = cos(radLat1) * sin(radLat2) - sin(radLat1) * cos(radLat2) * cos(delta_lon);
        long double top = sqrt(top_1 * top_1 + top_2 * top_2);
        long double bottom = sin(radLat1) * sin(radLat2) +cos(radLat1) * cos(radLat2) * cos(delta_lon);
        long double delta_sigma = atan2(top, bottom);
        long double distance = delta_sigma * 6378137.0;
        return distance;
}
 
//将2007-10-14-GPS.log文件中的GPS数据提取出来，转换之后另存起来
void init1(){
    ifstream in1;
    ofstream out1;
    in1.open("F:\\研一上\\滴滴算法竞赛\\城市计算\\Task\\Data\\task 1-compression\\2007-10-14-GPS.log");
    out1.open("F:\\研一上\\滴滴算法竞赛\\城市计算\\Task\\Data\\task 1-compression\\GPS.txt");
 
    for(int i=0;i<3150;i++){
        string temp;
        getline(in1, temp);
        string gps_E = temp.substr(20,10);
        string gps_N = temp.substr(33,9);
        out1<<gps_E<<" "<<gps_N<<endl;
    }
    out1.close();
    in1.close();
}
 
void init2(){
    ifstream in2;
    ofstream out2;
    in2.open("F:\\研一上\\滴滴算法竞赛\\城市计算\\Task\\Data\\task 1-compression\\GPS.txt");
    out2.open("F:\\研一上\\滴滴算法竞赛\\城市计算\\Task\\Data\\task 1-compression\\realGPS1.txt");
    for(int i=0;i<3150;i++){
        long double gps_E,gps_N;
        in2>>gps_E>>gps_N;
        gps_E = (gps_E - 11600.0)*1.0/60+116.0;
        gps_N = (gps_N - 3900)*1.0/60+39.0;
        out2 <<setiosflags(ios::fixed)<<setprecision(6)<<gps_E<<" "<<gps_N<<" "<<i+1<< endl;
        gps[i][0] = gps_E;
        gps[i][1] = gps_N;
    }
    out2.close();
    in2.close();
}
 
long double get_d(int point_A,int point_B,int point_C ){
    long double a  = abs( Geodist(gps[point_B][0],gps[point_B][1],gps[point_C][0],gps[point_C][1] )  );
    long double b  = abs( Geodist(gps[point_A][0],gps[point_A][1],gps[point_C][0],gps[point_C][1] )  );
    long double c  = abs( Geodist(gps[point_A][0],gps[point_A][1],gps[point_B][0],gps[point_B][1] )  );
    long double p = (a+b+c)/2.0;
    long double s = sqrtl( abs( p*(p-a)*(p-b)*(p-c)  )  );
    long double d = s*2.0/c;
    return d;
}
 
void dp_gps(int point_start,int point_end){
    if(point_start<point_end){  //递归进行条件
        long double maxDist = 0;  //最大距离
        int mid = 0;              //最大距离对应的下标
        for(int i=point_start+1;i<point_end;i++){
            long double temp = get_d(point_start,point_end,i);
            if(temp>maxDist){
                maxDist = temp;
                mid = i;
            }//求出最大距离及最大距离对应点的下标
        }
        if(maxDist>=DMax){
            gps_visit[mid] = 1; //记录当前点加入
            //将原来的线段以当前点为中心拆成两段，分别进行递归处理
            dp_gps(point_start,mid);
            dp_gps(mid,point_end);
        }
    }
}
 
int main(){
    int count = 0;  //记录输出点的个数
    long double Mean_distance_error;  //平均距离误差
    long double Compression_rate;     //压缩率
    init1();
    init2();
    gps_visit[0] = 1;
    gps_visit[3149] = 1;
    dp_gps(0,3149);
 
    ofstream out3;
    out3.open("F:\\研一上\\滴滴算法竞赛\\城市计算\\Task\\Data\\task 1-compression\\pointID.txt");
    for(int i=0;i<3150;i++ ){
        if(gps_visit[i]==1){
            out3<<i+1<<endl;
            count++;
        }
    }
    out3.close();
 
    long double sum_notVisit_d = 0;
    int start = 0,end;
    for(int i=0;i<3150;){
        if(start == 3149) break;     //如果开始点是尾点，那就结束
        for(int j=start+1;j<3150;j++){   //找出下一个压缩节点
            if(gps_visit[j]==1){
                end = j;
                break;
            }
        }
        for(int k=start+1;k<end;k++ ){
            if(gps_visit[k]==0){
                sum_notVisit_d+= get_d( start,end,k );
            }
        }
        start = end;
    }
    Mean_distance_error = sum_notVisit_d/3150.0;
    Compression_rate = count/3150.0;
 
    cout<<count<<endl;  //输出压缩后点的个数
    cout<<setiosflags(ios::fixed)<<setprecision(6)<<Mean_distance_error <<endl;
    cout<<setiosflags(ios::fixed)<<setprecision(4)<<Compression_rate*100<<"%"<<endl;
 
    system("pause");
    return 0;
}