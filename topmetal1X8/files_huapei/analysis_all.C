analysis_pd1calib_getped(char* datafile, char* pedoutfile="NONE", int switch_dopedself=1, int switch_dotauself=0){

  gROOT->ProcessLine(".L pd1Calib.C++");

  // create this pd1Calib instance

  pd1Calib *mycalib = new pd1Calib;

  // set switches of this pd1Calib -- the Init() MUST BE THE LAST OF THEM
  mycalib->SetDataInput(datafile);

  mycalib->SetPedOutFileName(pedoutfile);

  mycalib->SetDoPedSelf(switch_dopedself);

  mycalib->SetDoTauSelf(switch_dotauself);

  mycalib->Init();

  // and do the job

  mycalib->Run();

  mycalib->End();

}
analysis_pd1calib_gettau(char* datafile="../../data/out0_decay.pd1", char* tauoutfile="NONE", char* ped_external_file="NONE", int switch_dopedself=0, int switch_dotauself=1){

  gROOT->ProcessLine(".L pd1Calib.C++");

  // create this pd1Calib instance

  pd1Calib *mycalib = new pd1Calib;

  // set switches of this pd1Calib -- the Init() MUST BE THE LAST OF THEM
  mycalib->SetDataInput(datafile);

  mycalib->SetTauOutFileName(tauoutfile);

  mycalib->SetDoPedSelf(switch_dopedself);
  mycalib->SetPedExternalFileName(ped_external_file);

  mycalib->SetDoTauSelf(switch_dotauself);

  mycalib->Init();

  // and do the job

  mycalib->Run();

  mycalib->End();

}
