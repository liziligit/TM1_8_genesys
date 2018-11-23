dataFolder=$1
fileNum=$2
ich=0
#iframe=77
iframe=16
upLimit=100
lowLimit=0


runData=../${dataFolder}/runData/beam_

     if [ ! -f "${runData}${fileNum}.pd1" ]; 
	then 
      echo "${runData}${fileNum}.pd1 don't exist!"
	 continue
	fi

#	./pd1Topd3Analysis.sh ${dataFolder}  ${fileNum}
        ./pd3Analysis.sh  ${dataFolder} ${fileNum} ${ich} ${iframe} ${upLimit} ${lowLimit}




