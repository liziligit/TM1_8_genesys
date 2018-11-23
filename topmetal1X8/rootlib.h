#ifndef _ROOTLIB_
#define _ROOTLIB_
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TROOT.h>
#include <TMath.h>
#include <TParameter.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TString.h>
#include <TVirtualFFT.h>
#include <TRandom.h>
#include <TStyle.h>
#include <TPad.h>
#include <TPaveStats.h>
#include "../daq/clib.h"

#include <TSystem.h>
#include <TF1.h>
using namespace std;
class fftAna{
public:
fftAna(int num){
	dt=0;
	N=num;
	dat=new double[N];
	ma=new double[N];
	ph=new double[N];
	fr=new double[N];
	ps=new double[N];
	fftr2c=TVirtualFFT::FFT(1, &N, "R2C ES K"); 
	
}
~fftAna(){
	delete []dat;
	delete []ma;
	delete []ph;
	delete []fr;
	delete []ps;
	delete fftr2c;

}

void transform(){
	if(dt==0) {
		cout<<"dt==0"<<endl;
		exit(1) ;
	}
	double sinRms=sqrt(2.0)/2.0;
	fftr2c->SetPoints(dat);
	fftr2c->Transform();
	double re, im;
	for(int i=0;i<N;i++){
      fftr2c->GetPointComplex(i,re,im);

	  ma[i]=sqrt(re*re+im*im);
	  ph[i]=atan2(im,re);
	  
	  if(i==0){ps[i]=ma[i]/N;}
	  else{ps[i]=2*ma[i]/N;}
	  ps[i]=sqrt((ps[i]*ps[i])*N*dt)*sinRms;
	  fr[i]=i/(N*dt);
	}
}

double integrateRms(){
	double df=1/(N*dt);
	double rms=0;
	
	for(int i=1;i<N/2+1;i++){
		rms+=ps[i]*ps[i]*df;
	}
	rms=sqrt(rms);
	return rms;
}

void photo(char*xa,char*ya,char* na){
	double *px;
	double *py;
	double *x=new double[N];
	for(int i=0;i<N;i++){
		x[i]=i;
	}
	
	px=x;
	if(strcmp(xa,"fr")==0)px=fr;
	py=dat;
	if(strcmp(ya,"ma")==0)py=ma;
	if(strcmp(ya,"ps")==0)py=ps;//power spectral
	if(strcmp(ya,"ph")==0)py=ph;//phase
	
	int dn=N/2;
	if(px==x)dn=N;
		
   TGraph *gr = new TGraph(dn,px,py);
   gr->SetName(na);
   gr->Write();	
    delete gr;  	
    delete []x;  	
}

void getComplex(double* re,double *im){
	 fftr2c->GetPointsComplex(re,im);
}

/////input
int N;
double dt;
double * dat;	


/////output

double * ma;	
double * ph;	
double * fr;	
double * ps;	

/////
TVirtualFFT *fftr2c;

};


class fftc2r{
public:
fftc2r(int num){
	dt=0;
	N=num;
	dat=new double[N];
	dati=new double[N];
	ma=new double[N];
	fftr2c=TVirtualFFT::FFT(1, &N, "C2R M K"); 
	
}
~fftc2r(){
	delete []dat;
	delete []dati;
	delete []ma;
	delete fftr2c;

}

void transform(){


	fftr2c->SetPointsComplex(dat,dati);
	fftr2c->Transform();
	fftr2c->GetPoints(ma);
	maScale(N);
}

void maScale(float s){
	for(int i=0;i<N;i++){
		ma[i]/=s;
	}	
}

void photo(char*xa,char*ya,char* na){
	double *px;
	double *py;
	double *x=new double[N];
	for(int i=0;i<N;i++){
		x[i]=i;
	}
	
	px=x;
	py=dat;
	if(strcmp(ya,"ma")==0)py=ma;
	if(strcmp(ya,"im")==0)py=dati;//power spectral
	if(strcmp(ya,"re")==0)py=dat;//phase
	
	int dn=N;
		
   TGraph *gr = new TGraph(dn,px,py);
   gr->SetName(na);
   gr->Write();	
    delete gr;  	
    delete []x;  	
}

void get(double* re){
	 fftr2c->GetPoints(re);
}

/////input
int N;
double dt;
double * dat;	
double * dati;	


/////output

double * ma;	
	

/////
TVirtualFFT *fftr2c;

};

class pd1And2Draw{
	public:
	pd1And2Draw(){
		pd1=NULL;
		pd2=NULL;
		output="out1.root";
	}
	placData_1*pd1;
	placData_2*pd2;
	TString pedeFile;
	TString output;
	float *meanPed;
	float *rmsPed;
	void draw(){
		int debug = 1;
		int nTopMetalChips = pd1->adcCha();
		int nPixelsOnChip = pd1->nPix();

   
		//loading pedestal/rms calibration tables
		meanPed=new float[nTopMetalChips*nPixelsOnChip];
		rmsPed=new float[nTopMetalChips*nPixelsOnChip];
		cout<<"nTopMetalChips "<<nTopMetalChips<<endl;
		for(int i=0;i<nTopMetalChips*nPixelsOnChip;i++){
			meanPed[i]=0;
			rmsPed[i]=0;
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
    
		//int pid = strcmp(dataType, "pd1");
		//if(pid==0) {
		//	cout << "Data type " << pid << endl;     
			 int nChips ;
			 int nPixels ;
			 int nSamples;
			 int nFrames  ;
			 unsigned short *ps ;

			 
             if(pd1!=NULL){
			  nChips   = (int)pd1->adcCha();
			  nPixels  = (int)pd1->nPix();
			  nSamples = (int)pd1->samPerPix();
			  nFrames  = (int)pd1->nFrame();
			  
		std::ifstream inPD(pedeFile.Data());
		if(!inPD.is_open()) {
			cout << "No pedestal table found!" << endl;
			return ;
		}
		int iChipT = 0, iPixelT = 0, iCounter=0;
		float pedestalT = 0., noiseT = 0.;
		while(!inPD.eof() && iCounter<nTopMetalChips*nPixelsOnChip) {
			inPD >> iChipT >> iPixelT >> pedestalT >> noiseT;
			meanPed[iChipT*nPixelsOnChip+iPixelT] = pedestalT;
			rmsPed[iChipT*nPixelsOnChip+iPixelT] = noiseT;
			iCounter++;
		}			  
			  
			  
			   ps = (unsigned short *)pd1->p;
			  
            }else if(pd2!=NULL){
			  nChips   = (int)pd2->adcCha();
		 nPixels=nPixelsOnChip;
		 nSamples=2;
		int totalSample=pd2->dataSize()/sizeof(unsigned short);
		 nFrames=totalSample/nPixelsOnChip/nChips/nSamples;	
          ps = (unsigned short *)pd2->p;
		 
			
			for(int j=0;j<nChips;j++){	
            meanPed[j]=0;			
			for(int i=0;i<100;i++){
				meanPed[j]+=ps[i*nChips+j];
			}	
			meanPed[j]/=100.0;
			}
	

			
			}else{return;}

			 
			cout<<"Number of Chip: " << nChips << endl;
			cout<<"Number of Pixel per Chip: " << nPixels << endl;
			cout<<"Number of Samples per Pixel: " << nSamples << endl;
			cout<<"Number of Frame: " << nFrames << endl;
        
			
        
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
						if(pd1!=NULL){
						histAdc->SetBinContent( iFrame+1, int(0.5*adcValue - meanPed[code]) );
						}else if(pd2!=NULL){
						histAdc->SetBinContent( iFrame+1, int(0.5*adcValue - meanPed[iChip]) );
						}
						
					}//end frame loop per sample point
					// fout->cd();
					mHistAdcVec[ code ]->Write();
				}//end pixel-on-chip loop
			}//end chip loop
			int canvasX=nTopMetalChips*400;
			int canvasY=400;
			cout<<"nTopMetalChips "<<nTopMetalChips<<endl;
			TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "ADC map per chip", canvasX, canvasY);
			ca_adcMap->Divide(nTopMetalChips, 1, 0.01, 0.);
        
			std::vector< TH1S* >::iterator mHistAdcVecIter;
			for(int iFrame=0; iFrame<nFrames; iFrame++) {
			//	cout << "TopMetal II ADC Map at Frame " << iFrame << endl;
            
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
                
					// if(iChip==1 && iColumn==20 && iRow==14) continue;
					// if(iChip==3 && iColumn==54 && iRow==31) continue;
					// if(iChip==3 && iColumn==56 && iRow==20) continue;
					// if(iChip==3 && iColumn==57 && iRow==19) continue;
					// if(iChip==3 && iColumn==59 && iRow==29) continue;
					// if(iChip==3 && iColumn==60 && (iRow==19 || iRow==23)) continue;
					// if(iChip==3 && iColumn==61 && iRow==27) continue;
					// if(iChip==3 && iColumn==62 && iRow==33) continue;
					// if(iChip==3 && iColumn==63 && iRow==24) continue;
					// if(iChip==3 && iColumn==64 && iRow==20) continue;
					// if(iChip==3 && iColumn==65 && iRow==24) continue;
                
					float rawAdc= histAdc->GetBinContent(iFrame+1);
					//if(rawAdc>mAdcCut*rmsPed[channelIdx])
						// hAdcMap[iChip]->SetBinContent(iRow+1, iColumn+1, (int)rawAdc);
						hAdcMap[iChip]->SetBinContent(72-iPixel%72,iPixel/72+1, (int)rawAdc);
                
					if(debug==2)
						cout << iChip << ' ' << iPixel << ' ' << iColumn << ' ' << iRow << ' ' << rawAdc << endl;
				}
            
				for(int iC=0; iC<nChips; iC++) {
					ca_adcMap->cd(iC+1);
					hAdcMap[iC]->SetStats(kFALSE);
					hAdcMap[iC]->Draw("COLZ");
					hAdcMap[iC]->SetMaximum(80);
					hAdcMap[iC]->SetMinimum(-20);
					ca_adcMap->cd(iC+1)->Modified();
					ca_adcMap->cd(iC+1)->Update();
				}
            
				// if(gSystem->ProcessEvents()) break;
            
				if(debug==1 && (iFrame+1)%100==1) {
					ca_adcMap->cd();
					// ca_adcMap->Print(Form("adcMap_ZS_data_frame%d.png", iFrame));
				}
			} //end frame loop
		//}//end data type selection

		//clear memory
		//while (!mHistAdcVec.empty()) 
			delete mHistAdcVec.back(), mHistAdcVec.pop_back();
		
		fout->Write();
		fout->Close();
		delete []meanPed;
		delete []rmsPed;
		
	}
	
};

class pd1Pede{
	public:
    float meanPed[41472];
    float rmsPed[41472];
    void setup(char *a){
		
const int nPixelsOnChip = 5184;		
    std::ifstream inPD(a);
    if(!inPD.is_open()) {
        cout << "No pedestal table found!" << endl;
        return ;
    }
    
    int iChipT = 0, iPixelT = 0, iCounter=0;
    float pedestalT = 0., noiseT = 0.;
    while(!inPD.eof() && iCounter<41472) {
        inPD >> iChipT >> iPixelT >> pedestalT >> noiseT;
        meanPed[iChipT*nPixelsOnChip+iPixelT] = pedestalT;
        rmsPed[iChipT*nPixelsOnChip+iPixelT] = noiseT;
        iCounter++;
    }		
		
	}	
	
};

class pd1ChaDraw{
	public:
	
placData_1 *pd1;
fftAna *fft;

fftc2r *fftb;
vector <int> highFre;
pd1Pede pd1p;
int debugVerbose;//0 1 2
pd1ChaDraw(){
	debugVerbose=-1;
}
~pd1ChaDraw(){
	
	delete fft;
	delete fftb;
}

void setup(placData_1 *_pd1,char *inputPede){
	pd1=_pd1;
	pd1p.setup(inputPede);
    fft=new fftAna(pd1->nPix());
	fft->dt=(1.0/25.0)*(1e-6)*8*pd1->samPerPix();//sample time
    fftb=new fftc2r(pd1->nPix());
	
}

void draw(int start=0,int nf=1){
	TCanvas *c=new TCanvas("c","c",700,600);
	TH2D *h=new TH2D("h","h",72,0,72,72,0,72);
	for(int frame=start;frame<start+nf;frame++)
	{
		for(int i=0;i<pd1->nPix();i++)
		{
			// cha.dat[i]=double(pd2.chaDat[i*2]);
			h->SetBinContent(72-i%72,i/72+1,pd1->chaDat[frame*pd1->nPix()*pd1->samPerPix()+i*pd1->samPerPix()]);
			// if(i<10)cout<<i<<" "<<cha.dat[i]<<endl;
		}
		h->SetStats(0);
		h->Draw("colz");
		// if(frame==start)c->SaveAs("frame0.gif");
		// else if(frame>start){
			// c->SaveAs("frame0.gif+");
		// }
	}	
	
}


void fftDraw(){
	TFile *f=new TFile("fftAna.root","recreate");
	fft->photo("x","ma","frma");
	fft->photo("fr","ps","frps");
	fftb->photo("fr","ma","frmaBack");
	f->Write();
	f->Close();
}

void subPede(int iChip){
        const int nChips   = (int)pd1->adcCha();
        const int nPixels  = (int)pd1->nPix();
        const int nSamples = (int)pd1->samPerPix();
        const int nFrames  = (int)pd1->nFrame();	
		if(debugVerbose==0||debugVerbose==1||debugVerbose==2||debugVerbose>2){
			cout<<"Number of Chip: " << nChips << endl;
			cout<<"Number of Pixel per Chip: " << nPixels << endl;
			cout<<"Number of Samples per Pixel: " << nSamples << endl;
			cout<<"Number of Frame: " << nFrames << endl;
		}

        for(int iFrame=0; iFrame<nFrames; iFrame++) { //0 - 1617            
            for(int iPixel=0; iPixel<nPixels; iPixel++) { //0 - 5184
               for(int iSample=0; iSample<nSamples; iSample++){ //0 - 1      
                     pd1->chaDat[iFrame*nPixels*nSamples+iPixel*nSamples+iSample]-=pd1p.meanPed[iChip*nPixels+iPixel];
					 
				}
            }
                    // histAdc->SetBinContent( iFrame+1, int(0.5*adcValue - meanPed[code]) );
		}//end frame loop per sample point
        
		
	
}
void plusPede(int iChip){
	const int nChips   = (int)pd1->adcCha();
	const int nPixels  = (int)pd1->nPix();
	const int nSamples = (int)pd1->samPerPix();
	const int nFrames  = (int)pd1->nFrame();	
	for(int iFrame=0; iFrame<nFrames; iFrame++) { //0 - 1617            
		for(int iPixel=0; iPixel<nPixels; iPixel++) { //0 - 5184
		   for(int iSample=0; iSample<nSamples; iSample++){ //0 - 1      		   
			 pd1->chaDat[iFrame*nPixels*nSamples+iPixel*nSamples+iSample]+=pd1p.meanPed[iChip*nPixels+iPixel];
					 
			 }
		}
                    // histAdc->SetBinContent( iFrame+1, int(0.5*adcValue - meanPed[code]) );
	}//end frame loop per sample point
        
		
	
}


void supressTexture(float t,char* fftFreIndexFile,int flag=0){
	for(int frame=0;frame<pd1->nFrame();frame++){
		
		// if(frame>0) break;
		if(debugVerbose==1||debugVerbose==2||debugVerbose>2){
			if(frame%100==0)cout<<"fft on frame "<<frame<<endl;
		}
		
		fftDataTrans(pd1->nPix()*pd1->samPerPix()*frame);
	    fft->transform();	

	    fft->getComplex(fftb->dat,fftb->dati);
		
		if(frame==0){
			if(flag==0){
				//fftDraw();	
		// fft->photo("fr","ma","frma");
				if(debugVerbose==0||debugVerbose==1||debugVerbose==2||debugVerbose>2){
					cout<<"Create fft frequency index file "<<fftFreIndexFile<<endl;
				}
				if(debugVerbose>=2){
					fftDraw();
				}	
				fftPassThresholdCreateFile(t,fftFreIndexFile);//create fft frequency index file
			}
			fftPassThresholdReadFile(fftFreIndexFile);
			// fftPassThreshold(t);
		}
		
		fftZero();	
		fftb->transform();
		fftDataTrans2ChaDat(pd1->nPix()*pd1->samPerPix()*frame);
	}
	
}

void fftDataTrans(int offset){
	for(int i=0;i<fft->N;i++){
		fft->dat[i]=(pd1->chaDat[offset+i*2]+pd1->chaDat[offset+i*2+1])/2.0;
	}
}

void fftDataTrans2ChaDat(int offset){

	for(int i=0;i<fft->N;i++){
		fftb->get(fftb->dat);
		pd1->chaDat[offset+i*2]=fftb->dat[i]/fftb->N;
		pd1->chaDat[offset+i*2+1]=fftb->dat[i]/fftb->N;
		
	}
	// cout<<endl;
}


void fftSinData(float A, float fr,float phi){
	for(int i=0;i<fft->N;i++){
		fft->dat[i]=A*sin(2*TMath::Pi()*fr*fft->dt*i+phi);
	}	
}

void fftPassThreshold(float t){
	highFre.clear();
	for(int i=0;i<fft->N;i++){
		if(fft->ma[i]>t*fft->N){
		// if(fft->ps[i]>t*fft->N){
		// cout<<"fr="<<fft->fr[i]<<" "<<"ma="<<fft->ma[i]<<" "<<"ph="<<fft->ph[i]<<endl;
		highFre.push_back(i);
		
		}
	}		
	
}
void fftPassThresholdCreateFile(float t,char *fftFreIndexFile){
	highFre.clear();
	std::ofstream fout_fftFre(fftFreIndexFile, std::ios_base::out & std::ios_base::trunc);
	int count=0;
	for(int i=100;i<fft->N;i++){
	// for(int i=10;i<fft->N;i++){
		// if(fft->ma[i]>t*fft->N){
		if(fft->ma[i]>t*fft->N){
			if(debugVerbose==2||debugVerbose>2){
				cout<<i<<" "<<"fr="<<fft->fr[i]<<" "<<"ma="<<fft->ma[i]<<" "<<"t="<<t<<" fft->N="<<fft->N<<endl;
			}
		highFre.push_back(i);
		fout_fftFre<<i<<endl;
		count++;
		// cout<<"Frequency "<<i<<endl;
		}
	}		
	fout_fftFre.close();
}
void setChaDatToPd1(int chip){
	unsigned short *ps = (unsigned short *)pd1->p;
	for(int j=0;j<pd1->nChaDat;j++)
	{
		ps[pd1->adcCha()*j+chip]=(unsigned short)pd1->chaDat[j];
	}
}
void run(char *inputPede,char *inoutfftFreIndex,char *outFFTPD1,float threshold,int flag=0){
	// setup(&pd1,inputPede);
	char outFFTIdex[200];
	for(int i=0;i<pd1->adcCha();i++){
	// for(int i=0;i<1;i++){
		pd1->getChaDat(i);
		sprintf(outFFTIdex,"%s_%d.txt",inoutfftFreIndex,i);
		subPede(i);
		cout<<"threshold "<<threshold<<endl;
		
		supressTexture(threshold,outFFTIdex,flag);//threshold fft frequency index file, flag of if create a fft frequency index file
		plusPede(i);
		// cout<<"fft->N; "<<fft->N<<endl;
		
		setChaDatToPd1(i);
		
		// draw(10,100);
	}
	pd1->write(outFFTPD1);
	
}
void fftPassThresholdReadFile(char *fftFreIndexFile){
	highFre.clear();
	std::ifstream fin_fftFre(fftFreIndexFile);
	if(!fin_fftFre.is_open()) {
        cout << "No fft frequency pass threshold file found!" << endl;
        return;
    }
	int fftFreIndex=0;
	int datIndex=0;
	while(!fin_fftFre.eof()) {//read fft frequncy index file
        fin_fftFre >> fftFreIndex;
		highFre.push_back(fftFreIndex);
		if(datIndex==0) cout<<"Read file "<<fftFreIndexFile<<" "<<fftFreIndex<<endl;
		datIndex++;
    }	
	fin_fftFre.close();
}

void fftZero(){
	for(int i=0;i<(int)highFre.size();i++){
		fftb->dat[highFre[i]]=0;
		fftb->dati[highFre[i]]=0;
	}		
	
}

};

class polNPar{
	public:
		placData_1 *pd1;
		float pede;	
		float rms;	
		int maxi;
		int stop;
		int debugVerbose;
		std::vector<int>   	*chipIndex;//
		std::vector<int>   	*pixelIndex;
		std::vector<int>   	*pointNumber;
		std::vector<int>	*pixelPointStart;
		
		std::vector<float> *pedeVector;
		std::vector<float> *pedeRMSVector;
		std::vector<float> *pixelWaveDat;
		std::vector<int> *pixelWaveDatIndex;
		std::vector<float> *polNParamter;
		std::vector<float> *polNParamterRev;
		std::vector<float> *polNPointCharge;
		std::vector<float> *polNTotalCharge;
		int polFitParNum;
		
void setup(placData_1 *_pd1){
	pd1=_pd1;
}	
void vectorSetup(){
	chipIndex    =new vector<int>;
	pixelIndex   =new vector<int>;
	pointNumber  =new vector<int>;
	pixelPointStart  =new vector<int>;

	
	//polN fit dat and paramter
	pedeVector =new vector<float>;
	pedeRMSVector =new vector<float>;
	pixelWaveDat=new vector<float>;
	polNParamter=new vector<float>;
	pixelWaveDatIndex=new vector<int>;
	polNParamterRev=new vector<float>;
	polNPointCharge=new vector<float>;
	polNTotalCharge=new vector<float>;
	polFitParNum=5;
	
}
void vectorClear(){
	chipIndex->clear();
	pixelIndex->clear();
	pointNumber->clear();
	pixelPointStart->clear();

	
	pedeVector->clear();
	pedeRMSVector->clear();
	pixelWaveDat->clear();//
	polNParamter->clear();//
	pixelWaveDatIndex->clear();//
	polNParamterRev->clear();//
	polNPointCharge->clear();//
	polNTotalCharge->clear();//
}
void vectorDelete(){
	delete chipIndex;
	delete pixelIndex;
	delete pointNumber;
	delete pixelPointStart;

	delete pixelWaveDat;
	delete polNParamter;
	delete pixelWaveDatIndex;
	delete polNParamterRev;
	delete pedeVector;
	delete pedeRMSVector;
	delete polNPointCharge;
	delete polNTotalCharge;
}
polNPar(){
	debugVerbose=-1;
	vectorSetup();
	vectorClear();
}
void averagePixDat(){
	float s;
	for(int i=0;i<pd1->nFrame();i++){
		s=0;
	for(int j=0;j<pd1->samPerPix();j++){
		s+=pd1->pixDat[pd1->samPerPix()*i+j];
	}
	pd1->pixDat[pd1->samPerPix()*i]=s/float(pd1->samPerPix());
	}	
}

float pv(int i){
	return pd1->pixDat[pd1->samPerPix()*i];
}

void drawPixel(int chip,int pixel){
	TCanvas *c=new TCanvas("c","c",700,600);
	int returnValue=pd1->getPixDat(chip,pixel);
	cout<<returnValue<<endl;
	if(returnValue!=0){
		cout<<"error !!!! chip "<<chip<<" pixel "<<pixel<<" does not exists"<<endl;
		return ;
	}
	float *x=new float[pd1->nFrame()];
	float *y=new float[pd1->nFrame()];
	for(int i=0;i<pd1->nFrame();i++){
		x[i]=i;
		y[i]=pd1->pixDat[pd1->samPerPix()*i];
	}
	
	TGraph *a=new TGraph(pd1->nFrame(),x,y);
	a->Draw();
	char *decayData =Form("png/decayDataChip%dPixel%d.png",chip,pixel);
	c->SaveAs(decayData);
	delete []x;
	delete []y;
	delete a;
	delete c;
}	

void getCurve(){
	int peakat=-1;
	for(int i=0;i<pd1->nFrame()-11;i++){
			peakat=i+10;		
		for(int j=0;j<9;j++){
		if(pv(i+10)-pv(i+j)<100){
			peakat=-1;
            break;
		}
		}
		
		if(peakat!=-1) break;

	}
	
	if(peakat==-1) {
		if(debugVerbose==1||debugVerbose==2||debugVerbose>2){
			cout<<"NO peak detected"<<endl;
		}
		// exit(0);
		maxi=0;
		stop=0;
		return;
	}
	
	pede=0;
	rms=0;
	for(int j=0;j<9;j++){
		pede+=pv(peakat-10+j);
		rms+=pv(peakat-10+j)*pv(peakat-10+j);
	}
	pede/=9.0;
	rms/=9.0;
	rms=sqrt(rms-pede*pede);


	float max=pv(peakat-2);
	maxi=peakat-2;
	for(int i=-2;i<=2;i++){
		if(pv(peakat+i)>=max){max=pv(peakat+i);maxi=peakat+i;}
	}	


	for(int i=maxi;i<pd1->nFrame()-11;i++){
		if(pv(i)<pede+4*rms) {
			stop=i;
			break;}	
	}
	// if(stop!=0&(stop-maxi<5)){stop=maxi+5;}//
}
void singlePixCurve(int chip=0,int pixel=10){
	pd1->getPixDat(chip,pixel);
	averagePixDat();
	getCurve();
	// if(stop-maxi+1<)
	float *x=new float[stop-maxi+1];
	float *y=new float[stop-maxi+1];
	for(int i=maxi;i<stop+1;i++)
	{
		x[i-maxi]=i;
		y[i-maxi]=pd1->pixDat[pd1->samPerPix()*i];
	}
}
void saveAllPixCurveDat(char *output){
	std::ofstream out(output,std::ios_base::out & std::ios_base::trunc);
	int badPixel=0;
	chipIndex->clear();
	pixelIndex->clear();
	pointNumber->clear();
	pixelPointStart->clear();
	pixelWaveDat->clear();
	pixelWaveDatIndex->clear();
	int ipoint=0;
	int pointStart=0;
	for(int ichip=0;ichip<pd1->adcCha();ichip++)
	{
		for(int ipixel=0;ipixel<pd1->nPix();ipixel++)
		{
			chipIndex->push_back(ichip);
			pixelIndex->push_back(ipixel);

			pd1->getPixDat(ichip,ipixel);
			averagePixDat();
			getCurve();
			
			if(stop==maxi){ipoint=0;}
			else{ipoint=stop-maxi+1;}
			pointNumber->push_back(ipoint);
			pixelPointStart->push_back(pointStart);
			pointStart+=ipoint;
			out<<ichip<<" "<<ipixel<<" "<<ipoint<<endl;
			if(stop-maxi+1<3)
			{
				badPixel++;
				// drawPixel(ichip,ipixel);
				// cout<<"ichip "<<ichip<<" ipixel "<<ipixel<<endl;
			}
			if(maxi==stop){
				
			}
			else {
				for(int i=maxi;i<stop+1;i++)
				{
					out<<pd1->pixDat[pd1->samPerPix()*i]<<" ";
					pixelWaveDat->push_back(pd1->pixDat[pd1->samPerPix()*i]);
					pixelWaveDatIndex->push_back(i-maxi);
				}
			}
			out<<endl;
		}
	}
	out.close();
	if(debugVerbose==0||debugVerbose==1||debugVerbose==2||debugVerbose>2){
		cout<<"Totally "<<badPixel<<" bad pixel"<<endl;
	}
}

void readDecayDat(char *input){
	std::ifstream fin(input);


	if(!fin.is_open()) {
		if(debugVerbose){
			cout << "No curve file found!" << endl;
		}
        return;
    }
	chipIndex->clear();
	pixelIndex->clear();
	pointNumber->clear();
	pixelPointStart->clear();
	pixelWaveDat->clear();
	pixelWaveDatIndex->clear();
	int ichip=0; int ipixel=0; int ipoint=0;
	float data=0; int pixel=0;	int pointStart=0;
	while(!fin.eof()) {//read curve file and do spline fit
        fin >> ichip >> ipixel >>ipoint;
		
		chipIndex->push_back(ichip);
		pixelIndex->push_back(ipixel);
		pointNumber->push_back(ipoint);
		pixelPointStart->push_back(pointStart);
       // if(pixel%1000==0) cout<< "bad ichip "<<ichip<< "ipixel " << ipixel<<" pointNum " <<ipoint<<endl;
		float *dat;
		float *index;
		int n=ipoint;
		if(ipoint>0){
			dat=new float[n];
			index=new float[n];
			for(int i=0;i<n;i++){dat[i]=0;index[i]=0;}
		}
		for(int i=0;i<ipoint;i++)
		{
			fin>>data;
			// cout<<data<<" ";
			dat[i]=data;
			index[i]=(float)i;
			// vectorClear();
			pixelWaveDat->push_back(dat[i]);
			pixelWaveDatIndex->push_back(index[i]);
			
		}
		if(ipoint>0){
			// draw(ipoint,index,dat);
			// float *par=new float[5];
			// polNFit(ipoint,index,dat,par,5);
			// return;
			delete []dat;
			delete []index;
		}
		// curveDat->Fill();
		pixel++;
		pointStart+=ipoint;
		if(pixel==pd1->adcCha()*pd1->nPix()){break;}
    }
	if(debugVerbose==1||debugVerbose==2||debugVerbose>2){	
		cout<<"total "<<pixel<<" pixels"<<endl;
	}

	fin.close();
}
void fitPolNDat(char *outParName){
	cout<<"chipIndex "<<chipIndex->size()<<endl;
	cout<<"pixelIndex "<<pixelIndex->size()<<endl;
	cout<<"pointNumber "<<pointNumber->size()<<endl;
	cout<<"pixelWaveDat "<<pixelWaveDat->size()<<endl;
	cout<<"pixelWaveDatIndex "<<pixelWaveDatIndex->size()<<endl;
	std::ofstream foutPar(outParName,std::ios_base::out & std::ios_base::trunc);
	const int polFitParN=polFitParNum;//pol5 p0~p5
	float *par=new float[polFitParN+1];
	int datRange=0;
	for(int i=0;i<(int)chipIndex->size();i++)
	{
		int temp=(*pointNumber)[i];
		if(temp>0){
			float *dat=new float[temp];
			float *index=new float[temp];
			datRange+=temp;
			for(int j=0;j<temp;j++){
				index[j]=(float)(*pixelWaveDatIndex)[datRange-temp+j];
				dat[j]=(float)(*pixelWaveDat)[datRange-temp+j];
			}
			polNFit(temp,index,dat,par,polFitParN);
			for(int k=0;k<polFitParN+1;k++)
			{
				polNParamter->push_back(par[k]);
				foutPar<<par[k]<<" ";
			}
			foutPar<<endl;
			delete []dat;
			delete []index;
		}
		if(temp<=0){
			// cout<<"temp<=0 "<<temp<<endl;
			for(int k=0;k<polFitParN+1;k++)
			{
				polNParamter->push_back(0);
				foutPar<<0<<" ";
			}
			foutPar<<endl;
		}
	}
	foutPar.close();
	delete []par;
}
void fitPolNDatRev(char *outParNameRev){
	cout<<""<<chipIndex->size()<<endl;
	cout<<""<<pixelIndex->size()<<endl;
	cout<<""<<pointNumber->size()<<endl;
	cout<<""<<pixelWaveDat->size()<<endl;
	cout<<""<<pixelWaveDatIndex->size()<<endl;
	std::ofstream foutParRev(outParNameRev,std::ios_base::out & std::ios_base::trunc);
	const int polFitParN=polFitParNum;
	float *par=new float[polFitParN+1];
	int datRange=0;
	for(int i=0;i<(int)chipIndex->size();i++)
	{
		int temp=(*pointNumber)[i];
		// cout<<"temp "<<temp<<endl;
		if(temp>0){
			// cout<<"temp>0 "<<temp<<endl;
			float *dat=new float[temp];
			float *index=new float[temp];
			datRange+=temp;
			for(int j=0;j<temp;j++){
				dat[j]=(float)(*pixelWaveDatIndex)[datRange-temp+j];
				index[j]=(float)(*pixelWaveDat)[datRange-temp+j];
			}
			polNFit(temp,index,dat,par,polFitParN);
			for(int k=0;k<polFitParN+1;k++)
			{
				polNParamterRev->push_back(par[k]);
				foutParRev<<par[k]<<" ";
			}
			foutParRev<<endl;
			delete []dat;
			delete []index;
		}
		if(temp<=0){
			// cout<<"temp<=0 "<<temp<<endl;
			for(int k=0;k<polFitParN+1;k++)
			{
				polNParamterRev->push_back(0);
				foutParRev<<0<<" ";
			}
			foutParRev<<endl;
		}
	}
	foutParRev.close();
	delete []par;
}
float GetGuardRingPixPolNQ(int ichip,int ipixel,float baseline=0,bool debug=false){
	if(debug==false){
		polNPointCharge->clear();
	}
	const int fitPar=polFitParNum+1;
	float *par=new float[fitPar];
	float *parDer=new float[fitPar];
	float *parRev=new float[fitPar];
	float *parRevDer=new float[fitPar];
	for(int i=0;i<fitPar;i++){
		int j=ichip*pd1->nPix()*fitPar+ipixel*fitPar+i;
		par[i]=(*polNParamter)[j];
		parRev[i]=(*polNParamterRev)[j];
		// cout<<"par "<<par[i]<<" parRev"<<parRev[i]<<endl;
	}
	for(int i=0;i<fitPar;i++){
		parDer[i]=float(i)*par[i];// polN deri	
		parRevDer[i]=float(i)*par[i];	
	}
	int pointStart=0;
	int datRange=0;
	int pointN=(*pointNumber)[ichip*pd1->nPix()+ipixel];
	pointStart=(*pixelPointStart)[ichip*pd1->nPix()+ipixel];
	float *x=new float[pointN];
	float *y=new float[pointN];
	if(pointN>0){
		for(int j=0;j<pointN;j++){
			x[j]=(float)(*pixelWaveDatIndex)[pointStart+j];
			y[j]=(float)(*pixelWaveDat)[pointStart+j];
		}
	}
	
	float *chargeQ=new float[pointN];
	polNFit(pointN,x,y,par,polFitParNum);
	// polNFit(stop-maxi+1,x,y,par,4);
	// polNFit(stop-maxi+1,y,x,parRev,4);
	// polNFit(stop-maxi+1,y,x,parRev,polFitParNum);
	float position=0;
	float valueDecayInExpection=0;
	float totalCharge=0;
	for(int i=0;i<pointN;i++){
		
		if(i==0)chargeQ[i]=y[i]-y[pointN-1];//
		// if(i==0)chargeQ[i]=y[i]-baseline;//
		
		// cout<<"position "<<position<<endl;
		if(i>0){
			position=polValue(parRev,y[i-1]);
			valueDecayInExpection=polValue(par,position+1);
			chargeQ[i]=y[i]-valueDecayInExpection;
			// cout<<"y["<<i<<"] "<<y[i]<<" valueDecayInExpection "<<valueDecayInExpection<<endl;
		}
		totalCharge+=chargeQ[i];
	if(debug==false){
		polNPointCharge->push_back(chargeQ[i]);
	}
		
	}
	// cout<<"total charge is "<<totalCharge<<endl;
	if(totalCharge<0&ichip==0)
	{
		// char name[100];
		// sprintf(name,"chip%d_pixel%d_dat",ichip,ipixel);
		// draw(stop-maxi+1,x,y,name);
		// sprintf(name,"chip%d_pixel%d_Charge",ichip,ipixel);
		// draw(stop-maxi+1,x,chargeQ,name);
		// cout<<"ichip "<<ichip<<" ipixel "<<ipixel<<" totalCharge "<<totalCharge<<endl;
	}
	// 
	delete []par;
	delete []parDer;
	delete []parRev;
	delete []parRevDer;
	delete []x;
	delete []y;
	
	return totalCharge;
}

void GetGuardRingPolNQ(char *pedefile,char *chargeOut){
	polNTotalCharge->clear();
	loadPede(pedefile);//load pede to vector<flaot> *pede
	std::ofstream foutCharge(chargeOut,std::ios_base::out & std::ios_base::trunc);
	// TFile *fout=new TFile("polQ.root","recreate");
	cout<<"chargeOut "<<chargeOut<<endl;
	float charge=0;
	for(int ichip=0;ichip<pd1->adcCha();ichip++)
	{
		for(int ipixel=0;ipixel<pd1->nPix();ipixel++)
		{
			int pedePos=ichip*pd1->nFrame()+ipixel;
			charge=GetGuardRingPixPolNQ(ichip,ipixel,0);
			if(debugVerbose>2){
				if(ipixel%1000==0)cout<<"ichip "<<ichip<<" ipixel "<<ipixel<<"  pede "<<(*pedeVector)[pedePos]<<endl;
			}
			polNTotalCharge->push_back(charge);
			foutCharge<<ichip<<" "<<ipixel<<" "<<charge<<endl;
		}
	}
	// fout->Write();
	// fout->Close();
	foutCharge.close();
}

void polNFit(int n,float *x,float *y,float *par,int nPar=5){//save fit paramters in par
	// TCanvas c("c","c",700,600);
	TGraph *gr=new TGraph(n,x,y);
	// gr->SetMarkerStyle(20);
	// gr->SetMarkerColor(1);
	gr->Draw("apl*");
	char func[100];
	float start=x[0];
	float stop=x[n-1];
	sprintf(func,"pol%d",nPar);
	TF1 fit("fit",func,start,stop);
	// f->SetParameters(10,1,1);
	gr->Fit(&fit,"QMR");
	for(int i=0;i<nPar+1;i++){
		par[i]=fit.GetParameter(i);
		// cout<<"par["<<i<<"] "<< par[i]<<endl;
	}
	// char name[100];
	// sprintf(name,".png");
	// c.SaveAs();
	// delete gr;
}
void loadPede(char *pedefile){
	std::ifstream pedeOpen(pedefile);
	if( pedeOpen==NULL ){
		cout<<"error in open "<<pedefile<<endl;
		cout<<"please check if "<<pedefile<<" exists"<<endl;
		return ;
	}
	pedeVector->clear();
	pedeRMSVector->clear();
	int ichip=0;
	int ipixel=0;
	float pedeValue=0;
	float pedeRMS=0;
	int count=0;
	while(!pedeOpen.eof()){
		pedeOpen >> ichip >> ipixel >> pedeValue >>pedeRMS;
		pedeVector->push_back(pedeValue);
		pedeRMSVector->push_back(pedeRMS);
		count++;
		if(count==pd1->adcCha()*pd1->nPix()){break;}
	}
	pedeOpen.close();
}
float polValue(float *par,float x){
	float value=0;
	for(int i=0;i<polFitParNum+1;i++){
		value+=par[i]*TMath::Power(x,i);
	}
	return value;
}
void drawGuardRing2DCharge(char *chargeIn,char *outRoot){
	std::ifstream chargeOpen(chargeIn);
	cout<<"input "<<chargeIn<<endl;
	TFile *f=new TFile(outRoot,"recreate");
	int ichip=0;
	int ipixel=0;
	float charge=0; 
	TCanvas *c=new TCanvas("c","c",4000,800);
	c->Divide(8,2);
	TString histName;
	TString histName1;
	char title[100];
	char title1[100];
	TH2D *h[pd1->adcCha()];
	TH1D *h1[pd1->adcCha()];
	for(int i=0;i<pd1->adcCha();i++){
        histName = "Channel"; histName += i;
        histName1 = "1d_Channel"; histName1 += i;
        sprintf(title,"Channel %d Charge Distribution",i);
        sprintf(title1,"Channel %d Charge Distribution 1d",i);
        h[i] = new TH2D(histName, title, 72, 0, 72, 72, 0, 72);
        h1[i] = new TH1D(histName1, title1, 2500,0,2000);
	}
	int count=0;
	int chargeLessZero=0;
	int chargeLarge=0;
	while(!chargeOpen.eof()){
		chargeOpen >> ichip >> ipixel >>charge;
		// cout<<"ichip "<<ichip<<endl;
		h[ichip]->SetBinContent(72-ipixel%72,ipixel/72+1,charge);
		h1[ichip]->Fill(charge);
		if(charge<0){
			chargeLessZero++;
			if(chargeLessZero<100){
				cout<<"ichip "<<ichip<<" ipixel "<<ipixel<<" charge "<<charge<<endl;
			}
		}
		if(charge>1000){
			chargeLarge++;
			if(chargeLarge<100){
				cout<<"ichip "<<ichip<<" ipixel "<<ipixel<<" charge "<<charge<<endl;
			}
		}
		count++;
		if(count==pd1->adcCha()*pd1->nPix()){break;}
	}
	cout<<"read data ready "<<endl;
	cout<<"large number is "<<chargeLarge<<endl;
	cout<<"charge less zero "<<chargeLessZero<<endl;
	for(int i=0;i<pd1->adcCha();i++){
		c->cd(i+1);
		h[i]->Draw("colz");
		h[i]->GetZaxis()->SetRangeUser(0,999);
        h[i]->Draw("colz");
		h[i]->SetStats(0);
		
		c->cd(i+pd1->adcCha()+1);
		h1[i]->Draw();
		
		c->Modified();
        c->Update();
		// h[i]->Write();
		// h1[i]->Write();
	}
	// c->Modified();
	// c->Update();
	c->SaveAs("charge.pdf");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	chargeOpen.close();
	c->Write();
	f->Write();
	f->Close();
}

void draw(int n,float *x,float *y,char *nameIn,bool writeif=false){
	// TCanvas *c=new TCanvas("c","c",700,600);
	TGraph *gr=new TGraph(n,x,y);
	gr->Draw("apl*");
	gr->SetName(nameIn);
	if(writeif==true)gr->Write();
	// char name[100];
	// sprintf(name,"%s.png",nameIn);
	// c->SaveAs(name);
}

void run(char *decayData,char *polPar,char *polParRev){
	saveAllPixCurveDat(decayData);
	// readDecayDat(decayData);
	fitPolNDat(polPar);
	fitPolNDatRev(polParRev);
}
void guardRingRun(char *pedeFile,char *polNCharge,char *guardRingRoot){
	GetGuardRingPolNQ(pedeFile,polNCharge);
	drawGuardRing2DCharge(polNCharge,guardRingRoot);
}
~polNPar()
{
	vectorClear();
	vectorDelete();
}
};




