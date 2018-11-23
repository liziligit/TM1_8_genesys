////////////////////////////////////////////////////////// parameter
paralist2var pv;

unsigned int pattern=0xfc0;
pid=para((char*)"-pattern");  
if(pid>0){
	pv.c3=argv[pid+1];
	pv.transform();
	pattern=(unsigned int)pv.value;
}
unsigned int adcMode=0x0;
pid=para((char*)"-adcMode");  
if(pid>0){
	pv.c3=argv[pid+1];
	pv.transform();
	adcMode=(unsigned int)pv.value;
}



unsigned int bitSlip=3;
pid=para((char*)"-bitSlip");  
if(pid>0){
	pv.c3=argv[pid+1];
	pv.transform();
	bitSlip=(unsigned int)pv.value;
}

unsigned int iodelay=0;
pid=para((char*)"-iodelay");  
if(pid>0){
	pv.c3=argv[pid+1];
	pv.transform();
	iodelay=(unsigned int)pv.value;
}



if(help==1) {cout<<endl;return 0;}





char fn[200]="abc.bin";
char mode[200]="wb";

////////////////////////////////////////////////////////// code

#define write32breg(raddr,paddr) do {                                   \
        /* register addr and pulse addr */                              \
        n = cmd_write_register(&buf32, raddr, val & 0xffff);            \
        n = query_response(sockfd, buf, n, buf, 0);                     \
        n = cmd_write_register(&buf32, raddr+1, (val>>16) & 0xffff);    \
        n = query_response(sockfd, buf, n, buf, 0);                     \
        n = cmd_send_pulse(&buf32, (1<<paddr) & 0xffff);                \
        n = query_response(sockfd, buf, n, buf, 0);                     \
    } while (0);

config_reg(11,3);

    uint16_t ds[8], da, db;
    uint32_t *buf32, val;
    size_t n, sampleBytes = 1024;
    ssize_t i, j;
    char buf[BUFSIZ];

    buf32 = (uint32_t*)buf;



///////////////////-----------------iodelay start here
	
    /* configAddr is high 26 bits, bufrRSTAddr = 0x7fffffc0, lclkIodelayCtrlAddr = 0x7fffff80 */
    /* low 6 bits set lclkIodelayCtrlPW */
    /* bufrRST */
    val = 0x7fffffc0;
    write32breg(8,2);Sleep(20);
cout<<"iodelay="<<iodelay<<endl;	
cout<<"bitSlip="<<bitSlip<<endl;	
    /* iodelay */
    val = 0x7fffff80 | 0x0f; /* lclk */
    write32breg(8,2); 
    for(i=0; i<8; i++) {
        val = ((i+0)<<6 | iodelay) & 0x7fffffff;
        write32breg(8,2);
        val = ((i+8)<<6 | iodelay) & 0x7fffffff;
        write32breg(8,2);
    }    
    /* bit slip */
    for(i=0; i<8; i++) {
        val = ((i+16)<<6 | bitSlip) & 0x7fffffff;
        write32breg(8,2);
        val = ((i+24)<<6 | bitSlip) & 0x7fffffff;
        write32breg(8,2);
    }

	


#undef write32breg
