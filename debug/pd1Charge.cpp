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

int pd1Charge(int src=0,int des=1,int pulse=0){

//pd1Pede pede;
//pede.setup("../data/pedeData/pede0.txt");

placData_1 pd1;

char fn[200];
sprintf(fn,"../data/out%d.pd1",src);
pd1.read((char*)fn);

pd1.print();

pd1ToCharge pd1c;




char slopeFile[200];
sprintf(slopeFile,"../data/pedeData/slope%d.txt",pulse);

char chargeFile[200];
sprintf(chargeFile,"../data/out%d.pd1",des);




int cha=8;
int nPix=5184;

float tCharge=10;
///////////////////generate slope file




////////////////////////////read slope file


pd1c.read((char*)slopeFile);

cout<<"pd1c.nPix="<<pd1c.nPix<<endl;






////////////////////////////generate charge



for(int i=0;i<cha;i++){
//for(int i=4;i<5;i++){
for(int j=0;j<nPix;j++){
//for(int j=4;j<5;j++){
pd1.getPixDat(i,j);
for(int k=0;k<pd1.nPixDat-1;k++){ //the last frame is not good to calculate charge
pd1.pixDat[k]=pd1c.getCharge(pd1c.s[nPix*i+j],pd1.pixDat+k,tCharge);
//cout<<k<<"  ppp "<<pd1.pixDat[k]<<endl;
}
pd1.pixDat[pd1.nPixDat-1]=0; //the last frame charge is set to zero

pd1.setPixDat(i,j);

}
}
//write to file charge
pd1.write((char*)chargeFile);





return 0;


}
