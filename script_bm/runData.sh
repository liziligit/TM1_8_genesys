script_BM=../script_bm
cd ../daq
fileStart=$1
fileEnd=$2
sampleBytes=50331648
#sampleBytes=503316480
tem=
runData=../data
set i=0
for((i=${fileStart};i<${fileEnd};i++))
do
      if [ -f "${runData}_${i}.pd1" ]; 
	then 
       rm ${runData}_${i}.pd1
       echo delete==========  ${runData}_${i}.pd1       	 
	fi 
done

echo ./readPthreadDdrContinue.ce -out ${runData} -sampleBytes ${sampleBytes} -fileStart ${fileStart} -fileEnd ${fileEnd}
 ./readPthreadDdrContinue.ce -out ${runData} -sampleBytes ${sampleBytes} -fileStart ${fileStart} -fileEnd ${fileEnd}

#echo ./readPthreadDdrContinue.ce -out ${runData}  -fileStart ${fileStart} -fileEnd ${fileEnd}
 #./readPthreadDdrContinue.ce -out ${runData}  -fileStart ${fileStart} -fileEnd ${fileEnd}

cd ${script_BM}