class polNGetCharge{
		public:
		placData_1 *pd1;
		int debugVerbose;
		// placData_3 pd3;
		
		// std::vector<int>   	*chipIndex;//
		// std::vector<int>   	*pixelIndex;
		// std::vector<int>   	*pointNumber;
		// std::vector<int>	*pixelPointStart;
		
		std::vector<float> *pedeVector;
		std::vector<float> *pedeRMSVector;
		std::vector<float> *pixelWaveDat;
		std::vector<int> *pixelWaveDatIndex;
		std::vector<float> *polNParamter;
		std::vector<float> *polNParamterRev;
		std::vector<float> *polNPointCharge;
		std::vector<float> *polNTotalCharge;
		int polFitParNum;
		
void setup(placData_1 *_pd1){
	pd1=_pd1;
}	
void vectorSetup(){
	//polN fit dat and paramter
	pedeVector =new vector<float>;
	pedeRMSVector =new vector<float>;
	pixelWaveDat=new vector<float>;
	polNParamter=new vector<float>;
	pixelWaveDatIndex=new vector<int>;
	polNParamterRev=new vector<float>;
	polNPointCharge=new vector<float>;
	polNTotalCharge=new vector<float>;
	polFitParNum=5;
	
}
void vectorClear(){
	
	pedeVector->clear();
	pedeRMSVector->clear();
	pixelWaveDat->clear();//
	polNParamter->clear();//
	pixelWaveDatIndex->clear();//
	polNParamterRev->clear();//
	polNPointCharge->clear();//
	polNTotalCharge->clear();//
}
void vectorDelete(){
	delete pixelWaveDat;
	delete polNParamter;
	delete pixelWaveDatIndex;
	delete polNParamterRev;
	delete pedeVector;
	delete pedeRMSVector;
	delete polNPointCharge;
	delete polNTotalCharge;
}
polNGetCharge(){
	debugVerbose=-1;
	vectorSetup();
	vectorClear();
}
void averagePixDat(){
	float s;
	for(int i=0;i<pd1->nFrame();i++){
		s=0;
	for(int j=0;j<pd1->samPerPix();j++){
		s+=pd1->pixDat[pd1->samPerPix()*i+j];
	}
	pd1->pixDat[pd1->samPerPix()*i]=s/float(pd1->samPerPix());
	}	
}
void loadPolNPar(char *outParName){
	std::ifstream polPar(outParName);
	if( polPar==NULL ){
		cout<<"error in open "<<outParName<<endl;
		cout<<"please check if "<<outParName<<" exists"<<endl;
		return ;
	}
	polNParamter->clear();
	float *par=new float[polFitParNum+1];
	int count=0;
	while(!polPar.eof()){
		for(int i=0;i<polFitParNum+1;i++){
			polPar >> par[i];
			polNParamter->push_back(par[i]);
		}
		count++;
		if(count==pd1->adcCha()*pd1->nPix()){break;}
	}
	cout<<"count "<<count<<endl;
	polPar.close();
	delete []par;
}

void loadPolNParReV(char *outParNameRev){
	std::ifstream polParRev(outParNameRev);
	if( polParRev==NULL ){
		cout<<"error in open "<<outParNameRev<<endl;
		cout<<"please check if "<<outParNameRev<<" exists"<<endl;
		return ;
	}
	
	polNParamterRev->clear();
	float *par=new float[polFitParNum+1]; //polN has N+1 pars
	int count=0;
	while(!polParRev.eof()){
		for(int i=0;i<polFitParNum+1;i++){
			polParRev >> par[i];
			polNParamterRev->push_back(par[i]);
		}
		count++;
		if(count==pd1->adcCha()*pd1->nPix()){break;}
	}
	polParRev.close();
	cout<<"count "<<count<<endl;
	delete []par;
	
}
void loadPede(char *pedefile){
	std::ifstream pedeOpen(pedefile);
	if( pedeOpen==NULL ){
		cout<<"error in open "<<pedefile<<endl;
		cout<<"please check if "<<pedefile<<" exists"<<endl;
		return ;
	}
	pedeVector->clear();
	pedeRMSVector->clear();
	int ichip=0;
	int ipixel=0;
	float pedeValue=0;
	float pedeRMS=0;
	int count=0;
	while(!pedeOpen.eof()){
		pedeOpen >> ichip >> ipixel >> pedeValue >>pedeRMS;
		pedeVector->push_back(pedeValue);
		pedeRMSVector->push_back(pedeRMS);
		count++;
		if(count==pd1->adcCha()*pd1->nPix()){break;}
	}
	pedeOpen.close();
}
void sefPede(char *out){
	pedeVector->clear();
	pedeRMSVector->clear();
	std::ofstream foutCharge(out,std::ios_base::out & std::ios_base::trunc);
	float *pixDat=new float[pd1->nFrame()];
	float aver;
	float rms;
	for(int ichip=0;ichip<pd1->adcCha();ichip++){
		for(int ipixel=0;ipixel<pd1->nPix();ipixel++){
			pd1->getPixDat(ichip,ipixel);
			aver=0;
			rms=0;
			float s;
			for(int frame=0;frame<pd1->nFrame();frame++){
				s=0;
				for(int j=0;j<pd1->samPerPix();j++){
					s+=pd1->pixDat[pd1->samPerPix()*frame+j];
				}
				pixDat[frame]=s/float(pd1->samPerPix());
				if(frame<190){
					aver+=pixDat[frame];
					rms+=pixDat[frame]*pixDat[frame];
				}
			}
			aver/=190.0;
			rms/=190.0;
			rms=sqrt(rms-aver*aver);
			foutCharge << ichip <<" "<<ipixel << " "<< aver << " "<< rms<<endl;
			pedeVector->push_back(aver);
			pedeRMSVector->push_back(rms);
		}
	}
	foutCharge.close();
	
}
float GetPhyPixPolNQ(int ichip,int ipixel,float baseline=0, float baselineRMS=0,bool vectorClear=false){
	if(vectorClear==true){
		polNPointCharge->clear();
	}
	pd1->getPixDat(ichip,ipixel);
	averagePixDat();
	const int fitPar=polFitParNum+1;
	float *par=new float[fitPar];
	float *parDer=new float[fitPar];
	float *parRev=new float[fitPar];
	float *parRevDer=new float[fitPar];
	float *y=new float[pd1->nFrame()];//save data averaged
	float *tempIndex=new float[pd1->nFrame()];
	float *chargeQ=new float[pd1->nFrame()];
	for(int i=0;i<fitPar;i++){
		int j=ichip*pd1->nPix()*fitPar+ipixel*fitPar+i;
		par[i]=(*polNParamter)[j];
		parRev[i]=(*polNParamterRev)[j];
		if(debugVerbose==1||debugVerbose==2||debugVerbose>2){
			cout<<"par "<<par[i]<<" parRev"<<parRev[i]<<endl;
		}
	}
	for(int i=0;i<fitPar;i++){
		parDer[i]=float(i)*par[i];// polN deri	
		parRevDer[i]=float(i)*par[i];	
	}
	for(int i=0;i<pd1->nFrame();i++){
		tempIndex[i]=(float)i;
		y[i]=pd1->pixDat[pd1->samPerPix()*i];
		// if(i%100==0){cout<<"y "<<y[i]<<endl;}
	}
	// polNFit(pd1->nFrame(),tempIndex,y,par,polFitParNum);
	// polNFit(stop-maxi+1,y,x,parRev,polFitParNum);
	float position=0;
	float valueDecayInExpection=0;
	float totalCharge=0;
	for(int i=0;i<pd1->nFrame();i++){
		if(i==0){
			chargeQ[i]=y[i]-baseline;//
			// chargeQ[i]=y[i]-y[pointN-1];//
		}
		// cout<<"position "<<position<<endl;
		if(i>0){
			if(y[i-1]-baseline<4*baselineRMS){
				valueDecayInExpection=baseline;
			}
			else{
				position=polValue(parRev,y[i-1]);
				valueDecayInExpection=polValue(par,position+1);
			}
			chargeQ[i]=y[i]-valueDecayInExpection;
		}
		// cout<<" charge is "<<chargeQ[i]<<endl;
		totalCharge+=chargeQ[i];
	}
	//save charge to vector polNPointCharge->push_back()
	if(vectorClear==false){
		for(int i=0;i<pd1->nFrame();i++){
			polNPointCharge->push_back(chargeQ[i]);
		}
	}
	
	// draw(pd1->nFrame(),tempIndex,chargeQ,"test");
	// draw(pd1->nFrame(),tempIndex,y,"test");
	// cout<<"totalCharge "<<totalCharge<<endl;
	delete []par;
	delete []parDer;
	delete []parRev;
	delete []parRevDer;
	delete []y;
	delete []tempIndex;
	
	return totalCharge;
}
void GetPhyPolNQ(char *pedefile,char *chargeOut){
	polNTotalCharge->clear();
	polNPointCharge->clear();
	loadPede(pedefile);//load pede to vector<flaot> *pede
	std::ofstream foutCharge(chargeOut,std::ios_base::out & std::ios_base::trunc);

	cout<<"chargeOut "<<chargeOut<<endl;
	float charge=0;
	for(int ichip=0;ichip<pd1->adcCha();ichip++)
	{
		for(int ipixel=0;ipixel<pd1->nPix();ipixel++)
		{
			int pedePos=ichip*pd1->nPix()+ipixel;
			// charge=GetPhyPixPolNQ(ichip,ipixel,445,0);
			charge=GetPhyPixPolNQ(ichip,ipixel,(*pedeVector)[pedePos],(*pedeRMSVector)[pedePos]);
			if(ipixel%1000==0)cout<<"ichip "<<ichip<<" ipixel "<<ipixel<<"  pede "<<(*pedeVector)[pedePos]<<" pedeRMS "<<(*pedeRMSVector)[pedePos]<<endl;
			polNTotalCharge->push_back(charge);
			foutCharge<<ichip<<" "<<ipixel<<" "<<charge<<endl;
		}
	}
	foutCharge.close();

}
void savePlacData_3(char *file){
	
	// pd1->p;
	int vectorSize=polNPointCharge->size();
	cout<<"vectorSize "<<vectorSize<<endl;
	int dataSize=vectorSize*sizeof(float);
	cout<<"dataSize "<<dataSize<<endl;
	int adcCha=pd1->adcCha();
	int nPix=pd1->nPix();
	int nFrame=pd1->nFrame();
	float frameTime=pd1->dt()*(float)nPix*16.0;//
	placData_3 pd3;
	cout<<"frameTime is "<<frameTime<<endl;
	cout<<"sizeof(float) is "<<sizeof(float)<<endl;
	pd3.setupHeader(dataSize,adcCha,frameTime,nPix,nFrame);
	cout<<"dataSize "<<dataSize<<endl;
	pd3.createMem(dataSize);
	// cout<<"pd1->p"<<&(pd1->p)<<endl;
	// pd3.p=pd1->p;
	// cout<<"pd1->p"<<(int*)(pd1->p)[100]<<endl;

	cout<<"vectorSize "<<vectorSize<<endl;
	for(int i=0;i<vectorSize;i++){
		((float   *)pd3.p)[i]=(*polNPointCharge)[i];
		// if(i%1000000==0)cout<<"dataSize"<<dataSize<<" "<<i*4<<" "<<((float   *)pd3.p)[i]<<endl;
	}
	pd3.nByte=dataSize;
	cout<<"pd3.nByte "<<pd3.nByte<<endl;
	pd3.write(file);
}

void savePhyPolNQ(char *pointOutTxt,char *pointOutRoot){// a text and a root file
	std::ofstream foutCharge(pointOutTxt,std::ios_base::out & std::ios_base::trunc);
	TFile *f=new TFile(pointOutRoot,"recreate");
	float *x=new float[pd1->nFrame()];
	float *y=new float[pd1->nFrame()];
	char name[100];
	int size=polNPointCharge->size();
	for(int ichip=0;ichip<pd1->adcCha();ichip++){
	// for(int ichip=0;ichip<1;ichip++){
		for(int ipixel=0;ipixel<pd1->nPix();ipixel++){
		// for(int ipixel=0;ipixel<10;ipixel++){
			foutCharge<< ichip <<" " << ipixel<<" "<<pd1->nFrame()<<" ";
			for(int frame=0;frame<pd1->nFrame();frame++){
				int position=ichip*pd1->nPix()*pd1->nFrame()+ipixel*pd1->nFrame()+frame;
				foutCharge << (*polNPointCharge)[position]<<" ";
				x[frame]=frame;
				y[frame]=(*polNPointCharge)[position];
			}
			foutCharge<<endl;
			sprintf(name,"ichip%d_ipixel%d",ichip,ipixel);
			draw(pd1->nFrame(),x,y,name,true);
		}
	}
	foutCharge.close();
	f->Write();
	f->Close();
	delete []x;
	delete []y;
}

void draw(int n,float *x,float *y,char *nameIn,bool writeif=false){
	// TCanvas *c=new TCanvas("c","c",700,600);
	TGraph *gr=new TGraph(n,x,y);
	gr->Draw("apl*");
	gr->SetName(nameIn);
	if(writeif==true)gr->Write();
	// char name[100];
	// sprintf(name,"%s.png",nameIn);
	// c->SaveAs(name);
}
void drawPhy2DCharge(char *outCanvas){
	TFile *f=new TFile(outCanvas,"recreate");
	// int ichip=0;
	// int ipixel=0;
	float charge=0; 
	TCanvas *c=new TCanvas("c","c",4000,800);
	c->Divide(8,2);
	TString histName;
	TString histName1;
	char title[100];
	char title1[100];
	TH2D *h[pd1->adcCha()];
	TH1D *h1[pd1->adcCha()];
	for(int i=0;i<pd1->adcCha();i++){
        histName = "Channel"; histName += i;
        histName1 = "1d_Channel"; histName1 += i;
        sprintf(title,"Channel %d Charge Distribution",i);
        sprintf(title1,"Channel %d Charge Distribution 1d",i);
        h[i] = new TH2D(histName, title, 72, 0, 72, 72, 0, 72);
        h1[i] = new TH1D(histName1, title1, 2500,-200,1000);
	}
	int count=0;
	int chargeLessZero=0;
	int chargeLarge=0;
	int size=polNPointCharge->size();
	// float *x=new float[pd1->nFrame()];
	// float *y=new float[pd1->nFrame()];
	// for(int i=0;i<pd1->nFrame();i++){
		// x[i]=(float)i;
		// y[i]=(*polNPointCharge)[pd1->nFrame()*10+i];
	// }
	// draw(pd1->nFrame(),x,y,"test");
	// return ;
	char canvasName[100];
	for(int frame=0;frame<pd1->nFrame();frame++){
		for(int ichip=0;ichip<pd1->adcCha();ichip++){
			h[ichip]->Clear();
			h1[ichip]->Clear();
			h1[ichip]->Reset();
			for(int ipixel=0;ipixel<pd1->nPix();ipixel++){
				int position=ichip*pd1->nPix()*pd1->nFrame()+ipixel*pd1->nFrame()+frame;
				// if(frame<800)
				{
					h[ichip]->SetBinContent(72-ipixel%72,ipixel/72+1,(*polNPointCharge)[position]);
					h1[ichip]->Fill((*polNPointCharge)[position]);
				}
			}
		}
		// if(frame<800)
		{
			sprintf(canvasName,"canvas%d",frame);
			for(int i=0;i<pd1->adcCha();i++){
				c->cd(i+1);
				h[i]->Draw("colz");
				h[i]->GetZaxis()->SetRangeUser(-100,999);
				h[i]->Draw("colz");
				h[i]->SetStats(0);

				c->cd(i+pd1->adcCha()+1);
				h1[i]->Draw();

				c->Modified();

			}
			c->Update();
			c->Write();
		}
	}
	for(int i=0;i<pd1->adcCha();i++){

	}
	
	f->Write();
	f->Close();
}
float polValue(float *par,float x){
	float value=0;
	for(int i=0;i<polFitParNum+1;i++){
		value+=par[i]*TMath::Power(x,i);
	}
	return value;
}

void run(char *pedefile,char *polNFitPar,char *polNFitParRev,char *TotalchargeFile,char *placData3,char *pointChargeOutFile,char *pointOutRoot,char *CanvasRoot){
	vectorSetup();
	vectorClear();
	// loadPede(pedefile);
	loadPolNPar(polNFitPar);// read par fit by pol5
	loadPolNParReV(polNFitParRev);
	GetPhyPolNQ(pedefile,TotalchargeFile);// load pede and save total charge
	savePlacData_3(placData3);
	// savePhyPolNQ(pointChargeOutFile,pointOutRoot);//save point to point charge with a txt file and a root file, each line of txt file is arranged as : ichip ipixel nFrame nFrame point ; Exam 0 0 1618 0 0 0 0 0 0 ......
	// drawPhy2DCharge(CanvasRoot);// save as canvas of nFrame 
}
~polNGetCharge()
{
	vectorClear();
	vectorDelete();
}
};

class dacPar{
	public:
	int nChips;
	int nPix;
	int pixel;
	float lowEdgeTau;
	float highEdgeTau;
	float *dacParMem;
	float *badPixelMem;
	dacPar(){
		nChips=8;
		nPix=5184;
		pixel=72;
		lowEdgeTau=5.0;
		highEdgeTau=200.0;
		dacParMem=new float[nChips];
		badPixelMem=new float[nChips];
	}
	int getBadPixels(TH2D *h,float lowEdge,float highEdge){
		float value=0;
		int lowEdgeCount=0;
		int highEdgeCount=0;
		for(int i=0;i<nPix;i++){
			value=h->GetBinContent(i%pixel+1,i/pixel+1);
			if(value<lowEdge){lowEdgeCount++;}
			if(value>highEdge){highEdgeCount++;}
		}
		return lowEdgeCount+highEdgeCount;
	}
	void singleFile(char* rootFile,float *badPixels){
		TFile *f=new TFile(rootFile);
		TH2D *h[nChips];
		char histoName[100];
		for(int i=0;i<nChips;i++){
			sprintf(histoName,"tau_Chip_%d",i);
			h[i]=(TH2D*)f->Get(histoName);
			badPixels[i]=getBadPixels(h[i],lowEdgeTau,highEdgeTau);
			// cout<<"ichip "<<badPixels[i]<<endl;
		}
		
		f->Close();
	}
	void dacSetVreset(char* name,int start,int step,int stop,char *out){
		char Input[200];
		std::ofstream foutPar(out,std::ios_base::out & std::ios_base::trunc);
		float *badPixels=new float[nChips];
		int nfile=(stop-start)/step+1;
		float **x;
		x=new float*[nChips];
		float **y;
		y=new float*[nChips];
		for(int i=0;i<nChips;i++){
			x[i]=new float[nfile];
			y[i]=new float[nfile];
		}
		for(int i=0;i<nChips;i++){
			for(int j=0;j<nfile;j++){
				x[i][j]=0;
				y[i][j]=0;
			}
		}
		float *badPixel=new float[nChips];
		float *dacOut=new float[nChips];
		for(int i=0;i<nfile;i++){
			for(int ichip=0;ichip<nChips;ichip++)
			{
				badPixel[ichip]=0;				
			}
			int file=i*step+start;
			sprintf(Input,"%s%d.root",name,file);
			cout<<"Input "<<Input<<endl;
			singleFile(Input,badPixel);
			for(int ichip=0;ichip<nChips;ichip++){
				y[ichip][i]=badPixel[ichip];
				x[ichip][i]=file;
			}
			
		}
		float *min=new float[nChips];
		for(int i=0;i<nChips;i++){min[i]=10000000;}
		
		for(int ichip=0;ichip<nChips;ichip++){
			for(int j=0;j<nfile;j++){
				// min[i]=y[i][j];
				if(min[ichip]>y[ichip][j]){
					min[ichip]=y[ichip][j];
					dacParMem[ichip]=j;
				}
			}
			cout<<"ichip "<<ichip<<" min "<<min[ichip]<<" file Index "<<dacParMem[ichip]<<endl;
		}
		
		
		for(int ichip=0;ichip<nChips;ichip++){
			badPixelMem[ichip]=min[ichip];
			dacParMem[ichip]=dacParMem[ichip]*step+start;
			foutPar<<dacParMem[ichip]<<" ";
		}
		TFile *f=new TFile("dacPar.root","recreate");
		char grName[100];
		for(int ichip=0;ichip<nChips;ichip++){
			sprintf(grName,"ichip%d",ichip);
			draw(nfile,x[ichip],y[ichip],grName,true);
		}
		f->Write();
		f->Close();
		for(int ichip=0;ichip<nChips;ichip++){
			// cout<<i<<endl;
			delete []x[ichip];
			delete []y[ichip];
		}
		delete []x;
		delete []y;
		delete []badPixel;
		delete []min;
	}
	void draw(int n,float *x,float *y,char *nameIn,bool writeif=false){
	// TCanvas *c=new TCanvas("c","c",700,600);
	TGraph *gr=new TGraph(n,x,y);
	gr->Draw("apl*");
	gr->SetName(nameIn);
	gr->SetTitle("");
	if(writeif==true)gr->Write();
	// char name[100];
	// sprintf(name,"%s.png",nameIn);
	// c->SaveAs(name);
	}
	~dacPar(){
		nChips=0;
		nPix=0;
		pixel=0;
		lowEdgeTau=0;
		highEdgeTau=0;
		delete []dacParMem;
		delete []badPixelMem;
	}
};
class dacParPoint{
	public:
	
	
	int nChips;
	int nPix;
	int pixel;
	int debugVerbose;
	std::vector<int>   	*chipIndex;//
	std::vector<int>   	*pixelIndex;
	std::vector<int> *pointNumber;
	float lowEdgeTau;
	float highEdgeTau;
	float *dacParMem;
	float *badPixelMem;
	// void setup(placData_1 *_pd1){
		// pd1=_pd1;
	// }	
	dacParPoint(){
		debugVerbose=-1;
		nChips=8;
		nPix=5184;
		pixel=72;
		lowEdgeTau=5.0;
		highEdgeTau=200.0;
		// pointNumber=new vector<int>;
		// pointNumber->clear();
		// chipIndex=new vector<int>;
		// chipIndex->clear();
		// pixelIndex=new vector<int>;
		// pixelIndex->clear();
		dacParMem=new float[nChips];
		badPixelMem=new float[nChips];
	}
	~dacParPoint(){
		nChips=0;
		nPix=0;
		pixel=0;
		lowEdgeTau=0;
		highEdgeTau=0;
		// pointNumber->clear();
		// delete pointNumber;
		// chipIndex->clear();
		// delete chipIndex;
		// pixelIndex->clear();
		// delete pixelIndex;
		delete []dacParMem;
		delete []badPixelMem;
	}
	void getBadPixels(vector<int> *chipI,vector<int> *pointN,float lowEdge,float highEdge,int *badPixels){
		float value=0;
		int lowEdgeCount=0;
		int highEdgeCount=0;
		int size=pointN->size();
		if(debugVerbose==0||debugVerbose==1||debugVerbose==2||debugVerbose>2){
			cout<<"vector<int> *pointN "<<size<<" "<<chipI->size()<<endl;
		}

		for(int i=0;i<size;i++){
			int j=(*chipI)[i];
			int value=(*pointN)[i];
			if(value<lowEdge){badPixels[j]+=1;}
			if(value>highEdge){badPixels[j]+=1;}
		}
		for(int i=0;i<8;i++){
			if(debugVerbose==0||debugVerbose==1||debugVerbose==2||debugVerbose>2){
				cout<<(*chipI)[i+5184*i]<<endl;
				cout<<"ichip"<<badPixels[i]<<endl;
			}
		}
		
	}

 	void dacSetVreset(char* name,int start,int step,int stop,char *out){
		char Input[200];
		char midOut[200];
		std::ofstream foutPar(out,std::ios_base::out & std::ios_base::trunc);
		if( foutPar==NULL ){
			cout<<"error in open "<<out<<endl;
			cout<<"please check if "<<out<<" is opened elsewhere"<<endl;
			return ;
		}
		
		float *badPixels=new float[nChips];
		int nfile=(stop-start)/step+1;
		float **x;
		x=new float*[nChips];
		float **y;
		y=new float*[nChips];
		for(int i=0;i<nChips;i++){
			x[i]=new float[nfile];
			y[i]=new float[nfile];
		}
		for(int i=0;i<nChips;i++){
			for(int j=0;j<nfile;j++){
				x[i][j]=0;
				y[i][j]=0;
			}
		}
		int *badPixel=new int[nChips];
		for(int i=0;i<nChips;i++){
			badPixel[i]=0;
			dacParMem[i]=0;
		}
		float *dacOut=new float[nChips];
		char outPutRoot[100];
		for(int i=0;i<nfile;i++){
			for(int ichip=0;ichip<nChips;ichip++){
				badPixel[ichip]=0;
			}
			
			
			int file=i*step+start;
			sprintf(Input,"%s%d.pd1",name,file);
			sprintf(outPutRoot,"%s%d_pointN.root",name,file);
			if(debugVerbose==0||debugVerbose==1||debugVerbose==2||debugVerbose>2){
				cout<<Input<<endl;
			}
			TFile *fs=new TFile(outPutRoot,"recreate");
			TH1D *h[nChips];
			char hname[100];
			for(int ih=0;ih<nChips;ih++){
				sprintf(hname,"histo%d",ih);
				h[ih]=new TH1D(hname,hname,500,0,500);
			}
			sprintf(midOut,"%s%d.decayData",name,file);
			placData_1 pd1;
			pd1.read(Input);
			if(debugVerbose==0||debugVerbose==1||debugVerbose==2||debugVerbose>2){
				cout<<"read data ready"<<endl;
			}
			polNPar polNparObj;
			polNparObj.setup(&pd1);
			polNparObj.saveAllPixCurveDat(midOut);
			pointNumber=polNparObj.pointNumber;
			chipIndex=polNparObj.chipIndex;
			cout<<(*(polNparObj.chipIndex))[100]<<endl;
			cout<<(*(polNparObj.pointNumber))[100]<<endl;
			if(debugVerbose==0||debugVerbose==1||debugVerbose==2||debugVerbose>2){
				cout<<"Input "<<Input<<endl;
			}
			//save to root file
			int size=chipIndex->size();
			for(int isize=0;isize<size;isize++){
				int j=(*chipIndex)[isize];
				int value=(*pointNumber)[isize];
				h[j]->Fill(value);
				// if(value<lowEdgeTau){badPixels[j]+=1;}
				// if(value>highEdgeTau){badPixels[j]+=1;}
			}
			
			
			
			getBadPixels(polNparObj.chipIndex,polNparObj.pointNumber,lowEdgeTau,highEdgeTau,badPixel);
			for(int ichip=0;ichip<nChips;ichip++){
				y[ichip][i]=badPixel[ichip];
				x[ichip][i]=file;
				if(debugVerbose==2||debugVerbose>2){
					cout<<"y[ichip][i] "<<y[ichip][i]<<endl;
				}
				// h[ichip]->Write();
			}
			fs->Write();
			fs->Close();
			for(int ih=0;ih<nChips;ih++){
				// delete []h[ih];
			}
		}
		float *min=new float[nChips];
		for(int i=0;i<nChips;i++){min[i]=10000000;}
		
		for(int ichip=0;ichip<nChips;ichip++){
			for(int j=0;j<nfile;j++){
				if(min[ichip]>y[ichip][j]){
					min[ichip]=y[ichip][j];
					dacParMem[ichip]=j;
				}
			}
			if(debugVerbose==0||debugVerbose==1||debugVerbose==2||debugVerbose>2){
				cout<<"ichip "<<ichip<<" min "<<min[ichip]<<" file Index "<<dacParMem[ichip]<<endl;
			}
		}
		
		
		for(int ichip=0;ichip<nChips;ichip++){
			badPixelMem[ichip]=min[ichip];
			dacParMem[ichip]=dacParMem[ichip]*step+start;
			foutPar<<dacParMem[ichip]<<" ";
		}
		TFile *f=new TFile("dacPar.root","recreate");
		char grName[100];
		for(int ichip=0;ichip<nChips;ichip++){
			cout<<"draw graph "<<endl;
			sprintf(grName,"ichip%d",ichip);
			draw(nfile,x[ichip],y[ichip],grName,true);
		}
		f->Write();
		f->Close();
		if(debugVerbose==0||debugVerbose==1||debugVerbose==2||debugVerbose>2){
			cout<<"file write"<<endl;
		}
		for(int ichip=0;ichip<nChips;ichip++){
			if(debugVerbose==1||debugVerbose==2||debugVerbose>2){
				cout<<"delete success"<<ichip<<endl;
			}
			delete []x[ichip];
			delete []y[ichip];
		}
		delete []x;
		delete []y;
		delete []badPixel;
		delete []min;
	}
	
