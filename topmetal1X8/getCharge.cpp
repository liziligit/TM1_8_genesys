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


int getCharge(char *polNPath,char *InPolNName,char *InPolNRevName,int polNDataID = 0,char *dataPath=" ",char *InDataName=" ",char *pedePath="",char *pedefile="",int pedeID=0,char *inDataType = "pd1",char* outDataType="pd3", int dataID = 0){
	char polPar[100];
	sprintf(polPar,"%s/polNPar%d.txt",polNPath,polNDataID);
	cout<<polPar<<endl;
	char polParRev[100];
	sprintf(polParRev,"%s/polNParRev%d.txt",polNPath,polNDataID);
	cout<<polParRev<<endl;
	char pedeFile[100];//pede input
	sprintf(pedeFile,"%s/%s%d",pedePath,pedefile,pedeID);//pede input
	char polNCharge[100];
	sprintf(polNCharge,"%s/polNCharge%d.txt",dataPath,dataID);
	cout<<polPar<<endl;
	char Input[100];
	sprintf(Input,"%s/%s%d.%s",dataPath,InDataName,dataID,inDataType);
	char pointChargeFile[100];
	sprintf(pointChargeFile,"%s/pointCharge%d.txt",dataPath,dataID);
	char pointChargeRootFile[100];
	sprintf(pointChargeRootFile,"%s/pointChargeRoot%d.root",dataPath,dataID);
	char pointChargeCanvasRootFile[100];
	sprintf(pointChargeCanvasRootFile,"%s/pointChargeCanvas%d.root",dataPath,dataID);

	
	char outPlacData3[100];
	sprintf(outPlacData3,"%s/placData3_%s%d.%s",dataPath,InDataName,dataID,outDataType);
	placData_1 pd1;	

	polNGetCharge polNGetChargeObj;
	pd1.read(Input);
	cout<<"pd1.p"<<(int*)(pd1.p)[100]<<endl;
	polNGetChargeObj.setup(&pd1);

	// polNGetChargeObj.sefPede("zou.txt");
	polNGetChargeObj.run(pedeFile,polPar,polParRev,polNCharge,outPlacData3,pointChargeFile,pointChargeRootFile,pointChargeCanvasRootFile);
	// polNGetChargeObj.savePlacData_3("zou.bin");
	// polNGetChargeObj.loadPolNPar(polPar);
	// polNGetChargeObj.loadPolNParReV(polParRev);
	// polNGetChargeObj.loadPede(pedeFile);
	// polNGetChargeObj.GetPhyPolNQ(pedeFile,polNCharge);
	// polNGetChargeObj.savePhyPolNQ(pointChargeFile);
	// polNGetChargeObj.drawPhy2DCharge();

	return 0;
}
