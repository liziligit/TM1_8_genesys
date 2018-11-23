
################### for test


#cd ../debug
#rm -f a.jpg
#root -l pd1Draw1D.cpp\(4,4,1\) 
#shotwell a.jpg

#cd ../gui


################### real script



dataSet=0


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

if [ "$3" = "" ]
then
  echo dataSet is emnpty
  exit
fi



addr=$1
cha=$2
dataSet=$3

echo pixel addr= $addr
echo pixel channel= $cha



cd ../debug
rm -f a.jpg

g++ -o pd1Draw1D pd1Draw1D.cpp `root-config --cflags` `root-config --glibs`
./pd1Draw1D ${addr} ${cha} ${dataSet}
eog a.jpg

cd ../gui
