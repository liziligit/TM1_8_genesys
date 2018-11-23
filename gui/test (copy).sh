#ifndef _CLIB_
#define _CLIB_

#define DEF_CH 16

// #include "lib.h"

#include "drawPGM.h"


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>
#include<time.h>

 using namespace std;

//写文件的函数，mode=“wb”为写二进制文件
//fn为文件名 wb为数据缓冲区，wbz为写入的字节数
void write_file(char*fn,char* wb,int wbz,char *mode){

		FILE *p;
		p=fopen(fn,mode);
		fwrite(wb,wbz,1,p);	
        fclose(p);		

}	

//加载文件的类
class file_loder{
public:
file_loder(){//构造函数 初始化变量
	fz=0;
	buf=NULL;
}
~file_loder(){
if(buf!=NULL) delete []buf; //析造函数  清空缓冲区
buf=NULL;
}
//获得文件大小 以byte计数
static int file_size(char* fn){FILE *p =fopen(fn,"rb"); if(p==NULL) {printf("FILE %s Not exist",fn);return -1;} fseek(p,0L,SEEK_END);int fz=ftell(p);fclose(p);	return fz;}
//将文件存入缓冲区 
//fn为文件名 pd为缓冲区指针 length为大小 以byte计数
static int file_load(char* fn,char* pd, int length){FILE *fp=fopen(fn,"rb");if(fp==NULL){printf("FILE %s Is Not Open",fn);return 0;}fread(pd,1,length,fp);fclose(fp);	return 1;}

//获得文件创建时间
static long file_time(char* fn){
    FILE * fp;
    int fd;
    struct stat buf;
    fp=fopen(fn,"rb"); //C.zip in current directory, I use it as a test
	if(fp==NULL) return 0;
    fd=fileno(fp);
    fstat(fd, &buf);
    long modify_time=buf.st_mtime; //latest modification time (seconds passed from 01/01/00:00:00 1970 UTC)
    fclose(fp);
    return modify_time;
}


int fz;
char* buf;
void load(char *fn){
	fz=file_size(fn);
	if(buf!=NULL) delete []buf;
	buf=new char[fz];
	file_load(fn,buf,fz);
	
}

//写文件
void write(char*fn,char *mode){

write_file(fn,buf,fz,mode);

}	
//写文件  将string里的字符串写进文件
void write(char *fn,string s,char *mode){

write_file(fn,(char*)s.data(),s.length(),mode);

}
	
};



//ddrData 类处理在1x8系统中从fpga直接获得的数据
//目前只应用于 beam monitor系统，已过时

class ddrData{
	public:
	char *ip;  //原始数据
	char right[32];
	unsigned short v[2][8];
	int pixAddr;
	int paddr;
	unsigned int *ui;
	int start;
	int offset;
	int error;
	int checkAddr;
	ddrData(){
		error=0;
		checkAddr=0;
	}
	
//从fpga来的是192位的数据，将192位的adc数据，包含12位8个通道，2次采样，的数据
//转换成16个短整形，存在 unsigned short v[2][8]; 之中 
	int fill(){
		int ret=1;
		for(int i=0;i<32;i++){
			right[i]=ip[31-i];
		}
		for(int j=0;j<2;j++){
		for(int i=0;i<8;i++){
			start=((i+j*8)*12)/8;
			offset=(i+j*8)*12-start*8;
			ui=(unsigned int *)(right+start);
			v[j][i]=0x00000fff & (ui[0]>>offset);
		}
		}
		
		
		ui=(unsigned int *)(right+24);
		pixAddr=ui[0] & 0x1fff;
		ui=(unsigned int *)(right+28);
		// cout<<"xxxxxxxxxxx"<<hex<<ui[0]<<dec<<endl;		
		
		if(ui[0]==0xf0f0a5a5){ 
		// cout<<"return 1"<<endl;		
		}else {
			printf("error line \n");			
			for(int i=0; i<32; i++) {
				printf(" %02x ", (unsigned char)ip[i]);
			}
			printf("\n");
		
		return 0;}
		
		if(checkAddr==1){
		if(pixAddr>paddr){
			if(pixAddr-paddr!=1){
				error++;
				if(error>2)
				cout<<"adj add "<<paddr<<" "<<pixAddr<<endl;
				ret=2;
			}
			
		}else{
			if(paddr!=5183||pixAddr!=0){
				error++;
				if(error>2)				
				cout<<"adj add "<<paddr<<" "<<pixAddr<<endl;
				ret=2;
			}			
		}
		
	    paddr=pixAddr;
		}
		
		return ret;

	}
	void print(){
		printf(" \n");
		
		for(int j=0;j<2;j++){
		for(int i=0;i<8;i++){
			printf(" %03x ", v[j][i]);
		}
		printf("\n");		
		}		

        printAddr();
     
		
	}
	
	void printAddr(char cmd[]=(char*)"hex"){
		if(strcmp(cmd,"hex")==0)
		printf("pixAddr= %04x \n", pixAddr);	
		if(strcmp(cmd,"dec")==0)
		printf("pixAddr= %d \n", pixAddr);	
	
	}	
	
};


//ddrDataContinueMode 类处理在1x8系统中从fpga直接获得的数据
//目前只应用于 beam monitor系统

class ddrDataContinueMode{
	public:
	char *ip;
	char right[12];
	unsigned short v[1][8];
	unsigned int *ui;
	int start;
	int offset;
	ddrDataContinueMode(){

	}
	
//从fpga来的是96位的数据，将96位的adc数据，包含12位8个通道，1次采样，的数据
//转换成8个短整形，存在 unsigned short v[1][8]; 之中 
	int fill(){
		int ret=1;
        
        for(int j=0;j<3;j++){
            
		for(int i=0;i<4;i++){           
            right[4*j+i]=ip[4*j+3-i];    
		}
        }
		
		// for(int i=0;i<12;i++){
			// right[i]=ip[11-i];
		// }		
		
		
		
		for(int j=0;j<1;j++){
		for(int i=0;i<8;i++){
			start=((i+j*8)*12)/8;
			offset=(i+j*8)*12-start*8;
			ui=(unsigned int *)(right+start);
			v[j][i]=0x00000fff & (ui[0]>>offset);
		}
		}						
		return ret;

	}
	void print(){
		// printf(" \n");
		
		for(int j=0;j<1;j++){
		for(int i=0;i<8;i++){
			 // printf("%03x\t", v[j][i]);
			printf("%d\t", v[j][i]);
		}
		printf("\n");		
		}		
     
		
	}
	
	
};

