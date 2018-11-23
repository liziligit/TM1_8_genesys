float GetPixPolNQ(int ichip,int ipixel,float baseline=0){
	pd1->getPixDat(ichip,ipixel);
	averagePixDat();
	const int fitPar=polFitParNum+1;
	float *par=new float[fitPar];
	float *parDer=new float[fitPar];
	float *parRev=new float[fitPar];
	float *parRevDer=new float[fitPar];
	float *temp=new float[pd1->nFrame()];//save data averaged
	float *tempIndex=new float[pd1->nFrame()];
	float *charge=new float[pd1->nFrame()];
	for(int i=0;i<fitPar;i++){
		int j=ichip*pd1->nPix()*fitPar+ipixel*fitPar+i;
		par[i]=(*polNParamter)[j];
		parRev[i]=(*polNParamterRev)[j];
		// cout<<"par "<<par[i]<<" parRev"<<parRev[i]<<endl;
	}
	for(int i=0;i<fitPar;i++){
		parDer[i]=float(i)*par[i];// polN deri	
		parRevDer[i]=float(i)*par[i];	
	}
	for(int i=0;i<pd1->nFrame();i++){
		tempIndex[i]=(float)i;
		temp[i]=pd1->pixDat[pd1->samPerPix()*i];
		// if(i%100==0){cout<<"temp "<<temp[i]<<endl;}
	}
	
	getCurve();
	float *x=new float[stop-maxi];
	float *y=new float[stop-maxi];
	
	float *chargeQ=new float[stop-maxi];
	for(int i=maxi;i<stop;i++)
	{
		x[i-maxi]=i-maxi;
		y[i-maxi]=pd1->pixDat[pd1->samPerPix()*i];
	}
	polNFit(stop-maxi,x,y,par,polFitParNum);
	// polNFit(stop-maxi,y,x,parRev,polFitParNum);
	float position=0;
	float valueDecayInExpection=0;
	float totalCharge=0;
	for(int i=0;i<stop-maxi;i++){
		
		if(i==0)chargeQ[i]=y[i]-y[stop-maxi-1];//
		
		// cout<<"position "<<position<<endl;
		if(i>0){
			position=polValue(parRev,y[i-1]);
			valueDecayInExpection=polValue(par,position+1);
			chargeQ[i]=y[i]-valueDecayInExpection;
			// cout<<"y["<<i<<"] "<<y[i]<<" valueDecayInExpection "<<valueDecayInExpection<<endl;
		}
		cout<<" charge is "<<chargeQ[i]<<endl;
		// cout<<"y["<<i<<"] = "<<polValue(par,i)<<" "<<y[i]<<" "<<polValue(par,position+1)<<" position 52 "<<position<<" "<<par[0]<<" "<<polValue(par,position)<<endl;
		totalCharge+=chargeQ[i];
	}
	cout<<"totalCharge "<<totalCharge<<endl;
	delete []par;
	delete []parDer;
	delete []parRev;
	delete []parRevDer;
	delete []temp;
	delete []tempIndex;
	delete []charge;
	delete []x;
	delete []y;
	
	return totalCharge;
}
void splineAllPixCurveDat(char *input,char  *outputVT,char *outputTV){
	splineVT=new signalExtraction();
	splineTV=new signalExtraction();
	std::ifstream fin(input);
	std::ofstream foutVT(outputVT,std::ios_base::out & std::ios_base::trunc);
	std::ofstream foutTV(outputTV,std::ios_base::out & std::ios_base::trunc);
	if(!fin.is_open()) {
        cout << "No curve file found!" << endl;
        return;
    }
	int ichip=0; int ipixel=0; int ipoint=0;
	float data=0; int pixel=0;
	while(!fin.eof()) {//read curve file and do spline fit
        fin >> ichip >> ipixel >>ipoint;
        // cout<< "bad ichip "<<ichip<< "ipixel " << ipixel<<" pointNum " <<ipoint<<endl;
		float *dat;
		float *index;
		int n=ipoint;
		foutVT<<ichip<<" "<<ipixel<<" "<<ipoint<<" "<<endl;
		foutTV<<ichip<<" "<<ipixel<<" "<<ipoint<<" "<<endl;
		if(ipoint>0){
			splineVT->setup(10+n);
			splineTV->setup(10+n);
			dat=new float[n];
			index=new float[n];

			for(int i=0;i<n;i++){dat[i]=0;index[i]=0;}
		}
		for(int i=0;i<ipoint;i++)
		{
			fin>>data;
			// cout<<data<<" ";
			dat[i]=data;
			index[i]=(float)i;
			if(ipoint>0){
				splineVT->x[i]=index[i];// voltage Y and time X
				splineVT->y[i]=dat[i];
				
				splineTV->x[i]=dat[i];//voltage X and time Y
				splineTV->y[i]=index[i];
			}
		}
		// cout<<endl;
		if(ipoint>0){
			splineVT->point_num = n;  
			splineVT->set_end();
			splineVT->spline3();
			
			splineTV->point_num = n;  
			splineTV->set_end();
			splineTV->spline3();
			for(int i=0;i<splineVT->point_num;i++){
				// cout<<splineVT->x[i]<<"  "<<splineVT->si(splineVT->x[i]+0.3)<<endl;
				foutVT<<splineVT->a3[i]<<" "<<splineVT->a1[i]<<" "<<splineVT->b3[i]<<" "<<splineVT->b1[i]<<" ";
				foutTV<<splineTV->a3[i]<<" "<<splineTV->a1[i]<<" "<<splineTV->b3[i]<<" "<<splineTV->b1[i]<<" ";
				
				if(ichip==7&ipixel==5178){
					// cout<<splineTV->x[i]<<"  "<<splineTV->si(splineTV->x[i])<<"\t";
					cout<<splineTV->x[i]<<"  "<<splineTV->y[i]<<" || "<<splineVT->a3[i]<<" "<<splineVT->a1[i]<<" "<<splineVT->b3[i]<<" "<<splineVT->b1[i]<<" "<<endl;
					// cout<<splineTV->x[i]<<"  "<<splineTV->y[i]<<" ";
				}
			}
			foutVT<<endl;
			foutTV<<endl;
		}
		if(ipoint>0){
			delete []dat;
			delete []index;
		}
		pixel++;
		
		if(pixel==41472){break;}
    }	
	cout<<"total "<<pixel<<" pixels"<<endl;
	fin.close();
	foutVT.close();
	foutTV.close();
	// delete []splineVT;
	// delete []splineTV;
}	
void readSplineFile(char  *inputVT,char *inputTV){
	t1=new TTree("t1","Tree contain spline parameter VT");
	t2=new TTree("t2","Tree contain spline parameter TV");
	// vectorSetup();
	vectorClear();
	cout<<"Create TTree and Vector"<<endl;
	t1->Branch("VTa3","vector<float>",&splineParVTa3);
	t1->Branch("VTa1","vector<float>",&splineParVTa1);
	t1->Branch("VTb3","vector<float>",&splineParVTb3);
	t1->Branch("VTb1","vector<float>",&splineParVTb1);
	
	t2->Branch("TVa3","vector<float>",&splineParTVa3);
	t2->Branch("TVa1","vector<float>",&splineParTVa1);
	t2->Branch("TVb3","vector<float>",&splineParTVb3);
	t2->Branch("TVb1","vector<float>",&splineParTVb1);
	
	t1->Branch("chipIndex","vector<float>",&chipIndex);
	t1->Branch("pixelIndex","vector<float>",&pixelIndex);
	t1->Branch("pointNumber","vector<float>",&pointNumber);
	std::ifstream finVT(inputVT);
	std::ifstream finTV(inputTV);
	if(!finVT.is_open()) {
        cout <<inputVT <<" file is not found!" << endl;
        return;
    }
	
	if(!finTV.is_open()) {
        cout <<inputTV <<" file is not found!" << endl;
        return;
    }
	int ichip=0; int ipixel=0; int ipoint=0;
	float a3=0,a1=0,b3=0,b1=0;
	int count=0;
	while(!finVT.eof()){
		vectorClear();
		finVT >> ichip >> ipixel >>ipoint;
		// cout<< "bad ichip "<<ichip<< " ipixel " << ipixel<<" pointNum " <<ipoint<<endl;
		chipIndex->push_back(ichip);
		pixelIndex->push_back(ipixel);
		pointNumber->push_back(ipoint);
		
		for(int i=0;i<ipoint;i++)
		{
			finVT >>a3 >> a1 >> b3 >>b1;
			// if(count==20)cout<<"a3 "<< a3<<endl;
			splineParVTa3->push_back(a3);
			splineParVTa1->push_back(a1);
			splineParVTb3->push_back(b3);
			splineParVTb1->push_back(b1);
		}
		// if(count==0)cout<<"There are "<<count<<" count "<<" ichip "<<ichip<<" "<<ipixel<<endl;
		count++;
		if(count<41473)t1->Fill();
		// if(count==21){break;}
	}
	count=0;
	while(!finTV.eof()){
		vectorClear();
		finTV >> ichip >> ipixel >>ipoint;
		// cout<< "bad ichip "<<ichip<< " ipixel " << ipixel<<" pointNum " <<ipoint<<endl;
		chipIndex->push_back(ichip);
		pixelIndex->push_back(ipixel);
		pointNumber->push_back(ipoint);
		
		for(int i=0;i<ipoint;i++)
		{
			finTV >>a3 >> a1 >> b3 >>b1;
			// if(count==20)cout<<"a3 "<< a3<<endl;
			splineParTVa3->push_back(a3);
			splineParTVa1->push_back(a1);
			splineParTVb3->push_back(b3);
			splineParTVb1->push_back(b1);
		}
		// if(count==0)cout<<"There are "<<count<<" count "<<" ichip "<<ichip<<" "<<ipixel<<endl;
		count++;
		if(count<41473)t2->Fill();
			// if(count==21){break;}
	}
	finVT.close();
	finTV.close();
	cout<<"There are "<<count<<" count "<<endl;
	cout<<"t1 "<<t1->GetEntries()<<endl;
	t1->Show();
	cout<<"t2 "<<t2->GetEntries()<<endl;
	t2->Show();
	
	
	// vector<float> *a;
	// cout<<""<<endl;
	// t1->SetBranchAddress("VTa3",&a);
	// cout<<""<<t1->GetEntries()<<endl;
	// for(int i=0;i<t1->GetEntries();i++)
	// {
		// t1->GetEntry(i);
		// if(i<100)cout<<a->size()<<" ";
	// }
	// a->clear();
	// delete a;
}
		std::vector<float> *splineParVTa3;
		std::vector<float> *splineParVTa1;
		std::vector<float> *splineParVTb3;
		std::vector<float> *splineParVTb1;
		std::vector<float> *splineParTVa3;
		std::vector<float> *splineParTVa1;
		std::vector<float> *splineParTVb3;
		std::vector<float> *splineParTVb1;//
		
			splineParVTa3=new vector<float>;
	splineParVTa1=new vector<float>;
	splineParVTb3=new vector<float>;
	splineParVTb1=new vector<float>;
	splineParTVa3=new vector<float>;
	splineParTVa1=new vector<float>;
	splineParTVb3=new vector<float>;
	splineParTVb1=new vector<float>;
	
		splineParVTa3->clear();
	splineParVTa1->clear();
	splineParVTb3->clear();
	splineParVTb1->clear();
	splineParTVa3->clear();
	splineParTVa1->clear();
	splineParTVb3->clear();
	splineParTVb1->clear();//
	
		delete splineParVTa3;
	delete splineParVTa1;
	delete splineParVTb3;
	delete splineParVTb1;
	delete splineParTVa3;
	delete splineParTVa1;
	delete splineParTVb3;
	delete splineParTVb1;//
	
			TTree *curveDat;//save curve data and index 
		TTree *curveDatFitPar;//save curve data and index 
		TTree *t1;//save data spline Voltage vs Time
		TTree *t2;//save data spline Time vs Voltage
		
			// curveDat=new TTree("curveDat","Tree contain curve dat of guardring");
	// curveDat->Branch("chipIndex","vector<int>",&chipIndex);
	// curveDat->Branch("pixelIndex","vector<int>",&pixelIndex);
	// curveDat->Branch("pointNumber","vector<int>",&pointNumber);	
	// curveDat->Branch("pixelWaveDatIndex","vector<int>",&pixelWaveDatIndex);	
	// curveDat->Branch("pixelWaveDat","vector<float>",&pixelWaveDat);
	
		// cout<<"curveDat->GetEntries() "<<curveDat->GetEntries()<<endl;
	// curveDat->ls();
	void testTree(){
	vector<float> *a=new vector<float>;
	a->clear();
	cout<<""<<endl;
	t1->SetBranchAddress("VTa3",&a);
	cout<<""<<t1->GetEntries()<<endl;
	for(int i=0;i<t1->GetEntries();i++)
	{
		t1->GetEntry(i);
		if(i<100)cout<<a->size()<<" ";
	}
	a->clear();
	delete a;
}

