#ifndef _PD1_CALIB_H_
#define _PD1_CALIB_H_

#include "lib.h"
#include "pd1CalibConstant.h"
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


class pd1Calib{

 public:

  pd1Calib();
  virtual ~pd1Calib(){};

  int SetDataInput(char *inputdata) {datainfilename = inputdata; return CalibOK; }
  int SetDataType(char *dataType = "pd1") {dataintype = dataType; return CalibOK; }

  int SetDoPedSelf(int dopedself) { switch_dopedself = dopedself; return CalibOK; }
  int GetDoPedSelf() { return switch_dopedself > 0 ? 1 : 0; }
  int SetDoTauSelf(int dotauself) { switch_dotauself = dotauself; return CalibOK; }
  int GetDoTauSelf() { return switch_dotauself > 0 ? 1 : 0; }

  int SetPedExternalFileName(char *pedextfile) { pedexternalfilename = pedextfile; return CalibOK; }

  int SetPedOutFileName(char *pedoutfile);
  int SetTauOutFileName(char *tauoutfile);


  int Init();
  int Run();
  int End();

  int GetPedExternalFile();
  int DoPedSelf();
  int DoTauSelf();

  int FitTauExponential(TProfile* htimedistri, double& tau, double& tau_err, double& chi2perndf); 
  bool IsBadPixel(int ipixel, int position_x, int position_y);

  int WritePedTextFile();
  int WriteTauTextFile();

  placData_1 pd1_data;
  unsigned short *ps;

  int nChips;
  int nPixels;
  int nSamples;
  int nFrames;

  int switch_dopedself;
  int switch_dotauself;

  TString datainfilename;
  TString dataintype;
  TString pedexternalfilename;

  TString pedoutfilename;
  TString tauoutfilename;
  TString totalrootoutfilename;

  TProfile2D *hRawData[MAX_NCHIPS]; // if do pedstal self, this TProfile2D will be used to fill from data files, and generate pedestal (Mean) and noise (RMS)
  TH2D *hPedestal[MAX_NCHIPS]; // fill pedestal (Mean) into this, either from data or from external source
  TH2D *hNoise[MAX_NCHIPS]; // fill noise (RMS) into this, either from data or from external source

  TProfile *hTimeDistriAll[MAX_NCHIPS]; // this one is used to get tau from all pixels together (at same chip ? )
  TProfile *hTimeDistri; // this one is used to fit tau of each pixel at each chip
  TH2D *hTimeTau[MAX_NCHIPS];  TH2D *hTimeTauChi2NDF[MAX_NCHIPS]; // fill tau into this, from data, and their chi2 per NDF
  TH1D *hTimeTauAll; // this has nChips bins, each bin is a fitted value of tau

};



#endif
