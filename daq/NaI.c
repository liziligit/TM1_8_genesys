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



char *buf=new char[sampleBytes];


//配置fpga，连接数据通道 和 复位fpga内的缓冲区
config_reg(10,3);  
	pulse_reg(9);
	


//向fpga申请数据
	askTcpData(sampleBytes); 
    int n=0;
//从fpga读取数据
    n=read_mem_ns(buf,sampleBytes);	

  cout<<n<<" bytes is read"<<endl;	
//



//原始数据结构

//aaaa5555
//adcword1 |重，重复4096次
//adcword2 |复
//adcword3 |结构
//adcword1
//adcword2
//adcword3
//....
//....
//....
//adcword1
//adcword2
//adcword3
//aaaa5555
//adcword1 |重，重复4096次
//adcword2 |复
//adcword3 |结构

//用整形数指向数据缓冲区
	unsigned int *pi=(unsigned int *)buf;


//把原始数据变成8个adc采样，每个采样用short int 存储
ddrDataContinueMode da;
 




int b=1;
//对每一个word做循环
    for(int i=0; i<(sampleBytes)/4; i++) {

//寻找0x55aaaa
      if((pi[i]& 0x00ffffff)==0x55aaaa){
      b=0;
      continue;
		printf(" %x\n ", pi[i]);
      }
    

if(b%3==0){
        da.ip=(char*)(pi+i);
        int ret=da.fill();  //转换后的数据存在da.v[0][i]  i=0-7
		 if(b<10)
//printf(" %x\n ", pi[i]);
        da.print();		
}	



     b++;

    }



	  
	delete [] buf;  

