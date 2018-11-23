script_BM=../script_bm
cd ../daq


#temp reset
Dataset=../data
temp=$1
version=$2
reset=
mkdir ../data/dataBase
mkdir ../data/dataBase/temp${temp}_v${version}
