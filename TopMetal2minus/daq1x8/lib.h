#ifndef _LIB_H_
#define _LIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


using namespace std;

// #pragma comment(lib, "complexDict.lib") 
// #pragma comment(lib, "dequeDict.lib") 
// #pragma comment(lib, "freetype.lib") 
// #pragma comment(lib, "libAfterImage.lib") 
// #pragma comment(lib, "libASImage.lib") 
// #pragma comment(lib, "libASImageGui.lib") 
// #pragma comment(lib, "libCint.lib") 
// #pragma comment(lib, "libCintex.lib") 
// #pragma comment(lib, "libCore.lib") 
// #pragma comment(lib, "libEG.lib") 
// #pragma comment(lib, "libEGPythia8.lib") 
// #pragma comment(lib, "libEve.lib") 
// #pragma comment(lib, "libFitPanel.lib") 
// #pragma comment(lib, "libFoam.lib") 
// #pragma comment(lib, "libFTGL.lib") 
// #pragma comment(lib, "libFumili.lib") 
// #pragma comment(lib, "libGdml.lib") 
// #pragma comment(lib, "libGed.lib") 
// #pragma comment(lib, "libGenetic.lib") 
// #pragma comment(lib, "libGenVector.lib") 
// #pragma comment(lib, "libGeom.lib") 
// #pragma comment(lib, "libGeomBuilder.lib") 
// #pragma comment(lib, "libGeomPainter.lib") 
// #pragma comment(lib, "libGLEW.lib") 
// #pragma comment(lib, "libGpad.lib") 
// #pragma comment(lib, "libGraf.lib") 
// #pragma comment(lib, "libGraf3d.lib") 
// #pragma comment(lib, "libGui.lib") 
// #pragma comment(lib, "libGuiBld.lib") 
// #pragma comment(lib, "libGuiHtml.lib") 
// #pragma comment(lib, "libGviz3d.lib") 
// #pragma comment(lib, "libHist.lib") 
// #pragma comment(lib, "libHistPainter.lib") 
// #pragma comment(lib, "libHtml.lib") 
// #pragma comment(lib, "libMathCore.lib") 
// #pragma comment(lib, "libMathMore.lib") 
// #pragma comment(lib, "libMatrix.lib") 
// #pragma comment(lib, "libMinuit.lib") 
// #pragma comment(lib, "libMinuit2.lib") 
// #pragma comment(lib, "libMLP.lib") 
// #pragma comment(lib, "libNet.lib") 
// #pragma comment(lib, "libPhysics.lib") 
// #pragma comment(lib, "libPostscript.lib") 
// #pragma comment(lib, "libProof.lib") 
// #pragma comment(lib, "libProofDraw.lib") 
// #pragma comment(lib, "libProofPlayer.lib") 
// #pragma comment(lib, "libPyROOT.lib") 
// #pragma comment(lib, "libQuadp.lib") 
// #pragma comment(lib, "libRecorder.lib") 
// #pragma comment(lib, "libReflex.lib") 
// #pragma comment(lib, "libReflexDict.lib") 
// #pragma comment(lib, "libRGL.lib") 
// #pragma comment(lib, "libRHTTP.lib") 
// #pragma comment(lib, "libRint.lib") 
// #pragma comment(lib, "libRIO.lib") 
// #pragma comment(lib, "libRODBC.lib") 
// #pragma comment(lib, "libRooFit.lib") 
// #pragma comment(lib, "libRooFitCore.lib") 
// #pragma comment(lib, "libRooStats.lib") 
// #pragma comment(lib, "libRootAuth.lib") 
// #pragma comment(lib, "libSessionViewer.lib") 
// #pragma comment(lib, "libSmatrix.lib") 
// #pragma comment(lib, "libSpectrum.lib") 
// #pragma comment(lib, "libSpectrumPainter.lib") 
// #pragma comment(lib, "libSPlot.lib") 
// #pragma comment(lib, "libSQLIO.lib") 
// #pragma comment(lib, "libTable.lib") 
// #pragma comment(lib, "libThread.lib") 
// #pragma comment(lib, "libTMVA.lib") 
// #pragma comment(lib, "libTree.lib") 
// #pragma comment(lib, "libTreePlayer.lib") 
// #pragma comment(lib, "libTreeViewer.lib") 
// #pragma comment(lib, "libUnuran.lib") 
// #pragma comment(lib, "libVMC.lib") 
// #pragma comment(lib, "libWin32gdk.lib") 
// #pragma comment(lib, "libXMLIO.lib") 
// #pragma comment(lib, "listDict.lib") 
// #pragma comment(lib, "map2Dict.lib") 
// #pragma comment(lib, "mapDict.lib") 
// #pragma comment(lib, "mathtext.lib") 
// #pragma comment(lib, "multimap2Dict.lib") 
// #pragma comment(lib, "multimapDict.lib") 
// #pragma comment(lib, "multisetDict.lib") 
// #pragma comment(lib, "setDict.lib") 
// #pragma comment(lib, "vectorDict.lib") 












