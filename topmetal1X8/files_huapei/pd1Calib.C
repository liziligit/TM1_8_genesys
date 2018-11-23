#include "pd1Calib.h"


using namespace std;


pd1Calib::pd1Calib(){

  nChips = -9999;  nPixels = -9999; nSamples = -9999; nFrames = -9999;

  switch_dopedself = -9999;
  switch_dotauself = -9999;

  datainfilename = "NONE";
  dataintype = "NONE";
  pedexternalfilename = "NONE";

  pedoutfilename = "NONE";
  tauoutfilename = "NONE";
  totalrootoutfilename = "NONE";

  //  return CalibOK;
}


int pd1Calib::Init(){

  // Previously before we can calculate tau, we need to either do pedestal ourselves or read pedestal from external file
  // But then we decide tau and pedestal shall be two independent measurements.
  // Therefore we comment out the following check

  /*
  // First check if we read or write pededstal 
  if( !GetDoPedSelf() && pedexternalfilename=="NONE") {
    cout<<endl<<"Need external pedstal file name !!!"<<endl;
    return CalibFAIL;
  }
  if( GetDoPedSelf() && pedexternalfilename!="NONE") {
    cout<<endl<<"No external pedstal file name if you want to do it yourself !!!"<<endl;
    return CalibFAIL;
  }
  */


  if(totalrootoutfilename == "NONE") {
    cout<<endl<<endl<<"You didn't setup any actions, such as pedestals or timing tau, so we have to GUESS THE NAME OF ROOT FILE for you"<<endl
	<<"It will be put at the same position of your data file with an extension .root"<<endl<<endl;
    totalrootoutfilename = datainfilename; totalrootoutfilename += ".root";
  }

  // Begin read the data
  // find how many frames/pixels/samples/channels(chips) we have, 

  char tmpname[200]; strcpy(tmpname, datainfilename.Data());
  pd1_data.read(tmpname);
	
  nChips   = (int)pd1_data.adcCha();
  nPixels  = (int)pd1_data.nPix();
  nSamples = (int)pd1_data.samPerPix();
  nFrames  = (int)pd1_data.nFrame();
  
  cout<<"Number of Chip: " << nChips << endl;
  cout<<"Number of Pixel per Chip: " << nPixels << endl;
  cout<<"Number of Samples per Pixel: " << nSamples << endl;
  cout<<"Number of Frame: " << nFrames << endl;
  
  if(nPixels != PD1_DIMX * PD1_DIMY) {
    cout <<endl<<endl
	 <<"========================="<<endl
	 <<"   !!!   !!!   !!!   !!!  "<<endl
	 <<"HORRIBLE MISTAKE !!!  NPIXELS != PD1_DIMX * PD1_DIMY !!!"<<endl
	 <<"   !!!   !!!   !!!   !!!  "<<endl
	 <<"========================="<<endl
	 <<endl<<endl;
    
    return CalibFAIL;
  }
  

  TString histname, histtitle;

  // Initialize TProfile of Raw Data for pedestal

  if(GetDoPedSelf()) {
    for(int iChip=0; iChip<nChips; iChip++) {
      histname = "rawdata_chip_"; histname += iChip;
      histtitle = "Raw data of chip "; histtitle += iChip;
      hRawData[iChip] = new TProfile2D(histname, histtitle, PD1_DIMX, -0.5, PD1_DIMX-0.5, PD1_DIMY, -0.5, PD1_DIMY-0.5);
    }
  }

  // Initialize TH2 for pedestals, whether you read or calculate them
  // Also you can set any initial values to them here, even those unphysical ones
  // By default, newly created TH2D shall be zero of all bins 

  for(int iChip=0; iChip<nChips; iChip++) {
    histname = "meanPedestal_Chip_"; histname += iChip;
    histtitle = "Mean pedestal of chip "; histtitle += iChip;
    hPedestal[iChip] = new TH2D(histname, histtitle, PD1_DIMX, -0.5, PD1_DIMX-0.5, PD1_DIMY, -0.5, PD1_DIMY-0.5);

    histname = "rmsPedestal_Chip_"; histname += iChip;
    histtitle = "RMS noise of chip "; histtitle += iChip;
    hNoise[iChip] = new TH2D(histname, histtitle, PD1_DIMX, -0.5, PD1_DIMX-0.5, PD1_DIMY, -0.5, PD1_DIMY-0.5);
  }

  // Initialize all TH and TProfile for timing tau

  if(GetDoTauSelf()) {
    for(int iChip=0; iChip<nChips; iChip++) {
      histname = "time_distri_chip_"; histname += iChip;
      histtitle = "Timing distribution of chip "; histtitle += iChip;
      hTimeDistriAll[iChip] = new TProfile(histname, histtitle, nFrames, -0.5, nFrames-0.5);

      histname = "tau_Chip_"; histname += iChip;
      histtitle = "Timing #tau of chip "; histtitle += iChip;
      hTimeTau[iChip] = new TH2D(histname, histtitle, PD1_DIMX, -0.5, PD1_DIMX-0.5, PD1_DIMY, -0.5, PD1_DIMY-0.5);

      histname = "tau_chi2ndf_Chip_"; histname += iChip;
      histtitle = "Chi2 per NDF of Timing #tau of chip "; histtitle += iChip;
      hTimeTauChi2NDF[iChip] = new TH2D(histname, histtitle, PD1_DIMX, -0.5, PD1_DIMX-0.5, PD1_DIMY, -0.5, PD1_DIMY-0.5);
    }

    histname = "time_distri_singlepixel"; histtitle = histname;
    hTimeDistri = new TProfile(histname, histtitle, nFrames, -0.5, nFrames-0.5);
    histname = "tau_all_chips"; histtitle = histname;
    hTimeTauAll = new TH1D(histname, histtitle, nChips, -0.5, nChips-0.5);
  } 

  // Setup the output files

  return CalibOK;
}


