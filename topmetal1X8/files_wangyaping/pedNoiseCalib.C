#include "lib.h"

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TROOT.h>
#include <TMath.h>
#include <TParameter.h>
#include <TGraph.h>
#include <TH2D.h>
#include <TCanvas.h>

int pedNoiseCalib(char *dataType = "pd1", const int dataID = 0){
    char *input = Form("out%d.%s", dataID, dataType);
    char *output = Form("pedNoiseCalib_%s_%d.root", dataType, dataID);
    
    cout << input << "  " << output << endl;
    
    TH1S *hPedestalPerChip[8];
    TH1S *hPedestalPerChannel[41472];
    TString histName;
    for(int i=0; i<8; i++) {
        histName = "pedestal_Chip"; histName += i;
        hPedestalPerChip[i] = new TH1S(histName, histName, 128, 0, 1024);
        
        for(int j=0; j<5184; j++) {
            histName = "pedestal_Chip"; histName += i;
            histName += "_Channel"; histName += j;
            hPedestalPerChannel[i*5184+j] = new TH1S(histName, histName, 128, 0, 1024);
        }
    }
    
    TH1F *hPedestal[8];
    TH1F *hRMS[8];
    for(int i=0; i<8; i++) {
        histName = "meanPedestal_Chip"; histName += i;
        hPedestal[i] = new TH1F(histName, "mean pedestal;pixel index;<pedestal> per channel [ADC counts]", 5184, 0, 5184);
        
        histName = "rmsPedestal_Chip"; histName += i;
        hRMS[i] = new TH1F(histName, "RMS pedestal;pixel index;RMS per channel [ADC counts]", 5184, 0, 5184);
    }
    
    TFile * fout = new TFile(output,"RECREATE");//
    
    TString mPedNoiseFileName = Form("pedNoise_topMetal1X8_%d.txt", dataID);
    std::ofstream fout_pedNoise( mPedNoiseFileName.Data(), std::ios_base::out & std::ios_base::trunc );
    if( !fout_pedNoise ){
        cout << "Error opening file '" << mPedNoiseFileName << "'" << endl;
    }
    fout_pedNoise.setf(std::ios::fixed, std::ios::floatfield);
    fout_pedNoise.precision(5);
    
    int pid = strcmp(dataType, "pd1");
    if(pid==0) {
        cout << "Data type " << pid << endl;
        
        placData_1 pd1;
        pd1.read(input);
    
        const int nChips   = (int)pd1.adcCha();
        const int nPixels  = (int)pd1.nPix();
        const int nSamples = (int)pd1.samPerPix();
        const int nFrames  = (int)pd1.nFrame();
        
        cout<<"Number of Chip: " << nChips << endl;
        cout<<"Number of Pixel per Chip: " << nPixels << endl;
        cout<<"Number of Samples per Pixel: " << nSamples << endl;
        cout<<"Number of Frame: " << nFrames << endl;
        
        unsigned short *ps = (unsigned short *)pd1.p;
        
        for(int iChip=0; iChip<nChips; iChip++) { //0 - 8
            cout << "Loop chip " << iChip << " ...... " << endl;
            
            for(int iPixel=0; iPixel<nPixels; iPixel++) { //0 - 5184
                for(int iSample=0; iSample<nSamples; iSample++) { //0 - 1
                    for(int iFrame=0; iFrame<nFrames; iFrame++) { //0 - 1617
                        short adcValue = 0;
                        adcValue = short(ps[iFrame*nPixels*nSamples*nChips+iPixel*nSamples*nChips+iSample*nChips+iChip]);
                        
                        hPedestalPerChannel[iChip*nPixels+iPixel]->Fill(adcValue);
                        hPedestalPerChip[iChip]->Fill(adcValue);
                    }
                }
                hPedestalPerChannel[iChip*nPixels+iPixel]->Write();
            }
            hPedestalPerChip[iChip]->Write();
            
            for(int iPixel=0; iPixel<nPixels; iPixel++) {
                hPedestal[iChip]->SetBinContent(iPixel+1, hPedestalPerChannel[iChip*nPixels+iPixel]->GetMean());
                hPedestal[iChip]->SetBinError(iPixel+1, 3.0);
                
                hRMS[iChip]->SetBinContent(iPixel+1, hPedestalPerChannel[iChip*nPixels+iPixel]->GetRMS());
                hRMS[iChip]->SetBinError(iPixel+1, 0.3);
                
                fout_pedNoise << iChip << ' ' << iPixel << ' ' << hPedestalPerChannel[iChip*nPixels+iPixel]->GetMean() << ' ' << hPedestalPerChannel[iChip*nPixels+iPixel]->GetRMS() << endl;
            }
        }
        fout_pedNoise.close();
    }
    
    fout->Write();
    fout->Close();
    
    TCanvas *ca_ped = new TCanvas("ca_ped", "TopMetal1X8 Pedestal", 2000, 3200);
    ca_ped->Divide(1,8);
    
    for(int iChip=0; iChip<8; iChip++) {
        ca_ped->cd(iChip+1);
        hPedestal[iChip]->SetStats(kFALSE);
        hPedestal[iChip]->SetMarkerColor(kBlue);
        hPedestal[iChip]->SetMarkerSize(0.8);
        hPedestal[iChip]->SetMarkerStyle(20);
        hPedestal[iChip]->Draw("E1");
    }
    ca_ped->cd();
    ca_ped->Print(Form("meanPedestal_topMetal1X8_%d.pdf", dataID));
    //ca_ped->Print(Form("meanPedestal_topMetal1X8_%d.png", dataID));
    
    TCanvas *ca_rms = new TCanvas("ca_rms", "TopMetal1X8 RMS Noise", 2000, 3200);
    ca_rms->Divide(1,8);
    
    for(int iChip=0; iChip<8; iChip++) {
        ca_rms->cd(iChip+1);
        hRMS[iChip]->SetStats(kFALSE);
        hRMS[iChip]->SetMarkerColor(kRed);
        hRMS[iChip]->SetMarkerSize(0.8);
        hRMS[iChip]->SetMarkerStyle(20);
        hRMS[iChip]->Draw("E1");
    }
    ca_rms->cd();
    ca_rms->Print(Form("rmsPedestal_topMetal1X8_%d.pdf", dataID));
    //ca_rms->Print(Form("rmsPedestal_topMetal1X8_%d.png", dataID));
    
    return 0;
}