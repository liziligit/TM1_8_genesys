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

const bool debug = 0;

const int nTopMetalChips = 8;
const int nPixelsOnChip = 5184;

int pedNoiseCalib(char *dataType = "pd1", const int dataID = 0){
    char *input = Form("../out%d.%s", dataID, dataType);
    char *output = Form("pedNoiseCalib_%s_%d.root", dataType, dataID);
    cout << input << "  " << output << endl;
    
    std::vector< TH1S* > mHistPedVec; //tempoary histogram container
    mHistPedVec.assign( nTopMetalChips*nPixelsOnChip, (TH1S*)0 );
    
    TString histName;
    TH1F *hPedestal[nTopMetalChips];
    TH1F *hRMS[nTopMetalChips];
    for(int i=0; i<nTopMetalChips; i++) {
        histName = "meanPedestal_Chip"; histName += i;
        hPedestal[i] = new TH1F(histName, "mean pedestal;pixel index;<pedestal> per channel [ADC counts]", nPixelsOnChip, 0, nPixelsOnChip);
        
        histName = "rmsPedestal_Chip"; histName += i;
        hRMS[i] = new TH1F(histName, "RMS pedestal;pixel index;RMS per channel [ADC counts]", nPixelsOnChip, 0, nPixelsOnChip);
    }
    
    TFile * fout = new TFile(output,"RECREATE");//
    
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
        
        TString ss;
        for(int iChip=0; iChip<nChips; iChip++) { //0 - 8
            cout << "Loop chip " << iChip << " ...... " << endl;
            
            for(int iPixel=0; iPixel<nPixels; iPixel++) { //0 - 5184
                for(int iSample=0; iSample<nSamples; iSample++) { //0 - 1
                    for(int iFrame=0; iFrame<nFrames; iFrame++) { //0 - 1617
                        short adcValue = 0;
                        adcValue = short(ps[iFrame*nPixels*nSamples*nChips+iPixel*nSamples*nChips+iSample*nChips+iChip]);
                        
                        int code = iChip * nPixels + iPixel;
                        TH1S* histPed = mHistPedVec[code];
                        if( !histPed ){
                            ss="hPedestal_Chip"; ss += iChip;
                            ss += "_Channel"; ss += iPixel;
                            histPed = new TH1S( ss.Data(), ";ADC;Counts", 256, 0, 1024 );
                            mHistPedVec[ code ] = histPed;
                        }
                        histPed->Fill( adcValue );
                    }//end frame loop per sample point
                }//end sample point loop per pixel
            }//end pixel-on-chip loop
        }//end chip loop
    }//end data type selection
    
    TString mPedNoiseFileName = Form("pedNoise_topMetal1X8_%d.txt", dataID);
    std::ofstream fout_pedNoise( mPedNoiseFileName.Data(), std::ios_base::out & std::ios_base::trunc );
    if( !fout_pedNoise ){
        cout << "Error opening file '" << mPedNoiseFileName << "'" << endl;
    }
    fout_pedNoise.setf(std::ios::fixed, std::ios::floatfield);
    fout_pedNoise.precision(5);
    
    std::vector< TH1S* >::iterator mHistPedVecIter;
    int channelIdx = 0;
    
    for( mHistPedVecIter = mHistPedVec.begin(); mHistPedVecIter != mHistPedVec.end(); ++mHistPedVecIter ){
        TH1S *histPed = *mHistPedVecIter;
        histPed->Write();
        
        channelIdx = std::distance( mHistPedVec.begin(), mHistPedVecIter );
        int   entries = histPed->GetEntries();
        float meanPed = histPed->GetMean();
        float rmsPed  = histPed->GetRMS();
        
        if( entries == 0 ) {
            meanPed = 0.;
            rmsPed  = 0.; //marked as dead channel
        }
        
        int iChip = channelIdx / nPixelsOnChip;
        int iPixel = channelIdx % nPixelsOnChip;
        
        hPedestal[iChip]->SetBinContent(iPixel+1, meanPed);
        hPedestal[iChip]->SetBinError(iPixel+1, 0.01 * meanPed);
        
        hRMS[iChip]->SetBinContent(iPixel+1, rmsPed);
        hRMS[iChip]->SetBinError(iPixel+1, 0.01 * rmsPed);
        
        fout_pedNoise << iChip << ' ' << iPixel << ' ' << meanPed << ' ' << rmsPed << endl;
        
        if(debug)
            cout << iChip << ' ' << iPixel << ' ' << meanPed << ' ' << rmsPed << endl;
    }
    fout_pedNoise.close();
    
    //clear memory
    while (!mHistPedVec.empty()) delete mHistPedVec.back(), mHistPedVec.pop_back();
    
    //QA histograms
    TCanvas *ca_ped = new TCanvas("ca_ped", "TopMetal1X8 Pedestal", 2000, 3200);
    ca_ped->Divide(1,nTopMetalChips);
    
    for(int iChip=0; iChip<nTopMetalChips; iChip++) {
        hPedestal[iChip]->Write();
        
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
    ca_rms->Divide(1,nTopMetalChips);
    
    for(int iChip=0; iChip<nTopMetalChips; iChip++) {
        hRMS[iChip]->Write();
        
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
    
    fout->Write();
    fout->Close();
    
    return 0;
}