//存储数据类型的父类 
class placData{
	public:
	char header[1024];  //预留的1024 byte的数据头
	char *p;  //数据指针
	int memz;   //缓冲区大侠
	int nByte;  //数据大小
	int debug;  //调试开关
	placData(){
		debug=0;
		p=NULL;
		memz=0;
	}
	~placData(){
		if(memz!=0){
			delete []p;
			p=NULL;
			memz=0;
		}
	}	
	//产生缓冲区
	int createMem(int s){
		if(memz<s){
			delete []p;
			p=new char[s];
			memz=s;
		}	
        return s;		
	}
	//写文件
	void write(char *fn){
		write_file(fn,header,1024,(char*)"wb");
		write_file(fn,p,nByte,(char*)"ab");
		
	};
       //读文件
	void read(char *fn){
		int fz=file_loder::file_size(fn);
		fz-=1024;
		if(fz<=0){cout<<fz<<" is negtive"<<endl; exit(0);}
        createMem(fz);
		nByte=fz;
		FILE *fp;
		fp=fopen(fn,"rb");
		if(fp==NULL){cout<<fn<<" is not opened"<<endl; exit(0);}
		
		fread(header,1024,1,fp);
		fread(p,nByte,1,fp);
		fclose(fp);
		
	}
        //返回数据格式类型
	int dataType(){return ((int   *)header)[0];};	
	
};

//pd1数据类
class placData_1 : public placData {
	public:

//pd1数据头的内容	
	int dataSize(){return ((int   *)header+1*4)[0];};	
	int adcBit(){return ((int   *)header+2*4)[0];};
	int adcCha(){return ((int   *)header+3*4)[0];};
	int samPerPix(){return ((int   *)header+4*4)[0];};	
	float dt(){return ((float   *)header+5*4)[0];};	
	int nPix(){return ((int   *)header+6*4)[0];};
	int nFrame(){return ((int   *)header+7*4)[0];};		
	int year(){return ((int   *)header+8*4)[0];};
	int month(){return ((int   *)header+9*4)[0];};
	int day(){return ((int   *)header+10*4)[0];};
	int hour(){return ((int   *)header+11*4)[0];};
	int min(){return ((int   *)header+12*4)[0];};
	int sec(){return ((int   *)header+13*4)[0];};
	
	
	float *chaDat;//存储一个通道数据的缓冲区
	float *pixDat; //存储一个像素数据的缓冲区
	float *frameDat; //存储一个像素数据的缓冲区

	int nPixDat;   //一个像素上的采样数
	int nChaDat;   //一个通道内的采样数
	int nFrameDat;   //sample in frameDat
	
	
	placData_1(){
		
    ((int *)header+0*4)[0]=1;	//构造函数， id=1
	nPixDat=0;
	nChaDat=0;
	nFrameDat=0;
	}
	
