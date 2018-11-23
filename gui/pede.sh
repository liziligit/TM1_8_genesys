

if [ "$1" == "" ]
then
  echo pede input is emnpty
  exit
fi

if [ "$2" = "" ]
then
  echo pede output is emnpty
  exit
fi


echo re-generate pede !!!! data $1 out $2 yes/no?
read act

if [ "$act" != "yes" ]
then
  echo pede abort
  exit

fi


  cd ../daq
  ./mytcp -c pede.c inIndex $1 outIndex $2  -noTcp
  echo  generate pede is done

cd ../gui



