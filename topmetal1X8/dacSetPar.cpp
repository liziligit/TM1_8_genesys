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


int dacSetPar(int start=0,int step=1,int stop=0,char *tauPath="",char *InName="",char *dacParName=""){
	char outPut[100];
	char Input[100];
	// sprintf(outPut,"dacPar.txt");
	sprintf(outPut,"%s",dacParName);
	sprintf(Input,"%s/%s",tauPath,InName);
	dacParPoint dacParPointObj;
	float lowEdgeTau=5.0;
	float highEdgeTau=100.0;
	dacParPointObj.lowEdgeTau=lowEdgeTau;
	dacParPointObj.highEdgeTau=highEdgeTau;
	cout<<"dacParPointObj.lowEdgeTau "<<dacParPointObj.lowEdgeTau<<endl;
	cout<<"dacParPointObj.highEdgeTau "<<dacParPointObj.highEdgeTau<<endl;
	dacParPointObj.dacSetVreset(Input,start,step,stop,outPut);
	return 0;
}