	void draw(int n,float *x,float *y,char *nameIn,bool writeif=false){
	// TCanvas *c=new TCanvas("c","c",700,600);
	TGraph *gr=new TGraph(n,x,y);
	gr->Draw("apl*");
	gr->SetName(nameIn);
	gr->SetTitle("");
	if(writeif==true)gr->Write();
	// char name[100];
	// sprintf(name,"%s.png",nameIn);
	// c->SaveAs(name);
	}

};


class draw{
	public:
	int num;
	float *x;
	float *y;
	draw(){
		x=NULL;
		y=NULL;
		num=0;
	}
	int createMem(int s){
		delete []x;
		delete []y;
		num=s;
		x=new float[num];
		y=new float[num];
        return num;		
	}
	void drawGraph(char *nameIn,bool writeif=false){
	// TCanvas *c=new TCanvas("c","c",700,600);
	TGraph *gr=new TGraph(num,x,y);
	gr->Draw("apl*");
	gr->SetName(nameIn);
	gr->SetTitle("");
	if(writeif==true)gr->Write();
	// char name[100];
	// sprintf(name,"%s.png",nameIn);
	// c->SaveAs(name);
	}
	
	void drawGraph(int n,float *xx,float *yy,char *nameIn,bool writeif=false){
	// TCanvas *c=new TCanvas("c","c",700,600);
	TGraph *gr=new TGraph(n,xx,yy);
	gr->Draw("apl*");
	gr->SetName(nameIn);
	gr->SetTitle("");
	if(writeif==true)gr->Write();
	// char name[100];
	// sprintf(name,"%s.png",nameIn);
	// c->SaveAs(name);
	}
	
	~draw(){
		if(x!=NULL){delete []x;}
		if(y!=NULL){delete []y;}
		num=0;
	}
};


class pd3Debug{
	
	
	public:
	placData_3 pd3;
	
	int read(char* fn){
		pd3.read(fn);
		return 1;
	}
	
	void drawPixelDebug( int chip, int pixel){
	TCanvas *c=new TCanvas("p","p",700,600);
	pd3.getChaDat(chip);
	float *x=new float[pd3.nFrame()];
	float *y=new float[pd3.nFrame()];
	
	for(int i=0;i<pd3.nFrame();i++){
		y[i]=pd3.chaDat[pixel*pd3.nFrame()+i];
		x[i]=(float)i;
	}
	
	TGraph *a=new TGraph(pd3.nFrame(),x,y);
	a->Draw();
	char *decayData =Form("pd3png/decayDataChip%dPixel%d.png",chip,pixel);
	c->SaveAs(decayData);
	delete []x;
	delete []y;
	delete a;
	delete c;		
		
	}

	void drawPixel( int chip, int pixel){
	pd3.getChaDat(chip);
	float *x=new float[pd3.nFrame()];
	float *y=new float[pd3.nFrame()];
	
	for(int i=0;i<pd3.nFrame();i++){
		y[i]=pd3.chaDat[pixel*pd3.nFrame()+i];
		x[i]=(float)i;
	}
	
	TGraph *a=new TGraph(pd3.nFrame(),x,y);
	char *decayData =Form("pd3png/decayDataChip%dPixel%d.root",chip,pixel);
	a->SaveAs(decayData);
	delete []x;
	delete []y;
	delete a;
		
	}

    void drawPixel( int chip, int row,int col){
        drawPixel(chip,row*72+col);	
    }
    
    void drawChip(int chip, int frame){
        TCanvas *c=new TCanvas("c","c",700, 600);
        pd3.getChaDat(chip);	
        float *x = new float[pd3.nPix()];
        float *y = new float[pd3.nPix()];
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
            y[ipixel] = pd3.chaDat[ipixel*pd3.nFrame()+frame];
            x[ipixel] = (float)ipixel;
        }
        TGraph *g = new TGraph(pd3.nPix(),x,y);
        g->Draw();
        char *waveData =Form("pd3png/waveDataChip%dFrame%d.png",chip,frame);
        c->SaveAs(waveData);
        delete []x;
        delete []y;
        delete g;
        delete c;	        
         
    }
    
    void drawChip(int chip){
        TCanvas *c=new TCanvas("c","c",1500, 600);
        pd3.getChaDat(chip);	
        float *x = new float[pd3.nPix()*pd3.nFrame()];
        float *y = new float[pd3.nPix()*pd3.nFrame()];
        for(int iframe=0;iframe<pd3.nFrame();iframe++){
            for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
                y[iframe*pd3.nPix()+ipixel] = pd3.chaDat[ipixel*pd3.nFrame()+iframe];
                x[iframe*pd3.nPix()+ipixel] = (float)iframe*pd3.nPix()+ipixel;
            }    
        }
        TGraph *g = new TGraph(pd3.nPix()*pd3.nFrame(),x,y);
        g->Draw();
        char *waveData =Form("pd3png/waveDataChip%d.png",chip);
        c->SaveAs(waveData);
        delete []x;
        delete []y;
        delete g;
        delete c;	        
         
    }
    void drawFrame(int frame) //画所有通道
	{
		int nTopMetalChips=pd3.adcCha();
		TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "ADC map per chip", 3200, 400);
		ca_adcMap->Divide(nTopMetalChips, 1, 0.01, 0.);
		TH2S *hAdcMap[nTopMetalChips];
		TString histName;
		for(int i=0; i<nTopMetalChips; i++) {
			histName = "hAdcMap_Chip"; histName += i;
			hAdcMap[i] = new TH2S(histName, "ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
		}
//loop pd3 data
        float y;
		for(int ich=0;ich<pd3.adcCha();ich++){
			pd3.getChaDat(ich);
			for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
				for(int iframe=frame;iframe<=frame;iframe++){
					// y[iframe]=pd3.chaDat[ich*pd3.nPix()*pd3.nFrame()+ipixel*pd3.nFrame()+iframe];
					y=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
						
					// hAdcMap[ich]->SetBinContent(ipixel%72+1,ipixel/72+1,y);
					hAdcMap[ich]->SetBinContent(72-ipixel%72,ipixel/72+1,y);
				}	
			}
			ca_adcMap->cd(ich+1);
			hAdcMap[ich]->SetMaximum(50);
			hAdcMap[ich]->SetMinimum(-20);
			
			hAdcMap[ich]->Draw("colz");		
		
		}	
		
		
	}	
	void drawFrameUpdate(int frameStart,int frameStop) //画所有通道
	{
		int nTopMetalChips=pd3.adcCha();
		int nPixelsOnChip=pd3.nPix();
		int nFrames=pd3.nFrame();
		std::vector< TH1S* > mHistAdcVec; //tempoary histogram container
		mHistAdcVec.assign( nTopMetalChips*nPixelsOnChip, (TH1S*)0 );
		
		TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "ADC map per chip", 1600, 200);
		ca_adcMap->Divide(nTopMetalChips, 1, 0.01, 0.);
		TH2S *hAdcMap[nTopMetalChips];
		TString histName;
		for(int i=0; i<nTopMetalChips; i++) {
			histName = "hAdcMap_Chip"; histName += i;
			hAdcMap[i] = new TH2S(histName, "ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
		}
//loop pd3 data
        float y;
		TString ss;
		// frameStart=0;
		// frameStop=100;
		// frameStop=nFrames;
		int frameDraw=frameStop-frameStart+1;
		int count=0;
		for(int ich=0;ich<pd3.adcCha();ich++){
			pd3.getChaDat(ich);
			for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
				int code = ich * pd3.nPix() + ipixel;
				for(int iframe=frameStart;iframe<=frameStop;iframe++){
					// y[iframe]=pd3.chaDat[ich*pd3.nPix()*pd3.nFrame()+ipixel*pd3.nFrame()+iframe];
					y=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
						
					// hAdcMap[ich]->SetBinContent(ipixel%72+1,ipixel/72+1,y);
					hAdcMap[ich]->SetBinContent(72-ipixel%72,ipixel/72+1,y);
					
					
						TH1S* histAdc = mHistAdcVec[ code ];
						if( !histAdc ){
							ss="hAdc_ZS_Chip"; ss += ich;
							ss += "_Channel"; ss += ipixel;
							histAdc = new TH1S( ss.Data(), ";Time Bin [per 3.3 ms];ADC Counts", frameDraw, 0, frameDraw );
							mHistAdcVec[ code ] = histAdc;
						}
						histAdc->SetBinContent( iframe-frameStart+1, int(y) );
					
				}	
				
				// mHistAdcVec[ code ]->Write();
				// return;
			}
			
			
			
			
			
			// ca_adcMap->cd(ich+1);
			// hAdcMap[ich]->SetMaximum(50);
			// hAdcMap[ich]->SetMinimum(-20);
			
			// hAdcMap[ich]->Draw("colz");	
			// ca_adcMap->cd(ich+1)->Modified();
			// ca_adcMap->cd(ich+1)->Update();				
		}	
		
		std::vector< TH1S* >::iterator mHistAdcVecIter;

		for(int iframe=frameStart;iframe<=frameStop;iframe++) {
			//cout << "TopMetal II ADC Map at Frame " << iframe << endl;  
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
                
					float rawAdc= histAdc->GetBinContent(iframe-frameStart+1);
					//if(rawAdc>mAdcCut*rmsPed[channelIdx])
						// hAdcMap[iChip]->SetBinContent(iRow+1, iColumn+1, (int)rawAdc);
						hAdcMap[iChip]->SetBinContent(72-iPixel%72,iPixel/72+1, (int)rawAdc);
               
				}
            
				for(int iC=0; iC<pd3.adcCha(); iC++) {
					ca_adcMap->cd(iC+1);
					hAdcMap[iC]->SetStats(kFALSE);
					hAdcMap[iC]->Draw("COLZ");
					hAdcMap[iC]->SetMaximum(30);
					hAdcMap[iC]->SetMinimum(-20);
					ca_adcMap->cd(iC+1)->Modified();
					ca_adcMap->cd(iC+1)->Update();
				}
            
				if(gSystem->ProcessEvents()) break;
			} //end frame loop
		
		
	}
// display the chip and pixel based on the arrangement of tm2-_1*8_VHDCI    
	void drawAllFrameUpdate() //画所有通道
	{
		int nTopMetalChips=pd3.adcCha();
		int nPixelsOnChip=pd3.nPix();
		int nFrames=pd3.nFrame();
		int frameStart = 0;
		int frameStop = nFrames-1;
		std::vector< TH1S* > mHistAdcVec; //tempoary histogram container
		mHistAdcVec.assign( nTopMetalChips*nPixelsOnChip, (TH1S*)0 );
		
		TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "ADC map per chip", 1600, 200);
		ca_adcMap->Divide(nTopMetalChips, 1, 0.01, 0.);
		TH2S *hAdcMap[nTopMetalChips];
		TString histName;
		for(int i=0; i<nTopMetalChips; i++) {
			histName = "hAdcMap_Chip"; histName += i;
			hAdcMap[i] = new TH2S(histName, "ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
		}
//loop pd3 data
        float y;
		TString ss;
		// frameStart=0;
		// frameStop=100;
		// frameStop=nFrames;
		int frameDraw=frameStop-frameStart+1;
		int count=0;
		for(int ich=0;ich<pd3.adcCha();ich++){
			pd3.getChaDat(ich);
			for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
				int code = ich * pd3.nPix() + ipixel;
				for(int iframe=frameStart;iframe<=frameStop;iframe++){
					// y[iframe]=pd3.chaDat[ich*pd3.nPix()*pd3.nFrame()+ipixel*pd3.nFrame()+iframe];
					y=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
						
					// hAdcMap[ich]->SetBinContent(ipixel%72+1,ipixel/72+1,y);
					hAdcMap[ich]->SetBinContent(72-ipixel%72,ipixel/72+1,y);
					
					
						TH1S* histAdc = mHistAdcVec[ code ];
						if( !histAdc ){
							ss="hAdc_ZS_Chip"; ss += ich;
							ss += "_Channel"; ss += ipixel;
							histAdc = new TH1S( ss.Data(), ";Time Bin [per 3.3 ms];ADC Counts", frameDraw, 0, frameDraw );
							mHistAdcVec[ code ] = histAdc;
						}
						histAdc->SetBinContent( iframe-frameStart+1, int(y) );
					
				}	
				
				// mHistAdcVec[ code ]->Write();
				// return;
			}
			
			
			
			
			
			// ca_adcMap->cd(ich+1);
			// hAdcMap[ich]->SetMaximum(50);
			// hAdcMap[ich]->SetMinimum(-20);
			
			// hAdcMap[ich]->Draw("colz");	
			// ca_adcMap->cd(ich+1)->Modified();
			// ca_adcMap->cd(ich+1)->Update();				
		}	
		
		std::vector< TH1S* >::iterator mHistAdcVecIter;

		for(int iframe=frameStart;iframe<=frameStop;iframe++) {
		//	cout << "TopMetal II ADC Map at Frame " << iframe << endl;  
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
                
					float rawAdc= histAdc->GetBinContent(iframe-frameStart+1);
					//if(rawAdc>mAdcCut*rmsPed[channelIdx])
						// hAdcMap[iChip]->SetBinContent(iRow+1, iColumn+1, (int)rawAdc);
						hAdcMap[iChip]->SetBinContent(72-iPixel%72,iPixel/72+1, (int)rawAdc);
               
				}
            
				for(int iC=0; iC<pd3.adcCha(); iC++) {
					ca_adcMap->cd(iC+1);
					hAdcMap[iC]->SetStats(kFALSE);
					hAdcMap[iC]->Draw("COLZ");
					hAdcMap[iC]->SetMaximum(100);
					hAdcMap[iC]->SetMinimum(-500);
					ca_adcMap->cd(iC+1)->Modified();
					ca_adcMap->cd(iC+1)->Update();
				}
            
				if(gSystem->ProcessEvents()) break;
			} //end frame loop
		
		
	}
	
		void drawAllFrameUpdate_copy() //画所有通道
	{
		int nTopMetalChips=pd3.adcCha();
		int nPixelsOnChip=pd3.nPix();
		int nFrames=pd3.nFrame();
		int frameStart = 0;
		int frameStop = nFrames-1;
		std::vector< TH1S* > mHistAdcVec; //tempoary histogram container
		mHistAdcVec.assign( nTopMetalChips*nPixelsOnChip, (TH1S*)0 );
		
		TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "ADC map per chip", 1600, 200);
		ca_adcMap->Divide(nTopMetalChips, 1, 0.01, 0.);
		TH2S *hAdcMap[nTopMetalChips];
		TString histName;
		for(int i=0; i<nTopMetalChips; i++) {
			histName = "hAdcMap_Chip"; histName += i;
			hAdcMap[i] = new TH2S(histName, "ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
		}
//loop pd3 data
        float y;
		TString ss;
		// frameStart=0;
		// frameStop=100;
		// frameStop=nFrames;
		int frameDraw=frameStop-frameStart+1;
		int count=0;
		for(int ich=0;ich<pd3.adcCha();ich++){
			pd3.getChaDat(ich);
			for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
				int code = ich * pd3.nPix() + ipixel;
				for(int iframe=frameStart;iframe<=frameStop;iframe++){
					// y[iframe]=pd3.chaDat[ich*pd3.nPix()*pd3.nFrame()+ipixel*pd3.nFrame()+iframe];
					y=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
						
					// hAdcMap[ich]->SetBinContent(ipixel%72+1,ipixel/72+1,y);
					hAdcMap[ich]->SetBinContent(72-ipixel%72,ipixel/72+1,y);
					
					
						TH1S* histAdc = mHistAdcVec[ code ];
						if( !histAdc ){
							ss="hAdc_ZS_Chip"; ss += ich;
							ss += "_Channel"; ss += ipixel;
							histAdc = new TH1S( ss.Data(), ";Time Bin [per 3.3 ms];ADC Counts", frameDraw, 0, frameDraw );
							mHistAdcVec[ code ] = histAdc;
						}
						histAdc->SetBinContent( iframe-frameStart+1, int(y) );
					
				}	
				
				// mHistAdcVec[ code ]->Write();
				// return;
			}
			
			
			
			
			
			// ca_adcMap->cd(ich+1);
			// hAdcMap[ich]->SetMaximum(50);
			// hAdcMap[ich]->SetMinimum(-20);
			
			// hAdcMap[ich]->Draw("colz");	
			// ca_adcMap->cd(ich+1)->Modified();
			// ca_adcMap->cd(ich+1)->Update();				
		}	
		
		std::vector< TH1S* >::iterator mHistAdcVecIter;

		for(int iframe=frameStart;iframe<=frameStop;iframe++) {
		//	cout << "TopMetal II ADC Map at Frame " << iframe << endl;  
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
                
					float rawAdc= histAdc->GetBinContent(iframe-frameStart+1);
					//if(rawAdc>mAdcCut*rmsPed[channelIdx])
						// hAdcMap[iChip]->SetBinContent(iRow+1, iColumn+1, (int)rawAdc);
						hAdcMap[iChip]->SetBinContent(72-iPixel%72,iPixel/72+1, (int)rawAdc);
               
				}
            
				for(int iC=0; iC<pd3.adcCha(); iC++) {
					ca_adcMap->cd(iC+1);
					hAdcMap[iC]->SetStats(kFALSE);
					hAdcMap[iC]->Draw("COLZ");
					hAdcMap[iC]->SetMaximum(30);
					hAdcMap[iC]->SetMinimum(-20);
					ca_adcMap->cd(iC+1)->Modified();
					ca_adcMap->cd(iC+1)->Update();
				}
            
				if(gSystem->ProcessEvents()) break;
			} //end frame loop
		
		
	}
    void drawFrame(int chip, int frame) //画一个通道
	{
			TCanvas *c=new TCanvas("c","c",1000,500);
			c->Divide(2, 1);
			pd3.getChaDat(chip);	
			TH2D *hAdcMap;
			TH1D *hist=new TH1D("h","h",2100,-100,2000);
			TString histName;
			histName = "hAdcMap_Chip";histName += frame;
			hAdcMap = new TH2D(histName, "ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
			float y;
			for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
				for(int iframe=frame;iframe<=frame;iframe++){
					// y[iframe]=pd3.chaDat[ich*pd3.nPix()*pd3.nFrame()+ipixel*pd3.nFrame()+iframe];
					y=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
					hist->Fill(y);
					hAdcMap->SetBinContent(72-ipixel%72,ipixel/72+1,y);
				}	
			}
			hAdcMap->SetMaximum(50);
			hAdcMap->SetMinimum(-20);
		
			c->cd(2);
			hAdcMap->Draw("colz");	
			c->cd(1);
			hist->Draw();	

	}


    void maxSignalPixel( double upLimitPixel)
    {
		int nTopMetalChips=pd3.adcCha();
        int nFrames=pd3.nFrame();
		TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "Pd3 ADC Distribution of All Chips", 600, 500);
        char *adcMap = Form("pd3png/pd3Adc_signal.png");
        double lowLimitPixel = -20; // upper limit of single pixel   without signal
        TH1D *h1 = new TH1D("h1Adc", "Pd3 ADC distribution of all pixels", 1000, lowLimitPixel, upLimitPixel);
 
        double adc_value;
        for(int ich=0; ich<nTopMetalChips; ich++){
            pd3.getChaDat(ich);
            for(int ipixel=0; ipixel<pd3.nPix(); ipixel++){
                for(int iframe=0; iframe<nFrames; iframe++){
                    adc_value=pd3.chaDat[ipixel*nFrames+iframe];
                    if(adc_value>lowLimitPixel && adc_value<upLimitPixel)                    
                    h1->Fill(adc_value);
                }
                
            }
        }
        ca_adcMap->cd();
        h1->Draw();
        ca_adcMap->SaveAs(adcMap);
    }    


    void noiseRange( double lowLimitPixel, double upLimitPixel, double lowLimitFrame, double upLimitFrame)
    {
		int nTopMetalChips=pd3.adcCha();
        int nFrames=pd3.nFrame();
		TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "Pd3 ADC Distribution of All Chips", 1200, 500);
        ca_adcMap->Divide(2,1);
        char *adcMap = Form("pd3png/pd3Adc_pede.png");      
        TH1D *h1 = new TH1D("h1Adc", "Pd3 ADC distribution of all pixels", 1000, lowLimitPixel, upLimitPixel);
        TH1D *h2 = new TH1D("h2Adc", "Pd3 ADC distribution of each frame", 1000,lowLimitFrame, upLimitFrame);

        double adc_value;
        for(int ich=0; ich<nTopMetalChips; ich++){
            pd3.getChaDat(ich);
            for(int ipixel=0; ipixel<pd3.nPix(); ipixel++){
                for(int iframe=0; iframe<nFrames; iframe++){
                    adc_value=pd3.chaDat[ipixel*nFrames+iframe];
                    if(adc_value>lowLimitPixel && adc_value<upLimitPixel)                    
                    h1->Fill(adc_value);
                }
                
            }
        }
        ca_adcMap->cd(1);
        h1->Draw();
        double sum;
//	  double nSum;
        for(int iframe=0; iframe<nFrames; iframe++){
            sum = 0;
//		nSum = 0;
            for(int ich=0; ich<nTopMetalChips; ich++){
                pd3.getChaDat(ich);
                for(int ipixel=0; ipixel<pd3.nPix(); ipixel++){
                    adc_value=pd3.chaDat[ipixel*nFrames+iframe];
                    if(adc_value>lowLimitPixel && adc_value<upLimitPixel){
	                    sum = sum+adc_value;
//				  nSum = nSum+1;		
			}

                }
            }
	//	cout << "lowLimitFrame: "<<lowLimitFrame<<"  sum: "<<sum<<endl;
	//	cout << "upLimitFrame: "<<upLimitFrame<<"  sum: "<<sum<<endl;
      //cout<< "sum: " <<sum<<endl;
	//	cout<< "nSum: " << nSum<<endl;
//		sum = sum/nSum;
	//	cout<< "sum: " <<sum<<endl;
            if(sum>lowLimitFrame && sum<upLimitFrame)
                h2->Fill(sum);
        }
        ca_adcMap->cd(2);
        h2->Draw();
        
        ca_adcMap->SaveAs(adcMap);
    }


    
    void noiseRangeUpdate( double lowLimitPixel, double upLimitPixel, double lowLimitFrame, double upLimitFrame)
    {
		int nTopMetalChips=pd3.adcCha();
        int nFrames=pd3.nFrame();
		TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "Pd3 ADC Distribution of All Chips", 1200, 500);
        ca_adcMap->Divide(2,1);
        char *adcMap = Form("pd3png/pd3Adc_pede.png");      
        TH1D *h1 = new TH1D("h1Adc", "Pd3 ADC distribution of all pixels", 1000, lowLimitPixel, upLimitPixel);
        TH1D *h2 = new TH1D("h2Adc", "Pd3 ADC distribution of each frame", 1000,lowLimitFrame, upLimitFrame);

        double adc_value;
        for(int ich=0; ich<nTopMetalChips; ich++){
            pd3.getChaDat(ich);
            for(int ipixel=0; ipixel<pd3.nPix(); ipixel++){
                for(int iframe=0; iframe<nFrames; iframe++){
                    adc_value=pd3.chaDat[ipixel*nFrames+iframe];
                    if(adc_value>lowLimitPixel && adc_value<upLimitPixel)                    
                    h1->Fill(adc_value);
                }
                
            }
        }
        ca_adcMap->cd(1);
        h1->Draw();
        

		int nPixelsOnChip=pd3.nPix();        

		std::vector< TH1D* > mHistAdcVec; //tempoary histogram container
		mHistAdcVec.assign( nTopMetalChips*nPixelsOnChip, (TH1D*)0 );        
		TH2D *hAdcMap[nTopMetalChips];
		TString histName;
		for(int i=0; i<nTopMetalChips; i++) {
			histName = "hAdcMap_Chip"; histName += i;
			hAdcMap[i] = new TH2D(histName, "Pd3 ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
		}        
        
//loop pd3 data
        double y;
		TString ss;
		int frameStart=0;
		int frameStop=nFrames-1;
		int frameDraw=frameStop-frameStart+1;
		for(int ich=0;ich<pd3.adcCha();ich++){
			pd3.getChaDat(ich);
			for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
				int code = ich * pd3.nPix() + ipixel;
                TH1D* histAdc = mHistAdcVec[ code ];
                if( !histAdc ){
                    ss="hAdc_ZS_Chip"; ss += ich;
                    ss += "_Channel"; ss += ipixel;
                    histAdc = new TH1D( ss.Data(), ";Time Bin [per 3.3 ms];ADC Counts", frameDraw, 0, frameDraw );
                    mHistAdcVec[ code ] = histAdc;
                }                
				for(int iframe=frameStart;iframe<=frameStop;iframe++){
					y=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
                    histAdc->SetBinContent( iframe-frameStart+1, y );
					
				}	
				

			}
        }


		std::vector< TH1D* >::iterator mHistAdcVecIter;
        double sum;
        int channelIdx;
        double rawAdc;
       
		for(int iframe=frameStart;iframe<=frameStop;iframe++) {
			//cout << "TopMetal II ADC Map at Frame " << iframe << endl;  
				channelIdx = 0;                  
                sum = 0;
				for( mHistAdcVecIter = mHistAdcVec.begin(); mHistAdcVecIter != mHistAdcVec.end(); ++mHistAdcVecIter ){ //loop 8*5184 channels
					TH1D *histAdc = *mHistAdcVecIter;
					channelIdx = std::distance( mHistAdcVec.begin(), mHistAdcVecIter );
					int entries = histAdc->GetEntries();
					if( entries == 0 ) continue;
  
					int iChip   = channelIdx / nPixelsOnChip;
					int iPixel  = channelIdx % nPixelsOnChip;
                
				    rawAdc= histAdc->GetBinContent(iframe-frameStart+1);
                    hAdcMap[iChip]->SetBinContent(iPixel/72,iPixel%72, rawAdc);               
				}
				for(int iC=0; iC<pd3.adcCha(); iC++) {
                    sum = sum+hAdcMap[iC]->GetSum();
                }
                if(sum>lowLimitFrame && sum<upLimitFrame)                
                    h2->Fill(sum);
        }
                ca_adcMap->cd(2);
                h2->Draw();                
                ca_adcMap->SaveAs(adcMap);
  
        

}
    
// display the chip and pixel based on the arrangement of tm2-_1*8_VHDCI
    void displayInterfaceUpdate(double maxPixelSignal) //
	{
		int nTopMetalChips=pd3.adcCha();
		int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
		std::vector< TH1D* > mHistAdcVec; //tempoary histogram container
		mHistAdcVec.assign( nTopMetalChips*nPixelsOnChip, (TH1D*)0 );
        
		TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "Pd3 ADC map per chip", 1600, 200);
		// ca_adcMap->Divide(nTopMetalChips, 1, 0.01, 0.);
		ca_adcMap->Divide(nTopMetalChips, 1); 
        char *adcMap;
		TH2D *hAdcMap[nTopMetalChips];
		TString histName;
		for(int i=0; i<nTopMetalChips; i++) {
			histName = "hAdcMap_Chip"; histName += i;
			hAdcMap[i] = new TH2D(histName, "Pd3 ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
		}
//loop pd3 data
        double y;
		TString ss;
		int frameStart=0;
		int frameStop=nFrames-1;
		int frameDraw=frameStop-frameStart+1;
		for(int ich=0;ich<pd3.adcCha();ich++){
			pd3.getChaDat(ich);
			for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
				int code = ich * pd3.nPix() + ipixel;
                TH1D* histAdc = mHistAdcVec[ code ];
                if( !histAdc ){
                    ss="hAdc_ZS_Chip"; ss += ich;
                    ss += "_Channel"; ss += ipixel;
                    histAdc = new TH1D( ss.Data(), ";Time Bin [per 3.3 ms];ADC Counts", frameDraw, 0, frameDraw );
                    mHistAdcVec[ code ] = histAdc;
                }                
				for(int iframe=frameStart;iframe<=frameStop;iframe++){
					y=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
                    histAdc->SetBinContent( iframe-frameStart+1, y );
					
				}	
				

			}
        }

		std::vector< TH1D* >::iterator mHistAdcVecIter;
        double sum;
        int channelIdx;
        double rawAdc;
        int  dFrameSignal = 100;
        int  dFrameNoSignal = 100;
        int signalCount = 0;
        int noSignalCount = 0;            
        double upLimitPixel = maxPixelSignal; // up limit of single pixel
        double lowLimitPixel = -20; // low limit of single pixel
        double ref = 1e5;        
		for(int iframe=frameStart;iframe<=frameStop;iframe++) {
			//cout << "TopMetal II ADC Map at Frame " << iframe << endl;  
				channelIdx = 0;                  
                sum = 0;
				for( mHistAdcVecIter = mHistAdcVec.begin(); mHistAdcVecIter != mHistAdcVec.end(); ++mHistAdcVecIter ){ //loop 8*5184 channels
					TH1D *histAdc = *mHistAdcVecIter;
					channelIdx = std::distance( mHistAdcVec.begin(), mHistAdcVecIter );
					int entries = histAdc->GetEntries();
					if( entries == 0 ) continue;
  
					int iChip   = channelIdx / nPixelsOnChip;
					int iPixel  = channelIdx % nPixelsOnChip;
                
				    rawAdc= histAdc->GetBinContent(iframe-frameStart+1);
                    hAdcMap[iChip]->SetBinContent(iPixel/72,iPixel%72, rawAdc);               
				}
				for(int iC=0; iC<pd3.adcCha(); iC++) {
                    sum = sum+hAdcMap[iC]->GetSum();
                }
            if(sum<ref){
                signalCount = 0; 
                if(noSignalCount == 0){
                    cout<< "sum: "<<sum<<endl;
                    for(int ich = 0; ich<pd3.adcCha(); ich++){
                        if(ich<pd3.adcCha()/2){
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2)->Modified();
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2)->Update();
                        }else{
                            ca_adcMap->cd(pd3.adcCha()-ich);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(pd3.adcCha()-ich)->Modified();
                            ca_adcMap->cd(pd3.adcCha()-ich)->Update();                            
                        }
                        
                                                  
                    }
                    adcMap = Form("pd3png/adcMapFrame%d.png", iframe);                                   
                    ca_adcMap->SaveAs(adcMap);  
                    noSignalCount++;
                }
                else if(noSignalCount==dFrameNoSignal){
                    cout<< "sum: "<<sum<<endl;                    
                    for(int ich = 0; ich<pd3.adcCha(); ich++){
                        if(ich<pd3.adcCha()/2){
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2)->Modified();
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2)->Update();
                        }else{
                            ca_adcMap->cd(pd3.adcCha()-ich);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(pd3.adcCha()-ich)->Modified();
                            ca_adcMap->cd(pd3.adcCha()-ich)->Update();                            
                        }
                        
                                                  
                    }
                    adcMap = Form("pd3png/adcMapFrame%d.png", iframe);                                   
                    ca_adcMap->SaveAs(adcMap); 
                    noSignalCount = 1;
                }
                else{
                    noSignalCount++;
                }
            }
            else{
                noSignalCount = 0;    
                if(signalCount == 0){
                    cout<< "sum: "<<sum<<endl;                    
                    for(int ich = 0; ich<pd3.adcCha(); ich++){
                        if(ich<pd3.adcCha()/2){
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2)->Modified();
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2)->Update();
                        }else{
                            ca_adcMap->cd(pd3.adcCha()-ich);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(pd3.adcCha()-ich)->Modified();
                            ca_adcMap->cd(pd3.adcCha()-ich)->Update();                            
                        }
                        
                                                  
                    }
                    adcMap = Form("pd3png/adcMapFrame%d.png", iframe);               
                    ca_adcMap->SaveAs(adcMap);  
                    signalCount++;
                }
                else if(signalCount==dFrameSignal){
                        cout<< "sum: "<<sum<<endl;                    
                    for(int ich = 0; ich<pd3.adcCha(); ich++){
                        if(ich<pd3.adcCha()/2){
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2)->Modified();
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2)->Update();
                        }else{
                            ca_adcMap->cd(pd3.adcCha()-ich);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(pd3.adcCha()-ich)->Modified();
                            ca_adcMap->cd(pd3.adcCha()-ich)->Update();                            
                        }
                        
                                                  
                    }
                        adcMap = Form("pd3png/adcMapFrame%d.png", iframe);               
                        ca_adcMap->SaveAs(adcMap); 
                        signalCount = 1;
                }
                else {
                    signalCount++;
                }
            }
        }


  }


 

    
    void displayInterface(double maxPixelSignal, double ref) //
	{
		int nTopMetalChips=pd3.adcCha();
        int nFrames=pd3.nFrame();
		TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "Pd3 ADC map per chip", 1600, 200);
	   ca_adcMap->Divide(nTopMetalChips, 1, 0.01, 0.);
		//ca_adcMap->Divide(nTopMetalChips, 1);
        char *adcMap;
		TH2D *hAdcMap[nTopMetalChips];
		TString histName;
		for(int i=0; i<nTopMetalChips; i++) {
			histName = "hAdcMap_Chip"; histName += i;
			//hAdcMap[i] = new TH2D(histName, "Pd3 ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
			hAdcMap[i] = new TH2D(histName, "", 72, 0, 72, 72, 0, 72);
		}
//loop pd3 data
// With signal,  the interval of the frames is 10; without signal, the interval of the frames is 100.
        int  dFrameSignal = 1;
        int  dFrameNoSignal = 100;
        int signalCount = 0;
        int noSignalCount = 0;        
        double adc_value ;    
        double sum;