float GetGuardRingPixPolNQ(int ichip,int ipixel,float baseline=0){
	pd1->getPixDat(ichip,ipixel);
	averagePixDat();
	const int fitPar=polFitParNum+1;
	float *par=new float[fitPar];
	float *parDer=new float[fitPar];
	float *parRev=new float[fitPar];
	float *parRevDer=new float[fitPar];
	for(int i=0;i<fitPar;i++){
		int j=ichip*pd1->nPix()*fitPar+ipixel*fitPar+i;
		par[i]=(*polNParamter)[j];
		parRev[i]=(*polNParamterRev)[j];
		// cout<<"par "<<par[i]<<" parRev"<<parRev[i]<<endl;
	}
	for(int i=0;i<fitPar;i++){
		parDer[i]=float(i)*par[i];// polN deri	
		parRevDer[i]=float(i)*par[i];	
	}
	
	getCurve();
	// cout<<"stop "<<stop<<" maxi"<<maxi<<endl;
	float *x=new float[stop-maxi+1];
	float *y=new float[stop-maxi+1];
	
	float *chargeQ=new float[stop-maxi+1];
	for(int i=maxi;i<stop+1;i++)
	{
		x[i-maxi]=i-maxi;
		y[i-maxi]=pd1->pixDat[pd1->samPerPix()*i];
	}
	// polNFit(stop-maxi+1,x,y,par,polFitParNum);
	// polNFit(stop-maxi+1,x,y,par,4);
	// polNFit(stop-maxi+1,y,x,parRev,4);
	polNFit(stop-maxi+1,y,x,parRev,polFitParNum);
	float position=0;
	float valueDecayInExpection=0;
	float totalCharge=0;
	for(int i=0;i<stop-maxi+1;i++){
		
		if(i==0)chargeQ[i]=y[i]-y[stop-maxi+1-1];//
		
		// cout<<"position "<<position<<endl;
		if(i>0){
			position=polValue(parRev,y[i-1]);
			valueDecayInExpection=polValue(par,position+1);
			chargeQ[i]=y[i]-valueDecayInExpection;
			// cout<<"y["<<i<<"] "<<y[i]<<" valueDecayInExpection "<<valueDecayInExpection<<endl;
		}
		totalCharge+=chargeQ[i];

	}
	if(totalCharge<0&ichip==0)
	{
		// char name[100];
		// sprintf(name,"chip%d_pixel%d_dat",ichip,ipixel);
		// draw(stop-maxi+1,x,y,name);
		// sprintf(name,"chip%d_pixel%d_Charge",ichip,ipixel);
		// draw(stop-maxi+1,x,chargeQ,name);
		// cout<<"ichip "<<ichip<<" ipixel "<<ipixel<<" totalCharge "<<totalCharge<<endl;
	}
	// 
	delete []par;
	delete []parDer;
	delete []parRev;
	delete []parRevDer;
	delete []x;
	delete []y;
	
	return totalCharge;
}
void GetPolNQ(char *pedefile,char *chargeOut){
	polNTotalCharge->clear();
	loadPede(pedefile);//load pede to vector<flaot> *pede
	std::ofstream foutCharge(chargeOut,std::ios_base::out & std::ios_base::trunc);
	TFile *fout=new TFile("polQ.root","recreate");
	cout<<"chargeOut "<<chargeOut<<endl;
	float charge=0;
	for(int ichip=0;ichip<pd1->adcCha();ichip++)
	{
		for(int ipixel=0;ipixel<pd1->nPix();ipixel++)
		{
			int pedePos=ichip*pd1->nPix()+ipixel;
			// charge=GetPixPolNQ(ichip,ipixel,(*pedeVector)[pedePos]);
			charge=GetGuardRingPixPolNQ(ichip,ipixel,(*pedeVector)[pedePos],(*pedeRMSVector)[pedePos]);
			// charge=GetGuardRingPixPolNQ(ichip,ipixel,(*pedeVector)[pedePos]);
			// if(ipixel%1000==0)cout<<"ichip "<<ichip<<" ipixel "<<ipixel<<"  pede "<<(*pedeVector)[pedePos]<<endl;
			polNTotalCharge->push_back(charge);
			foutCharge<<ichip<<" "<<ipixel<<" "<<charge<<endl;
		}
	}
	fout->Write();
	fout->Close();
	foutCharge.close();
}







	// pd1_2.draw();
	// TFile *rf=new TFile("out.root","recreate");
	
