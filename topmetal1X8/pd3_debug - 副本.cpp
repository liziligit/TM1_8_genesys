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


int pd3_debug(char *fn,int frame){

	
    pd3Debug pd3d;
	pd3d.read(fn);

   // pd3d.drawPixel(4,100);
   // pd3d.drawFrame(4,385);
   pd3d.drawFrameUpdate(0,10);
   // for(int i=300;i<400;i++){
   // pd3d.drawFrame(4,frame);
    // int a;
// scanf("%d",&a);	
   // }
	return 0;
}
