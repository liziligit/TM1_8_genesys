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

int pd1_analysis(char *fn, char *pede , int ich, int iframe){



	pd1Debug pd1d;
	pd1d.read(fn);
 //   	pd1d.drawRow(pede ,ich, iframe);
//	pd1d.fitBeam(pede, ich, iframe);
	pd1d.beamPara(pede,ich);

	return 0;
}