//	  double nSum;
        double upLimitPixel = maxPixelSignal; // up limit of single pixel
        double lowLimitPixel = -20; // low limit of single pixel
 //       double ref = 1e6;
        for(int iframe=0; iframe<nFrames; iframe++){
            sum = 0;
//		nSum=0;
            for(int ich=0;ich<pd3.adcCha();ich++){
                pd3.getChaDat(ich);
                for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
                        adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
                        if(adc_value>lowLimitPixel && adc_value<upLimitPixel){
                            sum = sum + adc_value;
	//			    nSum = nSum +1;

				}                        

                    }
            }  
       //     sum = sum/nSum;        
            if(sum<ref){
                signalCount = 0; 
                if(noSignalCount == 0){
                    cout<< "sum: "<<sum<<endl;
                    for(int ich = 0; ich<pd3.adcCha(); ich++){
                        pd3.getChaDat(ich);
                        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
                                adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
                                hAdcMap[ich]->SetBinContent(ipixel/72,ipixel%72,adc_value);
                            }
                        if(ich<pd3.adcCha()/2){
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2)->Modified();
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2)->Update();
                        }else{
                            ca_adcMap->cd(pd3.adcCha()-ich);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(pd3.adcCha()-ich)->Modified();
                            ca_adcMap->cd(pd3.adcCha()-ich)->Update();                            
                        }
                        
                                                  
                    }
                    adcMap = Form("pd3png/adcMapFrame%d.png", iframe);                                   
                   ca_adcMap->SaveAs(adcMap);  
                    noSignalCount++;
                }
                else if(noSignalCount==dFrameNoSignal){
                    cout<< "sum: "<<sum<<endl;                    
                    for(int ich = 0; ich<pd3.adcCha(); ich++){
                        pd3.getChaDat(ich);
                        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
                                adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
                                hAdcMap[ich]->SetBinContent(ipixel/72,ipixel%72,adc_value);
                            }
                        if(ich<pd3.adcCha()/2){
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2)->Modified();
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2)->Update();
                        }else{
                            ca_adcMap->cd(pd3.adcCha()-ich);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(pd3.adcCha()-ich)->Modified();
                            ca_adcMap->cd(pd3.adcCha()-ich)->Update();                            
                        }
                        
                                                  
                    }
                    adcMap = Form("pd3png/adcMapFrame%d.png", iframe);                                   
                    ca_adcMap->SaveAs(adcMap); 
                    noSignalCount = 1;
                }
                else{
                    noSignalCount++;
                }
            }
            else{
                noSignalCount = 0;    
                if(signalCount == 0){
                    cout<< "sum: "<<sum<<endl;                    
                    for(int ich = 0; ich<pd3.adcCha(); ich++){
                        pd3.getChaDat(ich);
                        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
                                adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
                                hAdcMap[ich]->SetBinContent(ipixel/72,ipixel%72,adc_value);
                            }
                        if(ich<pd3.adcCha()/2){
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2)->Modified();
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2)->Update();
                        }else{
                            ca_adcMap->cd(pd3.adcCha()-ich);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(pd3.adcCha()-ich)->Modified();
                            ca_adcMap->cd(pd3.adcCha()-ich)->Update();                            
                        }
                        
                                                  
                    }
                    adcMap = Form("pd3png/adcMapFrame%d.png", iframe);               
                    ca_adcMap->SaveAs(adcMap);  
                    signalCount++;
                }
                else if(signalCount==dFrameSignal){
                        cout<< "sum: "<<sum<<endl;                    
                    for(int ich = 0; ich<pd3.adcCha(); ich++){
                        pd3.getChaDat(ich);
                        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
                                adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
                                hAdcMap[ich]->SetBinContent(ipixel/72,ipixel%72,adc_value);
                            }
                        if(ich<pd3.adcCha()/2){
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2)->Modified();
                            ca_adcMap->cd(ich+1+pd3.adcCha()/2)->Update();
                        }else{
                            ca_adcMap->cd(pd3.adcCha()-ich);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(pd3.adcCha()-ich)->Modified();
                            ca_adcMap->cd(pd3.adcCha()-ich)->Update();                            
                        }
                        
                                                  
                    }
                        adcMap = Form("pd3png/adcMapFrame%d.png", iframe);               
                        ca_adcMap->SaveAs(adcMap); 
                        signalCount = 1;
                }
                else {
                    signalCount++;
                }
            }
           
        }
    }
        
                
    void displayInterface_copy(double maxPixelSignal) //
	{
		int nTopMetalChips=pd3.adcCha();
        int nFrames=pd3.nFrame();
		TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "ADC map per chip", 1600, 200);
		// ca_adcMap->Divide(nTopMetalChips, 1, 0.01, 0.);
		ca_adcMap->Divide(nTopMetalChips, 1);
        char *adcMap;
		TH2D *hAdcMap[nTopMetalChips];
		TString histName;
		for(int i=0; i<nTopMetalChips; i++) {
			histName = "hAdcMap_Chip"; histName += i;
			hAdcMap[i] = new TH2D(histName, "ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
		}
//loop pd3 data
// With signal,  the interval of the frames is 10; without signal, the interval of the frames is 100.
        int  dFrameSignal = 10;
        int  dFrameNoSignal = 100;
        int signalCount = 0;
        int noSignalCount = 0;        
        double adc_value ;    
        double sum;
        double upLimitPixel = maxPixelSignal; // up limit of single pixel
        double lowLimitPixel = -20; // low limit of single pixel
        double ref = 1e6;
        for(int iframe=0; iframe<nFrames; iframe++){
            sum = 0;
            for(int ich=0;ich<pd3.adcCha();ich++){
                pd3.getChaDat(ich);
                for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
                        adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
                        if(adc_value>lowLimitPixel && adc_value<upLimitPixel)                        
                            sum = sum + adc_value;
                    }
            }          
            if(sum<ref){
                signalCount = 0; 
                if(noSignalCount == 0){
                    cout<< "sum: "<<sum<<endl;
                    for(int ich = 0; ich<pd3.adcCha(); ich++){
                        pd3.getChaDat(ich);
                        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
                                adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
                                hAdcMap[ich]->SetBinContent(72-ipixel%72,ipixel/72+1,adc_value);
                            }                        
                        ca_adcMap->cd(ich+1);
                        hAdcMap[ich]->SetStats(kFALSE);                    			
                        hAdcMap[ich]->Draw("colz");
                        hAdcMap[ich]->SetMaximum(upLimitPixel);
                        hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                        ca_adcMap->cd(ich+1)->Modified();
                        ca_adcMap->cd(ich+1)->Update();                             
                    }
                     adcMap = Form("pd3png/adcMapFrame%d.png", iframe);                                   
                     ca_adcMap->SaveAs(adcMap);  
                    noSignalCount++;
                }
                else if(noSignalCount==dFrameNoSignal){
                    cout<< "sum: "<<sum<<endl;                    
                    for(int ich = 0; ich<pd3.adcCha(); ich++){
                        pd3.getChaDat(ich);
                        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
                                adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
                                hAdcMap[ich]->SetBinContent(72-ipixel%72,ipixel/72+1,adc_value);
                            }                        
                        ca_adcMap->cd(ich+1);
                        hAdcMap[ich]->SetStats(kFALSE);                    			
                        hAdcMap[ich]->Draw("colz");
                        hAdcMap[ich]->SetMaximum(upLimitPixel);
                        hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                        ca_adcMap->cd(ich+1)->Modified();
                        ca_adcMap->cd(ich+1)->Update();                             
                    }
                     adcMap = Form("pd3png/adcMapFrame%d.png", iframe);                                   
                     ca_adcMap->SaveAs(adcMap); 
                    noSignalCount = 1;
                }
                else{
                    noSignalCount++;
                }
            }
            else{
                noSignalCount = 0;    
                if(signalCount == 0){
                    cout<< "sum: "<<sum<<endl;                    
                    for(int ich = 0; ich<pd3.adcCha(); ich++){
                        pd3.getChaDat(ich);
                        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
                                adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
                                hAdcMap[ich]->SetBinContent(72-ipixel%72,ipixel/72+1,adc_value);
                            }                        
                        ca_adcMap->cd(ich+1);
                        hAdcMap[ich]->SetStats(kFALSE);                    			
                        hAdcMap[ich]->Draw("colz");
                        hAdcMap[ich]->SetMaximum(upLimitPixel);
                        hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                        ca_adcMap->cd(ich+1)->Modified();
                        ca_adcMap->cd(ich+1)->Update();                             
                    }
                     adcMap = Form("pd3png/adcMapFrame%d.png", iframe);               
                     ca_adcMap->SaveAs(adcMap);  
                    signalCount++;
                }
                else if(signalCount==dFrameSignal){
                        cout<< "sum: "<<sum<<endl;                    
                        for(int ich = 0; ich<pd3.adcCha(); ich++){
                            pd3.getChaDat(ich);
                            for(int ipixel=0;ipixel<pd3.nPix();ipixel++){
                                    adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
                                    hAdcMap[ich]->SetBinContent(72-ipixel%72,ipixel/72+1,adc_value);
                                }                            
                            ca_adcMap->cd(ich+1);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(ich+1)->Modified();
                            ca_adcMap->cd(ich+1)->Update();                             
                        }
                        adcMap = Form("pd3png/adcMapFrame%d.png", iframe);               
                        ca_adcMap->SaveAs(adcMap); 
                        signalCount = 1;
                }
                else {
                    signalCount++;
                }
            }
           
        }
    }
		
  
  void drawRow(int ich, int iframe)
   {

 
		int nTopMetalChips=pd3.adcCha();
		int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        
		TCanvas *ca_adc2dMap = new TCanvas("ca_adc2dMap", "pd3 ADC 2d map per row", 600, 400);
        char *adc2dMap; 
	TString hist2dName;
        hist2dName = "hAdc2dMap_Chip"; hist2dName += ich;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);

        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }
        adc2dMap = Form("pd3png/adc2dMapFrame%dChip%d.png", iframe, ich);
        hAdc2dMap->Draw("colz");
	hAdc2dMap->SetMaximum(100);
	hAdc2dMap->SetMinimum(-20);
        ca_adc2dMap->SaveAs(adc2dMap);
       
        double upLimit = 40;
	double lowLimit = 0;
        
		TCanvas *ca_adc1dMap = new TCanvas("ca_adc1dMap", "pd3 ADC 1d map per row", 600, 400);
        char *adc1dMap; 
	TString hist1dName; 
        hist1dName = "hAdc1dMap_Chip"; hist1dName += ich;
       // TF1 *fgaus = new TF1("fgaus", "gaus", gua)
        for(int irow = 0; irow < 72; irow++){
 	       TH1D *hAdc1dMap= new TH1D(hist1dName, "pd3 ADC 1d Map;pixel column index", 72, 0, 72);        
            adc1dMap = Form("pd3png/adcMapRow%dFrame%dChip%d.png", irow, iframe, ich);
            for(int icol = 0; icol < 72; icol++){
            //    hAdc1dMap->SetBinContent(icol+1, hAdc2dMap->GetBinContent(icol+1, irow+1));
		adc_value = hAdc2dMap->GetBinContent(icol+1, irow+1);
		if (adc_value<upLimit&& adc_value>lowLimit){
               hAdc1dMap->SetBinContent(icol+1, adc_value);				
		
		}
		else{
               hAdc1dMap->SetBinContent(icol+1, 0);								
		}
            }
		hAdc1dMap->Draw();
	  
	 //  hAdc1dMap->Fit("gaus","Q");
	  hAdc1dMap->SetMaximum(100);
	  hAdc1dMap->SetMinimum(-20);
            ca_adc1dMap->SaveAs(adc1dMap);                 
        }

} 
  void fitBeam(int ich, int iframe)
   {

 
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        


        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;



	TH1D *hMeanVSFrame = new TH1D("hMeanVSFrame", "MeanVSFrame", nFrames, 0, nFrames);
	TString hist2dName;
        hist2dName = "hAdc2dMap_iframe"; hist2dName += iframe;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }

	double rowSum;
	double pixelValue;
	double pixelMean;
	double pixelSigma2;
        double upLimit = 100;
	double lowLimit = 0;
	double pitch = 83;
	double b;
	double k;



	TGraphErrors ge;	
        for(int irow = 0; irow < 72; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(pixelValue/rowSum)*pitch*pitch/12; 		
		}

		}
		
		ge.SetPoint(irow+1, irow*pitch, pixelMean );
		ge.SetPointError(irow+1, 0, sqrt(pixelSigma2));

	}



	TCanvas *c=new TCanvas("c","c",800,400);
	c->Divide(2,1);

        c->cd(1);

	TF1 *fit1 = new TF1("fit1", "pol1", pitch, 71*pitch);
	fit1->SetLineColor(1);
	fit1->SetLineWidth(3);

	ge.Draw("ap*");
	
	ge.Fit("fit1", "Q");
	
	c->cd(2);
// 
	TH1D *dMean = new TH1D("dMean", "dMean", 10, -100, 100);
	b = fit1->GetParameter(0);
	k = fit1->GetParameter(1);
	double di;
	double x, y;
	double ey;
        for(int irow = 1; irow < 71; irow++){
	ge.GetPoint(irow,x,y);
	ey=ge.GetErrorY(irow);
	cout<<"y: "<<y<<endl;
	cout<<"ey: "<<ey<<endl;
	di = y-(k*irow*pitch+b);
	cout<<"di: "<<di<<endl;

	dMean->Fill(di/ey);
	 	
	}
	dMean->Draw();
	dMean->Fit("gaus");
      
	char *beamMean =Form("pd3png/beamFitChip%dFrame%d.png",ich,iframe);
        c->SaveAs(beamMean);





 

} 

  void fitBeamError(int ich)
   {

 
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        


        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;

	TCanvas *c=new TCanvas("c","c",700,600);
	TH1D *dMean = new TH1D("dMean", "Mean", 100, -500, 500);
	for(int iframe=0; iframe<nFrames; iframe++){

	TString hist2dName;
        hist2dName = "hAdc2dMap_iframe"; hist2dName += iframe;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }

	double rowSum;
	double pixelValue;
	double pixelMean;
	double pixelSigma2;
        double upLimit = 100;
	double lowLimit = 0;
	double pitch = 83;
	double b;
	double k;



	TGraphErrors ge;	
        for(int irow = 0; irow < 72; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(pixelValue/rowSum)*pitch*pitch/12; 		
		}

		}
		
		ge.SetPoint(irow+1, irow*pitch, pixelMean );
		ge.SetPointError(irow+1, 0, sqrt(pixelSigma2)*32);

	}





	TF1 *fit1 = new TF1("fit1", "pol1", pitch, 71*pitch);

        ge.Fit("fit1", "QR");
	

// 

	b = fit1->GetParameter(0);
	k = fit1->GetParameter(1);
	double di;
	double x, y;
	double ey;
        for(int irow = 1; irow < 71; irow++){
      
	ge.GetPoint(irow,x,y);
	ey=ge.GetErrorY(irow);
//	cout<<"y: "<<y<<endl;
//	cout<<"ey: "<<ey<<endl;
	di = y-(k*irow*pitch+b);
//	cout<<"di: "<<di<<endl;

//	dMean->Fill(di/ey);
	dMean->Fill(di);
	 	
	}
}
	dMean->GetXaxis()->SetTitle("Mean (#mum)");
	dMean->GetXaxis()->CenterTitle();
	dMean->Draw();
	dMean->Fit("gaus", "Q");
      
	char *beamMean =Form("pd3png/beamFitErrorChip%d.png",ich);
        c->SaveAs(beamMean);





 

} 


  void beamPosResol(int ich, double upLimit, double lowLimit)
   {

 
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        

	double rowSum;
	double pixelValue;
	double pixelMean;
    //double upLimit = 100;
	//double lowLimit = 0;
	double pitch = 83;
	double b;
	double k;

	double di;
	double pointX, pointY; 
	double diMean=0;
	double diRMS=0;
	double resol=0;
	double resolMean=0;
	double resolRMS=0;
	double centerMean = 0;
	double centerRMS = 0;


        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;


	TCanvas *cPosResol=new TCanvas("cPosResol","Position Resolution",700,600);
	TH1D *h1PosResol = new TH1D("h1PosResol", "Position Resolution", 100, 5, 30);

//	TCanvas *cCenterPos=new TCanvas("cCenterPos","Center Position",700,600);
//	TH1D *h1CenterPos = new TH1D("h1CenterPos", "Center Position", 100, -0.1, 0.1);
	//nFrames = 2;
	for(int iframe=1; iframe<nFrames; iframe++){

	TString hist2dName;
        hist2dName = "hAdc2dMap_iframe"; hist2dName += iframe;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);



        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }




	TGraphErrors ge;	
        for(int irow = 1; irow < 71; irow++){
        	rowSum = 0;
		pixelMean = 0;

            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;

		}

		}
		
		ge.SetPoint(irow+1, (irow+1)*pitch, pixelMean );
		ge.SetPointError(irow+1, 0, 0);

	}





	TF1 *fit1 = new TF1("fit1", "pol1", 2*pitch, 71*pitch);

        ge.Fit("fit1", "QR");
	

// 

	b = fit1->GetParameter(0);
	k = fit1->GetParameter(1);
	diMean = 0;
	diRMS = 0;
	
        for(int irow = 1; irow < 71; irow++){
      
	ge.GetPoint((irow+1),pointX,pointY);

	di = (pointY-(k*(irow+1)*pitch+b))*cos(atan(k));
	
	diMean = diMean + di;
	diRMS = diRMS + di*di;
 
        }


	diRMS = sqrt(abs(diRMS-diMean*diMean/70)/(70-1));
	diMean = diMean/70;
	resol = diRMS/sqrt(70);
	

	h1PosResol->Fill(resol); 
	resolMean = resolMean + resol;
	resolRMS = resolRMS + resol*resol;	
	
//	h1CenterPos->Fill(diMean);

//	centerMean = centerMean + diMean;
//	centerRMS = centerRMS + diMean*diMean;
}

	resolRMS = sqrt(abs(resolRMS-resolMean*resolMean/(nFrames-1))/(nFrames-2));
	resolMean = resolMean/(nFrames-1);
	cout<<"resolMean: "<<resolMean<<endl;
	cout<<"resolRMS: "<<resolRMS<<endl;
// calculate FWHM

 	int bin1 = h1PosResol->FindFirstBinAbove(h1PosResol->GetMaximum()/2);
   	int bin2 = h1PosResol->FindLastBinAbove(h1PosResol->GetMaximum()/2);
        double fwhm = (h1PosResol->GetBinCenter(bin2) - h1PosResol->GetBinCenter(bin1));
	cout<<"FWHM: "<<fwhm<<endl;
	h1PosResol->GetXaxis()->SetTitle("Position Resolution (#mum)");
	h1PosResol->GetXaxis()->CenterTitle();
	cPosResol->cd();	
	h1PosResol->Draw();

	//h1PosResol->Fit("gaus", "Q");
      
	char *beamPosResolNamePng =Form("pd3png/beamPositionResolutionChip%d.png",ich);
        cPosResol->SaveAs(beamPosResolNamePng);
	char *beamPosResolNamePdf =Form("pd3png/beamPositionResolutionChip%d.pdf",ich);
        cPosResol->SaveAs(beamPosResolNamePdf);
/*	centerRMS = sqrt(abs(centerRMS-centerMean*centerMean/(nFrames-1))/(nFrames-2));	
	centerMean = centerMean/(nFrames-1);
	cout<<"centerMean: "<<centerMean<<endl;
	cout<<"centerRMS: "<<centerRMS<<endl;
	h1CenterPos->GetXaxis()->SetTitle("Center Position (#mum)");
	h1CenterPos->GetXaxis()->CenterTitle();
	cCenterPos->cd();
	h1CenterPos->Draw();
	h1CenterPos->Fit("gaus","Q");
	char *beamCenterPosName =Form("pd3png/beamCenterPosition%d.png",ich);
	cCenterPos->SaveAs(beamCenterPosName);*/
	delete cPosResol;

} 

  void beamAngleResol(int ich, double upLimit, int lowLimit)
   {

 
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        

	double rowSum;
	double pixelValue;
	double pixelMean;
   //     double upLimit = 100;
	//double lowLimit = 0;
	double pitch = 83;
	double b;
	double k;
	double theta = 0;
	double thetaMean = 0;
	double thetaRMS = 0;
        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;
    int rowLow=1;
    int rowUp=71;

	TCanvas *cAngle=new TCanvas("cAngle","Angle",700,600);
	TH1D *h1Angle = new TH1D("h1Angle", "Angle", 100,3, 16);


	//nFrames = 2;
	for(int iframe=1; iframe<nFrames; iframe++){

	TString hist2dName;
        hist2dName = "hAdc2dMap_iframe"; hist2dName += iframe;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);



        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }




	TGraphErrors ge;	
//        for(int irow = 0; irow < 72; irow++){
	int iPoint=0;
        for(int irow = rowLow; irow < rowUp; irow++){
		iPoint++;
        	rowSum = 0;
		pixelMean = 0;

            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;

		}

		}
		
		ge.SetPoint(iPoint, (irow+1)*pitch, pixelMean );
		ge.SetPointError(iPoint, 0, 0);

	}





	TF1 *fit1 = new TF1("fit1", "pol1", (rowLow+1)*pitch, (rowUp)*pitch);

        ge.Fit("fit1", "QR");
	

// 

	b = fit1->GetParameter(0);
	k = fit1->GetParameter(1);
	theta = atan(k)*180/TMath::Pi();
	thetaMean = thetaMean + theta;
	thetaRMS = thetaRMS + theta*theta;
	h1Angle->Fill(theta);
	
}


	thetaRMS = sqrt(abs(thetaRMS-thetaMean*thetaMean/(nFrames-1))/(nFrames-2));
	thetaMean = thetaMean/(nFrames-1);

	cout<<"thetaMean: "<<thetaMean<<endl;
	cout<<"thetaRMS: "<<thetaRMS<<endl;

	h1Angle->GetXaxis()->SetTitle("Angle (#circ)");
	h1Angle->GetXaxis()->CenterTitle();
	cAngle->cd();	
	h1Angle->Draw();
	h1Angle->Fit("gaus","Q");


      
	char *beamAngleNamePng =Form("pd3png/beamAngleChip%d.png",ich);
        cAngle->SaveAs(beamAngleNamePng);
	char *beamAngleNamePdf =Form("pd3png/beamAngleChip%d.pdf",ich);
        cAngle->SaveAs(beamAngleNamePdf);
	delete cAngle;

}

void beamPosResolCheck(int ich, double upLimit, double lowLimit)
   {

	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        



	TCanvas *cOddEven=new TCanvas("cOdd","cOdd",700,600);






	double rowSum;
	double pixelValue;
	double pixelMean;
    //double upLimit = 100;
	//double lowLimit = 0;
	double pitch = 83;


	double dCenter;
	double centerOdd;
	double centerEven;
	double kOdd;
	double kEven;
	double bOdd;
	double bEven;

        TH1D *hCenterPos = new TH1D("hCenterPos","#DeltaCenterPosition/#sqrt{2}" , 100, -150,150 );
	double dCenterMean=0;
	double dCenterRMS=0;

        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;

//	nFrames = 2;
	for(int iframe=1; iframe<nFrames; iframe++){


	TString hist2dName;
        hist2dName = "hAdc2dMap_iframe"; hist2dName += iframe;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }


	




	TGraphErrors ge1;
	int iPoint1=0;	
        for(int irow = 0; irow < 72; irow++){

		iPoint1++;
//		cout<<"irow: "<<irow<<endl;
//		cout<<"iPoint: "<<iPoint1<<endl;
        	rowSum = 0;
		pixelMean = 0;

            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;

		}

		}
		
		ge1.SetPoint(iPoint1, (irow+1)*pitch, pixelMean );
		ge1.SetPointError(iPoint1, 0, 0);
		irow++;

	}


	TGraphErrors ge2;
	int iPoint2=0;	
        for(int irow = 1; irow < 72; irow++){
		iPoint2++;
//		cout<<"irow: "<<irow<<endl;
//		cout<<"iPoint: "<<iPoint2<<endl;
        	rowSum = 0;
		pixelMean = 0;

            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;

		}

		}
		
		ge2.SetPoint(iPoint2, (irow+1)*pitch, pixelMean );

		ge2.SetPointError(iPoint2, 0, 0);
		irow++;

	}


	TF1 *fit1 = new TF1("fit1", "pol1",2*pitch, 70*pitch);

	TF1 *fit2 = new TF1("fit2", "pol1",2*pitch, 70*pitch);	
   if(iframe<20){


	TString hist2dNameRotat;
        hist2dNameRotat = "hAdc2dMap_iframe"; hist2dNameRotat += iframe;

        TH2D *hAdc2dMapRotat= new TH2D(hist2dNameRotat, "", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMapRotat->SetBinContent(binY+1,binX+1,adc_value);            
        }

	hAdc2dMapRotat->GetXaxis()->SetTitle("#mum");
	hAdc2dMapRotat->GetXaxis()->CenterTitle();
	hAdc2dMapRotat->GetYaxis()->SetTitleOffset(1.4);
	hAdc2dMapRotat->GetYaxis()->SetTitle("#mum");	
	hAdc2dMapRotat->GetYaxis()->CenterTitle();
	hAdc2dMapRotat->SetMaximum(upLimit);
	hAdc2dMapRotat->SetMinimum(-20);			


	
	cOddEven->cd();
	hAdc2dMapRotat->Draw("COLZ");


		
	fit1->SetLineColor(1);
	fit1->SetLineWidth(3);
	ge1.Draw("same p*");
	ge1.Fit("fit1","QR");

	
	fit2->SetLineColor(4);
	fit2->SetLineWidth(3);
	ge2.Draw("same p*");
	ge2.Fit("fit2","QR");	
	

	char *beamMeanOddEven =Form("pd3png/beamMeanOddEvenChip%dFrame%d.png",ich,iframe);
	cOddEven->SaveAs(beamMeanOddEven);

}
else{


	ge1.Fit("fit1","QR");

	ge2.Fit("fit2","QR");
}






	bOdd = fit1->GetParameter(0);
	kOdd = fit1->GetParameter(1);
	centerOdd = kOdd*36*pitch+bOdd;
	bEven = fit2->GetParameter(0);
	kEven = fit2->GetParameter(1);
	centerEven = kEven*36*pitch+bEven;
	dCenter = centerOdd-centerEven;
	hCenterPos->Fill(dCenter/sqrt(2));
	dCenterMean = dCenterMean + dCenter;
	dCenterRMS = dCenterRMS + dCenter*dCenter;

}



	dCenterRMS = sqrt(abs(dCenterRMS-dCenterMean*dCenterMean/(nFrames-1))/(nFrames-2));
	dCenterMean = dCenterMean/(nFrames-1);




	cout<<"dCenterMean: "<<dCenterMean<<endl;
	cout<<"dCenterRMS: "<<dCenterRMS<<endl;



	TCanvas *cCenterPos = new TCanvas("cCenterPos", "#DeltaCenterPosition/#sqrt{2}", 700, 600);
	cCenterPos->cd();
	hCenterPos->GetXaxis()->SetTitle("#DeltaCenterPosition/#sqrt{2} (#mum)");
	hCenterPos->GetXaxis()->CenterTitle();
	hCenterPos->Draw();
	hCenterPos->Fit("gaus","Q");
	char *beamPosResolCheckNamePng =Form("pd3png/beamPositionResolutionCheckChip%d.png",ich);
        cCenterPos->SaveAs(beamPosResolCheckNamePng);
	char *beamPosResolCheckNamePdf =Form("pd3png/beamPositionResolutionCheckChip%d.pdf",ich);
        cCenterPos->SaveAs(beamPosResolCheckNamePdf);



 	delete cOddEven;
	delete cCenterPos;


}




 void beamPosFit(int ich, int iframe, double upLimit, double lowLimit)
   {

 
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        

	double rowSum;
	double pixelValue;
	double pixelMean;
	double pixelSigma2;
    //double upLimit = 100;
	//double lowLimit = 0;
	double pitch = 83;
	double b;
	double k;

	double di;
	double pointX, pointY; 
	double diMean=0;
	double diRMS=0;
	double resol=0;
	double resolMean=0;
	double resolRMS=0;


        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;







	TString hist2dName;
        hist2dName = "hAdc2dMap_iframe"; hist2dName += iframe;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);



        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }




	TGraphErrors ge;
	TGraphErrors geRota;	
	int iPoint=0;
        for(int irow = 1; irow < 71; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 =0;
         for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;

		}

		}

	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*((icol+1)*pitch-pixelMean)*((icol+1)*pitch-pixelMean);

		}

		}
		
		ge.SetPoint(iPoint, (irow+1)*pitch, pixelMean );
		ge.SetPointError(iPoint, 0, sqrt(pixelSigma2));
//		ge.SetPointError(iPoint, 0, 0);
		geRota.SetPoint(iPoint, pixelMean, (irow+1)*pitch );
		geRota.SetPointError(iPoint, sqrt(pixelSigma2), 0);
//		geRota.SetPointError(iPoint, 0, 0);
		iPoint++;

	}





	TF1 *fit1 = new TF1("fit1", "pol1", 2*pitch, 71*pitch);

        ge.Fit("fit1", "QR");
	

// 

	b = fit1->GetParameter(0);
	k = fit1->GetParameter(1);

	diMean = 0;
	diRMS = 0;
	iPoint = 0;
	
        for(int irow = 1; irow < 71; irow++){
      
	ge.GetPoint(iPoint,pointX,pointY);

	di = (pointY-(k*(irow+1)*pitch+b))*cos(atan(k));
	
	diMean = diMean + di;
	diRMS = diRMS + di*di;
	iPoint++;
 
        }


	diRMS = sqrt(abs(diRMS-diMean*diMean/70)/(70-1));
	diMean = diMean/70;
	resol = diRMS/sqrt(70);


	cout<<"resol: "<<resol<<endl;

	TString hist2dNameRotat;
        hist2dNameRotat = "hAdc2dMap_iframe"; hist2dNameRotat += iframe;

        TH2D *hAdc2dMapRotat= new TH2D(hist2dNameRotat, "", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMapRotat->SetBinContent(binY+1,binX+1,adc_value);            
        }



	TString hist2dNameRotat90;
        hist2dNameRotat90 = "hAdc2dMaRotat90";
        TH2D *hAdc2dMapRotat90= new TH2D(hist2dNameRotat90, "", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);


        for(int irow = 0; irow < 72; irow++){
            for(int icol = 0; icol < 72; icol++){
		hAdc2dMapRotat90->SetBinContent(irow+1, icol+1, hAdc2dMapRotat->GetBinContent(icol+1, irow+1));
	    	
		}


	}

	hAdc2dMapRotat90->GetXaxis()->SetTitle("#mum");
	hAdc2dMapRotat90->GetXaxis()->CenterTitle();
	hAdc2dMapRotat90->GetYaxis()->SetTitleOffset(1.6);
	hAdc2dMapRotat90->GetYaxis()->SetTitle("#mum");	
	hAdc2dMapRotat90->GetYaxis()->CenterTitle();	
	hAdc2dMapRotat90->SetMaximum(upLimit);
	hAdc2dMapRotat90->SetMinimum(-20);	

			

        TCanvas *cBeamPosFit = new TCanvas("cBeamPosFit", "Beam Position Fit", 800 ,800);

	
	cBeamPosFit->cd();
        cBeamPosFit->SetCanvasSize(750, 750);
	hAdc2dMapRotat90->Draw("COLZ");

	geRota.Draw("same p*");


     /*   TF1 *fit1Rota = new TF1("fit1Rota", "1/[0]*x-[1]/[0]", pitch, 73*pitch);
	fit1Rota->SetParameter(0,k);
	fit1Rota->SetParameter(1,b);
	fit1Rota->Draw("same");*/

	TGraph gPoint;
	iPoint = 0;
       for(int irow = 1; irow < 71; irow++){

        gPoint.SetPoint(iPoint, (irow+1)*pitch*k+b, (irow+1)*pitch);
	iPoint++;

	}
	gPoint.Draw("same c");
	gPoint.SetLineColor(2);
	gPoint.SetLineWidth(4);

	char *beamPosFitNamePng =Form("pd3png/beamPositionFitChip%dFrame%d.png",ich, iframe);
        cBeamPosFit->SaveAs(beamPosFitNamePng);

	char *beamPosFitNamePdf =Form("pd3png/beamPositionFitChip%dFrame%d.pdf",ich, iframe);
        cBeamPosFit->SaveAs(beamPosFitNamePdf);
	delete cBeamPosFit;

} 


void beamPresentation(int ich, double upLimit, double lowLimit)
   {

	gStyle->SetOptStat(0);
//	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        
       cout<<"nFrames: "<<nFrames<<endl;


        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;


	double refMean=2712;

	int binMove;
	double centerMean=0;
	double centerRMS=0;
	double thetaMean=0;
	double thetaRMS=0;
   //     double upLimit = 100;
	//double lowLimit = 0;

	double rowSum;
	double pixelValue;
	double pixelMean;
	double pixelSigma2;

	double theta;
	double mean;
	double mean_fit = 2712;
	double theta_fit = 0.007;

	TString hist2dName;
        hist2dName = "hAdc2dMap";
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
	double pitch = 83; //um 
	TString hist2dNameRotat;
        hist2dNameRotat = "hAdc2dMaRotat";
        //TH2D *hAdc2dMapRotat= new TH2D(hist2dNameRotat, "pd3 ADC 2d Map", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);
        TH2D *hAdc2dMapRotat= new TH2D(hist2dNameRotat, "", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);

	//nFrames = 1;
	for(int iframe=0; iframe<nFrames; iframe++){
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];        
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }





	TGraphErrors ge;	
        for(int irow = 1; irow < 71; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(pixelValue/rowSum)*pitch*pitch/12; 		
		}

		}
		
		ge.SetPoint(irow+1, irow*pitch, pixelMean );
		//ge.SetPointError(irow+1, pitch/sqrt(12), sqrt(pixelSigma2));
		ge.SetPointError(irow+1, 0, 0);

	}

// first fit

	TF1 *fit1 = new TF1("fit1", "tan([1])*x+[0]/cos([1])",2*pitch, 70*pitch);


	//fit1->SetParameter(0, mean_fit);
	//fit1->SetParameter(1, theta_fit);		
	fit1->SetLineColor(1);
	fit1->SetLineWidth(3);
	fit1->SetParName(0, "Mean");
	fit1->SetParName(1, "#theta");
	ge.Fit("fit1","QR");
	mean = fit1->GetParameter(0);
	theta = fit1->GetParameter(1);
	binMove = TMath::Nint((refMean-mean)/pitch);
//	cout<<"refMean: "<<refMean<<endl;
//	cout<<"mean: "<<mean<<endl;
//	cout<<"binMove: "<<binMove<<endl;

// move bin


    

       for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe]; 
	    if(adc_value>upLimit || adc_value<lowLimit)
	    adc_value = 0;
		if(binX+1+binMove>=1 & binX+1+binMove<=72){
		if(adc_value!=0){	           
            hAdc2dMapRotat->SetBinContent(binY+1,binX+1+binMove,(adc_value+hAdc2dMapRotat->GetBinContent(binY+1,binX+1+binMove))/2);

		} 
		}

		if(binX<binMove){
		if(adc_value!=0){	           
           hAdc2dMapRotat->SetBinContent(binY+1,binX+1,(adc_value+hAdc2dMapRotat->GetBinContent(binY+1,binX+1))/2);

		} 
		}
		if(binX>71+binMove){
		if(adc_value!=0){	           
           hAdc2dMapRotat->SetBinContent(binY+1,binX+1,(adc_value+hAdc2dMapRotat->GetBinContent(binY+1,binX+1))/2);

		} 
	}
	
        } 



}	


 
	TString hist2dNameRotat90;
        hist2dNameRotat90 = "hAdc2dMaRotat90";
        TH2D *hAdc2dMapRotat90= new TH2D(hist2dNameRotat90, "", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);


        for(int irow = 0; irow < 72; irow++){
            for(int icol = 0; icol < 72; icol++){
		hAdc2dMapRotat90->SetBinContent(irow+1, icol+1, hAdc2dMapRotat->GetBinContent(icol+1, irow+1));

		}


	}
	hAdc2dMapRotat90->GetXaxis()->SetTitle("#mum");
	hAdc2dMapRotat90->GetXaxis()->CenterTitle();
//	hAdc2dMapRotat90->GetXaxis()->SetRangeUser(-20, 6000);
	hAdc2dMapRotat90->GetYaxis()->SetTitleOffset(1.6);
	hAdc2dMapRotat90->GetYaxis()->SetTitle("#mum");	
	hAdc2dMapRotat90->GetYaxis()->CenterTitle();	
