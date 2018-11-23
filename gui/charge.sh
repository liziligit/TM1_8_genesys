


if [ "$1" = "" ]
then
  echo charge source data is emnpty
  exit
fi

if [ "$2" = "" ]
then
  echo charge  output is emnpty
  exit
fi

if [ "$3" = "" ]
then
  echo charge  slope index is emnpty
  exit
fi



cd ../debug
root -l pd1Charge.cpp\($1,$2,$3\) -q


cd ../gui

