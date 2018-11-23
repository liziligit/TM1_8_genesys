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
if(help==1) {cout<<endl;return 0;}


unsigned int bitSlip=3;
unsigned int iodelay=0;

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



    uint16_t ds[8], da, db;
    uint32_t *buf32, val;
    size_t n, sampleBytes = 1024;
    ssize_t i, j;
    char buf[BUFSIZ];

    buf32 = (uint32_t*)buf;
    /* reset ads5282_interface module */
    n = cmd_send_pulse(&buf32, (1<<3) & 0xffff);
    n = query_response(sockfd, buf, n, buf, 0);

    /* serial data, low 24 bit is effective */
    val = 0x80000000 | (0x00<<16) | 0x01 ; /* RST */
    write32breg(8,2); Sleep(20);
    val = 0x80000000 | (0x11<<16) | 0x0444 ; /* drive strength */
    write32breg(8,2);Sleep(20);
    val = 0x80000000 | (0x12<<16) | 0x4444 ; /* termination */
    write32breg(8,2);Sleep(20);
    
	// val = 0x80000000 | (0x45<<16) | 0x02 ; /* 0x01: PAT_DESKEW, 0x02: PAT_SYNC */
    // write32breg(8,2);Sleep(20);

	unsigned int patHigh=(pattern>>10) | (adcMode<<4);
	unsigned int patLow=((pattern & (0x3ff))<<6);
	

	
	  val = 0x80000000 | (0x25<<16) | patHigh ; /* 0x01: PAT_DESKEW, 0x02: PAT_SYNC */
      write32breg(8,2);Sleep(20);

	  val = 0x80000000 | (0x26<<16) | patLow ; /* 0x01: PAT_DESKEW, 0x02: PAT_SYNC */
      write32breg(8,2);Sleep(20);

	
    /* configAddr is high 26 bits, bufrRSTAddr = 0x7fffffc0, lclkIodelayCtrlAddr = 0x7fffff80 */
    /* low 6 bits set lclkIodelayCtrlPW */
    /* bufrRST */
    val = 0x7fffffc0;
    write32breg(8,2);Sleep(20);
	
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