//	hAdc2dMapRotat90->GetYaxis()->SetRangeUser(-20, 6000);
	hAdc2dMapRotat90->SetMaximum(40);
	hAdc2dMapRotat90->SetMinimum(0);	

	TCanvas *cFrame = new TCanvas("cFrame","cFrame", 800,800);
	cFrame->cd();

        cFrame->SetCanvasSize(750, 750);


	hAdc2dMapRotat90->Draw("COLZ");	
	

	char *beamFrameAvePdf =Form("pd3png/beamFrameAveChip%d.png",ich);
	char *beamFrameAvePng =Form("pd3png/beamFrameAveChip%d.pdf",ich);
        cFrame->SaveAs(beamFrameAvePdf);
        cFrame->SaveAs(beamFrameAvePng);

	TGraphErrors ge1;
	TGraphErrors ge1Rota;	
	int iPoint=0;
        for(int irow = 1; irow < 71; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
          for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;

		}

		}

	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*((icol+1)*pitch-pixelMean)*((icol+1)*pitch-pixelMean);

		}

		}
		


		ge1.SetPoint(iPoint, (irow+1)*pitch, pixelMean );
		ge1.SetPointError(iPoint, 0, sqrt(pixelSigma2));
	//	ge1.SetPointError(iPoint, 0, 0);

		ge1Rota.SetPoint(iPoint, pixelMean, (irow+1)*pitch );
		ge1Rota.SetPointError(iPoint, sqrt(pixelSigma2), 0);
	//	ge1Rota.SetPointError(iPoint, 0, 0);
		iPoint++;
	}





	TF1 *fit2= new TF1("fit2", "pol1", 2*pitch, 71*pitch);
        ge1.Fit("fit2", "QR");

	double b;
	double k;

	double diMean;
	double diRMS;
	double pointX;
	double pointY;
	double di;
	
	b = fit2->GetParameter(0);
	k = fit2->GetParameter(1);
/*	diMean = 0;
	diRMS = 0;
	
        for(int irow = 1; irow < 71; irow++){
      
	ge1.GetPoint((irow+1),pointX,pointY);

	di = (pointY-(k*(irow+1)*pitch+b))*cos(atan(k));

	diMean = diMean + di;
	diRMS = diRMS + di*di;
 
        }


	diRMS = sqrt(abs(diRMS-diMean*diMean/70)/(70-1));
	diMean = diMean/70;


       for(int irow = 1; irow < 71; irow++){

        ge1.SetPointError(irow+1, 0, diRMS);
        ge1Rota.SetPointError(irow+1, diRMS, 0);
	}
		
*/

	TCanvas *cFrameFit = new TCanvas("cFrameFit", "cFrameFit", 800, 800);
	cFrameFit->cd();
        cFrameFit->SetCanvasSize(750, 750);
	//cFrameFit->SetLogz(); 
	hAdc2dMapRotat90->Draw("COLZ");


	





	ge1Rota.Draw("same p*");
       // ge1.Fit("fit2", "QR");

      /*TF1 *fit2Rota = new TF1("fit2Rota", "1/[0]*x-[1]/[0]",pitch, 72*pitch);
	fit2Rota->SetParameter(0,k);
	fit2Rota->SetParameter(1,b);
	fit2Rota->Draw("same ");*/

	TGraph gPoint;
	iPoint = 0;
       for(int irow = 1; irow < 71; irow++){

        gPoint.SetPoint(iPoint, (irow+1)*pitch*k+b, (irow+1)*pitch);
	iPoint++;

	}
	gPoint.Draw("same c");
	gPoint.SetLineColor(2);
	gPoint.SetLineWidth(4);

	char *beamFrameAveFitPdf =Form("pd3png/beamFrameAveFitChip%d.png",ich);
	char *beamFrameAveFitPng =Form("pd3png/beamFrameAveFitChip%d.pdf",ich);

       cFrameFit->SaveAs(beamFrameAveFitPdf);
       cFrameFit->SaveAs(beamFrameAveFitPng);


	delete cFrameFit;

	delete cFrame;


}




 void beamIntensityResol(int ich, double upLimit, double lowLimit)
   {

	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        

        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;

	TH1D *hIntensity = new TH1D("hIntensity","#DeltaQ/Q", 100, -0.1, 0.1);

	double pixelValue;
  //      double upLimit = 100;
//	double lowLimit = 0;
	double pitch = 83.2;

	double sumEup=0;
	double sumEdowm=0;
	double k;
	double Eup=0;
	double Edown=0;
	double deltaE;
	double deltaEMean = 0;
	double deltaERMS=0;

	

	//nFrames = 2;
	for(int iframe=1; iframe<nFrames; iframe++){
	TString hist2dName;
        hist2dName = "hAdc2dMap_iframe"; hist2dName += iframe;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }


	for(int irow= 1; irow<36; irow++){
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		sumEup = sumEup + pixelValue;		
		}			
		}
	
	}	

	for(int irow= 36; irow<71; irow++){
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		sumEdowm = sumEdowm+ pixelValue;		
		}			
		}
	
	}

}

	k = sumEup/sumEdowm;
	cout<<"k: "<< k<<endl;
	for(int iframe=0; iframe<nFrames; iframe++){
	Eup = 0;
	Edown = 0;
	TString hist2dName;
        hist2dName = "hAdc2dMap_iframe"; hist2dName += iframe;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }


	for(int irow= 1; irow<36; irow++){
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		Eup = Eup + pixelValue;		
		}			
		}
	
	}	

	for(int irow= 36; irow<71; irow++){
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		Edown = Edown+ pixelValue;		
		}			
		}
	
	}

     //  deltaE= (Eup-k*Edown)/(Eup+k*Edown);
	deltaE =  (Eup-k*Edown)*2/(Eup+k*Edown);
       hIntensity->Fill(deltaE);
	deltaEMean = deltaEMean+deltaE;
	deltaERMS = deltaERMS + deltaE*deltaE;	

}



	deltaERMS = sqrt(abs(deltaERMS-deltaEMean*deltaEMean/(nFrames-1))/(nFrames-2));
	deltaEMean = deltaEMean/(nFrames-1);


	cout<<"deltaEMean: "<<deltaEMean<<endl;
	cout<<"deltaERMS: "<<deltaERMS<<endl;



	TCanvas *cIntensity = new TCanvas("cIntensity", "Intensity Resolution", 700 , 600);
	cIntensity->cd();
        hIntensity->Draw();
	hIntensity->GetXaxis()->SetTitle("#DeltaQ/Q");
	hIntensity->GetXaxis()->CenterTitle();
	hIntensity->Fit("gaus","Q");

	char *beamIntensityPng = Form("pd3png/beamIntensityResolutionChip%d.png", ich);
	cIntensity->SaveAs(beamIntensityPng);

	char *beamIntensityPdf = Form("pd3png/beamIntensityResolutionChip%d.pdf", ich);
	cIntensity->SaveAs(beamIntensityPdf);

	delete cIntensity;


}



 void beamSize(int ich, double upLimit, double lowLimit)
   {

	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        



        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;

        TH1D *hBeamSize = new TH1D("hBeamSize","Beam Size", 100, 1000, 2000);
	TCanvas *cBeamSizeCol = new TCanvas("cBeamSizeCol", "Beam Size", 700, 600);

	double rowSum;
	double pixelValue;
	double pixelMean;
	double pixelSigma2;
	double beamSizeCol;
	double beamSize;
	double beamSizeMean;
	double beamSizeRMS;

    //    double upLimit = 100;
	//double lowLimit = 0;
	double pitch = 83;

	for(int iframe=1; iframe<nFrames; iframe++){
	TString hist2dName;
        hist2dName = "hAdc2dMap_iframe"; hist2dName += iframe;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }


	TGraph g;	
	beamSize = 0;
        for(int irow = 1; irow < 71; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;

		}

		}

	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*((icol+1)*pitch-pixelMean)*((icol+1)*pitch-pixelMean);

		}

		}
		beamSizeCol = sqrt(pixelSigma2);


		g.SetPoint((irow-1), irow*pitch,beamSizeCol );
		beamSize = beamSize + beamSizeCol;

	}

   if(iframe<20){
     cBeamSizeCol->cd();
    g.SetMinimum(0);
    g.SetMaximum(1650);
    g.Draw("ap*");
    g.SetTitle("Beam Size");
    g.GetXaxis()->SetTitle("Beam Direction");
   g.GetXaxis()->CenterTitle();
   g.GetYaxis()->SetTitle("beam size (#mum)");
   g.GetYaxis()->CenterTitle();
   g.GetYaxis()->SetTitleOffset(1.4);
   
	char *beamSizeColPng =Form("pd3png/beamSizeColumnChip%dFrame%d.png",ich,iframe);
        cBeamSizeCol->SaveAs(beamSizeColPng);
	}




	beamSize = beamSize/70;

	hBeamSize->Fill(beamSize);
	beamSizeMean = beamSizeMean+beamSize;
	beamSizeRMS = beamSizeRMS + beamSize*beamSize;	
  
	 
        }


        beamSizeRMS = sqrt(abs(beamSizeRMS-beamSizeMean*beamSizeMean/(nFrames-1))/(nFrames-2));
	beamSizeMean = beamSizeMean/(nFrames-1);

	cout<<"beamSizeMean: "<<beamSizeMean<<endl;
	cout<<"beamSizeRMS: "<<beamSizeRMS<<endl;




	TCanvas *cBeamSize = new TCanvas("cBeamSize", "Beam Size", 700 , 600);
	cBeamSize->cd();
	hBeamSize->GetXaxis()->SetTitle("Beam Size (#mum)");
	hBeamSize->GetXaxis()->CenterTitle();

	hBeamSize->Draw();
	hBeamSize->Fit("gaus","Q");

	char *BeamSizePng = Form("pd3png/beamSizeChip%d.png", ich);
	cBeamSize->SaveAs(BeamSizePng);
	char *BeamSizePdf = Form("pd3png/beamSizeChip%d.pdf", ich);
	cBeamSize->SaveAs(BeamSizePdf);

	delete cBeamSizeCol;
	delete cBeamSize;

}


void beamBadPixel(int ich, double upLimit, double lowLimit){

	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        



        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;

        TH1D *hLowPixel= new TH1D("hLowPixel","Low Pixels", 100, 400, 1200);
	TH1D *hUpPixel =new TH1D("hUpPixel","Up Pixels", 100, 400, 1400);
	TH1D *hGoodPixel = new TH1D("hGoodPixel", "Good Pixels", 100, 2500, 4500);



	double pixelValue;
        int lowPixels;
	int upPixels;
	int goodPixels;


    //    double upLimit = 100;
	//double lowLimit = 0;
	double pitch = 83;

	for(int iframe=1; iframe<nFrames; iframe++){
	lowPixels = 0;
	upPixels =0;
	goodPixels =0;
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];    
	      if(adc_value<upLimit & adc_value>lowLimit)
		goodPixels++;
	      if(adc_value>upLimit)
		upPixels++;
	      if(adc_value<lowLimit)
		lowPixels++;                
        }

	
	hLowPixel->Fill(lowPixels);
	hUpPixel->Fill(upPixels);
	hGoodPixel->Fill(goodPixels);
	
  
	 
        }







	TCanvas *cLowPixel = new TCanvas("cLowPixel", "Low Pixels", 700 , 600);
	cLowPixel->cd();
	hLowPixel->GetXaxis()->SetTitle("Low Pixels");
	hLowPixel->GetXaxis()->CenterTitle();

	hLowPixel->Draw();
	hLowPixel->Fit("gaus","Q");

	char *LowPixelPng = Form("pd3png/LowPixelChip%d.png", ich);
	cLowPixel->SaveAs(LowPixelPng);
	char *LowPixelPdf = Form("pd3png/LowPixelChip%d.pdf", ich);
	cLowPixel->SaveAs(LowPixelPdf);


	TCanvas *cUpPixel = new TCanvas("cUpPixel", "Up Pixels", 700 , 600);
	cUpPixel->cd();
	hUpPixel->GetXaxis()->SetTitle("Up Pixels");
	hUpPixel->GetXaxis()->CenterTitle();

	hUpPixel->Draw();
	hUpPixel->Fit("gaus","Q");

	char *UpPixelPng = Form("pd3png/UpPixelChip%d.png", ich);
	cUpPixel->SaveAs(UpPixelPng);
	char *UpPixelPdf = Form("pd3png/UpPixelChip%d.pdf", ich);
	cUpPixel->SaveAs(UpPixelPdf);

	TCanvas *cGoodPixel = new TCanvas("cGoodPixel", "Good Pixels", 700 , 600);
	cGoodPixel->cd();
	hGoodPixel->GetXaxis()->SetTitle("Good Pixels");
	hGoodPixel->GetXaxis()->CenterTitle();

	hGoodPixel->Draw();
	hGoodPixel->Fit("gaus","Q");

	char *GoodPixelPng = Form("pd3png/GoodPixelChip%d.png", ich);
	cGoodPixel->SaveAs(GoodPixelPng);
	char *GoodPixelPdf = Form("pd3png/GoodPixelChip%d.pdf", ich);
	cGoodPixel->SaveAs(GoodPixelPdf);

	delete cLowPixel;
	delete cUpPixel;
	delete cGoodPixel;


}
   
  void beamPara_old(int ich)
   {

	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        

        TRandom r;
//	TCanvas *c=new TCanvas("c","c",700,600);

        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;
        TH1D *hMean = new TH1D("hMean","hMean", 100, 2000, 3300);
	TH1D * hTheta = new TH1D("hTheta","hTheta", 100, -0.05, 0.05);
	TH1D *hIntensity = new TH1D("hIntensity","Intensity Uncertainty", 100, -0.1, 0.1);
	double centerMean=0;
	double centerRMS=0;
	double thetaMean=0;
	double thetaRMS=0;
	double deltaEMean = 0;
	double deltaERMS=0;

	TH1D *hMeanVSFrame = new TH1D("hMeanVSFrame", "MeanVSFrame", nFrames, 0, nFrames);
	for(int iframe=0; iframe<nFrames; iframe++){
	TString hist2dName;
        hist2dName = "hAdc2dMap_iframe"; hist2dName += iframe;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }

	double rowSum;
	double pixelValue;
	double pixelMean;
	double pixelSigma2;
        double upLimit = 100;
	double lowLimit = -20;
	double pitch = 83;
	double b;
	double theta;
	double cosTheta;
	double mean;

        TH1D *hBeamMean = new TH1D("hBeamMean", "hBeamMean", 72, 0, 72*pitch);	
        for(int irow = 0; irow < 72; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(pixelValue/rowSum)*pitch*pitch/12; 		
		}

		}
		
		hBeamMean->SetBinContent(irow+1, pixelMean);
		hBeamMean->SetBinError(irow+1, sqrt(pixelSigma2));

	}


	TF1 *fit1 = new TF1("fit1", "pol1", pitch, 70*pitch);
        //c->cd();
	hBeamMean->Fit("fit1", "QBR");

	//char *beamMean =Form("pd3png/beamMeanChip%dFrame%d.png",ich,iframe);
	//c->SaveAs(beamMean);

	b = fit1->GetParameter(0);
	theta = atan(fit1->GetParameter(1));
	cosTheta = cos(theta);
       	mean = b*cosTheta;
	hMean->Fill(mean);
	hTheta->Fill(theta);
	hMeanVSFrame->SetBinContent(iframe+1,mean);
	
	centerMean = centerMean+mean;
	centerRMS = centerRMS+ mean*mean;
	thetaMean = thetaMean+theta;
	thetaRMS = thetaRMS+theta*theta;


//  intensity

	double Eup=0;
	double Edown=0;
	double deltaE;
	int irow;

	for(int i= 0; i<36; i++){
	irow = int(r.Rndm()*71);
//	cout<< "i: "<<i<<endl;
//        cout<< "irow: "<<irow<<endl;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		Eup = Eup + pixelValue;		
		}			
		}
	
	}	

	for(int i= 0; i<36; i++){
	irow = int(r.Rndm()*71);
//	cout<< "i: "<<i<<endl;
//        cout<< "irow: "<<irow<<endl;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		Edown = Edown+ pixelValue;		
		}			
		}
	
	}
     // 	cout<< "Eup-Edown: "<<Eup-Edown<<endl;
//	cout<<"Eup+Edown: "<<Eup+Edown<<endl;
      deltaE= (Eup-Edown)/(Eup+Edown);
      hIntensity->Fill(deltaE);
	deltaEMean = deltaEMean+deltaE;
	deltaERMS = deltaERMS + deltaE*deltaE;	
  
	 
        }


	centerRMS = sqrt(abs(centerRMS-centerMean*centerMean/nFrames)/(nFrames-1));
	centerMean = centerMean/nFrames;
	thetaRMS = sqrt(abs(thetaRMS-thetaMean*thetaMean/nFrames)/(nFrames-1));
	thetaMean = thetaMean/nFrames;

	deltaERMS = sqrt(abs(deltaERMS-deltaEMean*deltaEMean/nFrames)/(nFrames-1));
	deltaEMean = deltaEMean/nFrames;

	cout<<"centerMean: "<<centerMean<<endl;
	cout<<"centerRMS: "<<centerRMS<<endl;
	cout<<"thetaMean: "<<thetaMean<<endl;
	cout<<"thetaRMS: "<<thetaRMS<<endl;
	cout<<"deltaEMean: "<<deltaEMean<<endl;
	cout<<"deltaERMS: "<<deltaERMS<<endl;

	TCanvas *c1 = new TCanvas("c1", "c1", 1200, 400);
	c1->Divide(3,1);
	c1->cd(1);
	hMean->Draw();
	hMean->Fit("gaus","Q");
//	hMean->SaveAs("hMean.C");
	c1->cd(2);
	hTheta->Draw();
        hTheta->Fit("gaus","Q");
//	hTheta->SaveAs("hTheta.C");
	c1->cd(3);
        hIntensity->Draw();
	hIntensity->Fit("gaus","Q");
//        hIntensity->SaveAs("hIntensity.C");
	char *beamPara = Form("pd3png/beamParaChip%d.png", ich);
	c1->SaveAs(beamPara);
	TCanvas *c2 = new TCanvas("c2", "c2", 700,600);
	hMeanVSFrame->Draw();
	char *beamVStime = Form("pd3png/beamVStimeChip%d.png", ich);
	c2->SaveAs(beamVStime);
//	delete c;    
	delete c1;
	delete c2;

}


 void beamPara(int ich)
   {

	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        

        TRandom r;

	TCanvas *c=new TCanvas("c","c",700,600);

        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;
        TH1D *hMean = new TH1D("hMean","Mean", 100, 2200, 3300);
	TH1D * hTheta = new TH1D("hTheta","#theta", 100, -0.05, 0.05);
	TH1D *hIntensity = new TH1D("hIntensity","#DeltaI/I", 100, -0.1, 0.1);
	double centerMean=0;
	double centerRMS=0;
	double thetaMean=0;
	double thetaRMS=0;
	double deltaEMean = 0;
	double deltaERMS=0;
	double dt=3.3;
	TH1D *hMeanVSFrame = new TH1D("hMeanVSFrame", "Mean vs Time", nFrames, 0, nFrames*dt);
	TH1D *hThetaVSFrame = new TH1D("hThetaVSFrame","#theta vs Time",nFrames, 0, nFrames*dt);
	//for(int iframe=0; iframe<nFrames; iframe++){
	for(int iframe=0; iframe<1; iframe++){
	TString hist2dName;
        hist2dName = "hAdc2dMap_iframe"; hist2dName += iframe;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }

	double rowSum;
	double pixelValue;
	double pixelMean;
	double pixelSigma2;
        double upLimit = 100;
	double lowLimit = 0;
	double pitch = 83;
	double b;
	double theta;
	double cosTheta;
	double mean;
	double mean_fit = 2712;
	double theta_fit = 0.007;
	int validPixel = 0;


	TGraphErrors ge;	
        for(int irow = 0; irow < 72; irow++){
        //for(int irow = 0; irow < 15; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(pixelValue/rowSum)*pitch*pitch/12; 

		}

		}

/*	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(icol*pitch-pixelMean)*(icol*pitch-pixelMean);

		}

		}*/
		
		ge.SetPoint(irow+1, irow*pitch, pixelMean );
		//ge.SetPointError(irow+1, pitch/sqrt(12), sqrt(pixelSigma2));
	//	ge.SetPointError(irow+1, 0, sqrt(pixelSigma2)*32);
		ge.SetPointError(irow+1, 0, sqrt(pixelSigma2));

	}

	TF1 *fit1 = new TF1("fit1", "tan([1])*x+[0]/cos([1])",2*pitch, 70*pitch);
	fit1->SetParameter(0, mean_fit);
	fit1->SetParameter(1, theta_fit);
   if(iframe<20){
     c->cd();

	TString hist2dNameRotat;
        hist2dNameRotat = "hAdc2dMap_iframe"; hist2dNameRotat += iframe;
        //TH2D *hAdc2dMapRotat= new TH2D(hist2dNameRotat, "pd3 ADC 2d Map", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);
        TH2D *hAdc2dMapRotat= new TH2D(hist2dNameRotat, "", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMapRotat->SetBinContent(binY+1,binX+1,adc_value);            
        }

	hAdc2dMapRotat->GetXaxis()->SetTitle("#mum");
	hAdc2dMapRotat->GetXaxis()->CenterTitle();
	hAdc2dMapRotat->GetYaxis()->SetTitleOffset(1.4);
	hAdc2dMapRotat->GetYaxis()->SetTitle("#mum");	
	hAdc2dMapRotat->GetYaxis()->CenterTitle();			
	hAdc2dMapRotat->Draw("COLZ");
	hAdc2dMapRotat->SetMaximum(100);
	hAdc2dMapRotat->SetMinimum(-20);
	
	
//	TF1 *fit1 = new TF1("fit1", "pol1", pitch, 71*pitch);

		
	fit1->SetLineColor(1);
	fit1->SetLineWidth(3);
	fit1->SetParName(0, "Mean");
	fit1->SetParName(1, "#theta");
	ge.Draw("same p*");
	ge.Fit("fit1","Q");

	
	


	char *beamMean =Form("pd3png/beamMeanChip%dFrame%d.png",ich,iframe);
        c->SaveAs(beamMean);
}
else{


	ge.Fit("fit1","Q");
}







/*	TF1 *fit1 = new TF1("fit1", "pol1", pitch, 71*pitch);	
	ge.Fit("fit1", "Q");
*/





//	b = fit1->GetParameter(0);
//	cout << "b: " <<b <<endl;
//	theta = atan(fit1->GetParameter(1));
//	cout << "theta: "<<theta<<endl;
//	cosTheta = cos(theta);
 //      	mean = b*cosTheta;
//	cout <<"mean: "<<mean<<endl;
	mean = fit1->GetParameter(0);
	theta = fit1->GetParameter(1);
//	cout <<"mean: "<<mean<<endl;
//	cout << "theta: "<<theta<<endl;
	hMean->Fill(mean);
	hTheta->Fill(theta);
	hMeanVSFrame->SetBinContent(iframe+1,mean);
	hThetaVSFrame->SetBinContent(iframe+1, theta);
	centerMean = centerMean+mean;
	centerRMS = centerRMS+ mean*mean;
	thetaMean = thetaMean+theta;
	thetaRMS = thetaRMS+theta*theta; 


//  intensity

	double Eup=0;
	double Edown=0;
	double deltaE;
	int irow;

	for(int i= 0; i<36; i++){
	irow = int(r.Rndm()*71);
//	cout<< "i: "<<i<<endl;
//        cout<< "irow: "<<irow<<endl;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		Eup = Eup + pixelValue;		
		}			
		}
	
	}	

	for(int i= 0; i<36; i++){
	irow = int(r.Rndm()*71);
//	cout<< "i: "<<i<<endl;
//        cout<< "irow: "<<irow<<endl;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		Edown = Edown+ pixelValue;		
		}			
		}
	
	}
     // 	cout<< "Eup-Edown: "<<Eup-Edown<<endl;
//	cout<<"Eup+Edown: "<<Eup+Edown<<endl;
      deltaE= (Eup-Edown)/(Eup+Edown);
      hIntensity->Fill(deltaE);
	deltaEMean = deltaEMean+deltaE;
	deltaERMS = deltaERMS + deltaE*deltaE;	
  
	 
        }


	centerRMS = sqrt(abs(centerRMS-centerMean*centerMean/nFrames)/(nFrames-1));
	centerMean = centerMean/nFrames;
	thetaRMS = sqrt(abs(thetaRMS-thetaMean*thetaMean/nFrames)/(nFrames-1));
	thetaMean = thetaMean/nFrames;

	deltaERMS = sqrt(abs(deltaERMS-deltaEMean*deltaEMean/nFrames)/(nFrames-1));
	deltaEMean = deltaEMean/nFrames;

	cout<<"centerMean: "<<centerMean<<endl;
	cout<<"centerRMS: "<<centerRMS<<endl;
	cout<<"thetaMean: "<<thetaMean<<endl;
	cout<<"thetaRMS: "<<thetaRMS<<endl;
	cout<<"deltaEMean: "<<deltaEMean<<endl;
	cout<<"deltaERMS: "<<deltaERMS<<endl;




	TCanvas *cMean = new TCanvas("cMean", "cMean", 700 , 600);
	cMean->cd();
	hMean->GetXaxis()->SetTitle("Mean (#mum)");
	hMean->GetXaxis()->CenterTitle();

	hMean->Draw();
	hMean->Fit("gaus","Q");
//	hMean->SaveAs("hMean.C");
	char *beamMeanPng = Form("pd3png/beamMeanChip%d.png", ich);
	cMean->SaveAs(beamMeanPng);
	char *beamMeanPdf = Form("pd3png/beamMeanChip%d.pdf", ich);
	cMean->SaveAs(beamMeanPdf);

	TCanvas *cTheta = new TCanvas("cTheta", "cTheta", 700 , 600);
	cTheta->cd();	
	hTheta->GetXaxis()->SetTitle("#theta (rad)");
	hTheta->GetXaxis()->CenterTitle();
	hTheta->Draw();
        hTheta->Fit("gaus","Q");
//	hTheta->SaveAs("hTheta.C");
	char *beamThetaPng = Form("pd3png/beamThetaChip%d.png", ich);
	cTheta->SaveAs(beamThetaPng);
	char *beamThetaPdf = Form("pd3png/beamThetaChip%d.pdf", ich);
	cTheta->SaveAs(beamThetaPdf);

	TCanvas *cIntensity = new TCanvas("cIntensity", "cIntensity", 700 , 600);
	cIntensity->cd();
        hIntensity->Draw();
	hIntensity->GetXaxis()->SetTitle("#DeltaI/I");
	hIntensity->GetXaxis()->CenterTitle();
	hIntensity->Fit("gaus","Q");

	char *beamIntensityPng = Form("pd3png/beamIntensityChip%d.png", ich);
	cIntensity->SaveAs(beamIntensityPng);
	char *beamIntensityPdf = Form("pd3png/beamIntensityChip%d.pdf", ich);
	cIntensity->SaveAs(beamIntensityPdf);
/*
	TCanvas *c2 = new TCanvas("c2", "c2",800,800);
	c2->Divide(2,2);
	c2->cd(1);
	hMeanVSFrame->Draw("p*");
	c2->cd(2);
	   TH1 *hm =0;
	   TVirtualFFT::SetTransform(0);
	   hm = hMeanVSFrame->FFT(hm, "MAG");
	   hm->SetTitle("Magnitude of the 1st transform");

	   hm->Draw();
	   gPad->SetLogy(1);
	c2->cd(3);
	hThetaVSFrame->Draw("p*");
	c2->cd(4);
	   TH1 *hn =0;
	   TVirtualFFT::SetTransform(0);
	   hn = hMeanVSFrame->FFT(hn, "MAG");
	   hn->SetTitle("Magnitude of the 1st transform");

	   hn->Draw();
	   gPad->SetLogy(1);	*/


	TCanvas *c2 = new TCanvas("c2", "c2",1600,800);
	c2->Divide(1,2);
	c2->cd(1);
	hMeanVSFrame->GetXaxis()->SetTitle("Time (ms)");
	hMeanVSFrame->GetXaxis()->CenterTitle();
	hMeanVSFrame->GetYaxis()->SetTitle("Mean (#mum)");
	hMeanVSFrame->GetYaxis()->CenterTitle();	
	hMeanVSFrame->Draw("p*");
	c2->cd(2);
	hThetaVSFrame->GetXaxis()->SetTitle("Time (ms)");
	hThetaVSFrame->GetXaxis()->CenterTitle();
	hThetaVSFrame->GetYaxis()->SetTitle("#theta (rad)");
	hThetaVSFrame->GetYaxis()->CenterTitle();	
	hThetaVSFrame->Draw("p*");

	char *beamVStime = Form("pd3png/beamVStimeChip%d.png", ich);
	c2->SaveAs(beamVStime);
	delete c;    

	delete c2;
	delete cMean;
	delete cTheta;
	delete cIntensity;
}







 void beamPara_correction_old(int ich)
   {

	gStyle->SetOptStat(1);
	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        

        TRandom r;

//	TCanvas *c=new TCanvas("c","c",700,600);

        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;
        TH1D *hMean = new TH1D("hMean","hMean", 100, -1000, 1000);
	TH1D * hTheta = new TH1D("hTheta","hTheta", 100, -0.05, 0.05);
	TH1D *hIntensity = new TH1D("hIntensity","Intensity Uncertainty", 100, -0.1, 0.1);
	double centerMean=0;
	double centerRMS=0;
	double thetaMean=0;
	double thetaRMS=0;
	double deltaEMean = 0;
	double deltaERMS=0;



	int period = 10;
	int nPeriod = nFrames/period;
	TH1D *hMeanVSFrame = new TH1D("hMeanVSFrame", "MeanVSFrame", nPeriod, 0, nPeriod);
	TH1D *hThetaVSFrame = new TH1D("hThetaVSFrame","hThetaVSFrame",nPeriod, 0, nPeriod);

	for(int iPeriod=0; iPeriod<nPeriod; iPeriod++){

	double mean_correction=0;
	double mean_medium=0;
	double theta_correction=0;
	double theta_medium=0;
	int iframe;
	for(int jframe=0; jframe<period; jframe++){
		iframe = iPeriod*period + jframe;	
	TString hist2dName;
        hist2dName = "hAdc2dMap_iframe"; hist2dName += iframe;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }

	double rowSum;
	double pixelValue;
	double pixelMean;
	double pixelSigma2;
        double upLimit = 100;
	double lowLimit = 0;
	double pitch = 83;
	double b;
	double theta;
	double cosTheta;
	double mean;
	double mean_fit = 2712;
	double theta_fit = 0.007;




	TGraphErrors ge;	
        for(int irow = 0; irow < 72; irow++){
        //for(int irow = 0; irow < 15; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(pixelValue/rowSum)*pitch*pitch/12; 		
		}

		}
		
		ge.SetPoint(irow+1, irow*pitch, pixelMean );
		//ge.SetPointError(irow+1, pitch/sqrt(12), sqrt(pixelSigma2));
		ge.SetPointError(irow+1, 0, sqrt(pixelSigma2)*32);

	}


/*
     c->cd();

	TString hist2dNameRotat;
        hist2dNameRotat = "hAdc2dMap_iframe"; hist2dNameRotat += iframe;
        //TH2D *hAdc2dMapRotat= new TH2D(hist2dNameRotat, "pd3 ADC 2d Map", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);
        TH2D *hAdc2dMapRotat= new TH2D(hist2dNameRotat, "", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMapRotat->SetBinContent(binY+1,binX+1,adc_value);            
        }

	hAdc2dMapRotat->GetXaxis()->SetTitle("#mum");
	hAdc2dMapRotat->GetXaxis()->CenterTitle();
	hAdc2dMapRotat->GetYaxis()->SetTitleOffset(1.4);
	hAdc2dMapRotat->GetYaxis()->SetTitle("#mum");	
	hAdc2dMapRotat->GetYaxis()->CenterTitle();			
	hAdc2dMapRotat->Draw("COLZ");
	hAdc2dMapRotat->SetMaximum(100);
	hAdc2dMapRotat->SetMinimum(-20);
	
	
//	TF1 *fit1 = new TF1("fit1", "pol1", pitch, 71*pitch);
	TF1 *fit1 = new TF1("fit1", "tan([1])*x+[0]/cos([1])",pitch, 71*pitch);
	fit1->SetParameter(0, mean_fit);
	fit1->SetParameter(1, theta_fit);		
	fit1->SetLineColor(1);
	fit1->SetLineWidth(3);
	fit1->SetParName(0, "Mean");
	fit1->SetParName(1, "#theta");
	ge.Draw("same p*");
	ge.Fit("fit1","QR");

	
	


	char *beamMean =Form("pd3png/beamMeanChip%dFrame%d.png",ich,iframe);
        c->SaveAs(beamMean);

*/



/*	TF1 *fit1 = new TF1("fit1", "pol1", pitch, 71*pitch);	
	ge.Fit("fit1", "Q");
*/

	TF1 *fit1 = new TF1("fit1", "tan([1])*x+[0]/cos([1])",pitch, 71*pitch);
	fit1->SetParameter(0, mean_fit);
	fit1->SetParameter(1, theta_fit);
	ge.Fit("fit1","QR");
//	b = fit1->GetParameter(0);
//	cout << "b: " <<b <<endl;
//	theta = atan(fit1->GetParameter(1));
//	cout << "theta: "<<theta<<endl;
//	cosTheta = cos(theta);
 //      	mean = b*cosTheta;
//	cout <<"mean: "<<mean<<endl;
	mean = fit1->GetParameter(0);
	theta = fit1->GetParameter(1);
//	cout <<"mean: "<<mean<<endl;
//	cout << "theta: "<<theta<<endl;


	if(jframe!=int(period/2)){
	mean_correction = mean_correction+mean;
	theta_correction = theta_correction+theta;}
	else{
	mean_medium = mean;
	theta_medium = theta;
}
	

	}
	
	mean_correction = mean_medium-mean_correction/(period-1);
	theta_correction = theta_medium-theta_correction/(period-1);
	hMean->Fill(mean_correction);
	hTheta->Fill(theta_correction);
	hMeanVSFrame->SetBinContent(iPeriod+1,mean_correction);
	hThetaVSFrame->SetBinContent(iPeriod+1, mean_correction);
	centerMean = centerMean+mean_correction;
	centerRMS = centerRMS+ mean_correction*mean_correction;
	thetaMean = thetaMean+theta_correction;
	thetaRMS = thetaRMS+theta_correction*theta_correction; 

 
	}


	centerRMS = sqrt(abs(centerRMS-centerMean*centerMean/nPeriod)/(nPeriod-1));
	centerMean = centerMean/nPeriod;
	thetaRMS = sqrt(abs(thetaRMS-thetaMean*thetaMean/nPeriod)/(nPeriod-1));
	thetaMean = thetaMean/nPeriod;



	cout<<"centerMean: "<<centerMean<<endl;
	cout<<"centerRMS: "<<centerRMS<<endl;
	cout<<"thetaMean: "<<thetaMean<<endl;
	cout<<"thetaRMS: "<<thetaRMS<<endl;

	TCanvas *c1 = new TCanvas("c1", "c1", 800, 400);
	c1->Divide(2,1);
	c1->cd(1);
	hMean->Draw();
	hMean->Fit("gaus","Q");
