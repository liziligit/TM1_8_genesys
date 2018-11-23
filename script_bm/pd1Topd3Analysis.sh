script_BM=../script_bm
dataFolder=$1
dataPath=../${dataFolder}/runData
dataName=beam_
cd ${script_BM}
guardRingDataPath=../${dataFolder}/tauData
proPath=../topmetal1X8
polNParChar=polNPar
polNParCharRev=polNParRev
polNData=0
###########
dataNum=$2
##########
pedePath=../${dataFolder}/pedeData
pedeFile=pede_
pedeNum=0
root -L -l -q -b  "${proPath}/getCharge.cpp++(\"${guardRingDataPath}\",\"${polNParChar}\",\"${polNParCharRev}\",${polNData},\"${dataPath}\",\"${dataName}\",\"${pedePath}\",\"${pedeFile}\",${pedeNum},\"pd1\",\"pd3\",${dataNum})" 
