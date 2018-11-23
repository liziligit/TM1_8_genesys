script_BM=../script_bm
cd ${script_BM}
temp=$1
version=$2
Vreset=$3
./sameVresetConfig.sh ${temp} ${version}  ${Vreset}
./sameVresetDecayPars.sh ${temp}  ${version} ${Vreset}

cd ${script_BM}

