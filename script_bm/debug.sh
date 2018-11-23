
script_BM=../script_bm
tem=
runData=../data/runData/beam
FileNum=0
cd ${script_BM}
  if [ -f "${runData}_${FileNum}.pd1" ]; 
then 
   rm ${runData}_${FileNum}.pd1
   echo delete==========  ${runData}_${FileNum}.pd1       	 
fi 

cd ../daq


while [ ! -f "${runData}_${FileNum}.pd1" ]
do

	echo "${runData}_${FileNum}.pd1 don't exist!"
	./readDdrContinue.ce -out ${runData}
done
cd ${script_BM}
#./pd1Topd3.sh ${FileNum}
./pd1Debug.sh ${FileNum}
#./pd3Debug.sh ${FileNum} 
