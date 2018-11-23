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


int pd3_analysis(char *fn, int ich, int iframe, double upLimit, double lowLimit ){

	
    pd3Debug pd3d;
	pd3d.read(fn);

    // pd3d.noiseRange();
//       pd3d.drawRow(ich, iframe);
//	pd3d.fitBeam(ich, iframe);
//   pd3d.fitBeamError(ich);
     pd3d.beamPosResol(ich, upLimit, lowLimit);
//     pd3d.beamPosResolCheck(ich, upLimit, lowLimit);
 //  pd3d.beamPosFit(ich, iframe, upLimit, lowLimit);
//pd3d.beamIntensityResol(ich, upLimit, lowLimit);
//pd3d.beamSize(ich, upLimit, lowLimit);
//pd3d.beamBadPixel(ich, upLimit, lowLimit);
//pd3d.beamPresentation(ich, upLimit, lowLimit);
//pd3d.beamAngleResol(ich,, upLimit, lowLimit);
//     pd3d.beamPara(ich);
//	pd3d.beamPara_correction(ich);
//     pd3d.beamFrame(ich);
//     pd3d.beamFrameAveFit(ich);
 //      pd3d.beamFrameAveFitRota(ich);
//	pd3d.beamParaAcu(ich);
//    pd3d.displayInterfaceUpdate(maxPixelSignal);
   // pd3d.displayInterface_copy(maxPixelSignal);
   // pd3d.drawFrameUpdate(0,10);

   // for(int i=300;i<400;i++){
   // pd3d.drawFrame(4,frame);
    // int a;
// scanf("%d",&a);	
   // }
	return 0;
}
