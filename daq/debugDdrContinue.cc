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

char *buf=new char[sampleBytes];

config_reg(10,3);
	pulse_reg(9);
	

int n = 0;
for(int i=fileStart;i<fileEnd;i++){

	cout<<i<<endl;
	askTcpData(sampleBytes);

    n=read_mem_ns(buf,sampleBytes);	

cout<<n<<" bytes is read"<<endl;	


	unsigned int *pi=(unsigned int *)buf;
		// int j = 0;
// for(int i=0; i<sampleBytes/4; i++) {
	// if(i%32==0)	printf("\n");
	  // if(pi[i]==0xaaaa5555)
	// printf(" %02x ", (unsigned char)pbuf[i]);

	 // if(pi[i]==0x5555aaaa){
		// printf("i = %d\n", i);
		// printf("i + 15552 = %d\n", i+15552);
		// j++;
	 // }
	// printf(" %x\n ", pi[i]);
// }


ddrDataContinueMode da;
// data continuity  
int error = 0;
int pp=0;
int nFrame=0;
//int flag = 0;
int flag = 1;
/*    for(int j=0; j<(sampleBytes)/4; j++) {
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
					return 0;
					 }
				else{
					nFrame++;
				}			 
		 }


            pp=j;
         }


	 }
	}
		
  
	if(nFrame==0){
		cout<<"Wrong Data!\n";
		return 0;
	}
*/
// data correction

int start=0;
int b=0;
pp=0;
    for(int i=0; i<(sampleBytes)/4; i++) {
        // cout << "i = " << i <<endl;

	 if((pi[i]& 0x00ffffff)==0x55aaaa){
		 start=1;
		 b=0;
		 
		 if((i-pp)!=15553)
		printf("i = %d     di = %d\n", i,i-pp);
		printf(" %x\n ", pi[i]);

		 pp=i;
		 continue;
		 
	 }

	 
	 if(start==1){
		 da.ip=(char*)(pi+i);
        int ret=da.fill();
		 if(b<10)
        da.print();		
        i=i+2;		
	 }
	 
	 
		b++;
		
    }

}

	  
	delete [] buf;  



 if(help==1)cout<<endl; 

 return 0;} 