	~placData_1(){
    if(nPixDat!=0){
	delete[]pixDat; 
	nPixDat=0;		
	}
	
    if(nChaDat!=0){
	delete[]chaDat; 
	nChaDat=0;		
	}	

    if(nFrameDat!=0){
	delete[]frameDat; 
	nFrameDat=0;		
	}	

	}	

///////////////////////////get frame data
	int getFrame(int i,int cha){//i 0~nFrame()-1 cha 0~8   
		if(i>=nFrame()) return -1;
		if(i<0) return -1;
		if(cha>=adcCha()) return -1;
		if(cha<0) return -1;

		int frameLen=dataSize()/adcCha()/nFrame()/(sizeof(unsigned short));
		//cout<<"data size is "<<dataSize()<<" adc channel "<<adcCha()<<" chaLen "<<chaLen<<endl;
		// int chaLen1=nFrame()*nPix()*samPerPix();
		// cout<<"--------------------"<<endl;
		// cout<<"frameLen="<<frameLen<<endl;
		// cout<<chaLen1<<endl;

		if(nFrameDat==0){
		frameDat=new float[frameLen];
		nFrameDat=frameLen;
		}else if(nFrameDat<frameLen){
			 delete[]frameDat;
		frameDat=new float[frameLen];
		nFrameDat=frameLen;				 
		}
		unsigned short *ps = (unsigned short *)p;
		for(int j=0;j<frameLen;j++)
		{
			frameDat[j]=(float)(ps[nPix()*adcCha()*i+adcCha()*j+cha]);
		}
		
		
		return 0;
	}

///////////////////////////get all frame data
	int getFrame(int i){//i 0~nFrame()-1
		if(i>=nFrame()) return -1;
		if(i<0) return -1;

		int frameLen=dataSize()/nFrame()/(sizeof(unsigned short));
		//cout<<"data size is "<<dataSize()<<" adc channel "<<adcCha()<<" chaLen "<<chaLen<<endl;
		// int chaLen1=nFrame()*nPix()*samPerPix();
		// cout<<"--------------------"<<endl;
		// cout<<"frameLen="<<frameLen<<endl;
		// cout<<chaLen1<<endl;

		if(nFrameDat==0){
		frameDat=new float[frameLen];
		nFrameDat=frameLen;
		}else if(nFrameDat<frameLen){
			 delete[]frameDat;
		frameDat=new float[frameLen];
		nFrameDat=frameLen;				 
		}
                frameLen/=adcCha();

		unsigned short *ps = (unsigned short *)p;

		for(int k=0;k<adcCha();k++)
		for(int j=0;j<frameLen;j++)
		{{
			frameDat[k*nPix()+j]=(float)(ps[nPix()*adcCha()*i+adcCha()*j+k]);
		}}
		
		
		return 0;
	}


///////////////////////////single channel data
	int getChaDat(int i){//i 0~7 channel    获取指定通道内的数据
		if(i>=adcCha()) return -1;
		if(i<0) return -1;
		int chaLen=dataSize()/adcCha()/(sizeof(unsigned short));
		//cout<<"data size is "<<dataSize()<<" adc channel "<<adcCha()<<" chaLen "<<chaLen<<endl;
		// int chaLen1=nFrame()*nPix()*samPerPix();
		// cout<<"--------------------"<<endl;
		// cout<<chaLen<<endl;
		// cout<<chaLen1<<endl;

		if(nChaDat==0){
		chaDat=new float[chaLen];
		nChaDat=chaLen;
		}else if(nChaDat<chaLen){
			 delete[]chaDat;
		chaDat=new float[chaLen];
		nChaDat=chaLen;				 
		}
		unsigned short *ps = (unsigned short *)p;
		for(int j=0;j<chaLen;j++)
		{
			chaDat[j]=(float)(ps[adcCha()*j+i]);
		}
		
		
		return 0;
	}
///////////////////////////single pixel data of a channel	
	int getPixDat(int i,int pixel){//i 0~7 channel   获取指定像素上的数据  pixel为像素号  0-5183
		if(i>=adcCha()) return -1;
		if(i<0) return -1;
		if(pixel>=nPix()) return -1;
		if(pixel<0) return -1;
		int nPixLen=dataSize()/adcCha()/(sizeof(unsigned short))/nPix();
		// cout<<"ichip "<<i<<" ipixel "<<pixel<<endl;
		// int nPixLen1=nFrame()*samPerPix();
		// cout<<"---------------------------"<<endl;
		// cout<<nPixLen<<endl;
		// cout<<nPixLen1<<endl;

		if(nPixDat==0){
		pixDat=new float[nPixLen];
		nPixDat=nPixLen;
		}else if(nPixDat<nPixLen){
			 delete[]pixDat;
		pixDat=new float[nPixLen];
		nPixDat=nPixLen;				 
		}
		unsigned short *ps = (unsigned short *)p;
		unsigned short tmpValue=0;
		//loop frame sample adc channel and data
			for(int frame=0;frame<nFrame();frame++) 
			{
				for(int sam=0;sam<samPerPix();sam++)
				{
					tmpValue=(ps[nPix()*samPerPix()*frame*adcCha()+samPerPix()*pixel*adcCha()+sam*adcCha()+i]);
					// if(i==0&&pixel==4)cout<<(unsigned short)tmpValue<<endl;
					pixDat[sam+frame*samPerPix()]=(float)tmpValue;
				}
			}
		
		return 0;
	}

//////////////////////////////////////// write back pixDat to p;
	int setPixDat(int i,int pixel){//i 0~7 channel   
		if(nPixDat!=nFrame()) {
                   cout<<"setPixDat   nPixDat!=nFrame()"<<endl;
                   exit(0);
                }

		unsigned short *ps = (unsigned short *)p;
		unsigned short tmpValue=0;
		//loop frame sample adc channel and data
			for(int frame=0;frame<nFrame();frame++) 
			{
				for(int sam=0;sam<samPerPix();sam++)
				{
					tmpValue=(unsigned short)(pixDat[sam+frame*samPerPix()]);
ps[nPix()*samPerPix()*frame*adcCha()+samPerPix()*pixel*adcCha()+sam*adcCha()+i]=tmpValue;
				}
			}
		
		return 0;
	}




/////////////////////////////把从fpga获取的数据转成pd1格式，已不用
	void setupFromGenesys1X8(char* pIn,int sampleBytes,int shift=49/2){
		
		float dt=40e-9;
		int adcBit=12;
		int adcCha=8;
		
    ddrData da;
	int nw=sampleBytes/32;
	
	int offset=20;
	int firstZero;
	int firstZeroDown;
	int secondZero;
	
	int nzero=0;
	da.paddr=0;
	
	for(int i=offset;i<nw;i++){
		da.ip=pIn+i*32;
		da.fill();
		//cout<<da.pixAddr<<endl;

		if(da.pixAddr==0&&da.paddr!=0){
			if(nzero==0){
				firstZero=i;
				nzero++;
				da.paddr=da.pixAddr;
				continue;
			}
			
			if(nzero==1){
				secondZero=i;
				nzero++;
				break;
			}			
		}
		if(nzero==1&&da.pixAddr!=0&&da.paddr==0){
			firstZeroDown=i;
		}
		
		da.paddr=da.pixAddr;
	}
	if(debug>0){
	cout<<"firstZero="<<firstZero<<endl;
	cout<<"secondZero="<<secondZero<<endl;
	cout<<"firstZeroDown="<<firstZeroDown<<endl;
	}
	firstZero+=shift;
	secondZero+=shift;
	firstZeroDown+=shift;
	
	int nFrame=(sampleBytes/32-firstZero)/(secondZero-firstZero);
	int samPerPix=firstZeroDown-firstZero;
	int nPix=(secondZero-firstZero)/samPerPix;
	if(nPix!=5184) {
		cout<<"nPix!=5184"<<endl;
		return ;
	}
	if(debug>0){	
	cout<<"nFrame="<<nFrame<<endl;
	cout<<"samPerPix="<<samPerPix<<endl;
	cout<<"nPix="<<nPix<<endl;	
	}

    int j=0;
	unsigned short *ps;
	for(int i=firstZero;i<firstZero+nFrame*samPerPix*nPix;i++){
		da.ip=pIn+i*32;
		da.fill();
		// cout<<"j="<<j<<endl;
		ps=(unsigned short *)(p+j*32);
		
		for(int si=0;si<2;si++){
		for(int sj=0;sj<8;sj++){
			ps[si*8+sj]=da.v[si][sj];
		}}
		j++;
		//cout<<da.pixAddr<<endl;
	}	
	
	int dataSize=j*32;
	nByte=dataSize;
	if(debug>0){	
	cout<<"nByte="<<nByte<<endl;
	}
    samPerPix*=2; //32 byte has 2 sample in firmware
	
    ((int   *)header+1*4)[0]=dataSize;		
    ((int   *)header+2*4)[0]=adcBit;		
    ((int   *)header+3*4)[0]=adcCha;		
    ((int   *)header+4*4)[0]=samPerPix;		
    ((float *)header+5*4)[0]=dt;		
    ((int   *)header+6*4)[0]=nPix;		
    ((int   *)header+7*4)[0]=nFrame;	
     

	}

////////////////////////// get real time data
    void setupFromGenesys1X8RealTimeMode(char* pIn,int nframe){
		float dt=40e-9;  //ADC采样时间间隔
		int adcBit=12;
                int bytePerSample=2;
		int adcCha=8;
               p=pIn;
        int samPerPix = 1;
        int nPix=5184;
        int dataSize=nframe*nPix*adcCha*samPerPix*bytePerSample;

if(debug>1)cout<<"nframe-===-=== "<<nframe<<endl;
if(debug>1)cout<<"dataSize-===-=== "<<dataSize<<endl;

        nByte=dataSize;  // data size in byte
    ((int   *)header+1*4)[0]=dataSize;		
    ((int   *)header+2*4)[0]=adcBit;		
    ((int   *)header+3*4)[0]=adcCha;		
    ((int   *)header+4*4)[0]=samPerPix;		
    ((float *)header+5*4)[0]=dt;		
    ((int   *)header+6*4)[0]=nPix;		
    ((int   *)header+7*4)[0]=nframe;	
  
// write local time to file header
      time_t now_time;
	struct tm *tp;

	time(&now_time);
	tp = localtime(&now_time);
	
    ((int   *)header+8*4)[0]=1900+tp->tm_year;
    ((int   *)header+9*4)[0]=1+tp->tm_mon;	
    ((int   *)header+10*4)[0]=tp->tm_mday;
    ((int   *)header+11*4)[0]=tp->tm_hour;
    ((int   *)header+12*4)[0]=tp->tm_min;
    ((int   *)header+13*4)[0]=tp->tm_sec;	
  //  printf("%d/%d/%d %d:%d:%d\n", 1900+tp->tm_year, 1+tp->tm_mon, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);

 
 
}

    
////////////////////////////把从fpga获取的数据转成pd1格式 beam monitor上使用
    void setupFromGenesys1X8ContinueMode(char* pIn,int sampleBytes){
		
		float dt=40e-9;  //ADC采样时间间隔
		int adcBit=12;
		int adcCha=8;
        
        unsigned int *pi=(unsigned int *)pIn;
        int firstZero; // 找第一帧的第零个像素
        int secondZero;  // 找第二帧的第零个像素
        
        

        int nzero = 0;
		//int flag = 0;
        int flag = 1;
        for(int i=0; i<(sampleBytes)/4;i++){ // 一个word有四个byte所以除以4
            if((pi[i]& 0x00ffffff)==0x55aaaa){
				if (flag == 0){
					flag++;
					continue;
				}
				if (flag ==1){
					if(nzero==0){
						firstZero = i+4;   // 第一个值是上一个像素的最后一个adc，所以这里+4，具体如下原因
									//2855aaaa
									//4ec 4bf 4bc 4e6 4d2 4d6 4bc 4d3 
									// 以上是四个字
					        // printf("%x\t%x\t%x\n", pi[i+4], pi[i+5], pi[i+6]);
						nzero++;
						continue;
					}   
					if(nzero==1){
						secondZero= i+4;  // 第一个值是上一个像素的最后一个adc，所以这里+4
						// printf("%x\t%x\t%x\n", pi[i+4], pi[i+5], pi[i+6]);                    
						break;
					}					
				}

            }

        } 
        
        int samPerPix = 1;
        int nPix=(secondZero-firstZero-1)/3;
        int nFrame = (sampleBytes/4-firstZero)/(nPix*3+1);
        if(nPix!=5184) {
		cout<<"nPix!=5184"<<endl;
		return ;
        }
      if(debug>0){
    	cout<<"firstZero="<<firstZero<<endl;
        cout<<"secondZero="<<secondZero<<endl;
        cout<<"nFrame="<<nFrame<<endl;
        cout<<"samPerPix="<<samPerPix<<endl;
        cout<<"nPix="<<nPix<<endl;	
        }
    unsigned short *ps;
    ddrDataContinueMode da;
    
    int j = 0;
    int count = 0;
    for(int i=firstZero; i<firstZero+nFrame*(nPix*3+1); i++) {   //  每 32bit 做一次
	 if((pi[i]& 0x00ffffff)==0x55aaaa){
		 // cout << "i = " << i <<endl;

        continue;		 
	 }

		da.ip=(char*)(pi+i);
        da.fill();   // 把数值 填入  da.v 里面
		// da.print();
        ps=(unsigned short *)(p+j*16);   // 格式整理好了的数据存在 p 中，  ps是一个指着 p存储空间的 位置
        
		for(int si=0;si<1;si++){   // 每个通道一个采样，所以是 1
		for(int sj=0;sj<8;sj++){   // 8个通道，所以是 8
			ps[si*8+sj]=da.v[si][sj];
	/*	        if(count<80)			{
			printf("%d:%x  ",count, ps[si*8+sj]);
			if(count%8==0)cout<<endl;}
			count++;	*/	
			
		}
			
		}

		j++;        
        i=i+2;		
    }
		int *pixDat = new int[j];
		unsigned short *pixelData = (unsigned short *)p;
		for(int i=0;i<j;i++)
		{
			pixDat[i]=(int)(pixelData[i]);
			        if(i<80)			{
			printf("%d:%d  ",i, pixDat[i]);
			if(i%8==7)cout<<endl;}

		}


	
	int dataSize=j*16;
	nByte=dataSize;
	if(debug>0){
	cout<<"nByte="<<nByte<<endl;
    }
	
    ((int   *)header+1*4)[0]=dataSize;		
    ((int   *)header+2*4)[0]=adcBit;		
    ((int   *)header+3*4)[0]=adcCha;		
    ((int   *)header+4*4)[0]=samPerPix;		
    ((float *)header+5*4)[0]=dt;		
    ((int   *)header+6*4)[0]=nPix;		
    ((int   *)header+7*4)[0]=nFrame;	
     
// write local time to file header
      time_t now_time;
	struct tm *p;

	time(&now_time);
	p = localtime(&now_time);
	
    ((int   *)header+8*4)[0]=1900+p->tm_year;
    ((int   *)header+9*4)[0]=1+p->tm_mon;	
    ((int   *)header+10*4)[0]=p->tm_mday;
    ((int   *)header+11*4)[0]=p->tm_hour;
    ((int   *)header+12*4)[0]=p->tm_min;
    ((int   *)header+13*4)[0]=p->tm_sec;	
  //  printf("%d/%d/%d %d:%d:%d\n", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
     }
	
	
void print(){
cout<<"dataSize="<<dataSize()<<endl;
cout<<"adcBit="<<adcBit()<<endl;
cout<<"adcCha="<<adcCha()<<endl;
cout<<"samPerPix="<<samPerPix()<<endl;
cout<<"dt="<<dt()<<endl;
cout<<"nPix="<<nPix()<<endl;
cout<<"nFrame="<<nFrame()<<endl;

}

	
	
};