//	hMean->SaveAs("hMean.C");
	c1->cd(2);
	hTheta->Draw();
        hTheta->Fit("gaus","Q");
//	hTheta->SaveAs("hTheta.C");
	char *beamPara = Form("pd3png/beamParaCorrectionChip%d.png", ich);
	c1->SaveAs(beamPara);

	TCanvas *c2 = new TCanvas("c2", "c2",800,800);
	c2->Divide(2,2);
	c2->cd(1);
	hMeanVSFrame->Draw("p*");
	c2->cd(2);
	   TH1 *hm =0;
	   TVirtualFFT::SetTransform(0);
	   hm = hMeanVSFrame->FFT(hm, "MAG");
	   hm->SetTitle("Magnitude of the 1st transform");

	   hm->Draw();
	   gPad->SetLogy(1);
	c2->cd(3);
	hThetaVSFrame->Draw("p*");
	c2->cd(4);
	   TH1 *hn =0;
	   TVirtualFFT::SetTransform(0);
	   hn = hMeanVSFrame->FFT(hn, "MAG");
	   hn->SetTitle("Magnitude of the 1st transform");

	   hn->Draw();
	   gPad->SetLogy(1);		
	char *beamVStime = Form("pd3png/beamVStimeCorrectionChip%d.png", ich);
	c2->SaveAs(beamVStime);
//	delete c;    
	delete c1;
	delete c2;

} 


 void beamPara_correction(int ich)
   {

	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        

        TRandom r;

	TCanvas *c=new TCanvas("c","c",700,600);

        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;
        TH1D *hMean = new TH1D("hMean","Mean", 100, 2500, 2900);
	TH1D * hTheta = new TH1D("hTheta","#theta", 100, -0.05, 0.05);
	TH1D *hIntensity = new TH1D("hIntensity","#DeltaI/I", 100, -0.1, 0.1);
	double refMean=2712;
	int binMove;
	double centerMean=0;
	double centerRMS=0;
	double thetaMean=0;
	double thetaRMS=0;
	double deltaEMean = 0;
	double deltaERMS=0;
	double dt=3.3;
	TH1D *hMeanVSFrame = new TH1D("hMeanVSFrame", "Mean vs Time", nFrames, 0, nFrames*dt);
	TH1D *hThetaVSFrame = new TH1D("hThetaVSFrame","#theta vs Time",nFrames, 0, nFrames*dt);
	//for(int iframe=0; iframe<nFrames; iframe++){
	for(int iframe=0; iframe<1; iframe++){
	TString hist2dName;
        hist2dName = "hAdc2dMap_iframe"; hist2dName += iframe;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }

	double rowSum;
	double pixelValue;
	double pixelMean;
	double pixelSigma2;
        double upLimit = 100;
	double lowLimit = 0;
	double pitch = 83.2;
	double b;
	double theta;
	double cosTheta;
	double mean;
	double mean_fit = 2712;
	double theta_fit = 0.007;



	TGraphErrors ge;	
        for(int irow = 0; irow < 72; irow++){
        //for(int irow = 0; irow < 15; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(pixelValue/rowSum)*pitch*pitch/12; 	

		}

		}
		//cout<<"pixelSigma2:"<<pixelSigma2<<endl;
		ge.SetPoint(irow+1, irow*pitch, pixelMean );
		//ge.SetPointError(irow+1, pitch/sqrt(12), sqrt(pixelSigma2));
		ge.SetPointError(irow+1, 0, sqrt(pixelSigma2));

	}

// first fit

	TF1 *fit1 = new TF1("fit1", "tan([1])*x+[0]/cos([1])",2*pitch, 70*pitch);


	//fit1->SetParameter(0, mean_fit);
	//fit1->SetParameter(1, theta_fit);		
	fit1->SetLineColor(1);
	fit1->SetLineWidth(3);
	fit1->SetParName(0, "Mean");
	fit1->SetParName(1, "#theta");
	ge.Fit("fit1","QR");
	mean = fit1->GetParameter(0);
	theta = fit1->GetParameter(1);
	binMove = TMath::Nint((refMean-mean)/pitch);
//	cout<<"refMean: "<<refMean<<endl;
//	cout<<"mean: "<<mean<<endl;
//	cout<<"binMove: "<<binMove<<endl;

// move bin

        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe]; 
		if(binX+1+binMove>=1 & binX+1+binMove<=72)           
            hAdc2dMap->SetBinContent(binX+1+binMove,binY+1,adc_value);            
        }

        for(int irow = 0; irow < 72; irow++){
        //for(int irow = 0; irow < 15; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(pixelValue/rowSum)*pitch*pitch/12; 		
		}

		}
		
		ge.SetPoint(irow+1, irow*pitch, pixelMean );
		//ge.SetPointError(irow+1, pitch/sqrt(12), sqrt(pixelSigma2));
		ge.SetPointError(irow+1, 0, sqrt(pixelSigma2)*32);

	}

// second fit
	fit1->SetParameter(0, refMean);

	if(iframe<20){
	TString hist2dNameRotat;
        hist2dNameRotat = "hAdc2dMap_iframe"; hist2dNameRotat += iframe;
        //TH2D *hAdc2dMapRotat= new TH2D(hist2dNameRotat, "pd3 ADC 2d Map", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);
        TH2D *hAdc2dMapRotat= new TH2D(hist2dNameRotat, "", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);
 /*       for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe]; 
	    hAdc2dMapRotat->SetBinContent(binY+1,binX+1,adc_value) ;
	}
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe]; 
		if(binX+1+binMove>=1 & binX+1+binMove<=72){           
            hAdc2dMapRotat->SetBinContent(binY+1,binX+1+binMove,adc_value);
		} 
	

        }  */

       for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe]; 
		if(binX+1+binMove>=1 & binX+1+binMove<=72){           
            hAdc2dMapRotat->SetBinContent(binY+1,binX+1+binMove,adc_value);
		} 
		if(binX<binMove){
            hAdc2dMapRotat->SetBinContent(binY+1,binX+1,adc_value);		
		}
		if(binX>71+binMove){
            hAdc2dMapRotat->SetBinContent(binY+1,binX+1,adc_value);	
	}
	
        }        



	hAdc2dMapRotat->GetXaxis()->SetTitle("#mum");
	hAdc2dMapRotat->GetXaxis()->CenterTitle();
	hAdc2dMapRotat->GetYaxis()->SetTitleOffset(1.4);
	hAdc2dMapRotat->GetYaxis()->SetTitle("#mum");	
	hAdc2dMapRotat->GetYaxis()->CenterTitle();	
	hAdc2dMapRotat->SetMaximum(100);
	hAdc2dMapRotat->SetMinimum(-20);
	

      	 c->cd();
	hAdc2dMapRotat->Draw("COLZ");

	
		
//	TF1 *fit1 = new TF1("fit1", "pol1", pitch, 71*pitch);
	ge.Fit("fit1","QR");
	ge.Draw("same p*");


	
	


	char *beamMean =Form("pd3png/beamMeanCorrectionChip%dFrame%d.png",ich,iframe);
        c->SaveAs(beamMean);

}else{
	ge.Fit("fit1","QR");

}







/*	TF1 *fit1 = new TF1("fit1", "pol1", pitch, 71*pitch);	
	ge.Fit("fit1", "Q");
*/


/*	TF1 *fit1 = new TF1("fit1", "tan([1])*x+[0]/cos([1])",pitch, 71*pitch);
	fit1->SetParameter(0, mean_fit);
	fit1->SetParameter(1, theta_fit);
	ge.Fit("fit1","Q");

*/


//	b = fit1->GetParameter(0);
//	cout << "b: " <<b <<endl;
//	theta = atan(fit1->GetParameter(1));
//	cout << "theta: "<<theta<<endl;
//	cosTheta = cos(theta);
 //      	mean = b*cosTheta;
//	cout <<"mean: "<<mean<<endl;
	mean = fit1->GetParameter(0);
	theta = fit1->GetParameter(1);
//	cout <<"mean: "<<mean<<endl;
//	cout << "theta: "<<theta<<endl;
	hMean->Fill(mean);
	hTheta->Fill(theta);
	hMeanVSFrame->SetBinContent(iframe+1,mean);
	hThetaVSFrame->SetBinContent(iframe+1, theta);
	centerMean = centerMean+mean;
	centerRMS = centerRMS+ mean*mean;
	thetaMean = thetaMean+theta;
	thetaRMS = thetaRMS+theta*theta; 


//  intensity

	double Eup=0;
	double Edown=0;
	double deltaE;
	int irow;

	for(int i= 0; i<36; i++){
	irow = int(r.Rndm()*71);
//	cout<< "i: "<<i<<endl;
//        cout<< "irow: "<<irow<<endl;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		Eup = Eup + pixelValue;		
		}			
		}
	
	}	

	for(int i= 0; i<36; i++){
	irow = int(r.Rndm()*71);
//	cout<< "i: "<<i<<endl;
//        cout<< "irow: "<<irow<<endl;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		Edown = Edown+ pixelValue;		
		}			
		}
	
	}
     // 	cout<< "Eup-Edown: "<<Eup-Edown<<endl;
//	cout<<"Eup+Edown: "<<Eup+Edown<<endl;
      deltaE= (Eup-Edown)/(Eup+Edown);
      hIntensity->Fill(deltaE);
	deltaEMean = deltaEMean+deltaE;
	deltaERMS = deltaERMS + deltaE*deltaE;	
  
	 
        }



	centerRMS = sqrt(abs(centerRMS-centerMean*centerMean/nFrames)/(nFrames-1));
	centerMean = centerMean/nFrames;
	thetaRMS = sqrt(abs(thetaRMS-thetaMean*thetaMean/nFrames)/(nFrames-1));
	thetaMean = thetaMean/nFrames;

	deltaERMS = sqrt(abs(deltaERMS-deltaEMean*deltaEMean/nFrames)/(nFrames-1));
	deltaEMean = deltaEMean/nFrames;

	cout<<"centerMean: "<<centerMean<<endl;
	cout<<"centerRMS: "<<centerRMS<<endl;
	cout<<"thetaMean: "<<thetaMean<<endl;
	cout<<"thetaRMS: "<<thetaRMS<<endl;
	cout<<"deltaEMean: "<<deltaEMean<<endl;
	cout<<"deltaERMS: "<<deltaERMS<<endl;

	TCanvas *cMean = new TCanvas("cMean", "cMean", 700 , 600);
	cMean->cd();
	hMean->GetXaxis()->SetTitle("Mean (#mum)");
	hMean->GetXaxis()->CenterTitle();

	hMean->Draw();
	hMean->Fit("gaus","Q");
//	hMean->SaveAs("hMean.C");

	char *beamMeanPng = Form("pd3png/beamMeanCorrectionChip%d.png", ich);
	cMean->SaveAs(beamMeanPng);
	char *beamMeanPdf = Form("pd3png/beamMeanCorrectionChip%d.pdf", ich);
	cMean->SaveAs(beamMeanPdf);

	TCanvas *cTheta = new TCanvas("cTheta", "cTheta", 700 , 600);
	cTheta->cd();	
	hTheta->GetXaxis()->SetTitle("#theta (rad)");
	hTheta->GetXaxis()->CenterTitle();
	hTheta->Draw();
        hTheta->Fit("gaus","Q");
//	hTheta->SaveAs("hTheta.C");
	char *beamThetaPng = Form("pd3png/beamThetaCorrectionChip%d.png", ich);
	cTheta->SaveAs(beamThetaPng);
	char *beamThetaPdf = Form("pd3png/beamThetaCorrectionChip%d.pdf", ich);
	cTheta->SaveAs(beamThetaPdf);

	TCanvas *cIntensity = new TCanvas("cIntensity", "cIntensity", 700 , 600);
	cIntensity->cd();
        hIntensity->Draw();
	hIntensity->GetXaxis()->SetTitle("#DeltaI/I");
	hIntensity->GetXaxis()->CenterTitle();
	hIntensity->Fit("gaus","Q");

	char *beamIntensityPng = Form("pd3png/beamIntensityCorrectionChip%d.png", ich);
	cIntensity->SaveAs(beamIntensityPng);

	char *beamIntensityPdf = Form("pd3png/beamIntensityCorrectionChip%d.pdf", ich);
	cIntensity->SaveAs(beamIntensityPdf);
/*
	TCanvas *c2 = new TCanvas("c2", "c2",800,800);
	c2->Divide(2,2);
	c2->cd(1);
	hMeanVSFrame->Draw("p*");
	c2->cd(2);
	   TH1 *hm =0;
	   TVirtualFFT::SetTransform(0);
	   hm = hMeanVSFrame->FFT(hm, "MAG");
	   hm->SetTitle("Magnitude of the 1st transform");

	   hm->Draw();
	   gPad->SetLogy(1);
	c2->cd(3);
	hThetaVSFrame->Draw("p*");
	c2->cd(4);
	   TH1 *hn =0;
	   TVirtualFFT::SetTransform(0);
	   hn = hMeanVSFrame->FFT(hn, "MAG");
	   hn->SetTitle("Magnitude of the 1st transform");

	   hn->Draw();
	   gPad->SetLogy(1);	*/


	TCanvas *c2 = new TCanvas("c2", "c2",1600,800);
	c2->Divide(1,2);
	c2->cd(1);
	hMeanVSFrame->GetXaxis()->SetTitle("Time (ms)");
	hMeanVSFrame->GetXaxis()->CenterTitle();
	hMeanVSFrame->GetYaxis()->SetTitle("Mean (#mum)");
	hMeanVSFrame->GetYaxis()->CenterTitle();	
	hMeanVSFrame->Draw("p*");
	c2->cd(2);
	hThetaVSFrame->GetXaxis()->SetTitle("Time (ms)");
	hThetaVSFrame->GetXaxis()->CenterTitle();
	hThetaVSFrame->GetYaxis()->SetTitle("#theta (rad)");
	hThetaVSFrame->GetYaxis()->CenterTitle();	
	hThetaVSFrame->Draw("p*");

	char *beamVStime = Form("pd3png/beamVStimeCorrectionChip%d.png", ich);
	c2->SaveAs(beamVStime);
	delete c;
	delete cMean;	
	delete cTheta;
	delete cIntensity;
	delete c2;

}





void beamFrame(int ich)
   {

	gStyle->SetOptStat(0);
//	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        
cout<<"nFrames: "<<nFrames<<endl;

	TCanvas *cFrame = new TCanvas("cFrame","cFrame", 700,600);
	cFrame->cd();
        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;


	double refMean=2712;
	int binMove;
	double centerMean=0;
	double centerRMS=0;
	double thetaMean=0;
	double thetaRMS=0;

	TString hist2dName;
        hist2dName = "hAdc2dMap";
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
	double pitch = 83; 
	TString hist2dNameRotat;
        hist2dNameRotat = "hAdc2dMaRotat";
        //TH2D *hAdc2dMapRotat= new TH2D(hist2dNameRotat, "pd3 ADC 2d Map", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);
        TH2D *hAdc2dMapRotat= new TH2D(hist2dNameRotat, "", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);
//	nFrames = 1;
//	for(int iframe=0; iframe<nFrames; iframe++){

	for(int iframe=5; iframe<6; iframe++){
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }

	double rowSum;
	double pixelValue;
	double pixelMean;
	double pixelSigma2;
        double upLimit = 100;
	double lowLimit = 0;

	double b;
	double theta;
	double cosTheta;
	double mean;
	double mean_fit = 2712;
	double theta_fit = 0.007;



	TGraphErrors ge;	
        for(int irow = 0; irow < 72; irow++){
        //for(int irow = 0; irow < 15; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(pixelValue/rowSum)*pitch*pitch/12; 		
		}

		}
		
		ge.SetPoint(irow+1, irow*pitch, pixelMean );
		//ge.SetPointError(irow+1, pitch/sqrt(12), sqrt(pixelSigma2));
		ge.SetPointError(irow+1, 0, sqrt(pixelSigma2)*32);

	}

// first fit

	TF1 *fit1 = new TF1("fit1", "tan([1])*x+[0]/cos([1])",2*pitch, 70*pitch);


	//fit1->SetParameter(0, mean_fit);
	//fit1->SetParameter(1, theta_fit);		
	fit1->SetLineColor(1);
	fit1->SetLineWidth(3);
	fit1->SetParName(0, "Mean");
	fit1->SetParName(1, "#theta");
	ge.Fit("fit1","QR");
	mean = fit1->GetParameter(0);
	theta = fit1->GetParameter(1);
	binMove = TMath::Nint((refMean-mean)/pitch);
//	cout<<"refMean: "<<refMean<<endl;
//	cout<<"mean: "<<mean<<endl;
//	cout<<"binMove: "<<binMove<<endl;

// move bin


    

       for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe]; 
		if(binX+1+binMove>=1 & binX+1+binMove<=72){           
            hAdc2dMapRotat->SetBinContent(binY+1,binX+1+binMove,adc_value);
		} 
		if(binX<binMove){
            hAdc2dMapRotat->SetBinContent(binY+1,binX+1,adc_value);		
		}
		if(binX>71+binMove){
            hAdc2dMapRotat->SetBinContent(binY+1,binX+1,adc_value);	
	}
	
        } 


	hAdc2dMapRotat->GetXaxis()->SetTitle("#mum");
	hAdc2dMapRotat->GetXaxis()->CenterTitle();
	hAdc2dMapRotat->GetYaxis()->SetTitleOffset(1.4);
	hAdc2dMapRotat->GetYaxis()->SetTitle("#mum");	
	hAdc2dMapRotat->GetYaxis()->CenterTitle();	
	hAdc2dMapRotat->SetMaximum(100);
	hAdc2dMapRotat->SetMinimum(-20);	

	if(iframe==0){
	hAdc2dMapRotat->Draw("COLZ");	
	
}else{
	hAdc2dMapRotat->Draw("COLZ same");
}
}	


	char *beamMeanPdf =Form("pd3png/beamFrameChip%d.png",ich);
	char *beamMeanPng =Form("pd3png/beamFrameChip%d.pdf",ich);
	char *beamMeanRoot =Form("pd3png/beamFrameChip%d.root",ich);
       cFrame->SaveAs(beamMeanPdf);
       cFrame->SaveAs(beamMeanPng);
        cFrame->SaveAs(beamMeanRoot);
	delete cFrame;


}


void beamFrameAveFit(int ich)
   {

	gStyle->SetOptStat(0);
//	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        
cout<<"nFrames: "<<nFrames<<endl;

	TCanvas *cFrame = new TCanvas("cFrame","cFrame", 700,600);
	cFrame->cd();
        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;


	double refMean=2712;
	int binMove;
	double centerMean=0;
	double centerRMS=0;
	double thetaMean=0;
	double thetaRMS=0;
        double upLimit = 100;
	double lowLimit = 0;

	double rowSum;
	double pixelValue;
	double pixelMean;
	double pixelSigma2;

	double theta;
	double mean;
	double mean_fit = 2712;
	double theta_fit = 0.007;

	TString hist2dName;
        hist2dName = "hAdc2dMap";
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
	double pitch = 83; 
	TString hist2dNameRotat;
        hist2dNameRotat = "hAdc2dMaRotat";
        //TH2D *hAdc2dMapRotat= new TH2D(hist2dNameRotat, "pd3 ADC 2d Map", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);
        TH2D *hAdc2dMapRotat= new TH2D(hist2dNameRotat, "", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);
	//nFrames = 1;
	for(int iframe=0; iframe<nFrames; iframe++){
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];        
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }





	TGraphErrors ge;	
        for(int irow = 0; irow < 72; irow++){
        //for(int irow = 0; irow < 15; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(pixelValue/rowSum)*pitch*pitch/12; 		
		}

		}
		
		ge.SetPoint(irow+1, irow*pitch, pixelMean );
		//ge.SetPointError(irow+1, pitch/sqrt(12), sqrt(pixelSigma2));
		ge.SetPointError(irow+1, 0, sqrt(pixelSigma2)*32);

	}

// first fit

	TF1 *fit1 = new TF1("fit1", "tan([1])*x+[0]/cos([1])",2*pitch, 70*pitch);


	//fit1->SetParameter(0, mean_fit);
	//fit1->SetParameter(1, theta_fit);		
	fit1->SetLineColor(1);
	fit1->SetLineWidth(3);
	fit1->SetParName(0, "Mean");
	fit1->SetParName(1, "#theta");
	ge.Fit("fit1","QR");
	mean = fit1->GetParameter(0);
	theta = fit1->GetParameter(1);
	binMove = TMath::Nint((refMean-mean)/pitch);
//	cout<<"refMean: "<<refMean<<endl;
//	cout<<"mean: "<<mean<<endl;
//	cout<<"binMove: "<<binMove<<endl;

// move bin


    

       for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe]; 
	    if(adc_value>upLimit || adc_value<lowLimit)
	    adc_value = 0;
		if(binX+1+binMove>=1 & binX+1+binMove<=72){
		if(adc_value!=0){	           
            hAdc2dMapRotat->SetBinContent(binY+1,binX+1+binMove,(adc_value+hAdc2dMapRotat->GetBinContent(binY+1,binX+1+binMove))/2);
		} 
		}

		if(binX<binMove){
		if(adc_value!=0){	           
            hAdc2dMapRotat->SetBinContent(binY+1,binX+1,(adc_value+hAdc2dMapRotat->GetBinContent(binY+1,binX+1))/2);
		} 
		}
		if(binX>71+binMove){
		if(adc_value!=0){	           
            hAdc2dMapRotat->SetBinContent(binY+1,binX+1,(adc_value+hAdc2dMapRotat->GetBinContent(binY+1,binX+1))/2);
		} 
	}
	
        } 



}	

	hAdc2dMapRotat->GetXaxis()->SetTitle("#mum");
	hAdc2dMapRotat->GetXaxis()->CenterTitle();
	hAdc2dMapRotat->GetYaxis()->SetTitleOffset(1.4);
	hAdc2dMapRotat->GetYaxis()->SetTitle("#mum");	
	hAdc2dMapRotat->GetYaxis()->CenterTitle();	
//	hAdc2dMapRotat->SetMaximum(100);
//	hAdc2dMapRotat->SetMinimum(-20);	


	hAdc2dMapRotat->Draw("COLZ");	
	char *beamMeanPdf =Form("pd3png/beamFrameChip%d.png",ich);
	char *beamMeanPng =Form("pd3png/beamFrameChip%d.pdf",ich);
	char *beamMeanRoot =Form("pd3png/beamFrameChip%d.root",ich);
       cFrame->SaveAs(beamMeanPdf);
       cFrame->SaveAs(beamMeanPng);
        cFrame->SaveAs(beamMeanRoot);
	
	TCanvas *cFrameFit = new TCanvas("cFrameFit", "cFrameFit", 700, 600);
	cFrameFit->cd();
	hAdc2dMapRotat->Draw("COLZ");
	

	TGraphErrors ge1;	
        for(int irow = 0; irow < 72; irow++){
        //for(int irow = 0; irow < 15; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMapRotat->GetBinContent(irow+1, icol+1);
		rowSum = rowSum + pixelValue;		
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMapRotat->GetBinContent(irow+1, icol+1);

		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(pixelValue/rowSum)*pitch*pitch/12; 		


		}
		
		ge1.SetPoint(irow+1, irow*pitch, pixelMean );
		//ge.SetPointError(irow+1, pitch/sqrt(12), sqrt(pixelSigma2));
		ge1.SetPointError(irow+1, 0, sqrt(pixelSigma2)*32);

	}



	TF1 *fit2 = new TF1("fit2", "tan([1])*x+[0]/cos([1])",2*pitch, 70*pitch);


	//fit1->SetParameter(0, mean_fit);
	//fit1->SetParameter(1, theta_fit);		
	fit2->SetLineColor(1);
	fit2->SetLineWidth(3);
	fit2->SetParName(0, "Mean");
	fit2->SetParName(1, "#theta");
	ge1.Draw("same p*");
	ge1.Fit("fit2","QR");
	char *beamMeanFitPdf =Form("pd3png/beamFrameFitChip%d.png",ich);
	char *beamMeanFitPng =Form("pd3png/beamFrameFitChip%d.pdf",ich);
	char *beamMeanFitRoot =Form("pd3png/beamFrameFitChip%d.root",ich);
       cFrameFit->SaveAs(beamMeanFitPdf);
       cFrameFit->SaveAs(beamMeanFitPng);
        cFrameFit->SaveAs(beamMeanFitRoot);

	delete cFrameFit;

	delete cFrame;


}





/*void beamFrameAveFitRota(int ich)
   {

	gStyle->SetOptStat(0);
//	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        
cout<<"nFrames: "<<nFrames<<endl;

	TCanvas *cFrame = new TCanvas("cFrame","cFrame", 700,600);
	cFrame->cd();
        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;


	double refMean=2712;
	int binMove;
	double centerMean=0;
	double centerRMS=0;
	double thetaMean=0;
	double thetaRMS=0;
        double upLimit = 100;
	double lowLimit = 0;

	double rowSum;
	double pixelValue;
	double pixelMean;
	double pixelSigma2;

	double theta;
	double mean;
	double mean_fit = 2712;
	double theta_fit = 0.007;

	TString hist2dName;
        hist2dName = "hAdc2dMap";
         
        TH2D *hAdc2dMap= new new TH2D(hist2dNameRotat, "", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);
	double pitch = 83; 

	nFrames = 1;
	for(int iframe=0; iframe<nFrames; iframe++){
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];        
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }





	TGraphErrors ge;	
        for(int irow = 0; irow < 72; irow++){
        //for(int irow = 0; irow < 15; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(pixelValue/rowSum)*pitch*pitch/12; 		
		}

		}
		
		ge.SetPoint(irow+1, pixelMean , irow*pitch);
		//ge.SetPointError(irow+1, pitch/sqrt(12), sqrt(pixelSigma2));
		ge.SetPointError(irow+1, 0,0);

	}

       hAdc2dMap->Draw("COLZ");

	TF1 *fit1 = new TF1("fit1", "pol1");


	//fit1->SetParameter(0, mean_fit);
	//fit1->SetParameter(1, theta_fit);		
	fit1->SetLineColor(1);
	fit1->SetLineWidth(3);
	fit1->SetParName(0, "Mean");
	fit1->SetParName(1, "#theta");
	ge.Draw("same p*");
	ge.Fit("fit1","Q");






	
      char *beamMeanPdf =Form("pd3png/beamFrameChip%d.png",ich);
	char *beamMeanPng =Form("pd3png/beamFrameChip%d.pdf",ich);
	char *beamMeanRoot =Form("pd3png/beamFrameChip%d.root",ich);
       cFrame->SaveAs(beamMeanPdf);
       cFrame->SaveAs(beamMeanPng);
        cFrame->SaveAs(beamMeanRoot);

	


	delete cFrame;

}


}*/



void beamParaAcu(int ich)
   {

	gStyle->SetOptStat(0);
//	gStyle->SetOptFit(1);

	int nTopMetalChips=pd3.adcCha();
	int nPixelsOnChip=pd3.nPix();        
        int nFrames=pd3.nFrame();
        



	TCanvas *cOdd=new TCanvas("cOdd","cOdd",700,600);
	TCanvas *cEven=new TCanvas("cEven","cEven",700,600);

        pd3.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;
      //  TH1D *hMean = new TH1D("hMean","#DeltaMean" , 100, -150,150 );
	//TH1D * hTheta = new TH1D("hTheta","#Delta#theta", 100, -0.05, 0.05);
        TH1D *hMean = new TH1D("hMean","#DeltaMean/#sqrt{2}" , 100, -150,150 );
	TH1D * hTheta = new TH1D("hTheta","#Delta#theta/#sqrt{2}", 100, -0.05, 0.05);
	double centerMean=0;
	double centerRMS=0;
	double thetaMean=0;
	double thetaRMS=0;
	double deltaEMean = 0;
	double deltaERMS=0;
	double dt=3.3;

	for(int iframe=0; iframe<nFrames; iframe++){
//	for(int iframe=0; iframe<1; iframe++){

	TString hist2dName;
        hist2dName = "hAdc2dMap_iframe"; hist2dName += iframe;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd3 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }

	double rowSum;
	double pixelValue;
	double pixelMean;
	double pixelSigma2;
        double upLimit = 100;
	double lowLimit = 0;
	double pitch = 83;

	double dTheta;
	double theta1;
	double theta2;
	double dMean;
	double mean1;
	double mean2;
	
//	double mean_fit = 2712;
//	double theta_fit = 0.007;



	TGraphErrors ge1;
	int iPoint1=0;	
        for(int irow = 0; irow < 72; irow++){

		iPoint1++;
//		cout<<"irow: "<<irow<<endl;
//		cout<<"iPoint: "<<iPoint1<<endl;
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(pixelValue/rowSum)*pitch*pitch/12; 		
		}

		}
		
		ge1.SetPoint(iPoint1, irow*pitch, pixelMean );
		//ge.SetPointError(irow+1, pitch/sqrt(12), sqrt(pixelSigma2));
		ge1.SetPointError(iPoint1, 0, sqrt(pixelSigma2)*32);
		irow++;

	}


	TGraphErrors ge2;
	int iPoint2=0;	
        for(int irow = 1; irow < 72; irow++){
        //for(int irow = 0; irow < 15; irow++){
		iPoint2++;
//		cout<<"irow: "<<irow<<endl;
//		cout<<"iPoint: "<<iPoint2<<endl;
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(pixelValue/rowSum)*pitch*pitch/12; 		
		}

		}
		
		ge2.SetPoint(iPoint2, irow*pitch, pixelMean );

		ge2.SetPointError(iPoint2, 0, sqrt(pixelSigma2)*32);
		irow++;

	}


	TF1 *fit1 = new TF1("fit1", "tan([1])*x+[0]/cos([1])",2*pitch, 70*pitch);

	TF1 *fit2 = new TF1("fit2", "tan([1])*x+[0]/cos([1])",2*pitch, 70*pitch);	
   if(iframe<20){


	TString hist2dNameRotat;
        hist2dNameRotat = "hAdc2dMap_iframe"; hist2dNameRotat += iframe;

        TH2D *hAdc2dMapRotat= new TH2D(hist2dNameRotat, "", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);
        for(int ipixel=0;ipixel<pd3.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value=pd3.chaDat[ipixel*pd3.nFrame()+iframe];            
            hAdc2dMapRotat->SetBinContent(binY+1,binX+1,adc_value);            
        }

	hAdc2dMapRotat->GetXaxis()->SetTitle("#mum");
	hAdc2dMapRotat->GetXaxis()->CenterTitle();
	hAdc2dMapRotat->GetYaxis()->SetTitleOffset(1.4);
	hAdc2dMapRotat->GetYaxis()->SetTitle("#mum");	
	hAdc2dMapRotat->GetYaxis()->CenterTitle();
	hAdc2dMapRotat->SetMaximum(100);
	hAdc2dMapRotat->SetMinimum(-20);			


	
	cEven->cd();
	hAdc2dMapRotat->Draw("COLZ");


		
	fit1->SetLineColor(1);
	fit1->SetLineWidth(3);
	fit1->SetParName(0, "Mean");
	fit1->SetParName(1, "#theta");
	ge1.Draw("same p*");
	ge1.Fit("fit1","QR");
	char *beamMeanEven =Form("pd3png/beamMeanEvenChip%dFrame%d.png",ich,iframe);
	cEven->SaveAs(beamMeanEven);


      cOdd->cd();
	hAdc2dMapRotat->Draw("COLZ");


		
	fit2->SetLineColor(4);
	fit2->SetLineWidth(3);
	fit2->SetParName(0, "Mean");
	fit2->SetParName(1, "#theta");
	ge2.Draw("same p*");
	ge2.Fit("fit2","QR");	
	
         char *beamMeanOdd =Form("pd3png/beamMeanOddChip%dFrame%d.png",ich,iframe);

        cOdd->SaveAs(beamMeanOdd);


}
else{


	ge1.Fit("fit1","QR");

	ge2.Fit("fit2","QR");
}







/*	TF1 *fit1 = new TF1("fit1", "pol1", pitch, 71*pitch);	
	ge.Fit("fit1", "Q");
*/






//	b = fit1->GetParameter(0);
//	cout << "b: " <<b <<endl;
//	theta = atan(fit1->GetParameter(1));
//	cout << "theta: "<<theta<<endl;
//	cosTheta = cos(theta);
 //      	mean = b*cosTheta;
//	cout <<"mean: "<<mean<<endl;
	mean1 = fit1->GetParameter(0);
//	cout <<"mean1: "<<mean1<<endl;
	theta1 = fit1->GetParameter(1);
	mean2 = fit2->GetParameter(0);
//	cout <<"mean2: "<<mean2<<endl;
	theta2 = fit2->GetParameter(1);
//	cout <<"mean: "<<mean<<endl;
//	cout << "theta: "<<theta<<endl;
	dMean = (mean1-mean2)/sqrt(2);
//	dMean = mean1-mean2;
//	dTheta = theta1-theta2;
	dTheta = (theta1-theta2)/sqrt(2);
	hMean->Fill(dMean);
	hTheta->Fill(dTheta);
	centerMean = centerMean+dMean;
	centerRMS = centerRMS+ dMean*dMean;
	thetaMean = thetaMean+dTheta;
	thetaRMS = thetaRMS+dTheta*dTheta; 
}



	centerRMS = sqrt(abs(centerRMS-centerMean*centerMean/nFrames)/(nFrames-1));
	centerMean = centerMean/nFrames;
	thetaRMS = sqrt(abs(thetaRMS-thetaMean*thetaMean/nFrames)/(nFrames-1));
	thetaMean = thetaMean/nFrames;



	cout<<"centerMean: "<<centerMean<<endl;
	cout<<"centerRMS: "<<centerRMS<<endl;
	cout<<"thetaMean: "<<thetaMean<<endl;
	cout<<"thetaRMS: "<<thetaRMS<<endl;


	TCanvas *c1 = new TCanvas("c1", "c1", 700, 600);
	c1->cd();
//	hMean->GetXaxis()->SetTitle("#DeltaMean (#mum)");
	hMean->GetXaxis()->SetTitle("#DeltaMean/#sqrt{2} (#mum)");
	hMean->GetXaxis()->CenterTitle();
	hMean->Draw();
	hMean->Fit("gaus","Q");
//	hMean->SaveAs("hMean.C");
	char *beamDeltaMeanPng = Form("pd3png/beamDeltaMeanChip%d.png", ich);
	c1->SaveAs(beamDeltaMeanPng);	

	char *beamDeltaMeanPdf = Form("pd3png/beamDeltaMeanChip%d.pdf", ich);
	c1->SaveAs(beamDeltaMeanPdf);

	TCanvas *c2 = new TCanvas("c2", "c2", 700, 600);
	c2->cd();
	hTheta->GetXaxis()->SetTitle("#Delta#theta/#sqrt{2} (rad)");
//	hTheta->GetXaxis()->SetTitle("#Delta#theta (rad)");
	hTheta->GetXaxis()->CenterTitle();
	hTheta->Draw();
        hTheta->Fit("gaus","Q");
//	hTheta->SaveAs("hTheta.C");
	char *beamDeltaThetaPng = Form("pd3png/beamDeltaThetaChip%d.png", ich);
	c2->SaveAs(beamDeltaThetaPng);
	char *beamDeltaThetaPdf = Form("pd3png/beamDeltaThetaChip%d.pdf", ich);
	c2->SaveAs(beamDeltaThetaPdf);




  	delete cOdd;
	delete cEven;
	delete c1;
	delete c2;

}


};

