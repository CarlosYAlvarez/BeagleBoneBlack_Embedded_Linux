#ifndef H_GPIO_H_
#define H_GPIO_H_

#include <iostream>
#include <array>
#include<fstream>

using namespace std;

#define GPIO_PINS 95

class GPIO
{
public:
	enum class DIRECTION
	{
		INPUT  = 0,
		OUTPUT = 1
	};

	enum class VALUE
	{
		LOW  = 0,
		HIGH = 1
	};

	GPIO(unsigned int pin, DIRECTION = DIRECTION::OUTPUT);
	~GPIO();
	void setValue(const VALUE GPIO_VALUE) const;
	void setDirection(const DIRECTION GPIO_DIRECTION) const;

private:

	static const string GPIO_PATH;
	static const std::array<std::pair<unsigned int, std::string>, GPIO_PINS> GPIO_PIN_LOOKUP_TABLE;

	string gpioPinPath;
	unsigned int gpioPinNumber;

	const string getPinName(const unsigned int GPIO_PIN_NUMBER);
	bool writeToFile(const string fileName, const string value) const;
};

#endif /* H_GPIO_H_ */