//pd2数据格式类

class placData_2 : public placData {
	public:
//pd2数据头	
	int dataSize(){return ((int   *)header+1*4)[0];};	
	int adcBit(){return ((int   *)header+2*4)[0];};
	int adcCha(){return ((int   *)header+3*4)[0];};
	int pixAddr(){return ((int   *)header+4*4)[0];};	
	float dt(){return ((float   *)header+5*4)[0];};	

	// int nPix(){return 5184;};
	// int samPerPix(){return 2;};
	// int nFrame(){return dataSize()/adcCha()/nPix()/samPerPix()/(sizeof(unsigned short));};
	
	float *chaDat; //在一个通道上的数据缓冲区
	// unsigned short *pixDat;
	// int nPixDat;
	int nChaDat;
	// placData_2(){
		
    // ((int *)header+0*4)[0]=2;	
	
	// }
	

	
	placData_2(){
		((int *)header+0*4)[0]=2;		
		// nPixDat=0;
		nChaDat=0;
		/// construct data as placData_1
	}
	
	~placData_2(){
			/// construct data as placData_1
		
    // if(nPixDat!=0){
	// delete[]pixDat; 
	// nPixDat=0;		
	// }
	
    if(nChaDat!=0){
	delete[]chaDat; 
	nChaDat=0;		
	}	
	}	
///////////////////////////single channel data
	int getChaDat(int i){//i 0~7 channel  //获取一个通道上的数据
		if(i>=adcCha()) return -1;
		if(i<0) return -1;
		int chaLen=dataSize()/adcCha()/sizeof(unsigned short);
		// int chaLen1=nFrame()*nPix()*samPerPix();
		// cout<<"--------------------"<<endl;
		// cout<<chaLen<<endl;
		// cout<<chaLen1<<endl;

		if(nChaDat==0){
		chaDat=new float[chaLen];
		nChaDat=chaLen;
		}else if(nChaDat<chaLen){
			 delete[]chaDat;
		chaDat=new float[chaLen];
		nChaDat=chaLen;				 
		}
		unsigned short *ps = (unsigned short *)p;
		for(int j=0;j<chaLen;j++)
		{
			chaDat[j]=(float)(ps[adcCha()*j+i]);
		}
		
		
		return 0;
	}

	
//把fpga读到的数据转化成pd2格式，带开始转化的参数start
	void setupFromGenesys1X8(char* pIn,int sampleBytes,int start,int addr){
		
		float dt=40e-9;
		int adcBit=12;
		int adcCha=8;
		
 	int nw=sampleBytes/4;
	


	unsigned int *pi=(unsigned int *)pIn;
    unsigned short *ps;
    ddrDataContinueMode da;

int j=0;

    for(int i=0; i<nw; i++) {   //  每 32bit 做一次


	da.ip=(char*)(pi+i+start);
        da.fill();   // 把数值 填入  da.v 里面
		// da.print();
        ps=(unsigned short *)(p+j*(DEF_CH));   // 格式整理好了的数据存在 p 中，  ps是一个指着 p存储空间的 位置
        
		for(int si=0;si<1;si++){   // 每个通道一个采样，所以是 1
		for(int sj=0;sj<8;sj++){   // 8个通道，所以是 8
			ps[si*8+sj]=da.v[si][sj];
			
		}}
		j++;        
        i=i+2;		
    }	
	


cout<<"nw===="<<nw<<endl;
cout<<"j===="<<j<<endl;

	int dataSize=j*(DEF_CH);
	nByte=dataSize;
	if(debug>0){	
	cout<<"nByte="<<nByte<<endl;
	}

    ((int   *)header+1*4)[0]=dataSize;		
    ((int   *)header+2*4)[0]=adcBit;		
    ((int   *)header+3*4)[0]=adcCha;		
    ((int   *)header+4*4)[0]=addr;		
    ((float *)header+5*4)[0]=dt;		

     

	}
	
	
};