class pd1Debug {
	public:

	pd1Debug(){
		pn.pd1=&pd1;
		debug=0;

	}
	int debug;
	placData_1 pd1;
	polNPar pn;
	TH2D **hAdcMap;
	TH1D **hist1D;
	// void setupHisto(){
		// hAdcMap=new TH2D *[pd1.adcCha()];
		// hist1D=new TH1D *[pd1.adcCha()];
		// TString histName;
		// TString histName1D;
		// for(int i=0;i<pd1.adcCha();i++){
			// histName = "hAdcMap_Chip";histName += i;
			// histName1D = "hAdcMap1D_Chip";histName1D += i;
			// hAdcMap[i] = new TH2D(histName, "ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
			// hist1D[i]=new TH1D(histName1D,"ADC Map;adc;counts",2100,-100,2000);		
		// }
	// }
	
	int read(char* fn){
		pd1.read(fn);
		return 1;
	}	
	void readHeaders(){
		cout<<"nPixel: "<<pd1.nPix()<<endl;
		cout<<"nFrame: "<<pd1.nFrame()<<endl;

	printf("%d/%d/%d %d:%d:%d\n", pd1.year(), pd1.month(), pd1.day(), pd1.hour(), pd1.min(), pd1.sec());
	}
	
void drawPixelDebug( int chip, int pixel){
	// pn.drawPixel(chip,pixel);
	
	TCanvas *c=new TCanvas("c","c",700,600);
	int returnValue=pd1.getPixDat(chip,pixel);
	// cout<<returnValue<<endl;
	if(returnValue!=0){
		cout<<"error !!!! chip "<<chip<<" pixel "<<pixel<<" does not exists"<<endl;
		return ;
	}
	float *x=new float[pd1.nFrame()];
	float *y=new float[pd1.nFrame()];
	for(int i=0;i<pd1.nFrame();i++){
		x[i]=i;
		y[i]=pd1.pixDat[pd1.samPerPix()*i];
		// cout<<y[i]<<endl;
	}
	
	TGraph *a=new TGraph(pd1.nFrame(),x,y);
	a->Draw();
	char *decayData =Form("pd1png/decayDataChip%dPixel%d.png",chip,pixel);
	c->SaveAs(decayData);
	delete []x;
	delete []y;
	delete a;
	delete c;	
	
	
}

void drawPixel( int chip, int pixel, char *pedeFile){

	pd1Pede pede;
	pede.setup(pedeFile);
	
//	TCanvas *c=new TCanvas("c","c",700,600);
	int returnValue=pd1.getPixDat(chip,pixel);
	// cout<<returnValue<<endl;
	if(returnValue!=0){
		cout<<"error !!!! chip "<<chip<<" pixel "<<pixel<<" does not exists"<<endl;
		return ;
	}


	float *x=new float[pd1.nFrame()];
	float *y=new float[pd1.nFrame()];
	for(int i=0;i<pd1.nFrame();i++){
		x[i]=i;
		y[i]=pd1.pixDat[pd1.samPerPix()*i];
		y[i]-=pede.meanPed[chip*pd1.nPix()+pixel];
		// cout<<y[i]<<endl;
	}
	
	TGraph *a=new TGraph(pd1.nFrame(),x,y);
	//a->Draw();
	char *decayData =Form("pd1png/decayDataChip%dPixel%d.root",chip,pixel);
	a->SaveAs(decayData);
	delete []x;
	delete []y;
	delete a;
	
	
	
}
/*void drawPixel( int chip, int row,int col){
	drawPixel(chip,row*72+col);	
}*/
//画出一个像素在所有帧上的值 

void drawPolyFit( int chip, int pixel) {
	int returnValue=pd1.getPixDat(chip,pixel);
	if(returnValue!=0){
		cout<<"error !!!! chip "<<chip<<" pixel "<<pixel<<" does not exists"<<endl;
		return ;
	}
	pn.getCurve();
	////////////////pn.singlePixCurve(int chip=0,int pixel=10);
	int maxi=pn.maxi;;
	int stop=pn.stop;
	int n=stop-maxi+1;
	float par[6];
	float *x=new float[n];
	float *y=new float[n];
	cout<<"stop-maxi+1 = "<<stop-maxi+1<<endl;
	for(int i=maxi;i<stop+1;i++){
		// y[i]=pd1.chaDat[pixel*pd1.nFrame()+i];
		y[i-maxi]=pd1.pixDat[i*pd1.samPerPix()];
		x[i-maxi]=(float)(i-maxi);
		cout<<i<<" "<<x[i-maxi]<<" "<<y[i-maxi]<<endl;
	}
	pn.polNFit(n,x,y,par,5);
	delete []x;
	delete []y;
}

// 画出一个像素上poly拟合的情况，pd3Debug没有这个函数 
 
void drawFrame(int frame) //画所有通道
{
	TCanvas *c=new TCanvas("c","c",3200,800);
	c->Divide(8, 2);
	TH2D *hAdcMap[pd1.adcCha()];
	TH1D *hist1D[pd1.adcCha()];
	TString histName;
	TString histName1D;
	for(int i=0;i<pd1.adcCha();i++){
		histName = "hAdcMap_Chip";histName += i;
		histName1D = "hAdcMap1D_Chip";histName1D += i;
		hAdcMap[i] = new TH2D(histName, "ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
		hist1D[i]=new TH1D(histName1D,"ADC Map;adc;counts",2100,-100,2000);		
	}
	for(int ich=0;ich<pd1.adcCha();ich++){
		pd1.getChaDat(ich);	
		float y;
		for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
			for(int iframe=frame;iframe<=frame;iframe++){
				// y[iframe]=pd3.chaDat[ich*pd3.nPix()*pd3.nFrame()+ipixel*pd3.nFrame()+iframe];
				y=0;
				for(int isample=0;isample<pd1.samPerPix();isample++){
					// adcValue += short(ps[iFrame*nPixels*nSamples*nChips+iPixel*nSamples*nChips+iSample*nChips+iChip]);
					y+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
				}
				y/=(float)pd1.samPerPix();
				hist1D[ich]->Fill(y);
				hAdcMap[ich]->SetBinContent(72-ipixel%72,ipixel/72+1,y);
			}	
		}
		// hAdcMap->SetMaximum(50);
		// hAdcMap->SetMinimum(-20);

		c->cd(ich*2+1);
		hAdcMap[ich]->Draw("colz");	
		c->cd(ich*2+2);
		hist1D[ich]->Draw();	
	}
} 
void drawFrame(int chip, int frame) //画一个通道
{
	TCanvas *c=new TCanvas("c","c",1000,500);
	c->Divide(2, 1);
	pd1.getChaDat(chip);
	cout<<"getChannel data "<<endl;	
	TH2D *hAdcMap;
	TH1D *hist=new TH1D("h","h",2100,-100,2000);
	TString histName;
	histName = "hAdcMap_Chip";histName += frame;
	hAdcMap = new TH2D(histName, "ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
	
	float y;
	for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
		for(int iframe=frame;iframe<=frame;iframe++){
			// y[iframe]=pd3.chaDat[ich*pd3.nPix()*pd3.nFrame()+ipixel*pd3.nFrame()+iframe];
			y=0;
			for(int isample=0;isample<pd1.samPerPix();isample++){
				// adcValue += short(ps[iFrame*nPixels*nSamples*nChips+iPixel*nSamples*nChips+iSample*nChips+iChip]);
				y+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
			}
			y/=(float)pd1.samPerPix();
			hist->Fill(y);
			hAdcMap->SetBinContent(72-ipixel%72,ipixel/72+1,y);
		}	
	}
	// hAdcMap->SetMaximum(50);
	// hAdcMap->SetMinimum(-20);

	c->cd(2);
	hAdcMap->Draw("colz");	
	c->cd(1);
	hist->Draw();	
}
// 画出一帧上所有像素的2D图值 
void drawFrameUpdate(int frameStart,int frameStop){
	int nTopMetalChips=pd1.adcCha();
	int nPixelsOnChip=pd1.nPix();
	int nFrames=pd1.nFrame();
	std::vector< TH1S* > mHistAdcVec; //tempoary histogram container
	mHistAdcVec.assign( nTopMetalChips*nPixelsOnChip, (TH1S*)0 );
	int canvasSizeX=pd1.adcCha()*400;
	int canvasSizeY=800;
	TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "ADC map per chip", canvasSizeX, canvasSizeY);
	ca_adcMap->Divide(nTopMetalChips, 2,0.01,0.);
	TH2D *hAdcMap[nTopMetalChips];
	TH1D *hist1D[pd1.adcCha()];
	TString histName;
	TString histName1D;
	for(int i=0;i<pd1.adcCha();i++){
		histName = "hAdcMap_Chip";histName += i;
		histName1D = "hAdcMap1D_Chip";histName1D += i;
		hAdcMap[i] = new TH2D(histName, "ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
		hist1D[i]=new TH1D(histName1D,"ADC Map;adc;counts",2100,-100,2000);		
	}
//loop pd1 data
        float y;
		TString ss;
		// frameStart=0;
		// frameStop=100;
		if(frameStart<0){frameStart=0;}
		if(frameStop>=nFrames){
			frameStop=nFrames;
		}
		int frameDraw=frameStop-frameStart;
		int count=0;
		for(int ich=0;ich<pd1.adcCha();ich++){
			pd1.getChaDat(ich);
			for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
				int code = ich * pd1.nPix() + ipixel;
				for(int iframe=frameStart;iframe<frameStop;iframe++){
					// y[iframe]=pd3.chaDat[ich*pd3.nPix()*pd3.nFrame()+ipixel*pd3.nFrame()+iframe];
					// y=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
					y=0;
					for(int isample=0;isample<pd1.samPerPix();isample++){
						// adcValue += short(ps[iFrame*nPixels*nSamples*nChips+iPixel*nSamples*nChips+iSample*nChips+iChip]);
						y+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
					}
					y/=(float)pd1.samPerPix();
					// hAdcMap[ich]->SetBinContent(ipixel%72+1,ipixel/72+1,y);
					// hAdcMap[ich]->SetBinContent(72-ipixel%72,ipixel/72+1,y);
					// hist1D[ich]->Fill(y);
					
						TH1S* histAdc = mHistAdcVec[ code ];
						if( !histAdc ){
							ss="hAdc_ZS_Chip"; ss += ich;
							ss += "_Channel"; ss += ipixel;
							histAdc = new TH1S( ss.Data(), ";Time Bin [per 3.3 ms];ADC Counts", frameDraw, 0, frameDraw );
							mHistAdcVec[ code ] = histAdc;
						}
						histAdc->SetBinContent( iframe-frameStart+1, int(y) );
						
					
				}	
				
				// mHistAdcVec[ code ]->Write();
				// return;
			}		
		}	
		
		std::vector< TH1S* >::iterator mHistAdcVecIter;

		for(int iframe=frameStart;iframe<frameStop;iframe++) {
			cout << "TopMetal II ADC Map at Frame " << iframe << endl;  
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
                // cout<<"iframe "<<iframe<<" entries "<<entries<<" "<<channelIdx<<endl;
					float rawAdc= histAdc->GetBinContent(iframe-frameStart+1);
					//if(rawAdc>mAdcCut*rmsPed[channelIdx])
						// hAdcMap[iChip]->SetBinContent(iRow+1, iColumn+1, (int)rawAdc);
						hAdcMap[iChip]->SetBinContent(72-iPixel%72,iPixel/72+1, (int)rawAdc);
						hist1D[iChip]->Fill((int)rawAdc);
               
				}
            
				for(int iC=0; iC<pd1.adcCha(); iC++) {
					ca_adcMap->cd(iC+1);
					hAdcMap[iC]->SetStats(kFALSE);
					hAdcMap[iC]->Draw("COLZ");
					// hAdcMap[iC]->SetMaximum(200);
					// hAdcMap[iC]->SetMinimum(-20);
					
					ca_adcMap->cd(iC+1)->Modified();
					ca_adcMap->cd(iC+1)->Update();
					
					ca_adcMap->cd(pd1.adcCha()+iC+1);
					hist1D[iC]->Draw();
					ca_adcMap->cd(pd1.adcCha()+iC+1)->Modified();
					ca_adcMap->cd(pd1.adcCha()+iC+1)->Update();
				}
            
				if(gSystem->ProcessEvents()) break;
			} //end frame l
			
			for(int i=0;i<8;i++){
				delete hAdcMap[i];
				delete hist1D[i];
			}
			delete ca_adcMap;
}

void drawFrameUpdate(int frameStart,int frameStop,char *pedeFile,TCanvas *ca_adcMap){
	pd1Pede pede;
	pede.setup(pedeFile);
	int nTopMetalChips=pd1.adcCha();
	int nPixelsOnChip=pd1.nPix();
	int nFrames=pd1.nFrame();
	cout<<"total frame is "<<nFrames<<endl;
	std::vector< TH1S* > mHistAdcVec; //tempoary histogram container
	mHistAdcVec.assign( nTopMetalChips*nPixelsOnChip, (TH1S*)0 );
	int canvasSizeX=pd1.adcCha()*400;
	int canvasSizeY=800;
	// TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "ADC map per chip", canvasSizeX, canvasSizeY);
	// ca_adcMap->Divide(nTopMetalChips, 2,0.01,0.);
	TH2D **hAdcMap=new TH2D *[nTopMetalChips];
	TH1D **hist1D=new TH1D *[pd1.adcCha()];
	TString histName;
	TString histName1D;
	for(int i=0;i<pd1.adcCha();i++){
		histName = "hAdcMap_Chip";histName += i;
		histName1D = "hAdcMap1D_Chip";histName1D += i;
		hAdcMap[i] = new TH2D(histName, "ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
		hist1D[i]=new TH1D(histName1D,"ADC Map;adc;counts",2100,-100,2000);		
	}
//loop pd1 data
        float y;
		TString ss;
		// frameStart=0;
		// frameStop=100;
		if(frameStart<0){frameStart=0;}
		if(frameStop>=nFrames){
			frameStop=nFrames;
		}
		int frameDraw=frameStop-frameStart;
		int count=0;
		for(int ich=0;ich<pd1.adcCha();ich++){
			pd1.getChaDat(ich);
			for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
				int code = ich * pd1.nPix() + ipixel;
				for(int iframe=frameStart;iframe<frameStop;iframe++){
					// y[iframe]=pd3.chaDat[ich*pd3.nPix()*pd3.nFrame()+ipixel*pd3.nFrame()+iframe];
					// y=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
					y=0;
					for(int isample=0;isample<pd1.samPerPix();isample++){
						// adcValue += short(ps[iFrame*nPixels*nSamples*nChips+iPixel*nSamples*nChips+iSample*nChips+iChip]);
						y+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
					}
					y/=(float)pd1.samPerPix();
					y-=pede.meanPed[ich*pd1.nPix()+ipixel];
					// hAdcMap[ich]->SetBinContent(ipixel%72+1,ipixel/72+1,y);
					// hAdcMap[ich]->SetBinContent(72-ipixel%72,ipixel/72+1,y);
					// hist1D[ich]->Fill(y);
					
						TH1S *histAdc = mHistAdcVec[ code ];
						if( !histAdc) {
							ss="hAdc_ZS_Chip"; ss += ich;
							ss += "_Channel"; ss += ipixel;
							histAdc =new TH1S( ss.Data(), ";Time Bin [per 3.3 ms];ADC Counts", frameDraw, 0, frameDraw );
							mHistAdcVec[ code ] = histAdc;
							// cout<<"-------------------------------------------------"<<endl;
						}
						histAdc->SetBinContent( iframe-frameStart+1, int(y) );
						
					
				}	
				
				// mHistAdcVec[ code ]->Write();
				// return;
			}		
		}	
		
		std::vector< TH1S* >::iterator mHistAdcVecIter;

		for(int iframe=frameStart;iframe<frameStop;iframe++) {
			cout << "TopMetal II ADC Map at Frame " << iframe << endl;  
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
                // cout<<"iframe "<<iframe<<" entries "<<entries<<" "<<channelIdx<<endl;
					float rawAdc= histAdc->GetBinContent(iframe-frameStart+1);
					//if(rawAdc>mAdcCut*rmsPed[channelIdx])
						// hAdcMap[iChip]->SetBinContent(iRow+1, iColumn+1, (int)rawAdc);
						hAdcMap[iChip]->SetBinContent(72-iPixel%72,iPixel/72+1, (int)rawAdc);
						hist1D[iChip]->Fill((int)rawAdc);
               
				}
            
				for(int iC=0; iC<pd1.adcCha(); iC++) {
					ca_adcMap->cd(iC+1);
					hAdcMap[iC]->SetStats(kFALSE);
					hAdcMap[iC]->Draw("COLZ");
					hAdcMap[iC]->SetMaximum(60);
					hAdcMap[iC]->SetMinimum(-20);
					sleep(0.1);
					ca_adcMap->cd(iC+1)->Modified();
					ca_adcMap->cd(iC+1)->Update();
					sleep(0.1);
					ca_adcMap->cd(pd1.adcCha()+iC+1);
					hist1D[iC]->Draw();
					ca_adcMap->cd(pd1.adcCha()+iC+1)->Modified();
					ca_adcMap->cd(pd1.adcCha()+iC+1)->Update();
				}
            
				if(gSystem->ProcessEvents()) break;
			} //end frame l
			
			for(int ich=0;ich<pd1.adcCha();ich++){
				
				for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
					int code = ich * pd1.nPix() + ipixel;
					delete mHistAdcVec[ code ];
				}
			}
			
			
			for(int i=0;i<8;i++){
				delete hAdcMap[i];
				delete hist1D[i];
			}
			delete [] hAdcMap;
			delete [] hist1D;
			// delete ca_adcMap;
}

void drawFrameUpdate2D(int frameStart,int frameStop,char *pedeFile,TCanvas *ca_adcMap){
	pd1Pede pede;
	pede.setup(pedeFile);
	int nTopMetalChips=pd1.adcCha();
	int nPixelsOnChip=pd1.nPix();
	int nFrames=pd1.nFrame();
	cout<<"total frame is "<<nFrames<<endl;
	std::vector< TH1S* > mHistAdcVec; //tempoary histogram container
	mHistAdcVec.assign( nTopMetalChips*nPixelsOnChip, (TH1S*)0 );
	int canvasSizeX=pd1.adcCha()*400;
	int canvasSizeY=800;
	// TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "ADC map per chip", canvasSizeX, canvasSizeY);
	// ca_adcMap->Divide(nTopMetalChips, 2,0.01,0.);
	TH2D **hAdcMap=new TH2D *[nTopMetalChips];
	TH1D **hist1D=new TH1D *[pd1.adcCha()];
	TString histName;
	TString histName1D;
	for(int i=0;i<pd1.adcCha();i++){
		histName = "hAdcMap_Chip";histName += i;
		histName1D = "hAdcMap1D_Chip";histName1D += i;
		hAdcMap[i] = new TH2D(histName, "ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
		hist1D[i]=new TH1D(histName1D,"ADC Map;adc;counts",2100,-100,2000);		
	}
//loop pd1 data
        float y;
		TString ss;
		// frameStart=0;
		// frameStop=100;
		if(frameStart<0){frameStart=0;}
		if(frameStop>=nFrames){
			frameStop=nFrames;
		}
		int frameDraw=frameStop-frameStart;
		int count=0;
		for(int ich=0;ich<pd1.adcCha();ich++){
			pd1.getChaDat(ich);
			for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
				int code = ich * pd1.nPix() + ipixel;
				for(int iframe=frameStart;iframe<frameStop;iframe++){
					// y[iframe]=pd3.chaDat[ich*pd3.nPix()*pd3.nFrame()+ipixel*pd3.nFrame()+iframe];
					// y=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
					y=0;
					for(int isample=0;isample<pd1.samPerPix();isample++){
						// adcValue += short(ps[iFrame*nPixels*nSamples*nChips+iPixel*nSamples*nChips+iSample*nChips+iChip]);
						y+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
					}
					y/=(float)pd1.samPerPix();
					y-=pede.meanPed[ich*pd1.nPix()+ipixel];
					// hAdcMap[ich]->SetBinContent(ipixel%72+1,ipixel/72+1,y);
					// hAdcMap[ich]->SetBinContent(72-ipixel%72,ipixel/72+1,y);
					// hist1D[ich]->Fill(y);
					
						TH1S *histAdc = mHistAdcVec[ code ];
						if( !histAdc) {
							ss="hAdc_ZS_Chip"; ss += ich;
							ss += "_Channel"; ss += ipixel;
							histAdc =new TH1S( ss.Data(), ";Time Bin [per 3.3 ms];ADC Counts", frameDraw, 0, frameDraw );
							mHistAdcVec[ code ] = histAdc;
							// cout<<"-------------------------------------------------"<<endl;
						}
						histAdc->SetBinContent( iframe-frameStart+1, int(y) );
						
					
				}	
				
				// mHistAdcVec[ code ]->Write();
				// return;
			}		
		}	
		
		std::vector< TH1S* >::iterator mHistAdcVecIter;

		for(int iframe=frameStart;iframe<frameStop;iframe++) {
			cout << "TopMetal II ADC Map at Frame " << iframe << endl;  
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
                // cout<<"iframe "<<iframe<<" entries "<<entries<<" "<<channelIdx<<endl;
					float rawAdc= histAdc->GetBinContent(iframe-frameStart+1);
					//if(rawAdc>mAdcCut*rmsPed[channelIdx])
						// hAdcMap[iChip]->SetBinContent(iRow+1, iColumn+1, (int)rawAdc);
						hAdcMap[iChip]->SetBinContent(72-iPixel%72,iPixel/72+1, (int)rawAdc);
						hist1D[iChip]->Fill((int)rawAdc);
               
				}
            
				for(int iC=0; iC<pd1.adcCha(); iC++) {
					ca_adcMap->cd(iC+1);
					hAdcMap[iC]->SetStats(kFALSE);
					hAdcMap[iC]->Draw("COLZ");
					hAdcMap[iC]->SetMaximum(60);
					hAdcMap[iC]->SetMinimum(-20);
					sleep(0.1);
					ca_adcMap->cd(iC+1)->Modified();
					ca_adcMap->cd(iC+1)->Update();
					sleep(0.1);
					// ca_adcMap->cd(pd1.adcCha()+iC+1);
					// hist1D[iC]->Draw();
					// ca_adcMap->cd(pd1.adcCha()+iC+1)->Modified();
					// ca_adcMap->cd(pd1.adcCha()+iC+1)->Update();
				}
				char nameS[100];
				sprintf(nameS,"%d.png",iframe);
				// ca_adcMap->SaveAs(nameS);
				if(gSystem->ProcessEvents()) break;
			} //end frame l
			
			for(int ich=0;ich<pd1.adcCha();ich++){
				
				for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
					int code = ich * pd1.nPix() + ipixel;
					delete mHistAdcVec[ code ];
				}
			}
			
			
			for(int i=0;i<8;i++){
				delete hAdcMap[i];
				delete hist1D[i];
			}
			delete [] hAdcMap;
			delete [] hist1D;
			// delete ca_adcMap;
}


void drawAllFrameUpdate2D(char *pedeFile,TCanvas *ca_adcMap){
	pd1Pede pede;
	pede.setup(pedeFile);
	int nTopMetalChips=pd1.adcCha();
	int nPixelsOnChip=pd1.nPix();
	int nFrames=pd1.nFrame();
	int frameStart = 0;
	int frameStop = nFrames; 
	cout<<"total frame is "<<nFrames<<endl;
	std::vector< TH1S* > mHistAdcVec; //tempoary histogram container
	mHistAdcVec.assign( nTopMetalChips*nPixelsOnChip, (TH1S*)0 );
	int canvasSizeX=pd1.adcCha()*400;
	int canvasSizeY=800;
	// TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "ADC map per chip", canvasSizeX, canvasSizeY);
	// ca_adcMap->Divide(nTopMetalChips, 2,0.01,0.);
	TH2D **hAdcMap=new TH2D *[nTopMetalChips];
	TH1D **hist1D=new TH1D *[pd1.adcCha()];
	TString histName;
	TString histName1D;
	for(int i=0;i<pd1.adcCha();i++){
		histName = "hAdcMap_Chip";histName += i;
		histName1D = "hAdcMap1D_Chip";histName1D += i;
		hAdcMap[i] = new TH2D(histName, "ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
		hist1D[i]=new TH1D(histName1D,"ADC Map;adc;counts",2100,-100,2000);		
	}
//loop pd1 data
        float y;
		TString ss;
		// frameStart=0;
		// frameStop=100;
		if(frameStart<0){frameStart=0;}
		if(frameStop>=nFrames){
			frameStop=nFrames;
		}
		int frameDraw=frameStop-frameStart;
		int count=0;
		for(int ich=0;ich<pd1.adcCha();ich++){
			pd1.getChaDat(ich);
			for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
				int code = ich * pd1.nPix() + ipixel;
				for(int iframe=frameStart;iframe<frameStop;iframe++){
					// y[iframe]=pd3.chaDat[ich*pd3.nPix()*pd3.nFrame()+ipixel*pd3.nFrame()+iframe];
					// y=pd3.chaDat[ipixel*pd3.nFrame()+iframe];
					y=0;
					for(int isample=0;isample<pd1.samPerPix();isample++){
						// adcValue += short(ps[iFrame*nPixels*nSamples*nChips+iPixel*nSamples*nChips+iSample*nChips+iChip]);
						y+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
					}
					y/=(float)pd1.samPerPix();
					y-=pede.meanPed[ich*pd1.nPix()+ipixel];
					// hAdcMap[ich]->SetBinContent(ipixel%72+1,ipixel/72+1,y);
					// hAdcMap[ich]->SetBinContent(72-ipixel%72,ipixel/72+1,y);
					// hist1D[ich]->Fill(y);
					
						TH1S *histAdc = mHistAdcVec[ code ];
						if( !histAdc) {
							ss="hAdc_ZS_Chip"; ss += ich;
							ss += "_Channel"; ss += ipixel;
							histAdc =new TH1S( ss.Data(), ";Time Bin [per 3.3 ms];ADC Counts", frameDraw, 0, frameDraw );
							mHistAdcVec[ code ] = histAdc;
							// cout<<"-------------------------------------------------"<<endl;
						}
						histAdc->SetBinContent( iframe-frameStart+1, int(y) );
						
					
				}	
				
				// mHistAdcVec[ code ]->Write();
				// return;
			}		
		}	
		
		std::vector< TH1S* >::iterator mHistAdcVecIter;

		for(int iframe=frameStart;iframe<frameStop;iframe++) {
			cout << "TopMetal II ADC Map at Frame " << iframe << endl;  
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
                // cout<<"iframe "<<iframe<<" entries "<<entries<<" "<<channelIdx<<endl;
					float rawAdc= histAdc->GetBinContent(iframe-frameStart+1);
					//if(rawAdc>mAdcCut*rmsPed[channelIdx])
						// hAdcMap[iChip]->SetBinContent(iRow+1, iColumn+1, (int)rawAdc);
						hAdcMap[iChip]->SetBinContent(72-iPixel%72,iPixel/72+1, (int)rawAdc);
						hist1D[iChip]->Fill((int)rawAdc);
               
				}
            
				for(int iC=0; iC<pd1.adcCha(); iC++) {
					ca_adcMap->cd(iC+1);
					hAdcMap[iC]->SetStats(kFALSE);
					hAdcMap[iC]->Draw("COLZ");
					hAdcMap[iC]->SetMaximum(50);
					hAdcMap[iC]->SetMinimum(-20);
					sleep(0.1);
					ca_adcMap->cd(iC+1)->Modified();
					ca_adcMap->cd(iC+1)->Update();
					sleep(0.1);
					// ca_adcMap->cd(pd1.adcCha()+iC+1);
					// hist1D[iC]->Draw();
					// ca_adcMap->cd(pd1.adcCha()+iC+1)->Modified();
					// ca_adcMap->cd(pd1.adcCha()+iC+1)->Update();
				}
				char nameS[100];
				sprintf(nameS,"%d.png",iframe);
				// ca_adcMap->SaveAs(nameS);
				if(gSystem->ProcessEvents()) break;
			} //end frame l
			
			for(int ich=0;ich<pd1.adcCha();ich++){
				
				for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
					int code = ich * pd1.nPix() + ipixel;
					delete mHistAdcVec[ code ];
				}
			}
			
			
			for(int i=0;i<8;i++){
				delete hAdcMap[i];
				delete hist1D[i];
			}
			delete [] hAdcMap;
			delete [] hist1D;
			// delete ca_adcMap;
}

void drawMeanRMS(int chip)//画一个通道mean, rms的2D图和柱状图 
{
	TCanvas *c=new TCanvas("mean","mean",1000,1000);
	c->Divide(2, 2);

	
	TH2D *mean2D = new TH2D("mean2D", "mean2D;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
	TH2D *rms2D = new TH2D("rms2D", "rms2D;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
	TH1D *meanhist=new TH1D("meanhist","meanhist",2100,-100,2000);
	TH1D *rmshist=new TH1D("rmshist","rmshist",2000,-100,100);
	
	float sum;
	float sum2;
	for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
	pd1.getPixDat(chip,ipixel);	
	sum=0;
	sum2=0;
		for(int iframe=0;iframe<pd1.nFrame();iframe++){
		for(int isample=0;isample<pd1.samPerPix();isample++){

			sum+=pd1.pixDat[iframe*pd1.samPerPix()+isample];
			sum2+=pow(pd1.pixDat[iframe*pd1.samPerPix()+isample],2);
			// if(ipixel==0){cout<<"ipixel "<<ipixel<<" pd1.pixDat[iframe*pd1.samPerPix()+isample] "<<(unsigned short)pd1.pixDat[iframe*pd1.samPerPix()+isample]<<endl;}
		}
		}
		if(debug>0){
			cout<<sum2<<" -- "<<sum<<endl;
			cout<<pd1.samPerPix()*pd1.nFrame()<<endl;					
		}

		
	sum/=(pd1.samPerPix()*pd1.nFrame());				
	sum2/=(pd1.samPerPix()*pd1.nFrame());
	if(ipixel==0){cout<<"ipixel "<<ipixel<<" sum "<<sum<<endl;}
					
		if(debug>0){
			cout<<sum2<<"  "<<sum<<"  "<<sum2-sum*sum<<endl;					
		}			
	mean2D->SetBinContent(72-ipixel%72,ipixel/72+1,sum);
	rms2D->SetBinContent(72-ipixel%72,ipixel/72+1,sqrt(sum2-sum*sum));
	//cout<<sum2<<"  "<<sum<<endl;
	meanhist->Fill(sum);
	rmshist->Fill(sqrt(sum2-sum*sum));
	}


	c->cd(1);
	mean2D->Draw("colz");	
	c->cd(2);
	meanhist->Draw();	
	c->cd(3);
	rms2D->Draw("colz");	
	c->cd(4);
	rmshist->Draw();		
}


void maxSignalPixel( char *pedeFile, double upLimitPixel)
    {
        pd1Pede pede;
        pede.setup(pedeFile);        
		int nTopMetalChips=pd1.adcCha();
        int nFrames=pd1.nFrame();
		TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "Pd1 ADC Distribution of All Chips", 600, 500);
        char *adcMap = Form("pd1png/pd1Adc_signal.png");
        double lowLimitPixel = -20; 
        TH1D *h1 = new TH1D("h1Adc", "Pd1 ADC distribution of all pixels", 1000, lowLimitPixel, upLimitPixel);
 
        double adc_value;        

	for(int ich=0;ich<pd1.adcCha();ich++){
		pd1.getChaDat(ich);	
		for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
			for(int iframe=0;iframe<nFrames;iframe++){
				adc_value=0;
				for(int isample=0;isample<pd1.samPerPix();isample++){
					adc_value+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
				}
				adc_value/=(double)pd1.samPerPix();
                adc_value-=pede.meanPed[ich*pd1.nPix()+ipixel];                
                h1->Fill(adc_value);
			}	
		}
    }

        ca_adcMap->cd();
        h1->Draw();
        ca_adcMap->SaveAs(adcMap);
        

    }   

    void noiseRange( char *pedeFile, double lowLimitPixel, double upLimitPixel, double lowLimitFrame, double upLimitFrame)
    {
        pd1Pede pede;
        pede.setup(pedeFile);        
		int nTopMetalChips=pd1.adcCha();
        int nFrames=pd1.nFrame();
		TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "Pd1 ADC Distribution of All Chips", 1200, 500);
        ca_adcMap->Divide(2,1);
        char *adcMap = Form("pd1png/pd1Adc_pede.png");      
        TH1D *h1 = new TH1D("h1Adc", "Pd1 ADC distribution of all pixels", 1000, lowLimitPixel, upLimitPixel);
        TH1D *h2 = new TH1D("h2Adc", "Pd1 ADC distribution of each frame", 1000,lowLimitFrame, upLimitFrame);

        double adc_value;
        for(int ich=0;ich<pd1.adcCha();ich++){
            pd1.getChaDat(ich);	
            for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
                for(int iframe=0;iframe<nFrames;iframe++){
                    adc_value=0;
                    for(int isample=0;isample<pd1.samPerPix();isample++){
                        adc_value+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
                    }
                    adc_value/=(double)pd1.samPerPix();
                    adc_value-=pede.meanPed[ich*pd1.nPix()+ipixel];                
                    h1->Fill(adc_value);
                }	
            }
        }
        ca_adcMap->cd(1);
        h1->Draw();
        double sum;
        double nSum;
        for(int iframe=0;iframe<nFrames;iframe++){
		cout<<"iframe: "<<iframe<<endl;
            sum = 0;
        //    nSum = 0;
            for(int ich=0;ich<pd1.adcCha();ich++){
                pd1.getChaDat(ich);	
                for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
                    adc_value=0;
                    for(int isample=0;isample<pd1.samPerPix();isample++){
                        adc_value+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
                    }
                    adc_value/=(double)pd1.samPerPix();


                    adc_value-=pede.meanPed[ich*pd1.nPix()+ipixel];                
                  	sum = sum+adc_value;
           //         nSum = nSum+1;
                }
            }
          //  sum = sum/nSum;
            if(sum>lowLimitFrame && sum<upLimitFrame)
                h2->Fill(sum);   
        }        
 
        ca_adcMap->cd(2);
        h2->Draw();
        
        ca_adcMap->SaveAs(adcMap);
    }
     
    
