////////////////////////////////////////////////////////// parameter

int sampleBytes=1024*1024*48;


char fn[200]="../data/out";
char mode[200]="wb";   
string dataType="pd1"; 
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

pid=para((char*)"-fileStart");  
if(pid>0){
	fileStart=atoi(argv[pid+1]);
}

pid=para((char*)"-fileEnd");  
if(pid>0){
	fileEnd=atoi(argv[pid+1]);
}
//cout<<"sampleBytes: "<< sampleBytes<<endl;

char *buf=new char[sampleBytes];

config_reg(10,3);
	pulse_reg(9);
	

int count=0;
int n=0;
placData_1 pd;
pd.createMem(1.5*sampleBytes);

char fileName[500];
for(int i=fileStart;i<fileEnd;i++){
//config_reg(10,3);
//	pulse_reg(9);
	cout<<i<<endl;


	askTcpData(sampleBytes);

    n=read_mem_ns(buf,sampleBytes);	

cout<<n<<" bytes is read"<<endl;	

//return 0;

// write_file(fn, buf, n, mode);

unsigned int *pi=(unsigned int *)buf;

int error = 0;
int pp=0;
int nFrame=0;
//int flag = 0;
int flag = 1;
    for(int j=0; j<(sampleBytes)/4; j++) {
        // cout << "i = " << i <<endl;

	 if((pi[j]& 0x00ffffff)==0x55aaaa){
		 if(flag == 0){
			flag++;
			continue;
		 }
		 if(flag == 1) {
			 if(pp == 0){
				pp = j;
				continue;
			 }
			 else{
				
				if((j-pp)!=15553){
					printf("j = %d     di = %d\n", j,j-pp);
					cout<<"Discontinuous Data!\n";
					error++;    
					 }
				else{
					nFrame++;
				}			 
		 }

            if(error>10) break;

            pp=j;
         }


	 }
	}
		
  
	if(nFrame==0){
		cout<<"Wrong Data!\n";
		break;
	}
	if(error>0){count++;cout <<"count " <<count<<endl;}

  if(error == 0){
	if(dataType=="pd1"){
	
	pd.setupFromGenesys1X8ContinueMode(buf,sampleBytes);

	sprintf(fileName,"%s_%d.%s",fn,i,dataType.data());
	pd.write(fileName);
  }  

    

}
}
cout<<"count "<<count<<endl;	  
	delete [] buf;  
 

