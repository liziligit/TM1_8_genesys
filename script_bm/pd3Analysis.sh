dataFolder=$1
dataNum=$2
dataFile=../${dataFolder}/runData/placData3_beam_${dataNum}.pd3
cppPath=../topmetal1X8


ich=$3
iframe=$4
upLimit=$5
lowLimit=$6
root -L  -l -q  "${cppPath}/pd3_analysis.cpp++(\"${dataFile}\",${ich},${iframe},${upLimit},${lowLimit})" 