void write_file(char*fn,char* wb,int wbz,char *mode){

		FILE *p;
		p=fopen(fn,mode);
		fwrite(wb,wbz,1,p);	
        fclose(p);		

}	


class file_loder{
public:
file_loder(){
	fz=0;
	buf=NULL;
}
~file_loder(){
if(buf!=NULL) delete []buf;
buf=NULL;
}
static int file_size(char* fn){FILE *p =fopen(fn,"rb"); if(p==NULL) {printf("FILE %s Not exist",fn);return -1;} fseek(p,0L,SEEK_END);int fz=ftell(p);fclose(p);	return fz;}
static int file_load(char* fn,char* pd, int length){FILE *fp=fopen(fn,"rb");if(fp==NULL){printf("FILE %s Is Not Open",fn);return 0;}fread(pd,1,length,fp);fclose(fp);	return 1;}




int fz;
char* buf;
void load(char *fn){
	fz=file_size(fn);
	if(buf!=NULL) delete []buf;
	buf=new char[fz];
	file_load(fn,buf,fz);
	
}
void write(char*fn,char *mode){

write_file(fn,buf,fz,mode);

}	

void write(char *fn,string s,char *mode){

write_file(fn,(char*)s.data(),s.length(),mode);

}
	
};


class ddrData{
	public:
	char *ip;
	char right[32];
	unsigned short v[2][8];
	int pixAddr;
	int paddr;
	unsigned int *ui;
	int start;
	int offset;
	int error;
	int checkAddr;
	ddrData(){
		error=0;
		checkAddr=0;
	}
	

	int fill(){
		int ret=1;
		for(int i=0;i<32;i++){
			right[i]=ip[31-i];
		}
		for(int j=0;j<2;j++){
		for(int i=0;i<8;i++){
			start=((i+j*8)*12)/8;
			offset=(i+j*8)*12-start*8;
			ui=(unsigned int *)(right+start);
			v[j][i]=0x00000fff & (ui[0]>>offset);
		}
		}
		
		
		ui=(unsigned int *)(right+24);
		pixAddr=ui[0] & 0x1fff;
		ui=(unsigned int *)(right+28);
		// cout<<"xxxxxxxxxxx"<<hex<<ui[0]<<dec<<endl;		
		
		if(ui[0]==0xf0f0a5a5){ 
		// cout<<"return 1"<<endl;		
		}else {
			printf("error line \n");			
			for(int i=0; i<32; i++) {
				printf(" %02x ", (unsigned char)ip[i]);
			}
			printf("\n");
		
		return 0;}
		
		if(checkAddr==1){
		if(pixAddr>paddr){
			if(pixAddr-paddr!=1){
				error++;
				if(error>2)
				cout<<"adj add "<<paddr<<" "<<pixAddr<<endl;
				ret=2;
			}
			
		}else{
			if(paddr!=5183||pixAddr!=0){
				error++;
				if(error>2)				
				cout<<"adj add "<<paddr<<" "<<pixAddr<<endl;
				ret=2;
			}			
		}
		
	    paddr=pixAddr;
		}
		
		return ret;

	}
	void print(){
		printf(" \n");
		
		for(int j=0;j<2;j++){
		for(int i=0;i<8;i++){
			printf(" %03x ", v[j][i]);
		}
		printf("\n");		
		}		

        printAddr();
     
		
	}
	
	void printAddr(char cmd[]=(char*)"hex"){
		if(strcmp(cmd,"hex")==0)
		printf("pixAddr= %04x \n", pixAddr);	
		if(strcmp(cmd,"dec")==0)
		printf("pixAddr= %d \n", pixAddr);	
	
	}	
	
};


class placData{
	public:
	char header[1024];
	char *p;
	int memz;
	int nByte;
	int debug;
	placData(){
		debug=0;
		p=NULL;
		memz=0;
	}
	~placData(){
		if(p!=NULL){
			delete []p;
			p=NULL;
			memz=0;
		}
	}	
	
	int createMem(int s){
		if(memz<s){
			delete []p;
			p=new char[s];
			memz=s;
		}	
        return s;		
	}
	
	void write(char *fn){
		write_file(fn,header,1024,(char*)"wb");
		write_file(fn,p,nByte,(char*)"ab");
		
	};
	void read(char *fn){
		int fz=file_loder::file_size(fn);
		fz-=1024;
		if(fz<=0){cout<<fz<<" is negtive"<<endl; exit(0);}
        createMem(fz);
		nByte=fz;
		FILE *fp;
		fp=fopen(fn,"rb");
		if(fp==NULL){cout<<fn<<" is not opened"<<endl; exit(0);}
		
		fread(header,1024,1,fp);
		fread(p,nByte,1,fp);
		fclose(fp);
		
	}
	
	
};


class placData_1 : public placData {
	public:
	
	int dataSize(){return ((int   *)header+1*4)[0];};	
	int adcBit(){return ((int   *)header+2*4)[0];};
	int adcCha(){return ((int   *)header+3*4)[0];};
	int samPerPix(){return ((int   *)header+4*4)[0];};	
	float dt(){return ((float   *)header+5*4)[0];};	
	int nPix(){return ((int   *)header+6*4)[0];};	
	int nFrame(){return ((int   *)header+7*4)[0];};
	
