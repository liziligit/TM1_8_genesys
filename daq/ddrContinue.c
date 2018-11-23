////////////////////////////////////////////////////////// parameter

int sampleBytes=1024*1024;



char fn[200]="out";
char mode[200]="wb";                                                                

char *buf=new char[sampleBytes];
char *bufN=new char[sampleBytes];

pid=para((char*)"-sampleBytes");  
if(pid>0){
	sampleBytes=atoi(argv[pid+1]);
}

pid=para((char*)"-out");  
if(pid>0){
	sprintf(fn,"%s",argv[pid+1]);
}


pthread_t pt;
int err;

processPara pp;
processPara ppN;
pp.p=buf;
// pp.a=100;
pp.a=sampleBytes;
ppN.p=bufN;
// ppN.a=999;
ppN.a=sampleBytes;

config_reg(10,3);
	pulse_reg(9);
	
char *pbuf;	
for(int i=0;i<1;i++){

// sdram_rd_start


	
	askTcpData(sampleBytes);
	cout<<i<<endl;
	int n=0;
	// for(int nb=0;nb<sampleBytes;nb+=n){

	if(i%2==0){
    n=read_mem_ns(buf,sampleBytes);	
	err=pthread_create(&pt,NULL, process,(void*)(&pp));
	pbuf=buf;
	}
    else{
    n=read_mem_ns(bufN,sampleBytes);	
	err=pthread_create(&pt,NULL, process,(void*)(&ppN));
	pbuf=bufN;
	}
cout<<n<<" bytes is read"<<endl;	

if(err!=0){
	cout<<"thread error"<<endl;
	
}	


write_file(fn, pbuf, n, mode);

	unsigned int *pi=(unsigned int *)pbuf;

for(int i=0; i<sampleBytes/4; i++) {
	// if(i%32==0)	printf("\n");
	  // if(pi[i]==0xaaaa5555)
	// printf(" %02x ", (unsigned char)pbuf[i]);
	 //if(pi[i]==0x5555aaaa)
	printf(" %x\n ", pi[i]);


}
// printf("\n");

}

	  
	delete [] buf;  
	delete [] bufN;  

