
 paralist2var pv;
	
	int i=0;
	int total=0;

		
	ifstream in;
	char lineBuf[5000];
		vector<char*> vb;
		vector <string> v[3];

		string fn="dacConfigNewBoard.txt";
	in.open(fn.data());
	if(!in.good()){
	cout<<fn.data()<<" is NOT opened "<<endl;
    exit(0);	
	}
	// cout<<fn.data()<<" is opened "<<endl;

	do{
	in.getline(lineBuf,5000);

	//cout<<total<<"   "<<lineBuf<<endl;

	vb.clear();
strsplit(lineBuf, vb);
if(vb.size()<2){ 
if(!in.good()) break;	
	continue;
}






if(strcmp(vb[0],"pulse_reg")==0){

	v[0].push_back(vb[0]);
	v[1].push_back(vb[1]);
	v[2].push_back(" ");

	total++;
}
if(strcmp(vb[0],"delay_ms")==0){

	v[0].push_back(vb[0]);
	v[1].push_back(vb[1]);
	v[2].push_back(" ");

	total++;
}



if(strcmp(vb[0],"reg")==0){

	v[0].push_back(vb[0]);
	v[1].push_back(vb[1]);
	v[2].push_back(vb[2]);

	total++;
}

if(strcmp(vb[0],"config_reg")==0){

	v[0].push_back(vb[0]);
	v[1].push_back(vb[1]);
	v[2].push_back(vb[2]);

	total++;
}

if(strcmp(vb[0],"config_dac")==0){

	v[0].push_back(vb[0]);
	v[1].push_back(vb[1]);
	v[2].push_back(vb[2]);

	total++;
}
if(strcmp(vb[0],"config_dac_B")==0){

	v[0].push_back(vb[0]);
	v[1].push_back(vb[1]);
	v[2].push_back(vb[2]);

	total++;
}
		if(!in.good()) break;



	}while(!in.eof());
	in.close();
	
	//cout<<argv[pid+1]<<" is closed "<<endl;



//
//	total*=4;
//	if(debug>0)
// cout<<"lines="<<total<<endl;



char c1[100];
char c2[100];
char c3[100];


//	 if(debug>0)
//	 cout<<"total="<<total<<endl;
//

	for(i=0;i<total;i++){

	sprintf(c1,"%s",v[0][i].data());
	sprintf(c2,"%s",v[1][i].data());
	sprintf(c3,"%s",v[2][i].data());
	

	if(strcmp(c1,"delay_ms")==0){
	Sleep(atoi(c2));
	continue;
	}
	if(strcmp(c1,"pulse_reg")==0){
		pv.c3=c2;
		pv.transform();
		pulse_reg(pv.value);
	continue;
	}
	if(strcmp(c1,"reg")==0){
		pv.c3=c3;
		pv.transform();
		unsigned int v=(atoi(c2)<<16)|pv.value;
		mytcp.sendWord(v);
	continue;
	}
	if(strcmp(c1,"config_reg")==0){
	
		pv.c3=c3;
		pv.transform();
		int addr=atoi(c2);
		unsigned int val=pv.value;
		 config_reg(addr,val);

	continue;
	}
	
	if(strcmp(c1,"config_dac")==0){
	
		double volt=atof(c3);

		int addr=atoi(c2);
		
		
		
		unsigned int val=65535*volt/5.0;
		
		if(val>65535) val=65535;
		
		 config_reg(addr,val);

	continue;
	}
	if(strcmp(c1,"config_dac_B")==0){
	
		double volt=atof(c3);

		int addr=atoi(c2);
		
		
		
		unsigned int val=65535*volt/2.5;
		
		if(val>65535) val=65535;
		
		 config_reg(addr,val);

	continue;
	}

	}







	
	

	
