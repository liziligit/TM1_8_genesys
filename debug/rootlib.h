#ifndef _ROOTLIB_
#define _ROOTLIB_
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TROOT.h>
#include <TMath.h>
#include <TParameter.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TString.h>
#include <TVirtualFFT.h>
#include <TRandom.h>
#include <TStyle.h>
#include <TPad.h>
#include <TPaveStats.h>
#include<TApplication.h>

#include <TSystem.h>
#include <TF1.h>

#include "../daq/clib.h"
#include "kbhit.h"

using namespace std;


class pd1Pede{
	public:


    float meanPed[41472];
    float rmsPed[41472];

    void setup(string a){
		
const int nPixelsOnChip = 5184;		
    std::ifstream inPD(a.c_str());
    if(!inPD.is_open()) {
        cout << "No pedestal table found!" << endl;
        return ;
    }
    
    int iChipT = 0, iPixelT = 0, iCounter=0;
    float pedestalT = 0., noiseT = 0.;
    while(!inPD.eof() && iCounter<41472) {
        inPD >> iChipT >> iPixelT >> pedestalT >> noiseT;
        meanPed[iChipT*nPixelsOnChip+iPixelT] = pedestalT;
        rmsPed[iChipT*nPixelsOnChip+iPixelT] = noiseT;
        iCounter++;
    }		
	//cout<<iCounter<<" items is read"<<endl;
	}


    void subPede(float *data){   //subtract pede from data for all channel
    for(int i=0;i<41472;i++){
    data[i]-=meanPed[i];
    }
    }	
	
    void subPede(float *data,int cha){  //subtract pede from data for one channel
    for(int i=0;i<5184;i++){
    data[i]-=meanPed[i+cha*5184];
    }
    }	

    float getMean(int cha,int pix){
    return meanPed[pix+cha*5184];
    }

    float getRms(int cha,int pix){
    return rmsPed[pix+cha*5184];
    }

    void reset(){
    for(int i=0;i<41472;i++){
    meanPed[i]=0;
    rmsPed[i]=0;
    }
   }


    void write(char * out){
ofstream of;
of.open(out);

for(int k=0;k<8;k++){
for(int i=0;i<5184;i++){

/////////////////output data format
of<<k<<" "<<i<<" "<<meanPed[i+k*5184]<<" "<<rmsPed[i+k*5184]<<endl;
//cout<<k<<" "<<i<<" "<<meanPed[i+k*5184]<<" "<<rmsPed[i+k*5184]<<endl;

}

}

of.close();
   }




};


