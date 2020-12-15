##############################################################################################
# Description:                                                                               #
# The purpose of this script is to test the GPIO functionality by repeatedly powering ON and #
# OFF (10 times at 1 second intervals) the LED connected to Pin 23 on Header P9 of the       #
# BeagleBone Black Board Embedded Linux Learning Board.                                      #
#                                                                                            #
# Author:    Carlos Y. Alvarez                                                               #
# Date:      2020 December, 05                                                               #
# File Name: ledBlinkTest.sh                                                                 #
# Source:    https://github.com/CarlosYAlvarez/BeagleBoneBlack-Embedded-Linux-Tutorials      #   
##############################################################################################

#!/bin/bash
echo "Starting LED Blink Test..."

#Configure Pin 23 on Header P9 as a GPIO pin.
config-pin p9.23 gpio

#Set the pin above as an output.
echo "out" > /sys/class/gpio/gpio49/direction

#Turn the LED on and off 10 times at 1 second intervals.
for i in `seq 1 10`;
do
	echo "Turning LED On."
	echo "1" > /sys/class/gpio/gpio49/value

	sleep 1s

	echo "Turning LED Off."
	echo "0" > /sys/class/gpio/gpio49/value

	sleep 1s
done

echo "LED BLink Test Complete."
