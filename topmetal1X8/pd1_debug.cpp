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

int pd1_debug(char *fn, char *pede, int ich, int ipixel){

    // pd1And2Draw pd1_2;
	// placData_1 pd1;	
	// pd1.read(fn);
	// pd1_2.pd1=&pd1;
	// pd1_2.pedeFile=pede;
    // pd1_2.draw();
	// return 0;
	
//	TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "ADC map per chip", 1600, 200);
//	ca_adcMap->Divide(8, 1,0.01,0.);
	// char fileName[200];
	// char fileName1[200];
	pd1Debug pd1d;
	pd1d.read(fn);
	pd1d.drawPixelDebug(ich, ipixel);
	// if(i==0){pd1d.setupHisto();}
	//pd1d.drawPixel(ich,ipixel, pede);
	// pd1d.drawPolyFit(0,100);// fit guard ring data
	// pd1d.drawMeanRMS(0);
	// pd1d.drawFrame(0,4);
	// pd1d.drawFrame(5);
	// pd1d.drawFrameUpdate(0,450);
	// pd1d.drawFrameUpdate(0,2000,pede,ca_adcMap);
	//pd1d.drawFrameUpdate2D(0,100,pede,ca_adcMap);
	//pd1d.drawAllFrameUpdate2D(pede,ca_adcMap);

	//delete ca_adcMap;

	pd1d.readHeaders();


      return 0;
}
