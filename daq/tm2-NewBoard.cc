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

unsigned int startAddr=0;
unsigned int clockDiv=3;
unsigned int stopMode=0;
unsigned int stopAddr=0;
unsigned int writeAddr=6;
unsigned int writeClt=0;
unsigned int reset=0;



pid=para((char*)"-startAddr");  
if(pid>0){
	startAddr=atoi(argv[pid+1]);
}

pid=para((char*)"-clockDiv");  
if(pid>0){
	clockDiv=atoi(argv[pid+1]);
}

pid=para((char*)"-stopMode");  
if(pid>0){
	stopMode=atoi(argv[pid+1]);
}


pid=para((char*)"-stopAddr");  
if(pid>0){
	stopAddr=atoi(argv[pid+1])+1;  // offset is 1 for stop address
}

pid=para((char*)"-writeAddr");  
if(pid>0){
	writeAddr=atoi(argv[pid+1]);
}

pid=para((char*)"-writeClt");  
if(pid>0){
	writeClt=atoi(argv[pid+1]);
}
//if(stopMode==1) writeClt=1;
pid=para((char*)"-reset");  
if(pid>0){
	reset=atoi(argv[pid+1]);
}

if(help==1) {cout<<endl;return 0;}
//////////////////////////////////

//3 using ADC clock to drive tm2-; 0 usiing FPGA 50MHz clock
config_reg(5,3);
//config_reg(5,0);

unsigned int reg2=(clockDiv & 0xf)+((startAddr<<4) & 0x30)+((writeAddr<<8) & 0xff00);

config_reg(2,reg2);

unsigned int reg3=(stopAddr & 0x1fff)+((stopMode<<13) & 0x2000)+((writeClt<<14) & 0x4000);

config_reg(3,reg3);


/////////////////////////tm2 module and adc_ddr_interface module and DAC module reset

if(reset==1){
unsigned int reg1;
reg1=0x0000;
config_reg(1,reg1);
Sleep(1);
reg1=0x0100;
config_reg(1,reg1);
Sleep(1);
reg1=0x0000;
config_reg(1,reg1);
Sleep(1);
}


if(stopMode==0){

/////////////////////////tm2- reset
reg2+=0x40;
config_reg(2,reg2);
Sleep(1);
reg2-=0x40;
config_reg(2,reg2);
Sleep(1);

/////////////////////////tm2- start
reg2+=0x80;
config_reg(2,reg2);
Sleep(1);
reg2-=0x80;
config_reg(2,reg2);
Sleep(1);
}


 if(help==1)cout<<endl; 

 return 0;} 

