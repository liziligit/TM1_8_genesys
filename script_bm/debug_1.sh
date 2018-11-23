#script_BM=../script_bm
#cd ../daq
#tem=
#runData=../data/runData/beam
#./readDdrContinue.ce -out ${runData}
#cd ${script_BM}
#FileNum=0
#./pd1Topd3.sh 0
#./pd1Debug.sh ${FileNum}
#./pd3Debug.sh ${FileNum} 




script_BM=../script_bm
cd ../daq
tem=
runData=../data/runData/beam
#./readDdrContinue.ce -out ${runData} 
cd ${script_BM}
FileNum=0
#./pd1Topd3.sh 0   
./pd3Debug.sh ${FileNum}
./pd1Debug.sh ${FileNum}
