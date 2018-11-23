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

int pd1_display(char *fn, char *pede , double maxPixelSignal, double ref){



	pd1Debug pd1d;
	pd1d.read(fn);
      pd1d.displayInterfaceUpdate(pede ,maxPixelSignal, ref);

	return 0;
}
