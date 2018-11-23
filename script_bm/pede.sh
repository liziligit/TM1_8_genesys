script_BM=../script_bm
cd ${script_BM}
fileNum=0
fileNum_cp=$1
cd ../TopMetal2minus/daq1x8
pedeDataChar=pede_
pedeAnalysisPathLocal=.
pedeDataPathLocal=../../data/pedeData
pedeDataBasePathLocal=../../data/pedeData



for((i=0;i<${fileNum}+1;i++))
do

	
	echo root -L -l -q -b "pedeRun.C(\"${pedeDataPathLocal}\",\"${pedeDataChar}\",${i},\"${pedeAnalysisPathLocal}\",\"${pedeDataBasePathLocal}\")"
	root -L -l -q -b "pedeRun.C(\"${pedeDataPathLocal}\",\"${pedeDataChar}\",${i},\"${pedeAnalysisPathLocal}\",\"${pedeDataBasePathLocal}\")"
done
cd ..
cd ${script_BM}
