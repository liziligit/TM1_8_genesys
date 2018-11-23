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

int start=0;
pid=para((char*)"-start");  
if(pid>0){
	start=atoi(argv[pid+1]);
}

int addr=0;
pid=para((char*)"-addr");  
if(pid>0){
	addr=atoi(argv[pid+1]);
}


char *buf=new char[sampleBytes];
unsigned int *pi=(unsigned int *)buf;
ddrDataContinueMode da;



////////////////read data from tcp
if(newData==1){
cout<<"ddddddddddddddddddddddddddd"<<endl;
config_reg(10,3);
	pulse_reg(9);
int n = 0;
	
	askTcpData(sampleBytes);
    n=read_mem_ns(buf,sampleBytes);	
cout<<n<<" bytes is read"<<endl;	
}

return 0;

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


    for(int i=0; i<(sampleBytes)/4*0+1000; i++) {
		 da.ip=(char*)(pi+i+start);
                 da.fill();
		 if(i<30){
                 da.print();		
		}
        i=i+2;		
    }
}
/////////////////write pd2
if(writePd2==1){
placData_2 pd2;
pd2.createMem(1.5*sampleBytes);
pd2.setupFromGenesys1X8(buf,sampleBytes,start,addr);
pd2.write(fn);
}

	  
	delete [] buf;  