//pd3数据格式类
class placData_3 : public placData {
	public:
//pd3数据头	
	int dataSize(){return ((int   *)header+1*4)[0];};	
	int adcCha(){return ((int   *)header+2*4)[0];};
	float frameTime(){return ((float   *)header+3*4)[0];};	
	int nPix(){return ((int   *)header+4*4)[0];};	
	int nFrame(){return ((int   *)header+5*4)[0];};
	
    void setupHeader(int _dataSize,int _adcCha,float _frameTime,int _nPix,int _nFrame){
		cout<<_dataSize<<endl;
		cout<<_adcCha<<endl;
		cout<<_frameTime<<endl;
		cout<<_nPix<<endl;
		cout<<_nFrame<<endl;
		((int   *)header+1*4)[0]=_dataSize;
		((int   *)header+2*4)[0]=_adcCha;
		((int *)header+3*4)[0]=_frameTime;
		cout<<_frameTime<<endl;
		((int   *)header+4*4)[0]=_nPix;
		((int   *)header+5*4)[0]=_nFrame;
		cout<<" setupHeader end"<<_dataSize<<endl;
		
	}
	
	float *chaDat;

	int nChaDat;
	
	placData_3(){
		((int *)header+0*4)[0]=3;		
		nChaDat=0;
	}
	
