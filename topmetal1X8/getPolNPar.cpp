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


int getPolNPar(char *path,char *InName,char *OutName,char *dataType = "pd1", const int dataID = 0){
	char Input[100];
	sprintf(Input,"%s/%s%d.%s",path,InName,dataID,dataType);
	char decayData[100];
	sprintf(decayData,"%s/decayData%d.txt",path,dataID);
	char polPar[100];
	sprintf(polPar,"%s/polNPar%d.txt",path,dataID);
	
	char polParRev[100];
	sprintf(polParRev,"%s/polNParRev%d.txt",path,dataID);
	char polNCharge[100];
	sprintf(polNCharge,"%s/polNCharge%d.txt",path,dataID);
	char guardRingRoot[100];
	sprintf(guardRingRoot,"%s/guardRingRoot%d.root",path,dataID);

	placData_1 pd1;	

	pd1.read(Input);

	polNPar polNparObj;
	polNparObj.setup(&pd1);
	polNparObj.run(decayData,polPar,polParRev);
	// polNparObj.guardRingRun(pedeFile,polNCharge,guardRingRoot);
	
	// polNparObj.saveAllPixCurveDat(decayData);
	// polNparObj.readDecayDat(decayData);
	// polNparObj.fitPolNDat(polPar);
	// polNparObj.fitPolNDatRev(polParRev);
	// polNparObj.GetGuardRingPolNQ(pedeFile,polNCharge);
	// polNparObj.drawGuardRing2DCharge(polNCharge,"guardring.root");


	return 0;
}