void noiseRangeUpdate( char *pedeFile, double lowLimitPixel, double upLimitPixel, double lowLimitFrame, double upLimitFrame)
    {
        pd1Pede pede;
        pede.setup(pedeFile);        
		int nTopMetalChips=pd1.adcCha();
        int nFrames=pd1.nFrame();
		TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "Pd1 ADC Distribution of All Chips", 1200, 500);
        ca_adcMap->Divide(2,1);
        char *adcMap = Form("pd1png/pd1Adc_pede.png");      
        TH1D *h1 = new TH1D("h1Adc", "Pd1 ADC distribution of all pixels", 1000, lowLimitPixel, upLimitPixel);
        TH1D *h2 = new TH1D("h2Adc", "Pd1 ADC distribution of each frame", 1000,lowLimitFrame, upLimitFrame);

        double adc_value;
        for(int ich=0;ich<pd1.adcCha();ich++){
            pd1.getChaDat(ich);	
            for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
                for(int iframe=0;iframe<nFrames;iframe++){
                    adc_value=0;
                    for(int isample=0;isample<pd1.samPerPix();isample++){
                        adc_value+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
                    }
                    adc_value/=(double)pd1.samPerPix();
                    adc_value-=pede.meanPed[ich*pd1.nPix()+ipixel];                
                    h1->Fill(adc_value);
                }	
            }
        }
        ca_adcMap->cd(1);
        h1->Draw();
        
        
        

		int nPixelsOnChip=pd1.nPix();        

		std::vector< TH1D* > mHistAdcVec; //tempoary histogram container
		mHistAdcVec.assign( nTopMetalChips*nPixelsOnChip, (TH1D*)0 );        
		TH2D *hAdcMap[nTopMetalChips];
		TString histName;
		for(int i=0; i<nTopMetalChips; i++) {
			histName = "hAdcMap_Chip"; histName += i;
			hAdcMap[i] = new TH2D(histName, "Pd1 ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
		}        
        
//loop pd1 data
        double y;
		TString ss;
		int frameStart=0;
		int frameStop=nFrames-1;
		int frameDraw=frameStop-frameStart+1;
		for(int ich=0;ich<pd1.adcCha();ich++){
			pd1.getChaDat(ich);
			for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
				int code = ich * pd1.nPix() + ipixel;
                TH1D* histAdc = mHistAdcVec[ code ];
                    if( !histAdc ){
                        ss="hAdc_ZS_Chip"; ss += ich;
                        ss += "_Channel"; ss += ipixel;
                        histAdc = new TH1D( ss.Data(), ";Time Bin [per 3.3 ms];ADC Counts", frameDraw, 0, frameDraw );
                        mHistAdcVec[ code ] = histAdc;
                    }                
				for(int iframe=frameStart;iframe<=frameStop;iframe++){
					y=0;
					for(int isample=0;isample<pd1.samPerPix();isample++){
						y+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
					}
					y/=(double)pd1.samPerPix();
					y-=pede.meanPed[ich*pd1.nPix()+ipixel];
                    histAdc->SetBinContent( iframe-frameStart+1, y );
				}	
				

			}
        }


		std::vector< TH1D* >::iterator mHistAdcVecIter;
        double sum;
        int channelIdx;
        double rawAdc;
       
		for(int iframe=frameStart;iframe<=frameStop;iframe++) {
			//cout << "TopMetal II ADC Map at Frame " << iframe << endl;  
				channelIdx = 0;                  
                sum = 0;
				for( mHistAdcVecIter = mHistAdcVec.begin(); mHistAdcVecIter != mHistAdcVec.end(); ++mHistAdcVecIter ){ //loop 8*5184 channels
					TH1D *histAdc = *mHistAdcVecIter;
					channelIdx = std::distance( mHistAdcVec.begin(), mHistAdcVecIter );
					int entries = histAdc->GetEntries();
					if( entries == 0 ) continue;
  
					int iChip   = channelIdx / nPixelsOnChip;
					int iPixel  = channelIdx % nPixelsOnChip;
                
				    rawAdc= histAdc->GetBinContent(iframe-frameStart+1);
                    hAdcMap[iChip]->SetBinContent(iPixel/72,iPixel%72, rawAdc);               
				}
				for(int iC=0; iC<pd1.adcCha(); iC++) {
                    sum = sum+hAdcMap[iC]->GetSum();
                }
                if(sum>lowLimitFrame && sum<upLimitFrame)                
                    h2->Fill(sum);
        }
                ca_adcMap->cd(2);
                h2->Draw();                
                ca_adcMap->SaveAs(adcMap);        
}
    
// display the chip and pixel based on the arrangement of tm2-_1*8_VHDCI 


void displayInterfaceUpdate(char *pedeFile, double maxPixelSignal, double ref) //
	{
        pd1Pede pede;
        pede.setup(pedeFile); 
		int nTopMetalChips=pd1.adcCha();
		int nPixelsOnChip=pd1.nPix();        
        int nFrames=pd1.nFrame();
		std::vector< TH1D* > mHistAdcVec; //tempoary histogram container
		mHistAdcVec.assign( nTopMetalChips*nPixelsOnChip, (TH1D*)0 );
        
		TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "pd1 ADC map per chip", 1600, 200);
		// ca_adcMap->Divide(nTopMetalChips, 1, 0.01, 0.);
		ca_adcMap->Divide(nTopMetalChips, 1); 
        char *adcMap;
		TH2D *hAdcMap[nTopMetalChips];
		TString histName;
		for(int i=0; i<nTopMetalChips; i++) {
			histName = "hAdcMap_Chip"; histName += i;
			hAdcMap[i] = new TH2D(histName, "pd1 ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
		}
//loop pd1 data
        double y;
		TString ss;
		int frameStart=0;
		int frameStop=nFrames-1;
		int frameDraw=frameStop-frameStart+1;
		for(int ich=0;ich<pd1.adcCha();ich++){
			pd1.getChaDat(ich);
			for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
				int code = ich * pd1.nPix() + ipixel;
                TH1D* histAdc = mHistAdcVec[ code ];
                if( !histAdc ){
                    ss="hAdc_ZS_Chip"; ss += ich;
                    ss += "_Channel"; ss += ipixel;
                    histAdc = new TH1D( ss.Data(), ";Time Bin [per 3.3 ms];ADC Counts", frameDraw, 0, frameDraw );
                    mHistAdcVec[ code ] = histAdc;
                }                
				for(int iframe=frameStart;iframe<=frameStop;iframe++){
					y=0;
					for(int isample=0;isample<pd1.samPerPix();isample++){
						y+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
					}
					y/=(double)pd1.samPerPix();
					y-=pede.meanPed[ich*pd1.nPix()+ipixel];                    
                    histAdc->SetBinContent( iframe-frameStart+1, y );
					
				}	
				

			}
        }

		std::vector< TH1D* >::iterator mHistAdcVecIter;
        double sum;
        int channelIdx;
        double rawAdc;
        int  dFrameSignal = 1;
        int  dFrameNoSignal = 100;
        int signalCount = 0;
        int noSignalCount = 0;            
        double upLimitPixel = maxPixelSignal; // up limit of single pixel
        double lowLimitPixel = -5; // low limit of single pixel
//        double ref = 1e6;        
		for(int iframe=frameStart;iframe<=frameStop;iframe++) {
	//		cout << "TopMetal II ADC Map at Frame " << iframe << endl;  
				channelIdx = 0;                  
                sum = 0;
				for( mHistAdcVecIter = mHistAdcVec.begin(); mHistAdcVecIter != mHistAdcVec.end(); ++mHistAdcVecIter ){ //loop 8*5184 channels
					TH1D *histAdc = *mHistAdcVecIter;
					channelIdx = std::distance( mHistAdcVec.begin(), mHistAdcVecIter );
					int entries = histAdc->GetEntries();
					if( entries == 0 ) continue;
  
					int iChip   = channelIdx / nPixelsOnChip;
					int iPixel  = channelIdx % nPixelsOnChip;
                
				    rawAdc= histAdc->GetBinContent(iframe-frameStart+1);
                    hAdcMap[iChip]->SetBinContent(iPixel/72,iPixel%72, rawAdc);               
				}
				for(int iC=0; iC<pd1.adcCha(); iC++) {
                    sum = sum+hAdcMap[iC]->GetSum();
                }
		/*		for(int iC=0; iC<1; iC++) {
                    sum = sum+hAdcMap[iC]->GetSum();
                }*/
            if(sum<ref){
                signalCount = 0; 
                if(noSignalCount == 0){
                    cout<< "sum: "<<sum<<endl;
                    for(int ich = 0; ich<pd1.adcCha(); ich++){
                        if(ich<pd1.adcCha()/2){
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2)->Modified();
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2)->Update();
                        }else{
                            ca_adcMap->cd(pd1.adcCha()-ich);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(pd1.adcCha()-ich)->Modified();
                            ca_adcMap->cd(pd1.adcCha()-ich)->Update();                            
                        }
                        
                                                  
                    }
                    adcMap = Form("pd1png/adcMapFrame%d.png", iframe);                                   
                   ca_adcMap->SaveAs(adcMap);  
                    noSignalCount++;
                }
                else if(noSignalCount==dFrameNoSignal){
                    cout<< "sum: "<<sum<<endl;                    
                    for(int ich = 0; ich<pd1.adcCha(); ich++){
                        if(ich<pd1.adcCha()/2){
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2)->Modified();
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2)->Update();
                        }else{
                            ca_adcMap->cd(pd1.adcCha()-ich);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(pd1.adcCha()-ich)->Modified();
                            ca_adcMap->cd(pd1.adcCha()-ich)->Update();                            
                        }
                        
                                                  
                    }
                    adcMap = Form("pd1png/adcMapFrame%d.png", iframe);                                   
                    ca_adcMap->SaveAs(adcMap); 
                    noSignalCount = 1;
                }
                else{
                    noSignalCount++;
                }
            }
            else{
                noSignalCount = 0;    
                if(signalCount == 0){
                    cout<< "sum: "<<sum<<endl;                    
                    for(int ich = 0; ich<pd1.adcCha(); ich++){
                        if(ich<pd1.adcCha()/2){
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2)->Modified();
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2)->Update();
                        }else{
                            ca_adcMap->cd(pd1.adcCha()-ich);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(pd1.adcCha()-ich)->Modified();
                            ca_adcMap->cd(pd1.adcCha()-ich)->Update();                            
                        }
                        
                                                  
                    }
                    adcMap = Form("pd1png/adcMapFrame%d.png", iframe);               
                    ca_adcMap->SaveAs(adcMap);  
                    signalCount++;
                }
                else if(signalCount==dFrameSignal){
                        cout<< "sum: "<<sum<<endl;                    
                    for(int ich = 0; ich<pd1.adcCha(); ich++){
                        if(ich<pd1.adcCha()/2){
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2)->Modified();
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2)->Update();
                        }else{
                            ca_adcMap->cd(pd1.adcCha()-ich);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(pd1.adcCha()-ich)->Modified();
                            ca_adcMap->cd(pd1.adcCha()-ich)->Update();                            
                        }
                        
                                                  
                    }
                        adcMap = Form("pd1png/adcMapFrame%d.png", iframe);               
                       ca_adcMap->SaveAs(adcMap); 
                        signalCount = 1;
                }
                else {
                    signalCount++;
                }
            }
        }




           
        
    }		

   
    void displayInterface(char *pedeFile, double maxPixelSignal) //
	{
        pd1Pede pede;
        pede.setup(pedeFile); 
        int nTopMetalChips=pd1.adcCha();
        int nFrames=pd1.nFrame();
		TCanvas *ca_adcMap = new TCanvas("ca_adcMap", "Pd1 ADC map per chip", 1600, 200);
		// ca_adcMap->Divide(nTopMetalChips, 1, 0.01, 0.);
		ca_adcMap->Divide(nTopMetalChips, 1);
        char *adcMap;
		TH2D *hAdcMap[nTopMetalChips];
		TString histName;
		for(int i=0; i<nTopMetalChips; i++) {
			histName = "pd1AdcMap_Chip"; histName += i;
			hAdcMap[i] = new TH2D(histName, "Pd1 ADC Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
		}
//loop pd1 data
// With signal,  the interval of the frames is 10; without signal, the interval of the frames is 100.
        int  dFrameSignal = 100;
        int  dFrameNoSignal = 100;
        int signalCount = 0;
        int noSignalCount = 0;        
        double adc_value ;    
        double sum;
 //       double nSum;
        double upLimitPixel = maxPixelSignal; // up limit of single pixel
        double lowLimitPixel = -20; // low limit of single pixel
        double ref = 1e5;
        for(int iframe=0; iframe<nFrames; iframe++){
            sum = 0;
  //          nSum=0;
            for(int ich=0;ich<pd1.adcCha();ich++){
                pd1.getChaDat(ich);	
                for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
                    adc_value=0;
                    for(int isample=0;isample<pd1.samPerPix();isample++){
                        adc_value+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
                    }
                    adc_value/=(double)pd1.samPerPix();
                    adc_value-=pede.meanPed[ich*pd1.nPix()+ipixel];                
                  	sum = sum+adc_value;
        //            nSum = nSum+1;
                }
            }
        //    sum = sum/nSum;       
            if(sum<ref){
                signalCount = 0; 
                if(noSignalCount == 0){
                    cout<< "sum: "<<sum<<endl;
                    for(int ich=0;ich<pd1.adcCha();ich++){
                        pd1.getChaDat(ich);	
                        for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
                            adc_value=0;
                            for(int isample=0;isample<pd1.samPerPix();isample++){
                                adc_value+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
                            }
                            adc_value/=(double)pd1.samPerPix();
                            adc_value-=pede.meanPed[ich*pd1.nPix()+ipixel];                
                            hAdcMap[ich]->SetBinContent(ipixel/72,ipixel%72,adc_value);
                        }
                        if(ich<pd1.adcCha()/2){
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2)->Modified();
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2)->Update();
                        }else{
                            ca_adcMap->cd(pd1.adcCha()-ich);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(pd1.adcCha()-ich)->Modified();
                            ca_adcMap->cd(pd1.adcCha()-ich)->Update();                            
                        }                    
                    }
                    adcMap = Form("pd1png/adcMapFrame%d.png", iframe);
                    ca_adcMap->SaveAs(adcMap);  
                    noSignalCount++;                                                 
                }
                else if(noSignalCount==dFrameNoSignal){
                    cout<< "sum: "<<sum<<endl;                    
                    for(int ich=0;ich<pd1.adcCha();ich++){
                        pd1.getChaDat(ich);	
                        for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
                            adc_value=0;
                            for(int isample=0;isample<pd1.samPerPix();isample++){
                                adc_value+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
                            }
                            adc_value/=(double)pd1.samPerPix();
                            adc_value-=pede.meanPed[ich*pd1.nPix()+ipixel];                
                            hAdcMap[ich]->SetBinContent(ipixel/72,ipixel%72,adc_value);
                        }
                        if(ich<pd1.adcCha()/2){
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2)->Modified();
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2)->Update();
                        }else{
                            ca_adcMap->cd(pd1.adcCha()-ich);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(pd1.adcCha()-ich)->Modified();
                            ca_adcMap->cd(pd1.adcCha()-ich)->Update();                            
                        }                    
                    }
                    adcMap = Form("pd1png/adcMapFrame%d.png", iframe);
                    ca_adcMap->SaveAs(adcMap); 
                    noSignalCount = 1;
                }
                else{
                    noSignalCount++;
                }
            }
            else{
                noSignalCount = 0;    
                if(signalCount == 0){
                    cout<< "sum: "<<sum<<endl;                    
                    for(int ich=0;ich<pd1.adcCha();ich++){
                        pd1.getChaDat(ich);	
                        for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
                            adc_value=0;
                            for(int isample=0;isample<pd1.samPerPix();isample++){
                                adc_value+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
                            }
                            adc_value/=(double)pd1.samPerPix();
                            adc_value-=pede.meanPed[ich*pd1.nPix()+ipixel];                
                            hAdcMap[ich]->SetBinContent(ipixel/72,ipixel%72,adc_value);
                        }
                        if(ich<pd1.adcCha()/2){
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2)->Modified();
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2)->Update();
                        }else{
                            ca_adcMap->cd(pd1.adcCha()-ich);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(pd1.adcCha()-ich)->Modified();
                            ca_adcMap->cd(pd1.adcCha()-ich)->Update();                            
                        }                    
                    }
                    adcMap = Form("pd1png/adcMapFrame%d.png", iframe);               
                    ca_adcMap->SaveAs(adcMap);  
                    signalCount++;
                }
                else if(signalCount==dFrameSignal){
                        cout<< "sum: "<<sum<<endl;                    
                    for(int ich=0;ich<pd1.adcCha();ich++){
                        pd1.getChaDat(ich);	
                        for(int ipixel=0;ipixel<pd1.nPix();ipixel++){
                            adc_value=0;
                            for(int isample=0;isample<pd1.samPerPix();isample++){
                                adc_value+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
                            }
                            adc_value/=(double)pd1.samPerPix();
                            adc_value-=pede.meanPed[ich*pd1.nPix()+ipixel];                
                            hAdcMap[ich]->SetBinContent(ipixel/72,ipixel%72,adc_value);
                        }
                        if(ich<pd1.adcCha()/2){
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2)->Modified();
                            ca_adcMap->cd(ich+1+pd1.adcCha()/2)->Update();
                        }else{
                            ca_adcMap->cd(pd1.adcCha()-ich);
                            hAdcMap[ich]->SetStats(kFALSE);                    			
                            hAdcMap[ich]->Draw("colz");
                            hAdcMap[ich]->SetMaximum(upLimitPixel);
                            hAdcMap[ich]->SetMinimum(lowLimitPixel);                    
                            ca_adcMap->cd(pd1.adcCha()-ich)->Modified();
                            ca_adcMap->cd(pd1.adcCha()-ich)->Update();                            
                        }                    
                    }
                        adcMap = Form("pd1png/adcMapFrame%d.png", iframe);               
                        ca_adcMap->SaveAs(adcMap); 
                        signalCount = 1;
                }
                else {
                    signalCount++;
                }
            }
           
        }
    }		


   void drawRow(char *pedeFile, int ich, int iframe)
   {

        pd1Pede pede;
        pede.setup(pedeFile); 
		int nTopMetalChips=pd1.adcCha();
		int nPixelsOnChip=pd1.nPix();        
        int nFrames=pd1.nFrame();
        
		TCanvas *ca_adc2dMap = new TCanvas("ca_adc2dMap", "pd1 ADC 2d map per row", 600, 400);
        char *adc2dMap; 
	TString  hist2dName;
        hist2dName = "hAdc2dMap_Chip"; hist2dName += ich;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd1 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);

        pd1.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;
        for(int ipixel=0;ipixel<pd1.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value = 0;
            for(int isample=0;isample<pd1.samPerPix();isample++){
                adc_value+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
            }
            adc_value/=(double)pd1.samPerPix();
            adc_value-=pede.meanPed[ich*pd1.nPix()+ipixel];
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }
        adc2dMap = Form("pd1png/adc2dMapFrame%dChip%d.png", iframe, ich);
        hAdc2dMap->Draw("colz");
	hAdc2dMap->SetMaximum(100);
	hAdc2dMap->SetMinimum(-20);
        ca_adc2dMap->SaveAs(adc2dMap);
       
        
		TCanvas *ca_adc1dMap = new TCanvas("ca_adc1dMap", "pd1 ADC 1d map per row", 600, 400);
        char *adc1dMap;  
	TString hist1dName;
        hist1dName = "hAdc1dMap_Chip"; hist1dName += ich;

        for(int irow = 0; irow < 72; irow++){
        TH1D *hAdc1dMap= new TH1D(hist1dName, "pd1 ADC 1d Map;pixel column index", 72, 0, 72);        
            adc1dMap = Form("pd1png/adcMapRow%dFrame%dChip%d.png", irow, iframe, ich);
            for(int icol = 0; icol < 72; icol++)
                hAdc1dMap->SetBinContent(icol+1, hAdc2dMap->GetBinContent(icol+1, irow+1));
            hAdc1dMap->Draw();
	  hAdc1dMap->SetMaximum(100);
	   hAdc1dMap->SetMinimum(-20);
            ca_adc1dMap->SaveAs(adc1dMap);                 
        }

}


  void fitBeam(char *pedeFile, int ich, int iframe)
   {

 
        pd1Pede pede;
        pede.setup(pedeFile); 
		int nTopMetalChips=pd1.adcCha();
		int nPixelsOnChip=pd1.nPix();        
        int nFrames=pd1.nFrame();
        

	TString hist2dName;
        hist2dName = "hAdc2dMap_Chip"; hist2dName += ich;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd1 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);

        pd1.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;
        for(int ipixel=0;ipixel<pd1.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
            adc_value = 0;
            for(int isample=0;isample<pd1.samPerPix();isample++){
                adc_value+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
            }
            adc_value/=(double)pd1.samPerPix();
            adc_value-=pede.meanPed[ich*pd1.nPix()+ipixel];
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }

       
	TCanvas *c=new TCanvas("c","c",700,600);
	

	  


	double rowSum;
	double pixelValue;
	double pixelMean;
	double pixelSigma2;
        double upLimit = 100;
	double lowLimit = -20;
	double pitch = 83;

        TH1D *hBeamMean = new TH1D("hBeamMean", "hBeamMean", 72, 0, 72*pitch);	
        for(int irow = 0; irow < 72; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(pixelValue/rowSum)*pitch*pitch/12; 		
		}

		}
		
		hBeamMean->SetBinContent(irow+1, pixelMean);
		hBeamMean->SetBinError(irow+1, sqrt(pixelSigma2));

	}


	TF1 *fit1 = new TF1("fit1", "pol1", pitch, 70*pitch);
	hBeamMean->Fit("fit1");
	double b = fit1->GetParameter(0);
	double theta = abs(atan(fit1->GetParameter(1)));
	double cosTheta = cos(theta);
     //   cout<< "b" <<b<< endl;
	cout << "Theta" << theta <<endl;
//	cout << "cosTheta" << cosTheta <<endl;
	//a->Draw("A*");
       double mean = b*cosTheta;
	cout << "mean" << mean <<endl;

	char *beamMean =Form("pd1png/beamMeanChip%dFrame%d.png",ich,iframe);
	c->SaveAs(beamMean);

	delete c;    

} 
   
  void beamPara_old(char *pedeFile, int ich)
   {

 
 
        pd1Pede pede;
        pede.setup(pedeFile); 
		int nTopMetalChips=pd1.adcCha();
		int nPixelsOnChip=pd1.nPix();        
        int nFrames=pd1.nFrame();
        
        


	TCanvas *c=new TCanvas("c","c",700,600);

        pd1.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;
        TH1D *hMean = new TH1D("hMean","hMean", 100, 2500, 3200);
	TH1D * hTheta = new TH1D("hTheta","hTheta", 100, -0.01, 0.05);
	double centerMean=0;
	double centerRMS=0;
	double centerThetaMean=0;
	double centerThetaRMS=0;
	for(int iframe=0; iframe<nFrames; iframe++){
	TString hist2dName;
        hist2dName = "hAdc2dMap_iframe"; hist2dName += iframe;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd1 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
        for(int ipixel=0;ipixel<pd1.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
           adc_value = 0;
            for(int isample=0;isample<pd1.samPerPix();isample++){
                adc_value+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
            }
            adc_value/=(double)pd1.samPerPix();
            adc_value-=pede.meanPed[ich*pd1.nPix()+ipixel];          
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }

	double rowSum;
	double pixelValue;
	double pixelMean;
	double pixelSigma2;
        double upLimit = 100;
	double lowLimit = 0;
	double pitch = 83;
	double b;
	double theta;
	double cosTheta;
	double mean;

        TH1D *hBeamMean = new TH1D("hBeamMean", "hBeamMean", 72, 0, 72*pitch);	
        for(int irow = 0; irow < 72; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(pixelValue/rowSum)*pitch*pitch/12; 		
		}

		}
		
		hBeamMean->SetBinContent(irow+1, pixelMean);
		hBeamMean->SetBinError(irow+1, sqrt(pixelSigma2));

	}


	TF1 *fit1 = new TF1("fit1", "pol1", pitch, 70*pitch);
        //c->cd();
	hBeamMean->Fit("fit1", "QBR");

	//char *beamMean =Form("pd3png/beamMeanChip%dFrame%d.png",ich,iframe);
	//c->SaveAs(beamMean);

	b = fit1->GetParameter(0);
	theta = abs(atan(fit1->GetParameter(1)));
	cosTheta = cos(theta);
       	mean = b*cosTheta;
	hMean->Fill(mean);
	hTheta->Fill(theta);
	centerMean = centerMean+mean;
	centerRMS = centerRMS+ mean*mean;
	centerThetaMean = centerThetaMean+theta;
	centerThetaRMS = centerThetaRMS+theta*theta;
	 
        }
	centerRMS = sqrt(abs(centerRMS-centerMean*centerMean/nFrames)/(nFrames-1));
	centerMean = centerMean/nFrames;
	centerThetaRMS = sqrt(abs(centerThetaRMS-centerThetaMean*centerThetaMean/nFrames)/(nFrames-1));
	centerThetaMean = centerThetaMean/nFrames;

	cout<<"centerMean: "<<centerMean<<endl;
	cout<<"centerRMS: "<<centerRMS<<endl;
	cout<<"centerThetaMean: "<<centerThetaMean<<endl;
	cout<<"centerThetaRMS: "<<centerThetaRMS<<endl;
	cout<<"ThetaPara: "<< centerThetaRMS/centerThetaMean*100<<endl;

	TCanvas *c1 = new TCanvas("c1", "c1", 800, 400);
	c1->Divide(2,1);
	c1->cd(1);
	hMean->Draw();
	c1->cd(2);
	hTheta->Draw();
	char *beamPara = Form("pd1png/beamParaChip%d.png", ich);
	c1->SaveAs(beamPara);
	delete c;    
	delete c1;

}



  void beamPara(char *pedeFile, int ich)
   {

 	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);

 
        pd1Pede pede;
        pede.setup(pedeFile); 
		int nTopMetalChips=pd1.adcCha();
		int nPixelsOnChip=pd1.nPix();        
        int nFrames=pd1.nFrame();
        


        

        TRandom r;
//	TCanvas *c=new TCanvas("c","c",700,600);

        pd1.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;
        TH1D *hMean = new TH1D("hMean","hMean", 100, 2200, 3300);
	TH1D * hTheta = new TH1D("hTheta","hTheta", 100, -0.05, 0.05);
	TH1D *hIntensity = new TH1D("hIntensity","Intensity Uncertainty", 100, -0.1, 0.1);
	double centerMean=0;
	double centerRMS=0;
	double thetaMean=0;
	double thetaRMS=0;
	double deltaEMean = 0;
	double deltaERMS=0;

	TH1D *hMeanVSFrame = new TH1D("hMeanVSFrame", "MeanVSFrame", nFrames, 0, nFrames);
	for(int iframe=0; iframe<nFrames; iframe++){
//	for(int iframe=124; iframe<126; iframe++){
	TString hist2dName;
        hist2dName = "hAdc2dMap_iframe"; hist2dName += iframe;
        TH2D *hAdc2dMap= new TH2D(hist2dName, "pd1 ADC 2d Map;pixel row index;pixel column index", 72, 0, 72, 72, 0, 72);
        for(int ipixel=0;ipixel<pd1.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
           adc_value = 0;
            for(int isample=0;isample<pd1.samPerPix();isample++){
                adc_value+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
            }
            adc_value/=(double)pd1.samPerPix();
            adc_value-=pede.meanPed[ich*pd1.nPix()+ipixel];             
            hAdc2dMap->SetBinContent(binX+1,binY+1,adc_value);            
        }

	double rowSum;
	double pixelValue;
	double pixelMean;
	double pixelSigma2;
        double upLimit = 100;
	double lowLimit = 0;
	double pitch = 83;
	double b;
	double theta;
	double cosTheta;
	double mean;
//	double b_fit = 2900;
//	double k_fit = 0.007;



	TGraphErrors ge;	
        for(int irow = 0; irow < 72; irow++){
        	rowSum = 0;
		pixelMean = 0;
		pixelSigma2 = 0;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		rowSum = rowSum + pixelValue;		
		}			
		}
	    for(int icol = 0; icol<72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	     if(pixelValue<upLimit & pixelValue>lowLimit){
		pixelMean = pixelMean+pixelValue/rowSum*icol*pitch;
		pixelSigma2 = pixelSigma2 + (pixelValue/rowSum)*(pixelValue/rowSum)*pitch*pitch/12; 		
		}

		}
		
		ge.SetPoint(irow+1, irow*pitch, pixelMean );
		//ge.SetPointError(irow+1, pitch/sqrt(12), sqrt(pixelSigma2));
		ge.SetPointError(irow+1, 0, sqrt(pixelSigma2));

	}



/*    c->cd();

	TString hist2dNameRotat;
        hist2dNameRotat = "hAdc2dMap_iframe"; hist2dNameRotat += iframe;
        TH2D *hAdc2dMapRotat= new TH2D(hist2dNameRotat, "pd1 ADC 2d Map", 72, 0*pitch, 72*pitch, 72, 0*pitch, 72*pitch);
        for(int ipixel=0;ipixel<pd1.nPix();ipixel++){        
            binX = ipixel/72;
            binY = ipixel%72;
           adc_value = 0;
            for(int isample=0;isample<pd1.samPerPix();isample++){
                adc_value+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
            }
            adc_value/=(double)pd1.samPerPix();
            adc_value-=pede.meanPed[ich*pd1.nPix()+ipixel];             
            hAdc2dMapRotat->SetBinContent(binY+1,binX+1,adc_value);            
        }
	hAdc2dMapRotat->Draw("COLZ");
	hAdc2dMapRotat->SetMaximum(100);
	hAdc2dMapRotat->SetMinimum(-20);
	
	
	TF1 *fit1 = new TF1("fit1", "pol1", pitch, 71*pitch);
	fit1->SetLineColor(1);
	fit1->SetLineWidth(3);
//	fit1->SetParameter(1, b_fit);
//	fit1->SetParameter(2, k_fit);	
	ge.Draw("same p*");
	
	ge.Fit("fit1", "Q");
	


	char *beamMean =Form("pd1png/beamMeanChip%dFrame%d.png",ich,iframe);
        c->SaveAs(beamMean);
*/

	TF1 *fit1 = new TF1("fit1", "pol1", pitch, 71*pitch);	
	ge.Fit("fit1", "Q");

	b = fit1->GetParameter(0);
//	cout << "b: " <<b <<endl;
	theta = atan(fit1->GetParameter(1));
//	cout << "theta: "<<theta<<endl;
	cosTheta = cos(theta);
       	mean = b*cosTheta;
//	cout <<"mean: "<<mean<<endl;
	hMean->Fill(mean);
	hTheta->Fill(theta);
	hMeanVSFrame->SetBinContent(iframe+1,mean);
	
	centerMean = centerMean+mean;
	centerRMS = centerRMS+ mean*mean;
	thetaMean = thetaMean+theta;
	thetaRMS = thetaRMS+theta*theta; 


//  intensity

	double Eup=0;
	double Edown=0;
	double deltaE;
	int irow;

	for(int i= 0; i<36; i++){
	irow = int(r.Rndm()*71);
//	cout<< "i: "<<i<<endl;
//        cout<< "irow: "<<irow<<endl;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		Eup = Eup + pixelValue;		
		}			
		}
	
	}	

	for(int i= 0; i<36; i++){
	irow = int(r.Rndm()*71);
//	cout<< "i: "<<i<<endl;
//        cout<< "irow: "<<irow<<endl;
            for(int icol = 0; icol < 72; icol++){
		pixelValue = hAdc2dMap->GetBinContent(icol+1, irow+1);
	      if(pixelValue<upLimit & pixelValue>lowLimit){
		Edown = Edown+ pixelValue;		
		}			
		}
	
	}
     // 	cout<< "Eup-Edown: "<<Eup-Edown<<endl;
//	cout<<"Eup+Edown: "<<Eup+Edown<<endl;
      deltaE= (Eup-Edown)/(Eup+Edown);
      hIntensity->Fill(deltaE);
	deltaEMean = deltaEMean+deltaE;
	deltaERMS = deltaERMS + deltaE*deltaE;	
  
	 
        }


	centerRMS = sqrt(abs(centerRMS-centerMean*centerMean/nFrames)/(nFrames-1));
	centerMean = centerMean/nFrames;
	thetaRMS = sqrt(abs(thetaRMS-thetaMean*thetaMean/nFrames)/(nFrames-1));
	thetaMean = thetaMean/nFrames;

	deltaERMS = sqrt(abs(deltaERMS-deltaEMean*deltaEMean/nFrames)/(nFrames-1));
	deltaEMean = deltaEMean/nFrames;

	cout<<"centerMean: "<<centerMean<<endl;
	cout<<"centerRMS: "<<centerRMS<<endl;
	cout<<"thetaMean: "<<thetaMean<<endl;
	cout<<"thetaRMS: "<<thetaRMS<<endl;
	cout<<"deltaEMean: "<<deltaEMean<<endl;
	cout<<"deltaERMS: "<<deltaERMS<<endl;

	TCanvas *c1 = new TCanvas("c1", "c1", 1200, 400);
	c1->Divide(3,1);
	c1->cd(1);
	hMean->Draw();
	hMean->Fit("gaus","Q");
//	hMean->SaveAs("hMean.C");
	c1->cd(2);
	hTheta->Draw();
        hTheta->Fit("gaus","Q");
//	hTheta->SaveAs("hTheta.C");
	c1->cd(3);
        hIntensity->Draw();
	hIntensity->Fit("gaus","Q");
//        hIntensity->SaveAs("hIntensity.C");
	char *beamPara = Form("pd1png/beamParaChip%d.png", ich);
	c1->SaveAs(beamPara);

	TCanvas *c2 = new TCanvas("c2", "c2",800,400);
	c2->Divide(2,1);
	c2->cd(1);
	hMeanVSFrame->Draw();
	c2->cd(2);
	   TH1 *hm =0;
	   TVirtualFFT::SetTransform(0);
	   hm = hMeanVSFrame->FFT(hm, "MAG");
	   hm->SetTitle("Magnitude of the 1st transform");

	   hm->Draw();
	   gPad->SetLogy(1);

		
	char *beamVStime = Form("pd1png/beamVStimeChip%d.png", ich);
	c2->SaveAs(beamVStime);
//	delete c;    
	delete c1;
	delete c2;

}


/*
  void badPixel(char *pedeFile, int ich)
   {

 	gStyle->SetOptStat(0);
	gStyle->SetOptFit(1);

 
        pd1Pede pede;
        pede.setup(pedeFile); 
		int nTopMetalChips=pd1.adcCha();
		int nPixelsOnChip=pd1.nPix();        
        int nFrames=pd1.nFrame();
        


        

        TRandom r;
//	TCanvas *c=new TCanvas("c","c",700,600);

        pd1.getChaDat(ich);
        double adc_value;
        int binX;
        int binY;
        TH1D *hBadPixel = new TH1D("hBadPixel","Bad Pixel Proportion", 100, 0, 1);

        double upLimit = 100;
	double lowLimit = 0;
	int nPixels = 72*72;
	int badPixel;
	double adc_value;
	for(int iframe=0; iframe<nFrames; iframe++){

	badPixel = 0;
        for(int ipixel=0;ipixel<pd1.nPix();ipixel++){        
 
           adc_value = 0;
            for(int isample=0;isample<pd1.samPerPix();isample++){
                adc_value+=pd1.chaDat[iframe*pd1.samPerPix()*pd1.nPix()+ipixel*pd1.samPerPix()+isample];
            }
            adc_value/=(double)pd1.samPerPix();
            adc_value-=pede.meanPed[ich*pd1.nPix()+ipixel];             
	    if(adc_value<upLimit & adc_value>lowLimit)
		badPixel++;
        }
	hBadPixel->Fill(double(badPixel/nPixels));
	}

	TCanvas *c1 = new TCanvas("c1", "c1", 1200, 400);

	c1->cd();
	hBadPixel->Draw();
	hBadPixel->Fit("gaus","Q");

	char *badPixelName = Form("pd1png/badPixelPro%d.png", ich);
	c1->SaveAs(badPixelName);


	delete c1;


}

*/

};







#endif
