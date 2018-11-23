
#dataFile=../2016-3-23-data/runData/placData3_beam_$1.pd3
dataFile=../data/runData/placData3_beam_$1.pd3
cppPath=../topmetal1X8


maxPixelSignal=$2
ref=$3
root -L  -l -q  "${cppPath}/pd3_display.cpp++(\"${dataFile}\",${maxPixelSignal},${ref})" 
