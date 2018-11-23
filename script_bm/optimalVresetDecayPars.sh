temp=$1
version=$2
file1=decayData0.txt
file2=polNPar0.txt
file3=polNParRev0.txt
cp  ../data/dataBase/temp${temp}_v${version}/${file1} ../data/tauData/
cp  ../data/dataBase/temp${temp}_v${version}/${file2} ../data/tauData/
cp  ../data/dataBase/temp${temp}_v${version}/${file3} ../data/tauData/