int pd1Calib::Run() {

  // Initialize the data stream
  ps = (unsigned short *)pd1_data.p;
  

  // Read or calculate the pedestal and noise
  //
  // Read
  if( !GetDoPedSelf() && pedexternalfilename != "NONE") { 
    int istatus = GetPedExternalFile();

    if(istatus == CalibFAIL) 
      return CalibFAIL;
  }
  else if ( GetDoPedSelf() && pedexternalfilename == "NONE" ) {
    int istatus = DoPedSelf();

    if(istatus == CalibFAIL) 
      return CalibFAIL;
  }
  else if (!GetDoPedSelf() && pedexternalfilename == "NONE") {
    cout<<endl<<"So you don't need pedestal. Make sure you really know what you want !!!"<<endl<<endl;
  }
  else {
    cout<<endl<<"WHAT THE HELL IS GOING ON ??? PEDESTAL IS BOTH READ AND CALCULATED ??? "<<endl<<endl;
    return CalibFAIL;
  }


  // Fit the timing tau 
  if( GetDoTauSelf() ) {
    
    int istatus = DoTauSelf();

    if(istatus == CalibFAIL) 
      return CalibFAIL;
  }
                
  return CalibOK;
}

int pd1Calib::End() {

  cout<<endl<<endl<<"Create TFile "<<totalrootoutfilename<<endl<<endl;

  TFile *rootfout = new TFile(totalrootoutfilename, "RECREATE");

  if(GetDoPedSelf()) {
    // output pedestal text file
    WritePedTextFile();
  }

    // Self or external, we output pedestal and RMS TH2D

  rootfout->cd();

  if(GetDoPedSelf() || pedexternalfilename != "NONE") {
    // out pedestal TH2D if we get pedestal from internal or external source
    // Note this pedestal is NOT necessary for tau, as of Nov 8, 2015 
   for(int iChip=0; iChip<nChips; iChip++) {
      hPedestal[iChip]->Write();
      hNoise[iChip]->Write();
    }
  } 
  

  if(GetDoTauSelf()) {
    //output tau text file
    WriteTauTextFile();

    // output tau TH2D
    rootfout->cd();
    hTimeTauAll->Write();
    hTimeDistri->Write();

    for(int iChip=0; iChip<nChips; iChip++) {
      hTimeTau[iChip]->Write();
      hTimeTauChi2NDF[iChip]->Write();
      hTimeDistriAll[iChip]->Write();
    }
  }

  rootfout->Save();
  rootfout->Close();

  return CalibOK;
}


