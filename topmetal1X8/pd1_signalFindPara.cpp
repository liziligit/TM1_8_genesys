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


int pd1_signalFindPara(char *fn,  char *pedeFile, double upLimitPixel){

	
    pd1Debug pd1d;
	pd1d.read(fn);

    pd1d.maxSignalPixel(pedeFile, upLimitPixel);
	return 0;
}