	~placData_3(){
	

	}	
///////////////////////////single channel data
// channel pixel 0~frame data, 1 frame data   获取一个通道上的数据
	int getChaDat(int i){//i 0~7 channel
		if(i>=adcCha()) return -1;
		if(i<0) return -1;
		int chaLen=dataSize()/adcCha()/sizeof(float);
		// int chaLen1=nFrame()*nPix()*samPerPix();
		// cout<<chaLen<<endl;
		// cout<<chaLen1<<endl;
	
		float *ps = (float *)p;
		chaDat=(ps+i*chaLen);
		return 0;
	}

	


	
};


class pd1ToCharge{

public:

pd1ToCharge(){
nPix=0;
}

~pd1ToCharge(){
if(nPix!=0){
for(int i=0;i<nPix;i++){
s[i].clear();
}
delete [] s;
nPix=0;
}
}

void setup(int i){
if(i==nPix) return;
if(nPix!=0){
for(int i=0;i<nPix;i++){
s[i].clear();
}
delete [] s;
}
s=new vector <float> [i];
nPix=i;
}


/////////////////////////////for guarding data, find slope on it vector[0] is the  peak pos   vector[0] is the  tail pos
        void findSlopeInPixData(float *pixDat,int nPixDat,vector <float> &slopeF,float threshold){
        
        

        ////////find peak
        float maxval=pixDat[0];
        int maxpos=0;
        for(int i=0;i<nPixDat;i++){
        if(pixDat[i]>maxval){
        maxval=pixDat[i];
        maxpos=i;
        }
        }
 
        /////////find tail
        int tailpos;
        for(int i=maxpos;i<nPixDat-1;i++){
        if(pixDat[i+1]>pixDat[i]){
        tailpos=i;
        break;
        }
        } 
        //check threshold
        if(pixDat[maxpos]-pixDat[tailpos]<threshold) return;

        //prepare slopeF
        slopeF.clear();
        for(int i=maxpos;i<=tailpos;i++){
        slopeF.push_back(pixDat[i]);
        }

        }



/////////////////////////////find Y value index from slope data
        int findLeftInSlope(vector <float> & slope,unsigned short val){

        int left=0;
        int right=slope.size()-1;
        if(slope.size()<=1) return -1;

        int mid;
        do{
        mid=(left+right)/2;
        if(slope[mid]>val){
        left=mid;
        }else{
        right=mid;
        }

        if(right-left<=1) break;

        }while(1);
        
        return left;
        }
/////////////////////////////calculate charge base on slope
        float getCharge(vector <float> & slope,float *val,float threshold){
        
        if(slope.size()<=4) return 0; //slope is too short, tao is too short, we set charge to zero
        
        if(val[0]-slope[slope.size()-1]<threshold){  ///value is too small, we take it as zero
        if(val[1]-val[0]<threshold){
        return 0;}
        }  

        int left=findLeftInSlope(slope,val[0]);

        if(left<0){  ///slope is too short, tao is too short, we set charge to zero
        return 0;
        }
        
        if(left==slope.size()-1) left--;


        return (slope[left]-slope[left+1])-(val[0]-val[1]);
        }


///////////////////////////////write slope to file
       void write(char *fn){
       ofstream out;
       out.open(fn);
       out<<nPix<<endl;
       for(int i=0;i<nPix;i++){
       out<<s[i].size()<<endl;
       for(int j=0;j<s[i].size();j++){
       out<<s[i][j]<<"  ";
       }
       out<<endl;
       }
       out.close();
       }

///////////////////////////////read slope from file
       void read(char *fn){
       char b[200];
       ifstream in;
       in.open(fn);
       in>>b;
       setup(atoi(b));
       int ss;
       for(int i=0;i<nPix;i++){
       in>>b;
       ss=atoi(b);
       for(int j=0;j<ss;j++){
       in>>b;
       s[i].push_back(atof(b));
       }
       }
       in.close();
       }

/////////////////
vector <float> *s;
int nPix;

};	
	


//样条函数拟合类



class signalExtraction    //定义样条结构体，用于存储一条样条的所有信息  
{ //初始化数据输入  
public:
signalExtraction(){

 MAXNUM=0;	
}

void setup(int n){
	if(MAXNUM!=0)clearMem();
	
	genMem(n);
 MAXNUM=n;		
}

void genMem(int n){
x=new float[n+1];
 y=new float[n+1];
 k1=new float[n+1];
 k2=new float[n+1];
 a3=new float[n];
 a1=new float[n];
 b3=new float[n];
 b1=new float[n];
H=new float[n];     //小区间的步长  
Fi=new float[n];     //中间量  
U=new float[n+1];    //中间量  
A=new float[n+1];    //中间量  
D=new float[n+1];    //中间量  
M=new float[n+1];    //M矩阵  
B=new float[n+1];    //追赶法中间量  
Y=new float[n+1];    //追赶法中间变量  	
}

void clearMem(){
delete []x;
delete []y;
delete []k1;
delete []k2;
delete []a3;
delete []a1;
delete []b3;
delete []b1;
 delete [] H;
 delete [] Fi;
 delete [] U;
 delete [] A;
 delete [] D;
 delete [] M;
 delete [] B;
 delete [] Y; 	
}


~signalExtraction(){
	if(MAXNUM!=0)clearMem();
    MAXNUM=0;
}
int MAXNUM;
 float *x;    //存储样条上的点的x坐标，最多51个点  
 float *y;    //存储样条上的点的y坐标，最多51个点  
 int point_num;   //存储样条上的实际的 点 的个数  
 float begin_k1;     //开始点的一阶导数信息  
 float end_k1;     //终止点的一阶导数信息  
 //float begin_k2;    //开始点的二阶导数信息  
 //float end_k2;     //终止点的二阶导数信息  
 //计算所得的样条函数S(x)  
 float *k1;    //所有点的一阶导数信息  
 float *k2;    //所有点的二阶导数信息  
 //51个点之间有50个段，func[]存储每段的函数系数  
 float *a3;
 float *a1;
 float *b3;
 float *b1;

 float *H;
 float *Fi;
 float *U;
 float *A;
 float *D;
 float *M;
 float *B;
 float *Y;
 
 
 int xbin(float a){
	int i=-1;
	for(i = 0;i < point_num;i++){
		if(x[i]>a&&i==0) return 0;
		if(x[i]<=a&&x[i+1]>a) break;
	}
	if(i>=point_num-1)	return point_num-2;	
	//cout<<"xbin= "<<i<<endl;
	return i;	 
	 
 }
 
