////////////////////////////////////////////////////////// parameter

	file_loder myfile;

	myfile.load((char*)"out");
	
	placData_1 pd1;
	
	pd1.createMem(myfile.fz);
	
	pd1.setupFromGenesys1X8(myfile.buf,myfile.fz);
	
	pd1.write((char*)"out.pd1");
	
	pd1.read((char*)"out.pd1");
	
	unsigned short *ps;
	ps=(unsigned short *)pd1.p;
	
	for(int i=pd1.nByte/2-128; i<pd1.nByte/2; i++) {
		
	if(i%8==0)	printf("\n");
	printf(" %04x ", ps[i]);

	}
	
	printf("\n");

	cout<<pd1.dt()<<endl;
	

	
	

	