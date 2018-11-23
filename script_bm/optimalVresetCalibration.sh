script_BM=../script_bm
cd ${script_BM}
temp=$1
version=$2
./optimalVresetConfig.sh ${temp} ${version}
./optimalVresetDecayPars.sh ${temp} ${version}
cd ${script_BM}
