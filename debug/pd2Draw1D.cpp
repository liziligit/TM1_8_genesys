#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TROOT.h>
#include <TMath.h>
#include <TParameter.h>
#include <TGraph.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TSystem.h>
#include "rootlib.h"
// #include <io.h>

int pd2Draw1D(int cha=0,int clockDiv=3){//查看的通道号


const int NN=1;

placData_2 pd2;
char fn[200];

sprintf(fn,"../data/out.pd2");
pd2.read((char*)fn);


//pd1.print();

int len=0;



/////////////////////////设置canvas
plot pt;
pt.setupCanvas(2,2); //设置canvas上图片的行数，列数





////////////////////////////////////看数据随时间的变化
pd2.getChaDat(cha);
int l=pd2.nChaDat-10;



pt.setup1D(1,1,l,0,l,"pixData");






pt.loadData2Hist1D(pd2.chaDat,0,l); //将一维数组 “排列” 的柱状图上



pt.drawOffset=0;
pt.draw1D();

//if(j==3){
//pt.h1[0]->GetYaxis()->SetRangeUser(500,2000);
//pt.draw1D();
//}
//else
//pt.draw1D((char*)"same");

////////////////////////////////////看噪声大小

double min=pd2.chaDat[0];
double max=pd2.chaDat[0];
for(int i=0;i<l;i++){
if(pd2.chaDat[i]>max)max=pd2.chaDat[i];
if(pd2.chaDat[i]<min)min=pd2.chaDat[i];

}



pt.setup1D(1,1,100,2*min-max,2*max-min,"pixData");

pt.fillData2Hist1D(pd2.chaDat,0,pd2.nChaDat); //将一维数组 “排列” 的柱状图上

pt.drawOffset=1;
pt.draw1D();



////////////////////////////////////噪声功率谱
int n=l;

TVirtualFFT *fftr2c=TVirtualFFT::FFT(1, &n, "R2C ES K"); 
double *data=new double[n];
double *ma=new double[n];
double *x=new double[n];

for(int i=0;i<n;i++){
data[i]=pd2.chaDat[i];
}

	fftr2c->SetPoints(data);
	fftr2c->Transform();
	double re, im;
double sinRms=sqrt(2.0)/2.0;
double dt=pow(2.0,clockDiv+1)*(1/25.0)*1e-6;
	for(int i=0;i<n;i++){
      fftr2c->GetPointComplex(i,re,im);

	  ma[i]=sqrt(re*re+im*im);
	  
	  if(i==0){ma[i]=ma[i]/n;}
	  else{ma[i]=2*ma[i]/n;}

           
	  ma[i]=sqrt((ma[i]*ma[i])*n*dt)*sinRms;
	  x[i]=i/(n*dt);
	}


   ma[0]=0;
   TGraph *gr = new TGraph(n/2,x,ma);
   gr->SetName("noise power spectrum");
pt.c->cd(3);
   gr->Draw();	
cout<<"n="<<n<<endl;





pt.c->SaveAs("a.jpg");


cout<<"end"<<endl;

	return 0;
}