 float si(float a){
	 int i=xbin(a);
	 return a3[i] * pow(x[i+1] - a,3)  + a1[i] * (x[i+1] - a) +  
        b3[i] * pow(a - x[i],3) + b1[i] * (a - x[i]) ;
 }
 
 float area_integral(float a,int i){
  // int i=xbin(a);
   
   float integral;
   integral=a3[i]*pow(x[i+1]-a,4)/4+a1[i]*pow(x[i+1]-a,2)/2-b3[i]*pow(a-x[i],4)/4-b1[i]*pow(a-x[i],2)/2;
   return integral;
}
 

 float area(float x1, float x2){
   if(x1>=x2) {
	   cout<<"x1>=x2"<<endl;
	   exit(0);
   }
   int i1=xbin(x1);
   int i2=xbin(x2);
   
   if(i1==i2){
   float  final_area; 
   return  final_area=area_integral(x1,i1)-area_integral(x2,i2);
   }
   double sum=0;
   for(int i=i1;i<=i2;i++){
	   if(i==i1){
		  sum+=area_integral(x1,i)-area_integral(x[i+1],i);
		  continue;
	   }
	   if(i==i2){
		  sum+=area_integral(x[i],i)-area_integral(x2,i);
		  continue;
	   }
        sum+=area_integral(x[i],i)-area_integral(x[i+1],i);
   }
   
   return sum;
}


void set_end(){
	begin_k1=(y[1]-y[0])/(x[1]-x[0]);
	end_k1=(y[point_num-1]-y[point_num-2])/(x[point_num-1]-x[point_num-2]);
} 
 
int spline3()  
{  

 int i = 0;  
 ////////////////////////////////////////计算中间参数  
 if((point_num < 3) || (point_num > MAXNUM + 1))  
 {  
  return -2;       //输入数据点个数太少或太多  
 }  
 for(i = 0;i <= point_num - 2;i++)  
 {          //求H[i]  
  H[i] = x[i+1] - x[i];  
  Fi[i] = (y[i+1] - y[i]) / H[i]; //求F[x(i),x(i+1)]  
 }  
 for(i = 1;i <= point_num - 2;i++)  
 {          //求U[i]和A[i]和D[i]  
  U[i] = H[i-1] / (H[i-1] + H[i]);  
  A[i] = H[i] / (H[i-1] + H[i]);  
  D[i] = 6 * (Fi[i] - Fi[i-1]) / (H[i-1] + H[i]);  
 }  
 //若边界条件为1号条件，则  
 U[i] = 1;  
 A[0] = 1;  
 D[0] = 6 * (Fi[0] - begin_k1) / H[0];  
 D[i] = 6 * (end_k1 - Fi[i-1]) / H[i-1];  
 //若边界条件为2号条件，则  
 //U[i] = 0;  
 //A[0] = 0;  
 //D[0] = 2 * begin_k2;  
 //D[i] = 2 * end_k2;  
 /////////////////////////////////////////追赶法求解M矩阵  
 B[0] = A[0] / 2;  
 for(i = 1;i <= point_num - 2;i++)  
 {  
  B[i] = A[i] / (2 - U[i] * B[i-1]);  
 }  
 Y[0] = D[0] / 2;  
 for(i = 1;i <= point_num - 1;i++)  
 {  
  Y[i] = (D[i] - U[i] * Y[i-1]) / (2 - U[i] * B[i-1]);  
 }  
 M[point_num - 1] = Y[point_num - 1];  
 for(i = point_num - 1;i > 0;i--)  
 {  
  M[i-1] = Y[i-1] - B[i-1] * M[i];  
 }  
 //////////////////////////////////////////计算方程组最终结果  
 for(i = 0;i <= point_num - 2;i++)  
 {  
  a3[i] = M[i] / (6 * H[i]);  
  a1[i] = (y[i] - M[i] * H[i] * H[i] / 6) / H[i];  
  b3[i] = M[i+1] / (6 * H[i]);  
  b1[i] = (y[i+1] - M[i+1] * H[i] * H[i] / 6) /H[i];  
 }

 
 return 1;  
}  
 
 
 
 //分段函数的形式为 Si(x) = a3[i] * {x(i+1) - x}^3  + a1[i] * {x(i+1) - x} +  
 //        b3[i] * {x - x(i)}^3 + b1[i] * {x - x(i)}  
 //xi为x[i]的值，xi_1为x[i+1]的值        
};





//多线程采样函数的传递参数

class processPara{
	public:
		char *p; // buf
		int sampleBytes; // sampleBytes
		char *fn; // filename
		int fileNum; // fileNum
		int fileStart; // fileNum
                int nFrame;

		char *tmpBuf;   //  用来保留上一次采样剩下的不完整的一帧数据，
		int tmpSize;  // 不完整数据的大小 （byte为单位）
		char *tmpBufOrder;   //  用来保留转化成ADC值之后的数据，


};


//多线程采样模式的处理函数



//实时处理函数

