/////////////////////////////////config sram
int ** addr=new int*[8];
int ** data=new int*[8];

for(int i=0;i<8;i++){
addr[i]=new int[72*72+1];
data[i]=new int[72*72+1];
}

for(int j=0;j<72*72+1;j++){
for(int i=0;i<8;i++){
addr[i][j]=j*8+i;
data[i][j]=20;
if(j==72*72){
	data[i][j]=8;
}
}}


for(int j=0;j<72*72+1;j++){

for(int i=0;i<8;i++){
config_reg(10,addr[i][j]);
config_reg(9,data[i][j]+(0<<12)+(1<<10)+(1<<13));
config_reg(9,data[i][j]+(1<<12)+(1<<10)+(1<<13));
}
config_reg(9,data[7][j]+(0<<12)+(1<<10)+(1<<13));
config_reg(9,data[7][j]+(1<<12)+(1<<10)+(1<<13));

}
/////////////////////////////////config sram



////////////////////////// read status
int memAddr=411;

int raddr=memAddr*8+3;
config_reg(10,raddr);
config_reg(9,0+(0<<12)+(0<<10)+(1<<13));
config_reg(9,0+(1<<12)+(0<<10)+(1<<13));

debug=6;
status_reg(0);
////////////////////////////////// read status


//////////////////////////////////read data
config_reg(10,0);


int sampleBytes = 1024;
 char *buf=new   char[sampleBytes];

int  n=read_mem(buf,sampleBytes);

	for(int i=0;i<sampleBytes;i++){
		printf("%x ",(unsigned char)(buf[i]));
	}
		printf("\n");

//////////////////////////////////read data