//fit each pixel data using polN function	
	pd1DecayCurve pdc;
	pdc.setup(&pd1);
	pdc.saveAllPixCurveDat(decayData);
	pdc.readDecayDat(decayData);
	// 
	pdc.fitPolNDat(polPar);
	// cout<<"fitPolNDatRev"<<endl;
	pdc.fitPolNDatRev(polParRev);
	// cout<<"loadPolNPar"<<endl;
	pdc.loadPolNPar(polPar);
	// cout<<"loadPolNParReV"<<endl;
	pdc.loadPolNParReV(polParRev);
	// cout<<"read"<<endl;
	// pdc.GetGuardRingPixPolNQ(0,98,0);//113  single pixel debug
	// return 0;
	float baseline=445;
	float baselineRMS=0;
	// pdc.GetPhyPixPolNQ(0,5180,baseline,baselineRMS);
	// return 0;
	pd1.read(input);
	// cout<<"setup"<<endl;
	pdc.setup(&pd1);
	// pdc.GetGuardRingPolNQ(pedeFile,polNCharge);
	// pdc.drawGuardRing2DCharge(polNCharge);
	pdc.GetPhyPolNQ(pedeFile,polNCharge);
	pdc.savePhyPolNQ(pointChargeFile);
	pdc.drawPhy2DCharge();
