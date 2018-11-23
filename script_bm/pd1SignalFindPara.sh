###When signal is on, find the maximum value of pixel ###
script_BM=../script_bm
cd ../daq
fileNum=0
runData=../data/runData/beam

if [ -f "${runData}_${fileNum}.pd1" ]
then
rm ${runData}_${fileNum}.pd1
echo delete========== ${runData}_${fileNum}.pd1
fi

#sampleBytes=503316480
sampleBytes=50331648
while [ ! -f "${runData}_${fileNum}.pd1" ]; 
do
	echo "${runData}_${fileNum}.pd1 don't exist!"
#	./readDdrContinue.ce -out ${runData}
	./readPthreadDdrContinue.ce -out ${runData} -sampleBytes ${sampleBytes}
done 

cd ${script_BM}


dataFile=../data/runData/beam_${fileNum}.pd1
pedeFile=../data/pedeData/pede_0
cppPath=../topmetal1X8  
  

upLimitPixel=$1


root -L  -l -q  "${cppPath}/pd1_signalFindPara.cpp++(\"${dataFile}\",\"${pedeFile}\",${upLimitPixel})" 