int pd1Calib::GetPedExternalFile() {
  int tmp_chip = 0;
  int tmp_pixel = 0;
  double tmp_pedestal = 0.0;
  double tmp_rms = 0.0;

  cout<<endl<<"Read pedestals from "<<pedexternalfilename.Data()<<endl<<endl;

  ifstream fin_pedestal(pedexternalfilename.Data());

    for(int iChip=0; iChip<nChips; iChip++) { //0 - 8 as of Nov 3, 2015
      cout << "Loop chip " << iChip << " for pedestal and rms...... " << endl;
      
      for(int iPixel=0; iPixel<nPixels; iPixel++) { //0 - 5184 = 72*72, PD1_DIMX*PD1_DIMY as of Nov 3, 2015
	// If end of file, we are in problem
	if( fin_pedestal.eof() ) {
	  cout<<endl<<endl<<" HORRIBLE MISTAKE !!! "<<endl
	      <<"WHY PEDESTAL FILE CONTAINS FEWER PIXELS THAN NEEDED !!!"<<endl<<endl;
	  return CalibFAIL;
	}

	fin_pedestal >> tmp_chip >> tmp_pixel >> tmp_pedestal >> tmp_rms;

	if( iChip != tmp_chip || iPixel != tmp_pixel ) {
	  // Expected format does NOT match the actual file
	  cout<<endl<<endl<<" HORRIBLE MISTAKE !!! "<<endl
	      <<"WHY CHIP AND PIXEL NUMBERS READ IN DON'T MATCH THE EXPECTED ORDER !!!"<<endl<<endl;
	  return CalibFAIL;

	}
	int position_y = iPixel / PD1_DIMX;
	int position_x = iPixel % PD1_DIMX;

	hPedestal[iChip]->SetBinContent(position_x +1, position_y +1, tmp_pedestal);
	hNoise   [iChip]->SetBinContent(position_x +1, position_y +1, tmp_rms     );

      } // loop nPixels
    } // loop nChips

  return CalibOK;

}

int pd1Calib::DoPedSelf() {

  cout<<endl<<"Calculating pedestals from "<<datainfilename.Data()<<endl<<endl;

  double tmp_counter = nFrames * nSamples;

    for(int iChip=0; iChip<nChips; iChip++) { //0 - 8 as of Nov 3, 2015
      cout << "Loop chip " << iChip << " for pedestal and rms...... " << endl;
      
      for(int iPixel=0; iPixel<nPixels; iPixel++) { //0 - 5184 = 72*72, PD1_DIMX*PD1_DIMY as of Nov 3, 2015
	// Now we read the signals of each pixel from each channel(chip)
	int position_y = iPixel / PD1_DIMX;
	int position_x = iPixel % PD1_DIMX;
	
	for(int iFrame=0; iFrame<nFrames; iFrame++) { //0 - 1617 as of Nov 3, 2015
	  for(int iSample=0; iSample<nSamples; iSample++) { //0 - 1 as of Nov 3, 2015
	    
	    short adcValue = -9999;
	    adcValue = short(ps[iFrame*nPixels*nSamples*nChips+iPixel*nSamples*nChips+iSample*nChips+iChip]);
	    hRawData[iChip]->Fill(position_x, position_y, adcValue);
	  } // loop nSamples
	} // loop nFrames

	// now we have all adcValues of this pixel at this chip, we can calculate the Mean and RMS of this pixel
	double tmp_pedestal = hRawData[iChip]->GetBinContent(position_x +1, position_y +1);
	double tmp_rms      = sqrt(tmp_counter) * hRawData[iChip]->GetBinError  (position_x +1, position_y +1);
	hPedestal[iChip]->SetBinContent(position_x +1, position_y +1, tmp_pedestal);
	hNoise   [iChip]->SetBinContent(position_x +1, position_y +1, tmp_rms     );

      } // loop nPixels
    } // loop nChips

  return CalibOK;
}


