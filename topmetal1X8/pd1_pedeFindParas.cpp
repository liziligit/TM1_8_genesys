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


int pd1_pedeFindParas(char *fn, char *pedeFile, double lowLimitPixel, double upLimitPixel, double lowLimitFrame,  double upLimitFrame){

	
    pd1Debug pd1d;
	pd1d.read(fn);

 //   pd1d.noiseRange(pedeFile, lowLimitPixel, upLimitPixel, lowLimitFrame, upLimitFrame);
    pd1d.noiseRangeUpdate(pedeFile, lowLimitPixel, upLimitPixel, lowLimitFrame, upLimitFrame);
	return 0;
}