// fit each pixel data using polN function	


//
class pd1DecayCurve{
	public:
		placData_1 *pd1;
		float pede;	
		float rms;	
		int maxi;
		int stop;

		signalExtraction *splineVT;//V-Y  T-X
		signalExtraction *splineTV;//T-Y  V-X
		std::vector<int>   	*chipIndex;//
		std::vector<int>   	*pixelIndex;
		std::vector<int>   	*pointNumber;
		std::vector<int>	*pixelPointStart;
		
		std::vector<float> *pedeVector;
		std::vector<float> *pedeRMSVector;
		std::vector<float> *pixelWaveDat;
		std::vector<int> *pixelWaveDatIndex;
		std::vector<float> *polNParamter;
		std::vector<float> *polNParamterRev;
		std::vector<float> *polNPointCharge;
		std::vector<float> *polNTotalCharge;
		int polFitParNum;
		
void setup(placData_1 *_pd1){
	pd1=_pd1;
}	
void vectorSetup(){
	chipIndex    =new vector<int>;
	pixelIndex   =new vector<int>;
	pointNumber  =new vector<int>;
	pixelPointStart  =new vector<int>;

	
	//polN fit dat and paramter
	pedeVector =new vector<float>;
	pedeRMSVector =new vector<float>;
	pixelWaveDat=new vector<float>;
	polNParamter=new vector<float>;
	pixelWaveDatIndex=new vector<int>;
	polNParamterRev=new vector<float>;
	polNPointCharge=new vector<float>;
	polNTotalCharge=new vector<float>;
	polFitParNum=5;
	
}
void vectorClear(){
	chipIndex->clear();
	pixelIndex->clear();
	pointNumber->clear();
	pixelPointStart->clear();

	
	pedeVector->clear();
	pedeRMSVector->clear();
	pixelWaveDat->clear();//
	polNParamter->clear();//
	pixelWaveDatIndex->clear();//
	polNParamterRev->clear();//
	polNPointCharge->clear();//
	polNTotalCharge->clear();//
}
void vectorDelete(){
	delete chipIndex;
	delete pixelIndex;
	delete pointNumber;
	delete pixelPointStart;

	delete pixelWaveDat;
	delete polNParamter;
	delete pixelWaveDatIndex;
	delete polNParamterRev;
	delete pedeVector;
	delete pedeRMSVector;
	delete polNPointCharge;
	delete polNTotalCharge;
}
pd1DecayCurve(){
	vectorSetup();
	vectorClear();
}
void averagePixDat(){
	float s;
	for(int i=0;i<pd1->nFrame();i++){
		s=0;
	for(int j=0;j<pd1->samPerPix();j++){
		s+=pd1->pixDat[pd1->samPerPix()*i+j];
	}
	pd1->pixDat[pd1->samPerPix()*i]=s/float(pd1->samPerPix());
	}	
}

float pv(int i){
	return pd1->pixDat[pd1->samPerPix()*i];
}

void drawPixel(int chip,int pixel){
	TCanvas *c=new TCanvas("c","c",700,600);
	pd1->getPixDat(chip,pixel);
	float *x=new float[pd1->nFrame()];
	float *y=new float[pd1->nFrame()];
	for(int i=0;i<pd1->nFrame();i++){
		x[i]=i;
		y[i]=pd1->pixDat[pd1->samPerPix()*i];
	}
	
	TGraph *a=new TGraph(pd1->nFrame(),x,y);
	a->Draw();
	char *decayData =Form("png/decayDataChip%dPixel%d.png",chip,pixel);
	c->SaveAs(decayData);
	delete []x;
	delete []y;
	delete a;
	delete c;
}	

void getCurve(){
	int peakat=-1;
	for(int i=0;i<pd1->nFrame()-11;i++){
			peakat=i+10;		
		for(int j=0;j<9;j++){
		if(pv(i+10)-pv(i+j)<100){
			peakat=-1;
            break;
		}
		}
		
		if(peakat!=-1) break;

	}
	
	if(peakat==-1) {
		cout<<"NO peak detected"<<endl;
		// exit(0);
		maxi=0;
		stop=0;
		return;
	}
	
	pede=0;
	rms=0;
	for(int j=0;j<9;j++){
		pede+=pv(peakat-10+j);
		rms+=pv(peakat-10+j)*pv(peakat-10+j);
	}
	pede/=9.0;
	rms/=9.0;
	rms=sqrt(rms-pede*pede);


	float max=pv(peakat-2);
	maxi=peakat-2;
	for(int i=-2;i<=2;i++){
		if(pv(peakat+i)>=max){max=pv(peakat+i);maxi=peakat+i;}
	}	


	for(int i=maxi;i<pd1->nFrame()-11;i++){
		if(pv(i)<pede+4*rms) {
			stop=i;
			break;}	
	}
	// if(stop!=0&(stop-maxi<5)){stop=maxi+5;}//
}
void singlePixCurve(int chip=0,int pixel=10){
	pd1->getPixDat(chip,pixel);
	averagePixDat();
	getCurve();
	// if(stop-maxi+1<)
	float *x=new float[stop-maxi+1];
	float *y=new float[stop-maxi+1];
	for(int i=maxi;i<stop+1;i++)
	{
		x[i-maxi]=i;
		y[i-maxi]=pd1->pixDat[pd1->samPerPix()*i];
	}
}
void saveAllPixCurveDat(char *output){
	std::ofstream out(output,std::ios_base::out & std::ios_base::trunc);
	int badPixel=0;
	chipIndex->clear();
	pixelIndex->clear();
	pointNumber->clear();
	pixelPointStart->clear();
	pixelWaveDat->clear();
	pixelWaveDatIndex->clear();
	int ipoint=0;
	int pointStart=0;
	for(int ichip=0;ichip<pd1->adcCha();ichip++)
	{
		for(int ipixel=0;ipixel<pd1->nPix();ipixel++)
		{
			chipIndex->push_back(ichip);
			pixelIndex->push_back(ipixel);

			pd1->getPixDat(ichip,ipixel);
			averagePixDat();
			getCurve();
			ipoint=stop-maxi+1;
			pointNumber->push_back(ipoint);
			pixelPointStart->push_back(pointStart);
			pointStart+=ipoint;
			out<<ichip<<" "<<ipixel<<" "<<stop-maxi+1<<endl;
			if(stop-maxi+1<3)
			{
				badPixel++;
				// drawPixel(ichip,ipixel);
				// cout<<"ichip "<<ichip<<" ipixel "<<ipixel<<endl;
			}
			for(int i=maxi;i<stop+1;i++)
			{
				out<<pd1->pixDat[pd1->samPerPix()*i]<<" ";
				pixelWaveDat->push_back(pd1->pixDat[pd1->samPerPix()*i]);
				pixelWaveDatIndex->push_back(i-maxi);
			}
			out<<endl;
		}
	}
	out.close();
	cout<<"Totally "<<badPixel<<" bad pixel"<<endl;
}

void readDecayDat(char *input){
	std::ifstream fin(input);


	if(!fin.is_open()) {
        cout << "No curve file found!" << endl;
        return;
    }
	chipIndex->clear();
	pixelIndex->clear();
	pointNumber->clear();
	pixelPointStart->clear();
	pixelWaveDat->clear();
	pixelWaveDatIndex->clear();
	int ichip=0; int ipixel=0; int ipoint=0;
	float data=0; int pixel=0;	int pointStart=0;
	while(!fin.eof()) {//read curve file and do spline fit
        fin >> ichip >> ipixel >>ipoint;
		
		chipIndex->push_back(ichip);
		pixelIndex->push_back(ipixel);
		pointNumber->push_back(ipoint);
		pixelPointStart->push_back(pointStart);
       // if(pixel%1000==0) cout<< "bad ichip "<<ichip<< "ipixel " << ipixel<<" pointNum " <<ipoint<<endl;
		float *dat;
		float *index;
		int n=ipoint;
		if(ipoint>0){
			dat=new float[n];
			index=new float[n];
			for(int i=0;i<n;i++){dat[i]=0;index[i]=0;}
		}
		for(int i=0;i<ipoint;i++)
		{
			fin>>data;
			// cout<<data<<" ";
			dat[i]=data;
			index[i]=(float)i;
			// vectorClear();
			pixelWaveDat->push_back(dat[i]);
			pixelWaveDatIndex->push_back(index[i]);
			
		}
		if(ipoint>0){
			// draw(ipoint,index,dat);
			// float *par=new float[5];
			// polNFit(ipoint,index,dat,par,5);
			// return;
			delete []dat;
			delete []index;
		}
		// curveDat->Fill();
		pixel++;
		pointStart+=ipoint;
		if(pixel==41472){break;}
    }	
	cout<<"total "<<pixel<<" pixels"<<endl;

	fin.close();
}
void fitPolNDat(char *outParName){
	cout<<"chipIndex "<<chipIndex->size()<<endl;
	cout<<"pixelIndex "<<pixelIndex->size()<<endl;
	cout<<"pointNumber "<<pointNumber->size()<<endl;
	cout<<"pixelWaveDat "<<pixelWaveDat->size()<<endl;
	cout<<"pixelWaveDatIndex "<<pixelWaveDatIndex->size()<<endl;
	std::ofstream foutPar(outParName,std::ios_base::out & std::ios_base::trunc);
	const int polFitParN=polFitParNum;//pol5 p0~p5
	float *par=new float[polFitParN];
	int datRange=0;
	for(int i=0;i<(int)chipIndex->size();i++)
	{
		int temp=(*pointNumber)[i];
		if(temp>0){
			float *dat=new float[temp];
			float *index=new float[temp];
			datRange+=temp;
			for(int j=0;j<temp;j++){
				index[j]=(float)(*pixelWaveDatIndex)[datRange-temp+j];
				dat[j]=(float)(*pixelWaveDat)[datRange-temp+j];
			}
			polNFit(temp,index,dat,par,polFitParN);
			for(int k=0;k<polFitParN+1;k++)
			{
				polNParamter->push_back(par[k]);
				foutPar<<par[k]<<" ";
			}
			foutPar<<endl;
			delete []dat;
			delete []index;
		}
		if(temp<=0){
			// cout<<"temp<=0 "<<temp<<endl;
			for(int k=0;k<polFitParN+1;k++)
			{
				polNParamter->push_back(0);
				foutPar<<0<<" ";
			}
			foutPar<<endl;
		}
	}
	foutPar.close();
	delete []par;
}
void fitPolNDatRev(char *outParNameRev){
	cout<<""<<chipIndex->size()<<endl;
	cout<<""<<pixelIndex->size()<<endl;
	cout<<""<<pointNumber->size()<<endl;
	cout<<""<<pixelWaveDat->size()<<endl;
	cout<<""<<pixelWaveDatIndex->size()<<endl;
	std::ofstream foutParRev(outParNameRev,std::ios_base::out & std::ios_base::trunc);
	const int polFitParN=polFitParNum;
	float *par=new float[polFitParN+1];
	int datRange=0;
	for(int i=0;i<(int)chipIndex->size();i++)
	{
		int temp=(*pointNumber)[i];
		// cout<<"temp "<<temp<<endl;
		if(temp>0){
			// cout<<"temp>0 "<<temp<<endl;
			float *dat=new float[temp];
			float *index=new float[temp];
			datRange+=temp;
			for(int j=0;j<temp;j++){
				dat[j]=(float)(*pixelWaveDatIndex)[datRange-temp+j];
				index[j]=(float)(*pixelWaveDat)[datRange-temp+j];
			}
			polNFit(temp,index,dat,par,polFitParN);
			for(int k=0;k<polFitParN+1;k++)
			{
				polNParamterRev->push_back(par[k]);
				foutParRev<<par[k]<<" ";
			}
			foutParRev<<endl;
			delete []dat;
			delete []index;
		}
		if(temp<=0){
			// cout<<"temp<=0 "<<temp<<endl;
			for(int k=0;k<polFitParN+1;k++)
			{
				polNParamterRev->push_back(0);
				foutParRev<<0<<" ";
			}
			foutParRev<<endl;
		}
	}
	foutParRev.close();
	delete []par;
}
void loadPolNPar(char *outParName){
	std::ifstream polPar(outParName);
	polNParamter->clear();
	float *par=new float[polFitParNum+1];
	int count=0;
	while(!polPar.eof()){
		for(int i=0;i<polFitParNum+1;i++){
			polPar >> par[i];
			polNParamter->push_back(par[i]);
		}
		count++;
		if(count==41472){break;}
	}
	cout<<"count "<<count<<endl;
	polPar.close();
	delete []par;
}

void loadPolNParReV(char *outParNameRev){
	std::ifstream polParRev(outParNameRev);
	polNParamterRev->clear();
	float *par=new float[polFitParNum+1]; //polN has N+1 pars
	int count=0;
	while(!polParRev.eof()){
		for(int i=0;i<polFitParNum+1;i++){
			polParRev >> par[i];
			polNParamterRev->push_back(par[i]);
		}
		count++;
		if(count==41472){break;}
	}
	polParRev.close();
	cout<<"count "<<count<<endl;
	delete []par;
	
}
void loadPede(char *pedefile){
	std::ifstream pedeOpen(pedefile);
	pedeVector->clear();
	pedeRMSVector->clear();
	int ichip=0;
	int ipixel=0;
	float pedeValue=0;
	float pedeRMS=0;
	int count=0;
	while(!pedeOpen.eof()){
		pedeOpen >> ichip >> ipixel >> pedeValue >>pedeRMS;
		pedeVector->push_back(pedeValue);
		pedeRMSVector->push_back(pedeRMS);
		count++;
		if(count==41472){break;}
	}
	pedeOpen.close();
}
float GetGuardRingPixPolNQ(int ichip,int ipixel,float baseline=0,bool debug=false){
	if(debug==false){
		polNPointCharge->clear();
	}
	const int fitPar=polFitParNum+1;
	float *par=new float[fitPar];
	float *parDer=new float[fitPar];
	float *parRev=new float[fitPar];
	float *parRevDer=new float[fitPar];
	for(int i=0;i<fitPar;i++){
		int j=ichip*pd1->nPix()*fitPar+ipixel*fitPar+i;
		par[i]=(*polNParamter)[j];
		parRev[i]=(*polNParamterRev)[j];
		// cout<<"par "<<par[i]<<" parRev"<<parRev[i]<<endl;
	}
	for(int i=0;i<fitPar;i++){
		parDer[i]=float(i)*par[i];// polN deri	
		parRevDer[i]=float(i)*par[i];	
	}
	int pointStart=0;
	int datRange=0;
	int pointN=(*pointNumber)[ichip*pd1->nPix()+ipixel];
	pointStart=(*pixelPointStart)[ichip*pd1->nPix()+ipixel];
	float *x=new float[pointN];
	float *y=new float[pointN];
	if(pointN>0){
		for(int j=0;j<pointN;j++){
			x[j]=(float)(*pixelWaveDatIndex)[pointStart+j];
			y[j]=(float)(*pixelWaveDat)[pointStart+j];
		}
	}
	
	float *chargeQ=new float[pointN];
	polNFit(pointN,x,y,par,polFitParNum);
	// polNFit(stop-maxi+1,x,y,par,4);
	// polNFit(stop-maxi+1,y,x,parRev,4);
	// polNFit(stop-maxi+1,y,x,parRev,polFitParNum);
	float position=0;
	float valueDecayInExpection=0;
	float totalCharge=0;
	for(int i=0;i<pointN;i++){
		
		if(i==0)chargeQ[i]=y[i]-y[pointN-1];//
		// if(i==0)chargeQ[i]=y[i]-baseline;//
		
		// cout<<"position "<<position<<endl;
		if(i>0){
			position=polValue(parRev,y[i-1]);
			valueDecayInExpection=polValue(par,position+1);
			chargeQ[i]=y[i]-valueDecayInExpection;
			// cout<<"y["<<i<<"] "<<y[i]<<" valueDecayInExpection "<<valueDecayInExpection<<endl;
		}
		totalCharge+=chargeQ[i];
	if(debug==false){
		polNPointCharge->push_back(chargeQ[i]);
	}
		
	}
	// cout<<"total charge is "<<totalCharge<<endl;
	if(totalCharge<0&ichip==0)
	{
		// char name[100];
		// sprintf(name,"chip%d_pixel%d_dat",ichip,ipixel);
		// draw(stop-maxi+1,x,y,name);
		// sprintf(name,"chip%d_pixel%d_Charge",ichip,ipixel);
		// draw(stop-maxi+1,x,chargeQ,name);
		// cout<<"ichip "<<ichip<<" ipixel "<<ipixel<<" totalCharge "<<totalCharge<<endl;
	}
	// 
	delete []par;
	delete []parDer;
	delete []parRev;
	delete []parRevDer;
	delete []x;
	delete []y;
	
	return totalCharge;
}

void GetGuardRingPolNQ(char *pedefile,char *chargeOut){
	polNTotalCharge->clear();
	loadPede(pedefile);//load pede to vector<flaot> *pede
	std::ofstream foutCharge(chargeOut,std::ios_base::out & std::ios_base::trunc);
	TFile *fout=new TFile("polQ.root","recreate");
	cout<<"chargeOut "<<chargeOut<<endl;
	float charge=0;
	for(int ichip=0;ichip<pd1->adcCha();ichip++)
	{
		for(int ipixel=0;ipixel<pd1->nPix();ipixel++)
		{
			int pedePos=ichip*pd1->nFrame()+ipixel;
			charge=GetGuardRingPixPolNQ(ichip,ipixel,0);
			// if(ipixel%1000==0)cout<<"ichip "<<ichip<<" ipixel "<<ipixel<<"  pede "<<(*pedeVector)[pedePos]<<endl;
			polNTotalCharge->push_back(charge);
			foutCharge<<ichip<<" "<<ipixel<<" "<<charge<<endl;
		}
	}
	fout->Write();
	fout->Close();
	foutCharge.close();
}

float GetPhyPixPolNQ(int ichip,int ipixel,float baseline=0, float baselineRMS=0,bool vectorClear=false){
	if(vectorClear==true){
		polNPointCharge->clear();
	}
	pd1->getPixDat(ichip,ipixel);
	averagePixDat();
	const int fitPar=polFitParNum+1;
	float *par=new float[fitPar];
	float *parDer=new float[fitPar];
	float *parRev=new float[fitPar];
	float *parRevDer=new float[fitPar];
	float *y=new float[pd1->nFrame()];//save data averaged
	float *tempIndex=new float[pd1->nFrame()];
	float *chargeQ=new float[pd1->nFrame()];
	for(int i=0;i<fitPar;i++){
		int j=ichip*pd1->nPix()*fitPar+ipixel*fitPar+i;
		par[i]=(*polNParamter)[j];
		parRev[i]=(*polNParamterRev)[j];
		// cout<<"par "<<par[i]<<" parRev"<<parRev[i]<<endl;
	}
	for(int i=0;i<fitPar;i++){
		parDer[i]=float(i)*par[i];// polN deri	
		parRevDer[i]=float(i)*par[i];	
	}
	for(int i=0;i<pd1->nFrame();i++){
		tempIndex[i]=(float)i;
		y[i]=pd1->pixDat[pd1->samPerPix()*i];
		// if(i%100==0){cout<<"y "<<y[i]<<endl;}
	}
	// polNFit(pd1->nFrame(),tempIndex,y,par,polFitParNum);
	// polNFit(stop-maxi+1,y,x,parRev,polFitParNum);
	float position=0;
	float valueDecayInExpection=0;
	float totalCharge=0;
	for(int i=0;i<pd1->nFrame();i++){
		
		// if(i==0)chargeQ[i]=y[i]-y[stop-maxi+1-1];//
		if(i==0){
			chargeQ[i]=y[i]-baseline;//
			// chargeQ[i]=y[i]-y[pointN-1];//
		}
		// cout<<"position "<<position<<endl;
		if(i>0){
			if(y[i-1]-baseline<3*baselineRMS){
				valueDecayInExpection=baseline;
			}
			else{
				position=polValue(parRev,y[i-1]);
				valueDecayInExpection=polValue(par,position+1);
			}
			chargeQ[i]=y[i]-valueDecayInExpection;
		}
		// cout<<" charge is "<<chargeQ[i]<<endl;
		totalCharge+=chargeQ[i];
	}
	//save charge to vector polNPointCharge->push_back()
	if(vectorClear==false){
		for(int i=0;i<pd1->nFrame();i++){
			polNPointCharge->push_back(chargeQ[i]);
		}
	}
	
	draw(pd1->nFrame(),tempIndex,chargeQ,"test");
	// draw(pd1->nFrame(),tempIndex,y,"test");
	// cout<<"totalCharge "<<totalCharge<<endl;
	delete []par;
	delete []parDer;
	delete []parRev;
	delete []parRevDer;
	delete []y;
	delete []tempIndex;
	
	return totalCharge;
}
void GetPhyPolNQ(char *pedefile,char *chargeOut){
	polNTotalCharge->clear();
	polNPointCharge->clear();
	loadPede(pedefile);//load pede to vector<flaot> *pede
	std::ofstream foutCharge(chargeOut,std::ios_base::out & std::ios_base::trunc);

	cout<<"chargeOut "<<chargeOut<<endl;
	float charge=0;
	for(int ichip=0;ichip<pd1->adcCha();ichip++)
	{
		for(int ipixel=0;ipixel<pd1->nPix();ipixel++)
		{
			int pedePos=ichip*pd1->nPix()+ipixel;
			// charge=GetPhyPixPolNQ(ichip,ipixel,445,0);
			charge=GetPhyPixPolNQ(ichip,ipixel,(*pedeVector)[pedePos],(*pedeRMSVector)[pedePos]);
			if(ipixel%1000==0)cout<<"ichip "<<ichip<<" ipixel "<<ipixel<<"  pede "<<(*pedeVector)[pedePos]<<endl;
			polNTotalCharge->push_back(charge);
			foutCharge<<ichip<<" "<<ipixel<<" "<<charge<<endl;
		}
	}
	foutCharge.close();

}
void savePhyPolNQ(char *pointOut){
	std::ofstream foutCharge(pointOut,std::ios_base::out & std::ios_base::trunc);
	TFile *f=new TFile("pointCharge.root","recreate");
	float *x=new float[pd1->nFrame()];
	float *y=new float[pd1->nFrame()];
	char name[100];
	int size=polNPointCharge->size();
	// for(int ichip=0;ichip<pd1->adcCha();ichip++){
	for(int ichip=0;ichip<1;ichip++){
		for(int ipixel=0;ipixel<pd1->nPix();ipixel++){
			foutCharge<< ichip <<" " << ipixel<<" ";
			for(int frame=0;frame<pd1->nFrame();frame++){
				int position=ichip*pd1->nPix()*pd1->nFrame()+ipixel*pd1->nFrame()+frame;
				foutCharge << (*polNPointCharge)[position]<<" ";
				x[frame]=frame;
				y[frame]=(*polNPointCharge)[position];
			}
			foutCharge<<endl;
			sprintf(name,"ichip%d_ipixel%d",ichip,ipixel);
			draw(pd1->nFrame(),x,y,name,true);
		}
	}
	foutCharge.close();
	f->Write();
	f->Close();
	delete []x;
	delete []y;
}



float polValue(float *par,float x){
	float value=0;
	for(int i=0;i<polFitParNum+1;i++){
		value+=par[i]*TMath::Power(x,i);
	}
	return value;
}
void print(vector<float> *f){
	for(unsigned int i=0;i<f->size();i++)
	{
		cout<<(*f)[i]<<endl;
	}
}

void polNFit(int n,float *x,float *y,float *par,int nPar=5){//save fit paramters in par
	// TCanvas c("c","c",700,600);
	TGraph *gr=new TGraph(n,x,y);
	// gr->SetMarkerStyle(20);
	// gr->SetMarkerColor(1);
	gr->Draw("apl*");
	char func[100];
	float start=x[0];
	float stop=x[n-1];
	sprintf(func,"pol%d",nPar);
	TF1 fit("fit",func,start,stop);
	// f->SetParameters(10,1,1);
	gr->Fit(&fit,"QMR");
	for(int i=0;i<nPar+1;i++){
		par[i]=fit.GetParameter(i);
		// cout<<"par["<<i<<"] "<< par[i]<<endl;
	}
	// char name[100];
	// sprintf(name,".png");
	// c.SaveAs();
	// delete gr;
}

void draw(int n,float *x,float *y,char *nameIn,bool writeif=false){
	// TCanvas *c=new TCanvas("c","c",700,600);
	TGraph *gr=new TGraph(n,x,y);
	gr->Draw("apl*");
	gr->SetName(nameIn);
	if(writeif==true)gr->Write();
	// char name[100];
	// sprintf(name,"%s.png",nameIn);
	// c->SaveAs(name);
}
void drawGuardRing2DCharge(char *chargeIn){
	std::ifstream chargeOpen(chargeIn);
	cout<<"input "<<chargeIn<<endl;
	TFile *f=new TFile("charge.root","recreate");
	int ichip=0;
	int ipixel=0;
	float charge=0; 
	TCanvas *c=new TCanvas("c","c",4000,800);
	c->Divide(8,2);
	TString histName;
	TString histName1;
	char title[100];
	char title1[100];
	TH2D *h[pd1->adcCha()];
	TH1D *h1[pd1->adcCha()];
	for(int i=0;i<pd1->adcCha();i++){
        histName = "Channel"; histName += i;
        histName1 = "1d_Channel"; histName1 += i;
        sprintf(title,"Channel %d Charge Distribution",i);
        sprintf(title1,"Channel %d Charge Distribution 1d",i);
        h[i] = new TH2D(histName, title, 72, 0, 72, 72, 0, 72);
        h1[i] = new TH1D(histName1, title1, 2500,0,2000);
	}
	int count=0;
	int chargeLessZero=0;
	int chargeLarge=0;
	while(!chargeOpen.eof()){
		chargeOpen >> ichip >> ipixel >>charge;
		// cout<<"ichip "<<ichip<<endl;
		h[ichip]->SetBinContent(ipixel/72+1,72-ipixel%72,charge);
		h1[ichip]->Fill(charge);
		if(charge<0){
			chargeLessZero++;
			if(chargeLessZero<100){
				cout<<"ichip "<<ichip<<" ipixel "<<ipixel<<" charge "<<charge<<endl;
			}
		}
		if(charge>1000){
			chargeLarge++;
			if(chargeLarge<100){
				cout<<"ichip "<<ichip<<" ipixel "<<ipixel<<" charge "<<charge<<endl;
			}
		}
		count++;
		if(count==41472){break;}
	}
	cout<<"read data ready "<<endl;
	cout<<"large number is "<<chargeLarge<<endl;
	cout<<"charge less zero "<<chargeLessZero<<endl;
	for(int i=0;i<pd1->adcCha();i++){
		c->cd(i+1);
		h[i]->Draw("colz");
		h[i]->GetZaxis()->SetRangeUser(0,999);
        h[i]->Draw("colz");
		h[i]->SetStats(0);
		
		c->cd(i+pd1->adcCha()+1);
		h1[i]->Draw();
		
		c->Modified();
        c->Update();
		// h[i]->Write();
		// h1[i]->Write();
	}
	// c->Modified();
	// c->Update();
	c->SaveAs("charge.pdf");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	c->SaveAs("charge.png");
	chargeOpen.close();
	c->Write();
	f->Write();
	f->Close();
}

void drawPhy2DCharge(){
	TFile *f=new TFile("charge.root","recreate");
	// int ichip=0;
	// int ipixel=0;
	float charge=0; 
	TCanvas *c=new TCanvas("c","c",4000,800);
	c->Divide(8,2);
	TString histName;
	TString histName1;
	char title[100];
	char title1[100];
	TH2D *h[pd1->adcCha()];
	TH1D *h1[pd1->adcCha()];
	for(int i=0;i<pd1->adcCha();i++){
        histName = "Channel"; histName += i;
        histName1 = "1d_Channel"; histName1 += i;
        sprintf(title,"Channel %d Charge Distribution",i);
        sprintf(title1,"Channel %d Charge Distribution 1d",i);
        h[i] = new TH2D(histName, title, 72, 0, 72, 72, 0, 72);
        h1[i] = new TH1D(histName1, title1, 2500,-200,1000);
	}
	int count=0;
	int chargeLessZero=0;
	int chargeLarge=0;
	int size=polNPointCharge->size();
	// float *x=new float[pd1->nFrame()];
	// float *y=new float[pd1->nFrame()];
	// for(int i=0;i<pd1->nFrame();i++){
		// x[i]=(float)i;
		// y[i]=(*polNPointCharge)[pd1->nFrame()*10+i];
	// }
	// draw(pd1->nFrame(),x,y,"test");
	// return ;
	char canvasName[100];
	for(int frame=0;frame<pd1->nFrame();frame++){
		for(int ichip=0;ichip<pd1->adcCha();ichip++){
			h[ichip]->Clear();
			h1[ichip]->Clear();
			h1[ichip]->Reset();
			for(int ipixel=0;ipixel<pd1->nPix();ipixel++){
				int position=ichip*pd1->nPix()*pd1->nFrame()+ipixel*pd1->nFrame()+frame;
				// if(frame<800)
				{
					h[ichip]->SetBinContent(ipixel/72+1,72-ipixel%72,(*polNPointCharge)[position]);
					h1[ichip]->Fill((*polNPointCharge)[position]);
				}
			}
		}
		// if(frame<800)
		{
			sprintf(canvasName,"canvas%d",frame);
			for(int i=0;i<pd1->adcCha();i++){
				c->cd(i+1);
				h[i]->Draw("colz");
				h[i]->GetZaxis()->SetRangeUser(-100,999);
				h[i]->Draw("colz");
				h[i]->SetStats(0);

				c->cd(i+pd1->adcCha()+1);
				h1[i]->Draw();

				c->Modified();

			}
			c->Update();
			c->Write();
		}
	}
	for(int i=0;i<pd1->adcCha();i++){

	}
	
	f->Write();
	f->Close();
}

~pd1DecayCurve()
{
	vectorClear();
	vectorDelete();
}
};


