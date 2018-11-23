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
// #include <io.h>

int pd1Pulse(int src=0,int des=0,float tSlope=100){   // tSlope is threshold for slope

//pd1Pede pede;
//pede.setup("../data/pedeData/pede0.txt");

placData_1 pd1;

char fn[200];
sprintf(fn,"../data/out%d.pd1",src);
pd1.read((char*)fn);

pd1.print();

pd1ToCharge pd1c;

int genSlope=1;
int readSlope=0;
int genCharge=0;



char sn[200];
sprintf(sn,"../data/pedeData/slope%d.txt",des);


int cha=8;
int nPix=5184;


///////////////////generate slope file


pd1c.setup(cha*nPix);

for(int i=0;i<cha;i++){
for(int j=0;j<nPix;j++){
pd1.getPixDat(i,j);

pd1c.findSlopeInPixData(pd1.pixDat,pd1.nPixDat,pd1c.s[nPix*i+j],tSlope);

}
}

pd1c.write((char*)sn);




return 0;

}
