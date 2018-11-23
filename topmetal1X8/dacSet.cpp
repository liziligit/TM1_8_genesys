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


int dacSet(int start=0,int step=1,int stop=0,char *tauPath="",char *InName="",char *dacParName=""){
	char outPut[100];
	char Input[100];
	// sprintf(outPut,"dacPar.txt");
	sprintf(outPut,"%s",dacParName);
	sprintf(Input,"%s/%s",tauPath,InName);
	dacPar dacParObj;
	dacParObj.dacSetVreset(Input,start,step,stop,outPut);
	return 0;
}
