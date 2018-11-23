////////////////////////////////////////////////////////// parameter


char in[200]="../data/out0.pd1";
pid=para((char*)"-in");  
if(pid>0){
	sprintf(in,"%s",argv[pid+1]);
}

pid=para((char*)"-inIndex");  
if(pid>0){
	sprintf(in,"../data/out%d.pd1",atoi(argv[pid+1]));
} 


char out[200]="../data/pedeData/pede0.txt";
pid=para((char*)"-out");  
if(pid>0){
	sprintf(out,"%s",argv[pid+1]);
}

pid=para((char*)"-outIndex");  
if(pid>0){
	sprintf(out,"../data/pedeData/pede%d.txt",atoi(argv[pid+1]));
} 


int debug=0;
pid=para((char*)"-debug");  
if(pid>0){
	debug=atoi(argv[pid+1]);
}


//////////////read pd1 data from disk

placData_1 pd;

pd.read(in);


cout<<"nFrame=="<<pd.nFrame()<<endl;
cout<<"nPix=="<<pd.nPix()<<endl;
cout<<"adcCha=="<<pd.adcCha()<<endl;


//////////////calculate mean and rms

ofstream of;
of.open(out);
double tsum2[8];

for(int k=0;k<pd.adcCha();k++){
tsum2[k]=0;
for(int i=0;i<pd.nPix();i++){
double sum=0;
double sum2=0;
pd.getPixDat(k,i);
for(int j=0;j<pd.nFrame();j++){

sum+=pd.pixDat[j];
sum2+=pd.pixDat[j]*pd.pixDat[j];

}
double np=pd.nFrame();
sum/=np;  //mean
sum2/=np;
sum2=sqrt(sum2-sum*sum); //rms
/////////////////output data format
of<<k<<" "<<i<<" "<<sum<<" "<<sum2<<endl;
if(debug>1)cout<<k<<" "<<i<<" "<<sum<<" "<<sum2<<endl;
tsum2[k]+=sum2;
}
tsum2[k]/=pd.nPix();
cout<<"chip"<<k<<"average RMS = "<<tsum2[k]<<endl;
}


of.close();