	placData_1(){
		
    ((int *)header+0*4)[0]=1;	
	
	}
	


	void setupFromGenesys1X8(char* pIn,int sampleBytes,int shift=49/2){
		
		float dt=40e-9;
		int adcBit=12;
		int adcCha=8;
		
    ddrData da;
	int nw=sampleBytes/32;
	
	int offset=20;
	int firstZero;
	int firstZeroDown;
	int secondZero;
	
	int nzero=0;
	da.paddr=0;
	
	for(int i=offset;i<nw;i++){
		da.ip=pIn+i*32;
		da.fill();
		//cout<<da.pixAddr<<endl;

		if(da.pixAddr==0&&da.paddr!=0){
			if(nzero==0){
				firstZero=i;
				nzero++;
				da.paddr=da.pixAddr;
				continue;
			}
			
			if(nzero==1){
				secondZero=i;
				nzero++;
				break;
			}			
		}
		if(nzero==1&&da.pixAddr!=0&&da.paddr==0){
			firstZeroDown=i;
		}
		
		da.paddr=da.pixAddr;
	}
	if(debug>0){
	cout<<"firstZero="<<firstZero<<endl;
	cout<<"secondZero="<<secondZero<<endl;
	cout<<"firstZeroDown="<<firstZeroDown<<endl;
	}
	firstZero+=shift;
	secondZero+=shift;
	firstZeroDown+=shift;
	
	int nFrame=(sampleBytes/32-firstZero)/(secondZero-firstZero);
	int samPerPix=firstZeroDown-firstZero;
	int nPix=(secondZero-firstZero)/samPerPix;
	if(nPix!=5184) {
		cout<<"nPix!=5184"<<endl;
		return ;
	}
	if(debug>0){	
	cout<<"nFrame="<<nFrame<<endl;
	cout<<"samPerPix="<<samPerPix<<endl;
	cout<<"nPix="<<nPix<<endl;	
	}

    int j=0;
	unsigned short *ps;
	for(int i=firstZero;i<firstZero+nFrame*samPerPix*nPix;i++){
		da.ip=pIn+i*32;
		da.fill();
		// cout<<"j="<<j<<endl;
		ps=(unsigned short *)(p+j*32);
		
		for(int si=0;si<2;si++){
		for(int sj=0;sj<8;sj++){
			ps[si*8+sj]=da.v[si][sj];
		}}
		j++;
		//cout<<da.pixAddr<<endl;
	}	
	
	int dataSize=j*32;
	nByte=dataSize;
	if(debug>0){	
	cout<<"nByte="<<nByte<<endl;
	}
    samPerPix*=2; //32 byte has 2 sample in firmware
	
    ((int   *)header+1*4)[0]=dataSize;		
    ((int   *)header+2*4)[0]=adcBit;		
    ((int   *)header+3*4)[0]=adcCha;		
    ((int   *)header+4*4)[0]=samPerPix;		
    ((float *)header+5*4)[0]=dt;		
    ((int   *)header+6*4)[0]=nPix;		
    ((int   *)header+7*4)[0]=nFrame;	
     

	}
	
	
};

class placData_2 : public placData {
	public:
	
	int dataSize(){return ((int   *)header+1*4)[0];};	
	int adcBit(){return ((int   *)header+2*4)[0];};
	int adcCha(){return ((int   *)header+3*4)[0];};
	int pixAddr(){return ((int   *)header+4*4)[0];};	
	float dt(){return ((float   *)header+5*4)[0];};	

	
	placData_2(){
		
    ((int *)header+0*4)[0]=2;	
	
	}
	


	void setupFromGenesys1X8(char* pIn,int sampleBytes){
		
		float dt=40e-9;
		int adcBit=12;
		int adcCha=8;
		
    ddrData da;
	int nw=sampleBytes/32;
	


	




    int j=0;
	unsigned short *ps;
	int addr=0;
	for(int i=0;i<nw;i++){
		da.ip=pIn+i*32;
		da.fill();
		if(i==0){
			addr=da.pixAddr;
		}
		
		// cout<<"j="<<j<<endl;
		ps=(unsigned short *)(p+j*32);
		
		for(int si=0;si<2;si++){
		for(int sj=0;sj<8;sj++){
			ps[si*8+sj]=da.v[si][sj];
		}}
		j++;
		//cout<<da.pixAddr<<endl;
	}	
	
	int dataSize=nw*32;
	nByte=dataSize;
	if(debug>0){	
	cout<<"nByte="<<nByte<<endl;
	}

    ((int   *)header+1*4)[0]=dataSize;		
    ((int   *)header+2*4)[0]=adcBit;		
    ((int   *)header+3*4)[0]=adcCha;		
    ((int   *)header+4*4)[0]=addr;		
    ((float *)header+5*4)[0]=dt;		

     

	}
	
	
};





#endif
