
# data to be analysised
fileNum=$1
ich=$2
ipixel=$3
dataFile=../data/out${fileNum}.pd1
pedeFile=../data/pedeData/pede0.txt
cppPath=../topmetal1X8

root  -L -l  -q "${cppPath}/pd1_debug.cpp++(\"${dataFile}\",\"${pedeFile}\",${ich},${ipixel})" 




