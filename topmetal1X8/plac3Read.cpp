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


// int plac3Read(char* name,char *dataType = "pd3", const int dataID = 0){
int plac3Read(char *dataType = "pd3", const int dataID = 0){
	char input[100];
	// sprintf(input,"zou.bin");
	sprintf(input,"../topmetal1x8/tauPolNData/placData3_tauPolN0.pd3");

	placData_3 pd3;	

	pd3.read(input);
	cout<<pd3.dataSize()<<endl;
	cout<<pd3.adcCha()<<endl;
	cout<<pd3.frameTime()<<endl;
	cout<<pd3.nPix()<<endl;
	cout<<pd3.nFrame()<<endl;
	pd3.getChaDat(0);
	draw drawObj;
	drawObj.createMem(pd3.nFrame());
	float x[pd3.nFrame()];
	float y[pd3.nFrame()];
	int pixel=100;
	char name[100];
	sprintf(name,"pixel");
	for(int i=0;i<pd3.nFrame();i++){
		drawObj.y[i]=pd3.chaDat[pixel*pd3.nFrame()+i];
		drawObj.x[i]=(float)i;
		y[i]=pd3.chaDat[pixel*pd3.nFrame()+i];
		if(y[i]>400)cout<<y[i]<<endl;
		x[i]=(float)i;
	}
	
	// for(int ich=0;ich<pd3.adcCha();ich++){
		// for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
			// for(int iframe=0;iframe<pd3.nFrame();iframe++){
				// y[iframe]=pd3.chaDat[ich*pd3.nPix()*pd3.nFrame()+ipixel*pd3.nFrame()+iframe];
				// x[iframe]=(float)iframe;
			// }	
		// }	
	// }	
	
	// drawObj.drawGraph(name);
	drawObj.drawGraph(pd3.nFrame(),x,y,name,false);
	return 0;
}
