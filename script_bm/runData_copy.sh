script_BM=../script_bm
cd ../daq
fileStart=$1
fileEnd=$2
sampleBytes=503316480
#sampleBytes=50331648
tem=
runData=../data/runData/beam
set i=0
for((i=${fileStart};i<${fileEnd};i++))
do
      if [ -f "${runData}_${i}.pd1" ]; 
	then 
       rm ${runData}_${i}.pd1
       echo delete==========  ${runData}_${i}.pd1       	 
	fi 
done

echo ./readDdrContinue.ce -out ${runData} -sampleBytes ${sampleBytes} -fileStart ${fileStart} -fileEnd ${fileEnd}
 ./readDdrContinue.ce -out ${runData} -sampleBytes ${sampleBytes} -fileStart ${fileStart} -fileEnd ${fileEnd}
#./readDdrContinue.ce -out ${runData}  -fileStart ${fileStart} -fileEnd ${fileEnd}
cd ${script_BM}

