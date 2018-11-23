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


int pd1And2(char *dataType = "pd1", const int dataID = 0){
    // char *input = Form("out%d.%s", dataID, dataType);
    char *input = Form("../topmetal1X8/tauPolNData/tauPolN0.pd1");
    char *inputPede = Form("pedNoise_topMetal1X8_%d.txt", dataID);
    char *output = Form("adcSpectrum_ZS_%s_%d.root", dataType, dataID);
	char *inoutfftFreIndex =Form("fftFreIndex%d.txt",dataID);
	char *decayData =Form("decayData%d.txt",dataID);
	char *decayDataSplineVT =Form("decayDataSplineVT%d.txt",dataID);
	char *decayDataSplineTV =Form("decayDataSplineTV%d.txt",dataID);
	char *polPar =Form("polNPar%d.txt",dataID);
	char *polParRev =Form("polNParRev%d.txt",dataID);
    cout << input << "  " << output << endl;
	
    // pd1And2Draw pd1_2;
	
	// pd1_2.pedeFile=inputPede;
	placData_1 pd1;	
	// placData_2 pd2;	
	// if(string(dataType)=="pd1"){
	pd1.read(input);
	// pd1_2.pd1=&pd1;		
	// }else if(string(dataType)=="pd2"){
	// pd2.read(input);
	// pd1_2.pd2=&pd2;				
	// }else{
    // cout << "dataType error" << endl;		
		// return 0;
	// }
	// pd1_2.draw();
	// TFile *rf=new TFile("out.root","recreate");
	
//fft tranform	
	// pd1.getChaDat(0);
	// pd1ChaDraw pd1cd;
	// pd1cd.setup(&pd1,inputPede);
	// pd1cd.subPede(0);
    // pd1cd.supressTexture(0.08,inoutfftFreIndex,0);//threshold fft frequency index file, flag of if create a fft frequency index file
	// pd1cd.draw(10,100);	
	// pd1cd.draw();	
//fft tranform	
	
//fit each pixel data using polN function	
	polNPar pdc;
	pdc.setup(&pd1);
			pdc.drawPixel(0,62);
	// pdc.saveAllPixCurveDat(decayData);
			// pdc.splineAllPixCurveDat(decayData,decayDataSplineVT,decayDataSplineTV);
			// pdc.readSplineFile(decayDataSplineVT,decayDataSplineTV);
	// pdc.readDecayDat(decayData);
	// pdc.fitPolNDat(polPar);
	// pdc.fitPolNDatRev(polParRev);
	// pdc.loadPolNPar(polPar);
	// pdc.loadPolNParReV(polParRev);
	// pdc.GetPixPolNQ(0,0);
// fit each pixel data using polN function	
	
	
	// rf->Write();
	// rf->Close();

	return 0;
}
