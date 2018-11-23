#include "lib.h"
 #include "clib.h"

 extern int help;



 int main(int argc, char* argv[]){  string par; if(argc>=2)par=argv[1];gargc=argc;gargv=argv;

 pid=para((char*)"-help");if(pid>0){help=1;} 

 //////////////////////////////////////////////////////// tcp setup
tcp mytcp;
mytcp.IP="192.168.2.3";
mytcp.port="1024";

mytcp.setup();
int sockfd=mytcp.sockfd;
sockfd=mytcp.sockfd;
////////////////////////////////////////////////////////// parameter
int sampleBytes=1024*1024*48;
char fn[200]="../data/out";
char mode[200]="wb";                                                                
int fileStart = 0;
int fileEnd = 1;  

char *tmpBuf = new char[5184*3*4+4];   //  用来保留上一次采样剩下的不完整的一帧数据，
char *tmpBufOrder = new char[int(sampleBytes*1.5)];   //  用来保留上转化成ADC值之后的数据，
int tmpSize;  // 不完整数据的大小 （byte为单位）


pid=para((char*)"-sampleBytes");  
if(pid>0){
	sampleBytes=atoi(argv[pid+1]);
}

pid=para((char*)"-out");  
if(pid>0){
	sprintf(fn,"%s",argv[pid+1]);
}



pid=para((char*)"-fileStart");  
if(pid>0){
	fileStart=atoi(argv[pid+1]);
}

pid=para((char*)"-fileEnd");  
if(pid>0){
	fileEnd=atoi(argv[pid+1]);
}


char *buf=new char[sampleBytes];
char *bufN=new char[sampleBytes];

pthread_t pt;
pthread_t ptN;
int flag = 0; 
int err;

processPara pp;
processPara ppN;
pp.p=buf;
pp.sampleBytes=sampleBytes;
pp.fn=fn;
pp.tmpBuf=tmpBuf;
pp.tmpBufOrder=tmpBufOrder;
pp.tmpSize=0;
pp.fileStart=fileStart;

ppN.p=bufN;
ppN.sampleBytes=sampleBytes;
ppN.fn=fn;
ppN.tmpBuf=tmpBuf;
ppN.tmpBufOrder=tmpBufOrder;
ppN.tmpSize=0;
ppN.fileStart=fileStart;

config_reg(10,3);
	pulse_reg(9);
//start daq in FPGA	

for(int i=fileStart;i<fileEnd;i++){

	cout<<fileEnd<<"----------------"<<i<<endl;
	askTcpData(sampleBytes);//set ethernet transmission data size in byte

	int n=0;
	if(i%2==0){
    n=read_mem_ns(buf,sampleBytes);// get data from FPGA	
	pp.fileNum = i;
        pp.tmpSize=ppN.tmpSize;
	//err=pthread_create(&pt,NULL, rtprocess,(void*)(&pp));// using thread to analysis data
        rtprocess((void*)(&pp));
	}
    else{
		
    n=read_mem_ns(bufN,sampleBytes);	
	ppN.fileNum = i;
	ppN.tmpSize=pp.tmpSize;
	//err=pthread_create(&ptN,NULL, rtprocess,(void*)(&ppN));
      rtprocess((void*)(&ppN));
	flag = 1;
	}
	cout<<n<<" bytes is read"<<endl;

//if(err!=0){
//	cout<<"thread error"<<endl;	
//}	


}

 
//pthread_join(pt, NULL);
//if(flag == 1)
//pthread_join(ptN, NULL); 	  
	delete [] buf;  
	delete [] bufN;  
	delete [] tmpBuf;
	delete [] tmpBufOrder;  



 if(help==1)cout<<endl; 

 return 0;} 

