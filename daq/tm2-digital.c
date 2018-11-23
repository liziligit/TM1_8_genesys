config_reg(10,0);
pulse_reg(5);

int sampleBytes = 1024;
char *buf=new char[sampleBytes];
askTcpData(sampleBytes);
int  n=read_mem(buf,sampleBytes);

int i,j;

    uint16_t ds[8], da, db;
	
    for(i=0; i<sampleBytes-12; i+=12) {
        for(j=0; j<12; j+=3) {
            da = buf[i+j],   db = buf[i+j+1], ds[j]   = ((da<<4) & 0x0ff0) | ((db>>4) & 0x000f);
            da = buf[i+j+1], db = buf[i+j+2], ds[j+1] = ((da<<8) & 0x0f00) | ((db>>0) & 0x00ff);
            printf(" 0x%04x 0x%04x", ds[j], ds[j+1]);
        }
        printf("\n");
    }	