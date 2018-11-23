script_BM=../script_bm
cd ../daq


#temp reset
Dataset=../data
temp=$1
version=$2
reset=
mkdir ../data/dataBase
mkdir ../data/dataBase/temp${temp}_v${version}

iVresetStart=76
iVresetStep=1
iVresetStop=82
for((i=${iVresetStart};i<=${iVresetStop};i+=${iVresetStep}))
do
	#mkdir ../data/dataBase/temp${temp}/reset${i}
	tmpDataPath=../data/dataBase/temp${temp}_v${version}
	./dacConfigNewBoard.sh ${i} ${i} ${i} ${i} ${i} ${i} ${i} ${i}
      if [  -f "${tmpDataPath}/reset${i}.pd1" ]; 
	then 
       rm ${tmpDataPath}/reset${i}.pd1
      echo delete==========  ${tmpDataPath}/reset${i}.pd1      
	fi

	./readDdrContinue.ce -out ${tmpDataPath}/reset${i}
      while [ ! -f "${tmpDataPath}/reset${i}_0.pd1" ]; 
	do
	./readDdrContinue.ce -out ${tmpDataPath}/reset${i}      	 
	done 
	mv ${tmpDataPath}/reset${i}_0.pd1 ${tmpDataPath}/reset${i}.pd1
done


cd ../TopMetal2minus/daq1x8
tauAnalysisPathLocal=.
guardRingDataPathLocal=../../data/dataBase/temp${temp}_v${version}
guardRingDataPath=../data/dataBase/temp${temp}_v${version}
tauGuardRingDataPathLocal=../../data/dataBase/temp${temp}_v${version}
#################################
dacParName=dacPar.txt

for((i=${iVresetStart};i<=${iVresetStop};i+=${iVresetStep}))
do
	guardRingDataChar=reset
	echo root -L -q -b "taufit.C(\"${guardRingDataPathLocal}\",\"${guardRingDataChar}\",${i},\"${tauAnalysisPathLocal}\",\"${tauGuardRingDataPathLocal}\")"	
	root -L -q -b "taufit.C(\"${guardRingDataPathLocal}\",\"${guardRingDataChar}\",${i},\"${tauAnalysisPathLocal}\",\"${tauGuardRingDataPathLocal}\")"
done
cd ../${script_BM}
dacSetPath=../topmetal1X8
root -L -q -b  "${dacSetPath}/dacSet.cpp++(${iVresetStart},${iVresetStep},${iVresetStop},\"${guardRingDataPath}\",\"${guardRingDataChar}\",\"${dacParName}\")" 
#root -L -q -b  "${dacSetPath}/dacSetPar.cpp++(${iVresetStart},${iVresetStep},${iVresetStop},\"${guardRingDataPath}\",\"${guardRingDataChar}\",\"${dacParName}\")" 

#######################################################################
dacPar=dacPar.txt
i=1
j=0
k=2
declare -a array=(0 0 0 0 0 0 0 0)
for ID in $(cat ${dacPar})
do
        NUM=$i
        S_ID=$( awk 'NR='$NUM' {print $'$i'}' ${dacPar} )
        T_ID=$( awk 'NR='$NUM' {print $'$k'}' ${dacPar} )
        array[$j]=${S_ID}
        echo $i "${array[${j}]}"
        #echo ${S_ID}
        #echo ${T_ID}
        : $(( i++ ))
        : $(( j++ ))
        : $(( k++ ))
done
echo ${array[0]} ${array[1]} ${array[2]} ${array[3]} ${array[4]} ${array[5]} ${array[6]} ${array[7]}
# dacPath=topmetal1X8
# root -L -q -b  "${dacPath}/dacSet.cpp++(0,1,2)" 
pwd
cp ${dacParName} ../data/dataBase/temp${temp}_v${version}/
mv ${dacParName} ../daq/
cd ../daq
./dacConfigNewBoard.sh  ${array[0]} ${array[1]} ${array[2]} ${array[3]} ${array[4]} ${array[5]} ${array[6]} ${array[7]}
########################################################################
cd ${script_BM}