int pd1Calib::DoTauSelf() {

  cout<<endl<<"Calculating timing tau from "<<datainfilename.Data()<<endl<<endl;

  for(int iChip=0; iChip<nChips; iChip++) { //0 - 8 as of Nov 3, 2015
    // hard code
    // for(int iChip=0; iChip<1; iChip++) { //0 - 8 as of Nov 3, 2015

    cout << "Loop chip " << iChip << " for timing tau ...... " << endl;
    hTimeDistriAll[iChip]->Reset();
    
    for(int iPixel=0; iPixel<nPixels; iPixel++) { //0 - 5184 = 72*72, PD1_DIMX*PD1_DIMY as of Nov 3, 2015
      // hard code
      // for(int iPixel=1000; iPixel<1001; iPixel++) {

      hTimeDistri->Reset();
      double tmp_tau=-9999, tmp_tau_err=-9999, tmp_tau_chi2perndf=-9999;
      
      // Now we read the signals of each pixel from each channel(chip)
      int position_y = iPixel / PD1_DIMX;
      int position_x = iPixel % PD1_DIMX;
      
      if( IsBadPixel(iPixel, position_x, position_y) ) continue;  // do nothing

      // For good pixels, we fill adc Values, then fit decay time tau
      
      for(int iFrame=0; iFrame<nFrames; iFrame++) { //0 - 1617 as of Nov 3, 2015
	for(int iSample=0; iSample<nSamples; iSample++) { //0 - 1 as of Nov 3, 2015
	  
	  short adcValue = 0;
	  adcValue = short(ps[iFrame*nPixels*nSamples*nChips+iPixel*nSamples*nChips+iSample*nChips+iChip]);
	  // Subtract pedestals
	  adcValue -= hPedestal[iChip]->GetBinContent(position_x +1, position_y +1);
	  
	  hTimeDistri->Fill(iFrame, adcValue);
	  hTimeDistriAll[iChip]->Fill(iFrame, adcValue);
	} // loop nSamples
      } // loop nFrames
      
      // now we fit the timing tau of this pixel
      // using information of hTimeDistri
      FitTauExponential(hTimeDistri, tmp_tau, tmp_tau_err, tmp_tau_chi2perndf);
      

      if( !TMath::IsNaN(tmp_tau) && !TMath::IsNaN(tmp_tau_err) && !TMath::IsNaN(tmp_tau_chi2perndf) )
	{ // only fill them after checking nan
	  hTimeTau[iChip]->SetBinContent(position_x +1, position_y +1, tmp_tau);
	  hTimeTau[iChip]->SetBinError  (position_x +1, position_y +1, tmp_tau_err);
	  
	  hTimeTauChi2NDF[iChip]->SetBinContent(position_x +1, position_y +1, tmp_tau_chi2perndf);
	}
      
    } // loop nPixels
    
    // now we fit the timing tau of this chip
    // using information of hTimeDistriAll[iChip]
    double tmp_tau=-9999, tmp_tau_err=-9999, tmp_tau_chi2perndf=-9999;
    // hard code
    FitTauExponential(hTimeDistriAll[iChip], tmp_tau, tmp_tau_err, tmp_tau_chi2perndf);
  
      if( !TMath::IsNaN(tmp_tau) && !TMath::IsNaN(tmp_tau_err) && !TMath::IsNaN(tmp_tau_chi2perndf) )
	{ // only fill them after checking nan
	  hTimeTauAll->SetBinContent(iChip +1, tmp_tau);
	  hTimeTauAll->SetBinError  (iChip +1, tmp_tau_err);
	}

  } // loop nChips
  
  return CalibOK;
}


int pd1Calib::WritePedTextFile() {
  int tmp_chip = 0;
  int tmp_pixel = 0;
  double tmp_pedestal = 0.0;
  double tmp_rms = 0.0;

  cout<<endl<<"Write pedestals to "<<pedoutfilename.Data()<<endl<<endl;

  ofstream fout_pedestal(pedoutfilename.Data(), std::ios_base::out & std::ios_base::trunc);
  fout_pedestal.setf(std::ios::fixed, std::ios::floatfield);
  fout_pedestal.precision(5);


    for(int iChip=0; iChip<nChips; iChip++) { //0 - 8 as of Nov 3, 2015
      cout << "Loop chip " << iChip << " for pedestal and rms...... " << endl;
      
      for(int iPixel=0; iPixel<nPixels; iPixel++) { //0 - 5184 = 72*72, PD1_DIMX*PD1_DIMY as of Nov 3, 2015
	int position_y = iPixel / PD1_DIMX;
	int position_x = iPixel % PD1_DIMX;

	tmp_pedestal = hPedestal[iChip]->GetBinContent(position_x +1, position_y +1);
	tmp_rms      = hNoise   [iChip]->GetBinContent(position_x +1, position_y +1);

	fout_pedestal << iChip<<" \t" << iPixel<<" \t" << tmp_pedestal<<" \t" << tmp_rms << endl;

      } // loop nPixels
    } // loop nChips
    
    fout_pedestal.close();

    cout<<"Finish writing pedestals"<<endl<<endl;

    return CalibOK;
    
}

