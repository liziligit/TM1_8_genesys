
  pedeSet=0
  dataSet=0


if [ "$1" = "" ]
then
  echo pedeSet is emnpty
  #exit
fi

if [ "$2" = "" ]
then
  echo dataSet is emnpty
  #exit
fi

#this if is not necessary 
if [ "$2" != "" ]
then
  pedeSet=$1
  dataSet=$2
fi


echo pedeSet = $pedeSet  dataSet= $dataSet


cd ../debug
g++ -o pd1Draw2D pd1Draw2D.cpp `root-config --cflags` `root-config --glibs`
./pd1Draw2D ${pedeSet} ${dataSet}
cd ../gui
