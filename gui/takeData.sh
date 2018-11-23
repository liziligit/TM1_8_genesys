s=0
e=1




if [ "$2" != "" ]
then
  s=$1
  e=$2
fi




cd ../daq
./mytcp -c realTime.c fileStart $s fileEnd $e

cd ../gui


echo xxxxxxxxxxx take data is done