int pd1Calib::WriteTauTextFile() {
  int tmp_chip = 0;
  int tmp_pixel = 0;
  double tmp_tau = -9999, tmp_tau_err = -9999, tmp_tau_chi2perndf = -9999;

  cout<<endl<<"Write timing tau to "<<tauoutfilename.Data()<<endl<<endl;

  ofstream fout_tau(tauoutfilename.Data(), std::ios_base::out & std::ios_base::trunc);
  fout_tau.setf(std::ios::fixed, std::ios::floatfield);
  fout_tau.precision(5);

    for(int iChip=0; iChip<nChips; iChip++) { //0 - 8 as of Nov 3, 2015
      cout << "Loop chip " << iChip << " for tau...... " << endl;
      
      for(int iPixel=0; iPixel<nPixels; iPixel++) { //0 - 5184 = 72*72, PD1_DIMX*PD1_DIMY as of Nov 3, 2015
	int position_y = iPixel / PD1_DIMX;
	int position_x = iPixel % PD1_DIMX;

	tmp_tau     = hTimeTau[iChip]->GetBinContent(position_x +1, position_y +1);
	tmp_tau_err = hTimeTau[iChip]->GetBinError  (position_x +1, position_y +1);
	tmp_tau_chi2perndf = hTimeTauChi2NDF[iChip]->GetBinContent(position_x +1, position_y +1);

	fout_tau << iChip<<" \t" << iPixel<<" \t" << tmp_tau<<" \t" << tmp_tau_err<<" \t" << tmp_tau_chi2perndf << endl;

      } // loop nPixels
    } // loop nChips
    
    fout_tau.close();

    cout<<"Finish writing timing tau"<<endl<<endl;

    return CalibOK;
    
}

int pd1Calib::SetPedOutFileName(char *pedoutfile) { 

  if( strcmp(pedoutfile, "NONE") ) { 
    pedoutfilename = pedoutfile;
  } else {
    pedoutfilename = datainfilename; pedoutfilename += ".ped";
  }
  totalrootoutfilename = pedoutfile; totalrootoutfilename += ".root";

  return CalibOK; 
}

int pd1Calib::SetTauOutFileName(char *tauoutfile) { 
  if( strcmp(tauoutfile, "NONE") ) { 
    tauoutfilename = tauoutfile;
  } else {
    tauoutfilename = datainfilename; tauoutfilename += ".tau";
  }
  totalrootoutfilename = tauoutfile; totalrootoutfilename += ".root";

  return CalibOK; 
}



