/*
 * This program switches the value of a GPIO pin between HIGH and LOW.
 */

#include<iostream>
#include<unistd.h>

#include "GPIO.h"

using namespace std;

int main()
{
	GPIO led(49, GPIO::DIRECTION::OUTPUT);

	for (int i = 0; i < 5; i++)
	{
		led.setValue(GPIO::VALUE::HIGH);
		sleep(1);

		led.setValue(GPIO::VALUE::LOW);
		sleep(1);
	}

	return 0;
}
