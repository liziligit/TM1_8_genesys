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


int pd3_pedeFindParas(char *fn, double lowLimitPixel, double upLimitPixel, double lowLimitFrame,  double upLimitFrame){

	
    pd3Debug pd3d;
	pd3d.read(fn);

    pd3d.noiseRange(lowLimitPixel, upLimitPixel, lowLimitFrame, upLimitFrame);
//    pd3d.noiseRangeUpdate(lowLimitPixel, upLimitPixel, lowLimitFrame, upLimitFrame);
    // pd3d.displayInterface(ref, maxPixelSignal);
   // pd3d.drawFrameUpdate(0,10);

   // for(int i=300;i<400;i++){
   // pd3d.drawFrame(4,frame);
    // int a;
// scanf("%d",&a);	
   // }
	return 0;
}
