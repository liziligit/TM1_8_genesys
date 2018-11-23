script_BM=../script_bm
cd ${script_BM}
temp=$1
version=$2
./optimalVresetGen.sh ${temp} ${version}
./optimalVresetConfig.sh ${temp} ${version}

guardRingDataPath=../data/dataBase/temp${temp}_v${version}
proPath=../topmetal1X8
guardRingDataChar=tau_
polNData=0

if [ -f "${guardRingDataPath}/${guardRingDataChar}${polNData}.pd1" ]
then
rm ${guardRingDataPath}/${guardRingDataChar}${polNData}.pd1
echo delete========== ${guardRingDataPath}/${guardRingDataChar}${polNData}.pd1
fi


cd ../daq


tem=
tauData=../data/dataBase/temp${temp}_v${version}/tau




while [ ! -f "${guardRingDataPath}/${guardRingDataChar}${polNData}.pd1" ]
do
	echo "${guardRingDataPath}/${guardRingDataChar}${polNData}.pd1 don't exist!"
	./readDdrContinue.ce -out ${tauData}
done
cd ${script_BM}


root -L -l -q -b  "${proPath}/getPolNPar.cpp++(\"${guardRingDataPath}\",\"${guardRingDataChar}\",\"\",\"pd1\",${polNData})" 

