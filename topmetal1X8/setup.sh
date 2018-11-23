#power on
#take data(loop dac Vreset from 0.76~0.82V,step 0.01V)

read -t 500 -p  "please power on (genesys and Topmetal-II-) and press Enter key to continue  " power
read -t 500 -p  "please download firmware and press Enter key to continue  " firmware
read -t 500 -p  "please Input GuardRing Vpp(mV) and press Enter key to continue  " guardrintVpp
echo guardrintVpp=${guardrintVpp}
# echo ${po}
# dacConfig=$1
# dacConfig=dacConfig.txt
# echo ${dacConfig}
# ./tm2-.ce -reset 1 
############################### GuardRing Test Start ###########################################################
iVresetStart=76
iVresetStep=1
iVresetStop=82
for((i=${iVresetStart};i<=${iVresetStop};i+=${iVresetStep}))
do
	Vreset=$i
	channel0=${Vreset}
	channel1=${Vreset}
	channel2=${Vreset}
	channel3=${Vreset}
	channel4=${Vreset}
	channel5=${Vreset}
	channel6=${Vreset}
	channel7=${Vreset}
	echo channel0 ${channel0}
	echo channel1 ${channel1}
	echo channel2 ${channel2}
	echo channel3 ${channel3}
	echo channel4 ${channel4}
	echo channel5 ${channel5}
	echo channel6 ${channel6}
	echo channel7 ${channel7}
	./dacConfig.sh ${channel0} ${channel1} ${channel2} ${channel3} ${channel4} ${channel5} ${channel6} ${channel7}
	# ./dac.ce
	# ./adc.ce
	# ./trigger.ce
	# ./readDdr.ce -out guardRingVreset${i} -dataType pd1
done
### mark best Vreset voltage to a single file named dacPar.txt(in a line)

############################### GuardRing Test End ###########################################################

############################### dac configure ###########################################################
dacPar=dacPar.txt
i=1
j=0
k=2
declare -a array=(0 0 0 0 0 0 0 0)
for ID in $(cat ${dacPar})
do
        NUM=$i
        S_ID=$( awk 'NR='$NUM' {print $'$i'}' ${dacPar} )
        T_ID=$( awk 'NR='$NUM' {print $'$k'}' ${dacPar} )
        array[$j]=${S_ID}
        echo $i "${array[${j}]}"
        #echo ${S_ID}
        #echo ${T_ID}
        : $(( i++ ))
        : $(( j++ ))
        : $(( k++ ))
done
echo ${array[0]} ${array[1]} ${array[2]} ${array[3]} ${array[4]} ${array[5]} ${array[6]} ${array[7]}

root -L -q -b  "dacSet.cpp++(0,1,2)" 

./dacConfig.sh  ${array[0]} ${array[1]} ${array[2]} ${array[3]} ${array[4]} ${array[5]} ${array[6]} ${array[7]}
############################### dac configure end ###########################################################
#set up topmetal dac adc
#./tm2-.ce -reset 1 
#./dac.ce
#./adc.ce
############################### pede data base ###########################################################

############################### pede data base end ###########################################################

############################### guardRing data base ###########################################################

############################### guardRing data base end ###########################################################