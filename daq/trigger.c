
int wait=1000;
pid=para((char*)"-wait");  
if(pid>0){
	wait=atoi(argv[pid+1]);
}

// select ddr
config_reg(10,3);
// CTRL_RESET
pulse_reg(6);
Sleep(20);
//sdram data fifo reset
pulse_reg(9);
Sleep(20);
// WR_ADDR_BEGIN
config_reg(12,0);
config_reg(13,0x0000);  //0x0000 for no Around, x08000 for  Around
// WR_START
pulse_reg(10);
// Sleep(400);
Sleep(wait);

