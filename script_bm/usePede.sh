script_BM=../script_bm
cd ${script_BM}
fileNum=0
fileNum_cp=$1
cd ../TopMetal2minus/daq1x8
pedeDataChar=pede_
pedeAnalysisPathLocal=.
pedeDataPathLocal=../../data/pedeData
pedeDataBasePathLocal=../../data/pedeData


cd ..
cd ${script_BM}
cd ../daq

tem=
pede=../data/pedeData/pede




cd ../TopMetal2minus/daq1x8
cd ..
cd ${script_BM} 
cd ../daq
cp ${pede}_${fileNum_cp}.pd1  ${pede}_${fileNum}.pd1
cd ../TopMetal2minus/daq1x8




for((i=0;i<${fileNum}+1;i++))
do
	rm ${pedeDataPathLocal}/${pedeDataChar}${i}
	echo delete========== ${pedeDataPathLocal}/${pedeDataChar}${i} 
	rm ${pedeDataPathLocal}/${pedeDataChar}${i}.root
	echo delete==========  ${pedeDataPathLocal}/${pedeDataChar}${i}.root
	
	echo root -L -l -q -b "pedeRun.C(\"${pedeDataPathLocal}\",\"${pedeDataChar}\",${i},\"${pedeAnalysisPathLocal}\",\"${pedeDataBasePathLocal}\")"
	root -L -l -q -b "pedeRun.C(\"${pedeDataPathLocal}\",\"${pedeDataChar}\",${i},\"${pedeAnalysisPathLocal}\",\"${pedeDataBasePathLocal}\")"
done
cd ..
cd ${script_BM}
