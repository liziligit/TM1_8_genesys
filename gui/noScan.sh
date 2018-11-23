
if [ "$1" = "" ]
then
  echo pixel is emnpty
  exit
fi

if [ "$2" = "" ]
then
  echo channel is emnpty
  exit
fi


addr=$1
cha=$2
echo pixel addr= $addr
echo pixel channel= $cha

# smaple rate is 25MHz/(2^(clockDiv+1))   max is 8 we don't know why
clockDiv=9


cd ../daq
./runTm2-.sh
#将像素停止在addr上
./tm2-NewBoard.ce -stopMode 1 -stopAddr $addr -clockDiv $clockDiv
./mytcp -c adcNewBoard.c
./mytcp -c dacNewBoard.c

cd ../daq
#写数据
./mytcp -c debugPd2.c sampleBytes 16000000 newData 1 checkData 1 writePd2 1 out ../data/out.pd2  addr $addr

cd ../debug
rm -f a.jpg
root -l pd2Draw1D.cpp\($cha,$clockDiv\) 
#shotwell a.jpg



cd ../gui


