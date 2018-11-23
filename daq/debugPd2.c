////////////////////////////////////////////////////////// parameter

int sampleBytes=1024*1024*48;


char fn[200]="../data/out";
char mode[200]="wb";   
string dataType="pd2"; 
int fileStart = 0;
int fileEnd = 1;                                                           



pid=para((char*)"-sampleBytes");  
if(pid>0){
	sampleBytes=atoi(argv[pid+1]);
}

pid=para((char*)"-out");  
if(pid>0){
	sprintf(fn,"%s",argv[pid+1]);
}

pid=para((char*)"-dataType");  
if(pid>0){
	dataType=argv[pid+1];
}

int newData=0;
pid=para((char*)"-newData");  
if(pid>0){
	newData=atoi(argv[pid+1]);
}

int writeData=0;
pid=para((char*)"-writeData");  
if(pid>0){
	writeData=atoi(argv[pid+1]);
}

int readData=0;
pid=para((char*)"-readData");  
if(pid>0){
	readData=atoi(argv[pid+1]);
}


int checkData=0;
pid=para((char*)"-checkData");  
if(pid>0){
	checkData=atoi(argv[pid+1]);
}


int writePd2=0;
pid=para((char*)"-writePd2");  
if(pid>0){
	writePd2=atoi(argv[pid+1]);
}


int addr=0;
pid=para((char*)"-addr");  
if(pid>0){
	addr=atoi(argv[pid+1]);
}


char *buf=new char[sampleBytes];
unsigned int *pi=(unsigned int *)buf;
ddrDataContinueMode da;
int start=0;


////////////////read data from tcp
if(newData==1){

config_reg(10,3);
	pulse_reg(9);
int n = 0;
cout<<"taking data"<<endl;	
cout<<"expect "<<sampleBytes<<endl;	
	askTcpData(sampleBytes);
    n=read_mem_ns(buf,sampleBytes);	
cout<<n<<" bytes is read"<<endl;	
}



////////////////write data to disk
if(writeData==1){
FILE *fp=fopen("tmp.bin","wb");
fwrite(buf,sampleBytes,1,fp);
fclose(fp);
}
////////////////read data from disk
if(readData==1){
FILE *fp=fopen("tmp.bin","rb");
fread(buf,sampleBytes,1,fp);
fclose(fp);
}

////////////////check data

if(checkData==1){

double rms2[3];
for(start=0;start<3;start++){  //find the right start value based on rms is minimum
double num=0;
double sum=0;
double sum2=0;

    for(int i=20000; i<(sampleBytes)/4*0+100000; i++) {
		 da.ip=(char*)(pi+i+start);
                 da.fill();
		 if(i<20300){
                 //da.print();
num++;
sum+=da.v[0][1];
sum2+=da.v[0][1]*da.v[0][1];
		
		}
        i=i+2;		
    }
sum/=num;
sum2/=num;


rms2[start]=sum2-sum*sum;
}
double min=1000;

for(int i=0; i<3; i++) {
if(rms2[i]<min){
start=i;
min=rms2[i];
}

}



}


/////////////////write pd2
if(writePd2==1){
placData_2 pd2;
pd2.createMem(1.5*sampleBytes);
//cout<<"sampleBytes===="<<sampleBytes-80000<<endl;
pd2.setupFromGenesys1X8(buf+80000,sampleBytes-80000,start,addr);
pd2.write(fn);
}

	  
	delete [] buf;  

