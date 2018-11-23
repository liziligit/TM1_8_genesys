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


int pd1FFTDebug(char *inputPede,char *pd1File,char *outFFTPD1,float threshold,char *fftIndex,int flag){
	
    pd1ChaDraw pd1fft;
	
	placData_1 pd1;	
	pd1.read(pd1File);
	pd1fft.debugVerbose=0;
	pd1fft.setup(&pd1,inputPede);
	pd1fft.run(inputPede,fftIndex,outFFTPD1,threshold,flag);
//fft tranform	
	// pd1.getChaDat(0);
	// pd1ChaDraw pd1cd;
	// pd1cd.setup(&pd1,inputPede);
	// pd1cd.subPede(0);
    // pd1cd.supressTexture(0.08,inoutfftFreIndex,0);//threshold fft frequency index file, flag of if create a fft frequency index file
	// pd1cd.draw(10,100);	
	// pd1cd.draw();	
//fft tranform	
	

	return 0;
}
