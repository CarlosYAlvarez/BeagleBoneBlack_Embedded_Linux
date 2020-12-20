#ifndef H_GPIO_H_
#define H_GPIO_H_

#include <iostream>
#include <array>

using namespace std;
typedef void (*edgeCallback)(void);

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

	enum class EDGE
	{
		NONE    = 0,
		RISING  = 1,
		FALLING = 2,
		BOTH    = 3
	};

	GPIO(unsigned int pin, DIRECTION = DIRECTION::OUTPUT, EDGE = EDGE::NONE);
	~GPIO();
	void setValue(const VALUE GPIO_VALUE) const;
	void setDirection(const DIRECTION GPIO_DIRECTION) const;
	void setEdge(const EDGE GPIO_EDGE) const;

	void triggerOnEdge(edgeCallback callback);

private:

	static const string GPIO_PATH;
	static const std::array<std::pair<unsigned int, std::string>, GPIO_PINS> GPIO_PIN_LOOKUP_TABLE;

	string gpioPinPath;
	unsigned int gpioPinNumber;

	const string getPinName(const unsigned int GPIO_PIN_NUMBER);
	void pollEdge(edgeCallback callback) const;
	bool writeToFile(const string fileName, const string value) const;
};

#endif /* H_GPIO_H_ */
