output=dacConfig.txt
echo ${i} ${n}
channel0=$1
channel1=$2
channel2=$3
channel3=$4
channel4=$5
channel5=$6
channel6=$7
channel7=$8
echo "config_dac(0,0.${channel0});// channel 8  csa Vreset ">${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,0.${channel1});// channel 6  csa Vreset ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,0.62);// channel 8   csa ref ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,0.62); //channel 6  csa ref ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,0.${channel2});// channel 7  csa Vreset  ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,0.${channel3}); //channel 5  csa Vreset  ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,0.62);//channel 7 csa ref 0.62 ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,0.62); //channel 5 csa ref 0.62 ">>${output}
echo "pulse_reg(4);" >>${output}
echo  >>${output}
echo "config_dac(0,0.${channel4}); //channel 4  csa Vreset  ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,0.${channel5});   // channel 2  csa Vreset ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,0.62);  //channel 4 csa ref 0.62 ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,0.62); //channel 2 csa ref 0.62 ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,0.${channel6});// channel 3  csa Vreset ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,0.${channel7});//channel 1  csa Vreset ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,0.62);//channel 3 csa ref 0.62 ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,0.62);// channel 1 csa ref 0.62 ">>${output}
echo "pulse_reg(4);" >>${output}
echo  >>${output}
echo  >>${output}
echo  >>${output}
echo "config_dac(0,1.3);  //channal 8 ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,1.3);  //channal 7 ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,1.3); ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,1.3); ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,1.3); ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,1.3); ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,1.3); ">>${output}
echo "pulse_reg(4);" >>${output}
echo "config_dac(0,1.3);  //channal 1 ">>${output}
echo "pulse_reg(4);" >>${output}
echo  >>${output}
echo  >>${output}
echo "config_reg(1,0x0000);" >>${output}
echo "config_reg(1,0x0001);" >>${output}
echo "config_reg(1,0x0000);" >>${output}
# echo  >>${output}
# echo  >>${output}
