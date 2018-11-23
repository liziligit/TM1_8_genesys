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
#include <TSystem.h>

const int debug = 1;

const int nTopMetalChips = 8;
const int nPixelsOnChip = 5184;

const float mAdcCut = 1.0;

int trackingAlpha_ZS(char *dataType = "pd1", const int dataID = 0){
	char *input = Form("../out%d.%s", dataID, dataType);
    char *output = Form("adcSpectrum_ZS_%s_%d.root", dataType, dataID);
    cout << input << "  " << output << endl;
    
    //loading pedestal/rms calibration tables
    float meanPed[41472];
    float rmsPed[41472];
    std::ifstream inPD(Form("pedNoise_topMetal1X8_%d.txt", dataID));
    if(!inPD.is_open()) {
        cout << "No pedestal table found!" << endl;
        return -1;
    }
    
    int iChipT = 0, iPixelT = 0, iCounter=0;
    float pedestalT = 0., noiseT = 0.;
    while(!inPD.eof() && iCounter<41472) {
        inPD >> iChipT >> iPixelT >> pedestalT >> noiseT;
        meanPed[iChipT*nPixelsOnChip+iPixelT] = pedestalT;
        rmsPed[iChipT*nPixelsOnChip+iPixelT] = noiseT;
        iCounter++;
    }
    
    std::vector< TH1S* > mHistAdcVec; //tempoary histogram container
    mHistAdcVec.assign( nTopMetalChips*nPixelsOnChip, (TH1S*)0 );
    
    TString histName;
    TH2S *hAdcMap[nTopMetalChips];
    for(int i=0; i<nTopMetalChips; i++) {
        histName = "hAdcMap_Chip"; histName += i;
        hAdcMap[i] = new TH2S(histName, "ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
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
                int code = iChip * nPixels + iPixel;
                
                for(int iFrame=0; iFrame<nFrames; iFrame++) { //0 - 1617
                    short adcValue = 0;
                    for(int iSample=0; iSample<nSamples; iSample++) //0 - 1
                        adcValue += short(ps[iFrame*nPixels*nSamples*nChips+iPixel*nSamples*nChips+iSample*nChips+iChip]);
                    
                    TH1S* histAdc = mHistAdcVec[ code ];
                    if( !histAdc ){
                        ss="hAdc_ZS_Chip"; ss += iChip;
                        ss += "_Channel"; ss += iPixel;
                        histAdc = new TH1S( ss.Data(), ";Time Bin [per 3.3 ms];ADC Counts", nFrames, 0, nFrames );
                        mHistAdcVec[ code ] = histAdc;
                    }
                    histAdc->SetBinContent( iFrame+1, int(0.5*adcValue - meanPed[code]) );
                }//end frame loop per sample point
                mHistAdcVec[ code ]->Write();
            }//end pixel-on-chip loop
        }//end chip loop
        
        TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "ADC map per chip", 3200, 400);
        ca_adcMap->Divide(nTopMetalChips, 1, 0.01, 0.);
        
        std::vector< TH1S* >::iterator mHistAdcVecIter;
        for(int iFrame=0; iFrame<nFrames; iFrame++) {
            cout << "TopMetal II ADC Map at Frame " << iFrame << endl;
            
            int channelIdx = 0;
            for( mHistAdcVecIter = mHistAdcVec.begin(); mHistAdcVecIter != mHistAdcVec.end(); ++mHistAdcVecIter ){ //loop 8*5184 channels
                TH1S *histAdc = *mHistAdcVecIter;
                channelIdx = std::distance( mHistAdcVec.begin(), mHistAdcVecIter );
                int entries = histAdc->GetEntries();
                if( entries == 0 ) continue;
                
                int iChip   = channelIdx / nPixelsOnChip;
                int iPixel  = channelIdx % nPixelsOnChip;
                int iColumn = iPixel / 72;
                int iRow    = iPixel % 72;
                //mask out hot channels
                
                if(iChip==1 && iColumn==20 && iRow==14) continue;
                if(iChip==3 && iColumn==54 && iRow==31) continue;
                if(iChip==3 && iColumn==56 && iRow==20) continue;
                if(iChip==3 && iColumn==57 && iRow==19) continue;
                if(iChip==3 && iColumn==59 && iRow==29) continue;
                if(iChip==3 && iColumn==60 && (iRow==19 || iRow==23)) continue;
                if(iChip==3 && iColumn==61 && iRow==27) continue;
                if(iChip==3 && iColumn==62 && iRow==33) continue;
                if(iChip==3 && iColumn==63 && iRow==24) continue;
                if(iChip==3 && iColumn==64 && iRow==20) continue;
                if(iChip==3 && iColumn==65 && iRow==24) continue;
                
                float rawAdc= histAdc->GetBinContent(iFrame+1);
                //if(rawAdc>mAdcCut*rmsPed[channelIdx])
                    hAdcMap[iChip]->SetBinContent(iRow+1, iColumn+1, (int)rawAdc);
                
                if(debug==2)
                    cout << iChip << ' ' << iPixel << ' ' << iColumn << ' ' << iRow << ' ' << rawAdc << endl;
            }
            
            for(int iC=0; iC<nChips; iC++) {
                ca_adcMap->cd(iC+1);
                hAdcMap[iC]->SetStats(kFALSE);
                hAdcMap[iC]->Draw("COLZ");
                hAdcMap[iC]->SetMaximum(80);
                ca_adcMap->cd(iC+1)->Modified();
                ca_adcMap->cd(iC+1)->Update();
            }
            
            if(gSystem->ProcessEvents()) break;
            
            if(debug==1 && (iFrame+1)%100==1) {
                ca_adcMap->cd();
                ca_adcMap->Print(Form("adcMap_ZS_data%d_frame%d.png", dataID, iFrame));
            }
        } //end frame loop
    }//end data type selection

    //clear memory
    while (!mHistAdcVec.empty()) delete mHistAdcVec.back(), mHistAdcVec.pop_back();

    fout->Close();

    return 0;
}
