unsigned int guardRintEnable=0x000;
pid=para((char*)"-guardRintEnable");  
if(pid>0){
	guardRintEnable=atoi(argv[pid+1]);
}
config_reg(6,guardRintEnable);