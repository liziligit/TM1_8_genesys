


if [ "$1" = "" ]
then
  echo pulse source data is emnpty
  exit
fi

if [ "$2" = "" ]
then
  echo pulse output slope index is emnpty
  exit
fi

if [ "$3" = "" ]
then
  echo pulse  slope threshold is emnpty
  exit
fi



cd ../debug
root -l pd1Pulse.cpp\($1,$2,$3\) -q


cd ../gui

