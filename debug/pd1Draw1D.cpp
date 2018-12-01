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



int main(int argc, char **argv){
	int pix;
	pix = atol(argv[1]);
	int cha;
	cha = atol(argv[2]);
	int dataSet;
	dataSet = atol(argv[3]);

const int NN=1;

placData_1 pd1[NN];
char fn[200];

sprintf(fn,"../data/out%d.pd1",dataSet);
pd1[0].read((char*)fn);

//pd1[i].getPixDat(3,10);

//pd1.print();

int len=0;
int j=pix;


/////////////////////////设置canvas
plot pt;
pt.setupCanvas(2,2); //设置canvas上图片的行数，列数




////////////////////////////////////看数据随时间的变化
pd1[0].getPixDat(cha,j);
int l=pd1[0].nPixDat;
pt.setup1D(1,1,l,0,l,"pixData");






pt.loadData2Hist1D(pd1[0].pixDat,0,pd1[0].nPixDat); //将一维数组 “排列” 的柱状图上



pt.drawOffset=0;
pt.draw1D();

//if(j==3){
//pt.h1[0]->GetYaxis()->SetRangeUser(500,2000);
//pt.draw1D();
//}
//else
//pt.draw1D((char*)"same");

////////////////////////////////////看噪声大小


double min=pd1[0].pixDat[0];
double max=pd1[0].pixDat[0];
for(int i=0;i<l;i++){
if(pd1[0].pixDat[i]>max)max=pd1[0].pixDat[i];
if(pd1[0].pixDat[i]<min)min=pd1[0].pixDat[i];
}



pt.setup1D(1,1,100,2*min-max,2*max-min,"pixData");

pt.fillData2Hist1D(pd1[0].pixDat,0,pd1[0].nPixDat); //将一维数组 “排列” 的柱状图上

pt.drawOffset=1;
pt.draw1D();



////////////////////////////////////噪声功率谱
int n=pd1[0].nPixDat;

TVirtualFFT *fftr2c=TVirtualFFT::FFT(1, &n, "R2C ES K"); 
double *data=new double[n];
double *ma=new double[n];
double *x=new double[n];

for(int i=0;i<n;i++){
data[i]=pd1[0].pixDat[i];
}

	fftr2c->SetPoints(data);
	fftr2c->Transform();
	double re, im;
double sinRms=sqrt(2.0)/2.0;
double dt=5184*(16.0/25.0)*1e-6;
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





/////////////////////////////////

pt.c->SaveAs("a.jpg");


cout<<"end"<<endl;

	return 0;
}
