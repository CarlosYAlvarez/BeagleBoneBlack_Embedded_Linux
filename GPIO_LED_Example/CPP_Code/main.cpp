/*
 * This program switches the value of a GPIO pin between HIGH and LOW.
 */

#include<iostream>
#include<unistd.h>

#include "GPIO.h"

using namespace std;

void ledTest(void);
void buttonTest(void);
void i2cTest(void);
void pwmTest(void);
void spiTest(void);
void analogTest(void);

void activateLed(void);

enum TEST
{
	TEST_GPIO_LED = 0,
	TEST_GPIO_BUTTON,
	TEST_I2C,
	TEST_PWM,
	TEST_SPI,
	TEST_ANALOG,
	TEST_NUM
};

void (*test[TEST_NUM]) (void);

int main()
{
	unsigned int testNumber = 0;
	test[TEST_GPIO_LED] = ledTest;
	test[TEST_GPIO_BUTTON] = buttonTest;
	test[TEST_I2C] = i2cTest;
	test[TEST_PWM] = pwmTest;
	test[TEST_SPI] = spiTest;
	test[TEST_ANALOG] = analogTest;

	while(true)
	{
		cout << "=== Choose test to execute ===" << endl;
		cout << "GPIO LED Test:    " << TEST_GPIO_LED << endl;
		cout << "GPIO Button Test: " << TEST_GPIO_BUTTON << endl;
		cout << "I2C Test:         " << TEST_I2C << endl;
		cout << "PWM Test:         " << TEST_PWM << endl;
		cout << "SPI Test:         " << TEST_SPI << endl;
		cout << "Analog Test:      " << TEST_ANALOG << endl;
		cout << "Exit:             " << TEST_NUM << endl;

		cin >> testNumber;

		if(testNumber == TEST_NUM)
		{
			break;
		}
		else
		{
			(*test[testNumber])();
		}
	}

	return 0;
}

void ledTest(void)
{
	cout << "Running GPIO LED Test" << endl;

	GPIO led(49, GPIO::DIRECTION::OUTPUT);

	for (int i = 0; i < 5; i++)
	{
		led.setValue(GPIO::VALUE::HIGH);
		sleep(1);

		led.setValue(GPIO::VALUE::LOW);
		sleep(1);
	}
	cout << "Running GPIO LED Test Completed" << endl;
}

void buttonTest(void)
{
	cout << "Running GPIO Button Test" << endl;
	GPIO button(115,
			    GPIO::DIRECTION::INPUT,
			    GPIO::EDGE::RISING);

	//button.inputWaitTimeMS = 10000; //10 seconds
	button.triggerOnEdge(&activateLed);

	cout << "Running GPIO Button Test Completed" << endl;
}

void activateLed(void)
{
	static unsigned int count = 0;
	cout << "BUTTON PRESSED: " << count++ << endl;
}

void i2cTest(void)
{
	cout << "Running I2C Test" << endl;

}

void pwmTest(void)
{
	cout << "Running PWM Test" << endl;

}

void spiTest(void)
{
	cout << "Running SPI Test" << endl;

}

void analogTest(void)
{
	cout << "Running Analog Test" << endl;

}

