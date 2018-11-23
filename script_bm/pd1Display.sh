fileNum=$1

dataFile=../data/out${fileNum}.pd1
pedeFile=../data/pedeData/pede0.txt
cppPath=../topmetal1X8

maxPixelSignal=$2
ref=$3

root  -L -l  -q "${cppPath}/pd1_display.cpp++(\"${dataFile}\",\"${pedeFile}\",${maxPixelSignal}, ${ref})" 




