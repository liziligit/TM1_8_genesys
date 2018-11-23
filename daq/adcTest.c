
char *buf=new char[20];

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

    buf32 = (uint32_t*)buf;
    /* reset ads5282_interface module */
    n = cmd_send_pulse(&buf32, (1<<3) & 0xffff);
    n = query_response(sockfd, buf, n, buf, 0);
    /* serial data, low 24 bit is effective */
    val = 0x80000000 | (0x00<<16) | 0x01 ; /* RST */
    write32breg(8,2); Sleep(2);
    val = 0x80000000 | (0x11<<16) | 0x0444 ; /* drive strength */
    write32breg(8,2);
    val = 0x80000000 | (0x12<<16) | 0x4444 ; /* termination */
    write32breg(8,2);
    
	val = 0x80000000 | (0x45<<16) | 0x02 ; /* 0x01: PAT_DESKEW, 0x02: PAT_SYNC */
    write32breg(8,2);

	 // val = 0x80000000 | (0x25<<16) | 0x13 ; /* 0x01: PAT_DESKEW, 0x02: PAT_SYNC */
     // write32breg(8,2);
	 // val = 0x80000000 | (0x26<<16) | 0xf000 ; /* 0x01: PAT_DESKEW, 0x02: PAT_SYNC */
     // write32breg(8,2);

	
    /* configAddr is high 26 bits, bufrRSTAddr = 0x7fffffc0, lclkIodelayCtrlAddr = 0x7fffff80 */
    /* low 6 bits set lclkIodelayCtrlPW */
    /* bufrRST */
    val = 0x7fffffc0;
    write32breg(8,2);
    /* iodelay */
    val = 0x7fffff80 | 0x0f; /* lclk */
    write32breg(8,2);
    for(i=0; i<8; i++) {
        val = ((i+0)<<6 | 0x00) & 0x7fffffff;
        write32breg(8,2);
        val = ((i+8)<<6 | 0x00) & 0x7fffffff;
        write32breg(8,2);
    }    
    /* bit slip */
    for(i=0; i<8; i++) {
        val = ((i+16)<<6 | 0x03) & 0x7fffffff;
        write32breg(8,2);
        val = ((i+24)<<6 | 0x03) & 0x7fffffff;
        write32breg(8,2);
    }

    /* data source to fifo96 */
    n = cmd_write_register(&buf32, 10, 0x0000);
    n = query_response(sockfd, buf, n, buf, 0);
    /* write to fifo96 */
    n = cmd_send_pulse(&buf32, (1<<5) & 0xffff);
    n = query_response(sockfd, buf, n, buf, 0);
    Sleep(2);
    /* read fifo96 */
    n = cmd_read_datafifo(&buf32, sampleBytes/sizeof(int32_t));
    n = query_response(sockfd, buf, n, buf, sampleBytes);

    for(i=0; i<sampleBytes-12; i+=12) {
        for(j=0; j<12; j+=3) {
            da = buf[i+j],   db = buf[i+j+1], ds[j]   = ((da<<4) & 0x0ff0) | ((db>>4) & 0x000f);
            da = buf[i+j+1], db = buf[i+j+2], ds[j+1] = ((da<<8) & 0x0f00) | ((db>>0) & 0x00ff);
            printf(" 0x%04x 0x%04x", ds[j], ds[j+1]);
        }
        printf("\n");
    }

    /*
    for(i=0; i<sampleBytes; i++) {
        if(i>0 && i%8==0)
            printf("\n");
        printf(" 0x%02x", (unsigned char)(buf[i]));
    }
    printf("\n");
    */
    return n;

#undef write32breg