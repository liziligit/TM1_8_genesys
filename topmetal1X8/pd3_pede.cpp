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


int pd3_pede(char *fn){

	
    pd3Debug pd3d;
	pd3d.read(fn);

   pd3d.drawChip(0,10);
   // pd3d.drawFrame(4,385);
   // pd3d.drawFrameUpdate(0,100);
   // for(int i=300;i<400;i++){
   // pd3d.drawFrame(4,frame);
    // int a;
// scanf("%d",&a);	
   // }
	return 0;
}
