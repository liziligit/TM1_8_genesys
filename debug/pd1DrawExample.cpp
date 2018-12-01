#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TROOT.h>
#include <TMath.h>
#include <TParameter.h>
#include <TGraph.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TSystem.h>
#include "rootlib.h"
// #include <io.h>

int pd1DrawExample(){

pd1Pede pede;
pede.setup("../data/pedeData/pede0.txt");

placData_1 pd1;
pd1.read((char*)"../data/out0.pd1");

pd1.print();



pd1.getFrame(3,0);  // 输入帧数和adc通道数，不加adc通道数这个参数，会获取所有通道的数据
pede.subPede(pd1.frameDat,0);  //subtract pede 不加第二个参数，会操作所有通道的数据

plot pt;
pt.setupCanvas(2,2); //设置canvas上图片的行数，列数
pt.setup2D(1,1,72,72,"hist2d"); //Tcanvas中的图的行数，列数，阵列的行数，列数，图的名字
pt.draw2D(pd1.frameDat,72,72);  //要画图的数据，阵列的行数，列数


pt.setup1D(1,1,100,-10,10,"hist1d");
pt.load2DHistTo1DHist(); //将二维图投影到一维图中
pt.drawOffset=1;
pt.draw1D();


pt.setup1D(1,1,pd1.nFrameDat,0,pd1.nFrameDat,"loadData1d");
pt.loadData2Hist1D(pd1.frameDat,0,pd1.nFrameDat); //将一维数组 “排列” 的柱状图上
pt.drawOffset=2;
pt.draw1D();

pt.setup1D(1,1,100,-10,10,"hist1d");
pt.fillData2Hist1D(pd1.frameDat,0,pd1.nFrameDat); //将一维ie数组 “投点” 到柱状图上
pt.drawOffset=3;
pt.draw1D();

cout<<"end"<<endl;

	return 0;
}
