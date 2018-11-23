#include "lib.h"
#include "pd1CalibConstant.h"
#include "pd1Calib.h"
#include "pd1Calib.C"

#include <iostream>
#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TROOT.h>
#include <TMath.h>
#include <TParameter.h>
#include <TGraph.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TString.h>




int analysis_drawpixel(char* datainfilename, int iChip, int iPixel) {

  //  gROOT->ProcessLine(".L pd1Calib.C++");

  int nChips, nPixels, nSamples, nFrames;
  unsigned short *ps;

  placData_1 pd1_data;
  pd1_data.read(datainfilename);

  pd1Calib *mycalib = new pd1Calib;

  ps = (unsigned short *)pd1_data.p;

  nChips   = (int)pd1_data.adcCha();
  nPixels  = (int)pd1_data.nPix();
  nSamples = (int)pd1_data.samPerPix();
  nFrames  = (int)pd1_data.nFrame();
  
  cout<<"Number of Chip: " << nChips << endl;
  cout<<"Number of Pixel per Chip: " << nPixels << endl;
  cout<<"Number of Samples per Pixel: " << nSamples << endl;
  cout<<"Number of Frame: " << nFrames << endl;
  
  
  cout<<endl<<"Calculating timing tau from "<<datainfilename<<endl<<endl;
  
  
  cout << "Loop chip " << iChip << " for timing tau ...... " << endl;

  TString histname, histtitle;
  TProfile* hTimeDistri;

  histname = "time_distri_singlepixel"; histtitle = histname;
  hTimeDistri = new TProfile(histname, histtitle, nFrames, -0.5, nFrames-0.5);
  
  hTimeDistri->Reset();
  double tmp_tau=-9999, tmp_tau_err=-9999, tmp_tau_chi2perndf=-9999;
  
  // Now we read the signals of each pixel from each channel(chip)
  int position_y = iPixel / PD1_DIMX;
  int position_x = iPixel % PD1_DIMX;
  
  if( mycalib->IsBadPixel(iPixel, position_x, position_y) ) {
    cout<<"Bad pixel "<<iPixel<<" of chip "<<iChip<<" at x="<<position_x<<"\t y="<<position_y<<endl;
    return 0;
  }

    // we fill adc Values, then fit decay time tau
    cout<<endl<<"begin to fill adc for chip "<<iChip<<" pixel "<<iPixel<<endl<<endl;    

    for(int iFrame=0; iFrame<nFrames; iFrame++) { //0 - 1617 as of Nov 3, 2015
      for(int iSample=0; iSample<nSamples; iSample++) { //0 - 1 as of Nov 3, 2015
	
	short adcValue = 0;
	adcValue = short(ps[iFrame*nPixels*nSamples*nChips+iPixel*nSamples*nChips+iSample*nChips+iChip]);
	// Subtract pedestals
	//	adcValue -= hPedestal[iChip]->GetBinContent(position_x +1, position_y +1);
	//	cout<<"iFrame "<<iFrame<<"  iSample "<<iSample<<endl;
	
	hTimeDistri->Fill(iFrame, adcValue);
      } // loop nSamples
    } // loop nFrames

    cout<<endl<<"finish the fill adc for chip "<<iChip<<" pixel "<<iPixel<<endl<<endl;    
    
    hTimeDistri->Draw();
    
    // now we fit the timing tau of this pixel
    // using information of hTimeDistri
    mycalib->FitTauExponential(hTimeDistri, tmp_tau, tmp_tau_err, tmp_tau_chi2perndf);
    
  // finish fill adc Values and fit decay time tau
  
  
  return 0;
}

