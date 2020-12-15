##############################################################################################
# Description:                                                                               #
# The purpose of this script is to test the GPIO functionality by asking the user to press   #
# a button a certain number of times before the script exits. The button being used within   #
# the BeagleBone Black Board Embedded Linux Learning Board, is connected to a pull-down      #
# resistor. Therefore the default input value is 0, when the button is pressed the GPIO pin  #
# will be driven high and so the value will change to 1.                                     #
#                                                                                            #
# Author:    Carlos Y. Alvarez                                                               #
# Date:      2020 December, 09                                                               #
# File Name: buttonPressTest.sh                                                              #
# Source:    https://github.com/CarlosYAlvarez/BeagleBoneBlack_Embedded_Linux.git            #   
##############################################################################################

#!/bin/bash
echo "Starting Button Press Test..."

#Configure Pin 23 on Header P9 as a GPIO pin.
config-pin p9.27 gpio

#Set the pin above as an input.
echo "in" > /sys/class/gpio/gpio115/direction

inputFile="/sys/class/gpio/gpio115/value" #When the button is pressed, the contents of this file will change.
inputValue=0 #The value of 0 means the button is not pressed.
counter=1 #Keep count of the number of times the button was pressed.

echo "PRESS the button 3 times:"

while [ $counter -le 3 ]
do
	#Continously read the file until the butotn is pressed
	while [ $inputValue != 1 ]
	do
		#Read the file, and store the results to be evaluated later.
		while read line
		do
			inputValue=$line
		done < "$inputFile"
	done
       
        echo "Detected a button press." 	
	inputValue=0 #Reset the value to 0 to wait for another putton press.
	counter=$(($counter + 1))

	#Wait for 0.5s so that a single button press doesn't detect as multiple. The system can read faster than you can press and release.
	sleep 0.5s
done

echo "Button Press Test Complete."
