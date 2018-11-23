
dataFolder=$1
fileNum=$2
ich=$3
ipixel=$4
dataFile=../${dataFolder}-data/runData/placData3_beam_${fileNum}.pd3
cppPath=../topmetal1X8

root -L  -l -q  "${cppPath}/pd3_debug.cpp++(\"${dataFile}\",${ich},${ipixel})" 