void * rtprocess(void *arg){
	processPara *proPar=(processPara*)arg;
        int debug=0;

		float dt=40e-9;  //ADC采样时间间隔
		int adcBit=12;
		int adcCha=8;
        
        unsigned int *pi=(unsigned int *)proPar->p;
        int firstZero; // 找第一帧的第零个像素
        int secondZero;  // 找第二帧的第零个像素
        
        

        int nzero = 0;
		//int flag = 0;
        int flag = 1;
        for(int i=0; i<(proPar->sampleBytes)/4;i++){ // 一个word有四个byte所以除以4
            if((pi[i]& 0x00ffffff)==0x55aaaa){
				if (flag == 0){
					flag++;
					continue;
				}
				if (flag ==1){
					if(nzero==0){
						firstZero = i+4;   // 第一个值是上一个像素的最后一个adc，所以这里+4，具体如下原因
									//2855aaaa
									//4ec 4bf 4bc 4e6 4d2 4d6 4bc 4d3 
									// 以上是四个字
						// printf("%x\t%x\t%x\n", pi[i+4], pi[i+5], pi[i+6]);
						nzero++;
						continue;
					}   
					if(nzero==1){
						secondZero= i+4;  // 第一个值是上一个像素的最后一个adc，所以这里+4
						// printf("%x\t%x\t%x\n", pi[i+4], pi[i+5], pi[i+6]);                    
						break;
					}					
				}

            }

        } 

      
     if(debug>2) cout<<"firstZero+proPar->tmpSize/4=="<<firstZero+proPar->tmpSize/4<<endl;
 
     if(debug>2)  cout<<"firstZero= "<<firstZero<<endl;
     if(debug>2)  cout<<"secondZero= "<<secondZero<<endl;
     if(debug>1)  cout<<"secondZero- firstZero=="<<secondZero- firstZero<<endl;
       
      int framew=(5184*12+4)/4;//frame size in word
     if(debug>2)  cout<<"framew ="<<framew<<endl;




 unsigned short *ps;
    ddrDataContinueMode da;
char *p=proPar->tmpBufOrder;
   int j = 0;
   int k=0;
unsigned long int max_ft=0;
if(proPar->tmpSize!=0){

	pi=(unsigned int *)proPar->tmpBuf; //上次留下的不完整一帧的数据

   	int tr=(proPar->tmpSize/4)%3;
 	if(debug>2)cout<<"tr="<<tr<<endl;
	if(tr!=0){ //从新采集的数据中借数
		for(int i=0;i<(3-tr)*4;i++){
			proPar->tmpBuf[proPar->tmpSize+i]=proPar->p[i];
		}
	}
	
	unsigned long int ft;
	ft=proPar->tmpSize/4;
max_ft=ft;
	if(tr!=0){
		ft=ft+(3-tr);
	}
 
    for(int i=0; i<ft; i++) {   //  每 32bit 做一次
	 if((pi[i]& 0x00ffffff)==0x55aaaa){
		 // cout << "i = " << i <<endl;
         	k++;
        	continue;		 
	 }

		da.ip=(char*)(pi+i);
        da.fill();   // 把数值 填入  da.v 里面
		// da.print();
        ps=(unsigned short *)(p+j*(DEF_CH));   // 格式整理好了的数据存在 p 中，  ps是一个指着 p存储空间的 位置
        
		for(int si=0;si<1;si++){   // 每个通道一个采样，所以是 1
		for(int sj=0;sj<8;sj++){   // 8个通道，所以是 8
			ps[si*8+sj]=da.v[si][sj];
			
		}}
		j++;        
        i=i+2;		
    }
////////转化了上一次采样剩下的数据，只有不完整的一帧，下面转化新采到的数据的开头，共同组成一帧完整的数据


	pi=(unsigned int *)(proPar->p+((3-tr)%3)*4);  //新采集的数据

    for(int i=0; i<firstZero-((3-tr)%3); i++) {   //  每 32bit 做一次
	 if((pi[i]& 0x00ffffff)==0x55aaaa){
		 // cout << "i = " << i <<endl;
       		 k++;
        	continue;		 
	 }

	da.ip=(char*)(pi+i);
        da.fill();   // 把数值 填入  da.v 里面
		// da.print();
        ps=(unsigned short *)(p+j*(DEF_CH));   // 格式整理好了的数据存在 p 中，  ps是一个指着 p存储空间的 位置
        
		for(int si=0;si<1;si++){   // 每个通道一个采样，所以是 1
		for(int sj=0;sj<8;sj++){   // 8个通道，所以是 8
			ps[si*8+sj]=da.v[si][sj];
			
		}}
		j++;        
        i=i+2;		
    }



	if(debug>2) cout<<" ADC sample in first frame ="<<j<<endl;
	if(debug>2)cout<<" token in first frame ="<<k<<endl;

}// 我们转化了第一帧数据
else{


	if(proPar->fileNum-proPar->fileStart!=0){
		firstZero=0;

		if(firstZero!=framew){
			cout<<"firstZero!=framew"<<endl;
			exit(0);
		}

	}
}


//我们开始转化第一帧之后的数据
pi=(unsigned int *)(proPar->p);
      int totw=proPar->sampleBytes/4; //total data in word
      

int frame=(totw-firstZero)/framew;
      if(debug>2)  cout<<"frame ="<<frame<<endl;



    for(int i=0; i<framew*frame; i++) {   //  每 32bit 做一次
	 if((pi[i+firstZero]& 0x00ffffff)==0x55aaaa){
                  //if(i<100000)
		  //cout << "i at = " << i%(framew)<<"  "<<i <<endl;
         k++;
        continue;		 
	 }

		da.ip=(char*)(pi+firstZero+i);
        da.fill();   // 把数值 填入  da.v 里面
		// da.print();
        ps=(unsigned short *)(p+j*(DEF_CH));   // 格式整理好了的数据存在 p 中，  ps是一个指着 p存储空间的 位置
        
		for(int si=0;si<1;si++){   // 每个通道一个采样，所以是 1
		for(int sj=0;sj<8;sj++){   // 8个通道，所以是 8
			ps[si*8+sj]=da.v[si][sj];
			
		}}
		j++;        
        i=i+2;		
    }

//我们转化完所有数据


//把剩余的数据转移到tmpBuf里面

for(int i=0;i<(totw-firstZero-framew*frame)*4;i++){
    proPar->tmpBuf[i]=proPar->p[i+(firstZero+framew*frame)*4];
}

int rest=(totw-firstZero)%framew;


   if(debug>2) cout<<"rest1 ="<<totw-firstZero-framew*frame<<endl;
   if(debug>2) cout<<"rest2 ="<<rest<<endl;
    proPar->tmpSize=rest*4;

if(debug>2)cout<<"total ADC sample ="<<j<<endl;
if(debug>1)cout<<"total token ="<<k<<endl;




/******************* save pd1 data *******************/

placData_1 pd;
pd.debug=debug;

pd.setupFromGenesys1X8RealTimeMode(p,k);

char fnn[200];
sprintf(fnn,"%s%d.pd1",proPar->fn,proPar->fileNum);

pd.write(fnn);



return 0;

}



#endif

