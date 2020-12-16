#include "GPIO.h"

const string GPIO::GPIO_PATH = "/sys/class/gpio/";
const std::array<std::pair<unsigned int, std::string>, GPIO_PINS> GPIO::GPIO_PIN_LOOKUP_TABLE = {{

	/*GPIO pins on header P8*/
	{38, "p8.3"},
	{39, "p8.4"},
	{34, "p8.5"},
	{35, "p8.6"},
	{66, "p8.7"},
	{67, "p8.8"},
	{69, "p8.9"},
	{68, "p8.10"},
	{45, "p8.11"},
	{44, "p8.12"},
	{23, "p8.13"},
	{26, "p8.14"},
	{47, "p8.15"},
	{46, "p8.16"},
	{27, "p8.17"},
	{65, "p8.18"},
	{22, "p8.19"},
	{63, "p8.20"},
	{62, "p8.21"},
	{37, "p8.22"},
	{36, "p8.23"},
	{33, "p8.24"},
	{32, "p8.25"},
	{61, "p8.26"},
	{86, "p8.27"},
	{88, "p8.28"},
	{87, "p8.29"},
	{89, "p8.30"},
	{10, "p8.31"},
	{11, "p8.32"},
	{9 , "p8.33"},
	{81, "p8.34"},
	{8 , "p8.35"},
	{80, "p8.36"},
	{78, "p8.37"},
	{79, "p8.38"},
	{76, "p8.39"},
	{77, "p8.40"},
	{74, "p8.41"},
	{75, "p8.42"},
	{72, "p8.43"},
	{73, "p8.44"},
	{70, "p8.45"},
	{71, "p8.46"},

	/*GPIO pins on header P9*/
	{30 , "p9.11"},
	{60 , "p9.12"},
	{31 , "p9.13"},
	{40 , "p9.14"},
	{48 , "p9.15"},
	{51 , "p9.16"},
	{4  , "p9.17"},
	{5  , "p9.18"},
	{3  , "p9.21"},
	{2  , "p9.22"},
	{49 , "p9.23"},
	{15 , "p9.24"},
	{117, "p9.25"},
	{14 , "p9.26"},
	{125, "p9.27"},
	{123, "p9.128"},
	{121, "p9.29"},
	{122, "p9.30"},
	{120, "p9.31"},
	{20 , "p9.41"},
	{7  , "p9.42"}
}};

/*
 * Setup the chosen GPIO pin.
 */
GPIO::GPIO(unsigned int pin, DIRECTION direction) : gpioPinNumber(pin), gpioPinDirection(direction), gpioPinValue(GPIO::VALUE::LOW)
{
	/*
	 * Create the path /sys/class/gpio/gpio<PinNumber>/. This gives access to the following attributes:
	 *     - active_low
	 *     - direction
	 *     - edge
	 *     - label
	 *     - uevent
	 *     - value
	 */
	this->gpioPinPath = GPIO_PATH + "gpio" + to_string(this->gpioPinNumber) + "/";

	//TODO: If the config-pin command is used to configure pins as something other than GPIO, make it its own class.
	//Configure the selected pin as a GPIO pin by making a call to the system command config-pin
	string str = "config-pin " + getPinName(pin) + " gpio";
	const char *configPinCmd = str.c_str();

	system(configPinCmd);
}

/*
 * Description: Access the lookup table to match a GPIO pin number with the physical pin number on the BBB.
 * Args PIN_NUMBER the GPIO pin number being used
 * Return The physical pin number on the board that maps to the GPIO pin number being used
 */
const string GPIO::getPinName(const unsigned int GPIO_PIN_NUMBER)
{
	string gpioPinName = "none";

	for(unsigned int index = 0; index < GPIO_PINS; ++index)
	{
		if(GPIO::GPIO_PIN_LOOKUP_TABLE[index].first == GPIO_PIN_NUMBER)
		{
			gpioPinName = GPIO::GPIO_PIN_LOOKUP_TABLE[index].second;
		}
	}

	return gpioPinName;
}

/*
 * Destructor
 */
GPIO::~GPIO()
{
}
