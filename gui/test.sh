

cd ../debug
#root -l pd1Pulse.cpp\(0,0,100\) -q
#root -l pd1Charge.cpp\(0,1,0\) -q

cd ../gui


exit


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



cd ../daq
#./runTm2-.sh
#将像素停止在addr上
#./runTm2-Stop.sh $addr    

cd ../daq
#写数据
./mytcp -c debugPd2.c newData 1 checkData 1 writePd2 1 out ../data/out.pd2  addr $addr





cd ../gui

end:
