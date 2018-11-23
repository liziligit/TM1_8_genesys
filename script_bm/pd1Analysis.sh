

#dataFile=../2016-3-23-data/runData/beam_$1.pd1
#pedeFile=../2016-3-23-data/pedeData/pede_0
dataFile=../2016-3-25-data/runData/beam_$1.pd1
pedeFile=../2016-3-25-data/pedeData/pede_0
cppPath=../topmetal1X8

ich=$2
iframe=$3

root  -L -l  -q "${cppPath}/pd1_analysis.cpp++(\"${dataFile}\",\"${pedeFile}\",${ich}, ${iframe})" 