class plot{
public:

plot(){
drawOffset=0;
h1=NULL;
h2=NULL;
}

void draw2D(float *data, int xbin,int ybin){

	for(int i=0;i<nCol*nRow;i++){
        loadData2Hist2D(data+i*xbin*ybin,i);
	}

	
	for(int i=0;i<nCol*nRow;i++){
		c->cd(i+1+drawOffset);

		h2[i]->GetZaxis()->SetRangeUser(min2d,max2d);
                h2[i]->Draw("colz");
		h2[i]->SetStats(0);
		



	}

		c->Modified();
                c->Update();
      

} 

void draw2DOrder1X8(float *data, int xbin,int ybin){


        loadData2Hist2D(data+0*xbin*ybin,4);  /// this channel is bad for my FPGA
        loadData2Hist2D(data+1*xbin*ybin,5);
        loadData2Hist2D(data+2*xbin*ybin,6);
        loadData2Hist2D(data+3*xbin*ybin,7);  //this is the last 2D, with alpha source
        loadData2Hist2D(data+4*xbin*ybin,3);
        loadData2Hist2D(data+5*xbin*ybin,2);
        loadData2Hist2D(data+6*xbin*ybin,1);
        loadData2Hist2D(data+7*xbin*ybin,0);

	
	for(int i=0;i<nCol*nRow;i++){
		c->cd(i+1+drawOffset);

		h2[i]->GetZaxis()->SetRangeUser(min2d,max2d);
                h2[i]->Draw("colz");
		h2[i]->SetStats(0);
		



	}

		c->Modified();
                c->Update();
      

} 


void draw2D(){

	
	for(int i=0;i<nCol*nRow;i++){
		c->cd(i+1+drawOffset);

		h2[i]->GetZaxis()->SetRangeUser(min2d,max2d);
                h2[i]->Draw("colz");
		h2[i]->SetStats(0);
		



	}
		c->Modified();
                c->Update();
} 

void draw1D(char*opt=NULL){

	
	for(int i=0;i<nCol*nRow;i++){
		c->cd(i+1+drawOffset);

                h1[i]->Draw(opt);
		//h1[i]->SetStats(0);
		



	}
		c->Modified();
                c->Update();
} 



void loadData2Hist2D(float *data,int a){

	for(int i=0;i<xbin;i++){
	for(int j=0;j<ybin;j++){
		h2[a]->SetBinContent(j+1,i+1,data[j*ybin+i]);

	}}

}

void loadData2Hist2D(float *data,int a,int b){


for(int k=a;k<b;k++){
	for(int i=0;i<xbin;i++){
	for(int j=0;j<ybin;j++){
		h2[k]->SetBinContent(j+1,i+1,data[j*ybin+i+(k-a)*xbin*ybin]);

	}}
}

}



void loadData2Hist1D(float *data,int a,int dataSize){

	for(int i=0;i<dataSize;i++){

		h1[a]->SetBinContent(i+1,data[i]);

	}

}


void loadData2Hist1D(float *data,int a,int dataSize,int offset){

	for(int i=0;i<dataSize;i++){

		h1[a]->SetBinContent(i+1+offset,data[i]);

	}

}

void fillData2Hist1D(float *data,int a,int dataSize){

	for(int i=0;i<dataSize;i++){

		h1[a]->Fill(data[i]);

	}

}



void load2DHistTo1DHist(){

for(int i=0;i<nCol*nRow;i++){
for(int j=0;j<h2[i]->GetNbinsX();j++){
for(int k=0;k<h2[i]->GetNbinsY();k++){
h1[i]->Fill(h2[i]->GetBinContent(j+1,k+1));
}
}
}
}


void setup2DMinMax(double min_,double max_){
min2d=min_;
max2d=max_;
}


void setupCanvas(int nCol_,int nRow_,int xrange=1600,int yrange=800){

TApplication* theApp = new TApplication("App", 0, 0);
nColCanvas=nCol_;
nRowCanvas=nRow_;
c=new TCanvas("c","c",xrange,yrange);
c->Connect("Closed()", "TApplication", theApp, "Terminate()");
c->Divide(nColCanvas,nRowCanvas);

}

void setup2D(int nCol_,int nRow_, int xbin_,int ybin_, string hn){

nCol=nCol_;
nRow=nRow_;
xbin=xbin_;
ybin=ybin_;
min2d=-10;
max2d=8;

//if(h2!=NULL) vh2.push_back(h2);

string histName;
        h2=new TH2D*[nCol*nRow];
	for(int i=0;i<nCol*nRow;i++){
        histName = hn; histName += i;
        h2[i] = new TH2D(histName.c_str(), histName.c_str(), xbin, 0, xbin, ybin, 0, ybin);
	}

}

void setup1D(int nCol_,int nRow_, int xbin_,double min1d_,double max1d_, string hn){

nCol=nCol_;
nRow=nRow_;
xbin=xbin_;

min1d=min1d_;
max1d=max1d_;

//if(h1!=NULL) vh1.push_back(h1);

string histName;
        h1=new TH1D*[nCol*nRow];
	for(int i=0;i<nCol*nRow;i++){
        histName = hn; histName += i;
        h1[i] = new TH1D(histName.c_str(), histName.c_str(), xbin, min1d, max1d);
	}

}


void delete2D(){


	for(int i=0;i<nCol*nRow;i++){
        delete h2[i];
	}
delete [] h2;
h2=NULL;
}

void delete1D(){


	for(int i=0;i<nCol*nRow;i++){
        delete h1[i];
	}
delete [] h1;
h1=NULL;
}


TH2D **h2;
TH1D **h1;
TCanvas *c;

//vector <TH1D **> vh1;
//vector <TH2D **> vh2;


int nColCanvas;
int nRowCanvas;

int nCol;
int nRow;
int xbin;
int ybin;

int drawOffset;
double max2d;
double min2d;

double max1d;
double min1d; 

};



#endif
