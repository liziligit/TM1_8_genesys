////////////////////////////////////////////////////////// parameter

int sampleBytes=1024*1024*256;



char fn[200]="out";
char mode[200]="wb";
string dataType="pd1";

char *buf=new char[sampleBytes];

pid=para((char*)"-sampleBytes");  
if(pid>0){
	sampleBytes=atoi(argv[pid+1]);
}

pid=para((char*)"-dataType");  
if(pid>0){
	dataType=argv[pid+1];
}


pid=para((char*)"-out");  
if(pid>0){
	sprintf(fn,"%s",argv[pid+1]);
}

int checkAddr=0;
pid=para((char*)"-checkAddr");  
if(pid>0){
	checkAddr=atoi(argv[pid+1]);
}

if(help==1) {cout<<endl;return 0;}
// cout<<"sampleBytes="<<sampleBytes<<endl;

//
//
//// select ddr
//config_reg(10,3);
//// CTRL_RESET
//pulse_reg(6);
//Sleep(20);
////sdram data fifo reset
//pulse_reg(9);
//Sleep(20);
//// WR_ADDR_BEGIN
//config_reg(12,0);
//config_reg(13,0x0000);  //0x0000 for no Around, x08000 for  Around
//// WR_START
//pulse_reg(10);
//Sleep(20);
//// WR_STOP
//pulse_reg(7);
//Sleep(20);
//// RD_ADDR_END
//

unsigned int addr=sampleBytes/8-1;

	unsigned int high=(addr>>16) ;
	unsigned int low=addr & 0xffff;

config_reg(16,low);
config_reg(17,high);
// sdram_rd_start
pulse_reg(8);	

Sleep(20);

	askTcpData(sampleBytes);
	
	int n=0;
	// for(int nb=0;nb<sampleBytes;nb+=n){

    n=read_mem(buf,sampleBytes);
	// cout<<n<<endl;
    // pulse_reg(8);	
// Sleep(20);
	// askTcpData(sampleBytes);
	
// }	
	//write_file(fn,buf,n,mode);

// for(int i=0; i<sampleBytes; i++) {
	// if(i%32==0)	printf("\n");
	// printf(" %02x ", (unsigned char)buf[i]);

// }
// printf("\n");

ddrData da;

int error=0;
for(int i=0; i<sampleBytes/32; i++) {
	da.ip=buf+i*32;
	int ret=da.fill();
	if(ret==0){
		cout<<"format error at "<<i<<endl; 
		error++;
		da.print();}else if(ret==2){
			if(da.error>2)
			cout<<" "<<i<<endl;
		}
	
	// if(i%100==0) da.print();
	
	if(error>10) break;
	
	// da.printAddr((char*)"dec");
}


if(error==0){
	if(dataType=="pd1"){
	placData_1 pd;
	
	pd.createMem(sampleBytes);
	
	pd.setupFromGenesys1X8(buf,sampleBytes,4/2);
	
	// unsigned short *ps=(unsigned short *)pd.p;
	
// for(int i=0;i<sampleBytes/2;i=i+100){

	// cout<<i<<" =  "<<ps[i]<<endl;

// }	
	char fileName[500];
	sprintf(fileName,"%s.%s",fn,dataType.data());
	pd.write(fileName);
	
	}
	
	if(dataType=="pd2"){
	placData_2 pd;
	
	pd.createMem(sampleBytes);
	
	pd.setupFromGenesys1X8(buf,sampleBytes);
	
	char fileName[500];
	sprintf(fileName,"%s.%s",fn,dataType.data());
	pd.write(fileName);
	}	
	
    if(dataType=="bin"){
	write_file(fn,buf,n,mode);
	}
}

      // printf("n=%d\n",n);
	  
	delete [] buf;  

