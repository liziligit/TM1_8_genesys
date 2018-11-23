
script_BM=../script_bm
cd ../daq
#./mytcp -c readDdrContinue.c

./mytcp -c debugDdrContinue.c
cd ${script_BM}
