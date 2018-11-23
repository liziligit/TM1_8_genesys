fileStart=$1
fileEnd=$2


maxPixelSignal=15
ref=-1000000

set i=0
runData=../data/runData/beam_
for((i=${fileStart};i<${fileEnd};i++))
do
      if [ ! -f "${runData}${i}.pd1" ]; 
	then 
       echo "${runData}${i}.pd1 don't exist!"
	 continue
	fi
	./pd1Display.sh ${i} ${maxPixelSignal} ${ref}
#	./pd1Topd3.sh ${i}
#     ./pd3Display.sh ${i} ${maxPixelSignal} ${ref}
done


