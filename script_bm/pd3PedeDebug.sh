
dataFile=../data/runData/placData3_beam_$1.pd3
cppPath=../topmetal1X8

root -L  -l -q  "${cppPath}/pd3_pede.cpp++(\"${dataFile}\")" 