int pd1Calib::FitTauExponential(TProfile *htimedistri, double &tmp_tau, double &tmp_tau_err, double &tmp_chi2perndf) {

  // First we need to find 1) the rising edge, 2) the underlying levels, 3) the reversal edge
  // Even if pedestals are already subtracted, the underlying level can still be non-zero due to fluctuations of pedestals with time, temperature, etc.

  // As of Nov 7, 2015
  // So we don't assume the underlying level is ZERO. Instead we fit it.
  // OTH, we assume the following:
  // a) One pixel can have multiple rising peaks of similar height. These peaks are sharp and much bigger than the underlying level.
  // b) the points between the reversal edge and the next rising peak is all of underlying level
  // c) such points in b) occupy roughly about half of each peroid between peaks

  // The idea is to find the maximum value ->GetMaximumBin (this is peak but usually not the only peak)
  // and the average bin content as underlying (since peak is usually bigger than the reversal edge, this averaged value shall bias to bigger value than real underlying level)
  // then use (max + average) / 2 as threshold, and look for the first group of points over this threshold
  // we define one group to be >= 2 points
  const int bin_offset = 2; // assuming the rising edge is at most 2 points, and the fitting starts from the 3rd point


  // also due to the reversal edges between peaks, we need to find two peaks, and use the first 1/5 of points following the first peak, so as to avoid the reversal parts. About this 1/5, see assumption c) above.

  int nbins = htimedistri->GetNbinsX();
  double binwidth = htimedistri->GetBinWidth(1);
  int bin_peak1=-1, bin_peak2=nbins+1, bin_between_peak12=-1; // initialize peak 1 to be underflow, peak 2 to be overflow

  double keep_points = 1.0/5.0; // keep only first 1/5 points between two peaks to avoid the reversal 


  // int binlow = 3;
  // int binhigh = nbins / 2;
  //  double avg_value = htimedistri->Integral() / htimedistri->GetNbinsX();
  double avg_value = htimedistri->GetMean(2); // Note this is the mean of y which is close to the underlying level
  // cout<<endl<<"avg_value = "<<avg_value<<endl<<endl;

  int peak_bin = htimedistri->GetMaximumBin();
  double peak_value = htimedistri->GetBinContent(peak_bin);

  double mid_threshold = (peak_value + avg_value) * 0.5;


  for(int ibin=1; ibin<=nbins; ibin++) {
    if( htimedistri->GetBinContent(ibin) > mid_threshold
	&&
	htimedistri->GetBinContent(ibin+1) > mid_threshold )
      {
	bin_peak1 = ibin+bin_offset; break; // peak1: start from the 3rd point to avoid singularity ?
      }
  } // look for peak 1
  for(int ibin=bin_peak1+1; ibin<=nbins; ibin++) {
    if( htimedistri->GetBinContent(ibin) < mid_threshold
	&&
	htimedistri->GetBinContent(ibin+1) < mid_threshold )
      {
	bin_between_peak12 = ibin+bin_offset; break; // peak1: find the lower part of decay curve. 
      }
  } // go far away from peak 1

  for(int ibin=bin_between_peak12+1; ibin<=nbins; ibin++) {
    if( htimedistri->GetBinContent(ibin) > mid_threshold
	&&
	htimedistri->GetBinContent(ibin+1) > mid_threshold )
      {
	bin_peak2 = ibin+bin_offset; break; // peak2: start from the 3rd point to avoid singularity ?
      }
  } // look for peak 2

  // cout<<"bin_peak1 = "<<bin_peak1<<"\t bin_between_peak12 = "<<bin_between_peak12<<"\t bin_peak2 = "<<bin_peak2<<endl<<endl;

  int binlow = bin_peak1;
  int binhigh = bin_peak1 + int(keep_points*(bin_peak2 - bin_peak1));

  //  cout<<endl<<"binlow = "<<binlow<<"\t\t binhigh = "<<binhigh<<endl<<endl;

  double xlow = htimedistri->GetBinCenter(binlow) - 0.5*binwidth;
  double xhigh = htimedistri->GetBinCenter(binhigh) + 0.5*binwidth;

  double ylow = htimedistri->GetBinContent(binlow);
  double yhigh = htimedistri->GetBinContent(binhigh);
  double tau_init = (xhigh - xlow) / log( ylow / yhigh ); // note ylow is higher than yhigh ...

  //  TF1 *tf1exp = new TF1("tau_exp", "[0] + [1] * exp(x*[2])", xlow, xhigh);
  //  tf1exp->SetParameter(0, avg_value);
  TF1 *tf1exp = new TF1("tau_exp", "[0]+[1]*exp(-(x - [3])/[2])", xlow, xhigh);

  tf1exp->SetParameter(0, htimedistri->GetBinContent(bin_peak2 - 3*bin_offset) );
  tf1exp->SetParameter(1, ylow);       // tf1exp->SetParLimits(1, 0.0, 10.0*ylow);
  tf1exp->SetParameter(2, tau_init); 
  //  tf1exp->SetParameter(3, xlow);        tf1exp->SetParLimits(3, -1.0, nbins);
  tf1exp->FixParameter(3, xlow);

  htimedistri->Fit(tf1exp, "QMR");
  // hard code
  // htimedistri->Fit(tf1exp, "MR");

  //  cout<<"Total chisquare = "<<tf1exp->GetChisquare()<<endl;
  //  cout<<"NDF = "<<tf1exp->GetNDF()<<endl;

  tmp_tau = tf1exp->GetParameter(2);
  tmp_tau_err = tf1exp->GetParError(2);

  tmp_chi2perndf = tf1exp->GetChisquare() / tf1exp->GetNDF();

  delete tf1exp;

  return CalibOK;
}

bool pd1Calib::IsBadPixel(int ipixel, int position_x, int position_y) {
  
  if( ipixel==0 || ipixel==1 || ipixel==2 ) {
    return true;
  } 
  else {
    return false;
  }
  
}

