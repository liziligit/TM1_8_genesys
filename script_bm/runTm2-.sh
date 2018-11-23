script_BM=../script_bm
cd ../daq
./mytcp -c tm2-NewBoard.c
./mytcp -c adcNewBoard.c
./mytcp -c dacNewBoard.c
#./mytcp -c readDdrContinue.c
#./mytcp -c debugDdrContinue.c
cd ${script_BM}
