#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <fstream>

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
	{115, "p9.27"},
	{113, "p9.28"},
	{111, "p9.29"},
	{112, "p9.30"},
	{110, "p9.31"},
	{20 , "p9.41"},
	{7  , "p9.42"}
}};

/*
 * Description:
 * 	Setup the chosen GPIO pin.
 */
GPIO::GPIO(unsigned int pin, DIRECTION direction, EDGE edge) : gpioPinNumber(pin)
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

	setValue(GPIO::VALUE::LOW);
	setDirection(direction);
	setEdge(edge);

	//Will apply only to GPIOs set as inputs.
	inputWaitTimeMS = -1; //Wait indefinitely for a file descriptor to be ready
}

/*
 * Description:
 * 	Access the lookup table to match a GPIO pin number with the physical pin number on the BBB.
 *
 * Args:
 * 	PIN_NUMBER the GPIO pin number being used
 *
 * Return
 * 	The physical pin number on the board that maps to the GPIO pin number being used
 */
const string GPIO::getPinName(const unsigned int GPIO_PIN_NUMBER)
{
	const string FAILURE = "none";
	string gpioPinName = FAILURE;

	for(unsigned int index = 0; index < GPIO_PINS; ++index)
	{
		if(GPIO::GPIO_PIN_LOOKUP_TABLE[index].first == GPIO_PIN_NUMBER)
		{
			gpioPinName = GPIO::GPIO_PIN_LOOKUP_TABLE[index].second;
		}
	}

	if(strcmp(gpioPinName.c_str(), FAILURE.c_str()) == 0)
	{
		   perror("GPIO: Failed to find the pin specified.");
		   exit(EXIT_FAILURE);
	}

	return gpioPinName;
}

/*
 * Description:
 *	Updates the value of the selected GPIO pin.
 *
 * Args:
 *	GPIO_VALUE The new value of the selected GPIO pin
 *
 * Return
 * 	None
 */
void GPIO::setValue(const VALUE GPIO_VALUE) const
{
	writeToFile("value", to_string((int)GPIO_VALUE));
}

/*
 * Description:
 *	Updates the direction of the selected GPIO pin.
 *
 * Args:
 *	GPIO_DIRECTION The new direction for the selected GPIO pin
 *
 * Return
 * 	None
 */
void GPIO::setDirection(const DIRECTION GPIO_DIRECTION) const
{
	string directionValue;

	switch(GPIO_DIRECTION)
	{
	case DIRECTION::INPUT:
		directionValue = "in";
		break;
	case DIRECTION::OUTPUT:
		directionValue = "out";
		break;
	default:
		directionValue = "out";
		break;
	}

	writeToFile("direction", directionValue);
}
void GPIO::setEdge(const EDGE GPIO_EDGE) const
{
	string edgeValue;

	switch(GPIO_EDGE)
	{
	case EDGE::NONE:
		edgeValue = "none";
		break;
	case EDGE::RISING:
		edgeValue = "rising";
		break;
	case EDGE::FALLING:
		edgeValue = "falling";
		break;
	case EDGE::BOTH:
		edgeValue = "both";
		break;
	default:
		edgeValue = "none";
		break;
	}

	writeToFile("edge", edgeValue);
}

void GPIO::triggerOnEdge(edgeCallback callback)
{
	thread edgeTrigger(&GPIO::pollEdge, this, callback);
	edgeTrigger.join();
}

/*
 * Read urgent data on edge trigger.
 */
void GPIO::pollEdge(edgeCallback callback) const
{
	/*
	 * What is a file descriptor:
	 * In Unix and related computer operating systems, a file descriptor is an abstract
	 * indicator (handle) used to access a file or other input/output resource, such as
	 * a pipe or network socket. File descriptors form part of the POSIX application
	 * programming interface.
	 */

	/* ************************************************************************
	 * Create a new epoll instance to monitor a file descriptor for I/O
	 * ************************************************************************
	 * returns  a  file  descriptor referring to the new epoll
	 * instance.  This file descriptor is used for all the subsequent calls to
	 * the  epoll  interface.   When  no  longer required, the file descriptor
	 * returned by epoll_create() should be closed by  using  close(2).   When
	 * all  file  descriptors referring to an epoll instance have been closed,
	 * the kernel destroys the instance and releases the associated  resources
	 * for reuse.
	 */
	int epollFileDescriptor = epoll_create(1);
    if (epollFileDescriptor == -1)
    {
	   perror("GPIO::pollEdge - Failed to create a new epoll instance: epoll_create()");
	   exit(EXIT_FAILURE);
    }

    /* ************************************************************************
     * Open the "value" file for reading. This file tells us when the button is pressed
     * ************************************************************************
     * The open() function establishes the connection between a file and a file
     * descriptor. It creates an open file description that refers to a file and
     * a file descriptor that refers to that open file description. The file
     * descriptor is used by other I/O functions to refer to that file. The path
     * argument points to a pathname naming the file.
     *
     * Open a file and return a new file descriptor for it, or -1 on error.
     *
     * O_RDONLY:
     * 			Open for reading only.
     *
     * O_NONBLOCK:
     * 			1) If O_NONBLOCK is set, an open() for reading-only shall
     * 			   return without delay. An open() for writing-only shall
     * 			   return an error if no process currently has the file
     * 			   open for reading.
     *
     * 			2) If O_NONBLOCK is clear, an open() for reading-only shall
     * 			   block the calling thread until a thread opens the file
     * 			   for writing. An open() for writing-only shall block the
     * 			   calling thread until a thread opens the file for reading.
     *
	 * Return:
	 * 		Upon successful completion, the function will open the file and return a non-negative
	 * 		integer representing the lowest numbered unused file descriptor. Otherwise, -1 is
	 * 		returned and errno is set to indicate the error. No files will be created or modified
	 * 		if the function returns -1.
     */
    int fileDescriptor = open((this->gpioPinPath + "value").c_str(), O_RDONLY | O_NONBLOCK);
    if ( fileDescriptor == -1)
    {
       perror("GPIO::pollEdge - Failed to open the GPIO value file: open()");
       exit(EXIT_FAILURE);
    }

    /* ************************************************************************
     * Describe the object linked to the file descriptor
     * ************************************************************************
     * EPOLLIN:
     * 			The associated file is available for read(2) operations.
     *
     * EPOLLET:
     * 			Sets the Edge Triggered behavior for the associated file descriptor.
     *
     * EPOLLPRI:
     * 			There is urgent data available for read(2) operations.
     */

    //TODO: Look into adding EPOLLONESHOT. Because the epoll_wait function always seems to return 1 time on the first go around,
    //      adding EPOLLONESHOT, will remove the file descriptor before the button is even actually pressed.
	struct epoll_event epollEvent;
    epollEvent.events = EPOLLIN | EPOLLET | EPOLLPRI; // read operation | edge triggered | urgent data
    epollEvent.data.fd = fileDescriptor;              // Associate the file's file descriptor

    /* ************************************************************************
     * Add the file descriptor entry to the new epoll instance created
     * ************************************************************************
     * EPOLL_CTL_ADD:
     * 			Register the target file descriptor fileDescriptor onto the epoll instance
     *        	referred to by the file descriptor epollFileDescriptor, and associate the
     *        	event event with the internal file linked to fileDescriptor.
     */
    if (epoll_ctl(epollFileDescriptor,	// An instance of epoll to be manipulated
    			  EPOLL_CTL_ADD, 		// Add a file descriptor to the interface
				  fileDescriptor,		// The target of the operation
				  &epollEvent)			// Describes which events the caller is interested in and any associated user data
    		== -1)
    {
       perror("GPIO::pollEdge - Failed to add control interface: epoll_ctl()");
       close(fileDescriptor);
       exit(EXIT_FAILURE);
    }

    int epollTriggerCount = 0; //The number of times the epoll_wait() function returns.
    int epollEventsNum = 0;    //The number of triggered events, one for every file descriptor.

	#ifdef DEBUG
		const unsigned int MAX_BYTES_TO_READ = 2;
		char readBuffer[MAX_BYTES_TO_READ];
		int bytesRead = 0;
	#endif

	while(true)
	{
	    /* ************************************************************************
	     * Wait indefinitely until a file descriptor is ready for the requested I/O
	     * ************************************************************************
		 * int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
		 *
		 * The epoll_wait() system call waits for events on the epoll(7) instance
		 * referred to by the file descriptor epfd. The memory area pointed to by
		 * events will contain the events that will be available for the caller.
		 * Up to maxevents are returned by epoll_wait(). The maxevents argument must
		 * be greater than zero.
		 */
		//NOTE: If a timeout value is specified, and epoll_wait returns, the timeout value restarts.
		//      to make it work so that after it is pressed once this function exits, add a break
		//      after call to the callback function.
		epollEventsNum = epoll_wait(epollFileDescriptor, &epollEvent, 1, this->inputWaitTimeMS);
		epollTriggerCount++; //It seems like epoll_wait always returns once, use to to ignore the first trigger.

		if (epollEventsNum == -1)
		{
			perror("GPIO::pollEdge - Failed to wait for a file descriptor to be ready: epoll_wait()");
			break;
		}
		else if(epollEventsNum == 0)
		{
			//No file descriptor became read during the requested timeout.
			cout << "Warning: No file descriptor became available within the time specified (" << this->inputWaitTimeMS << " ms)" << endl;
			break;
		}
		else
		{
			//Trigger occurred
			if(epollEvent.data.fd == fileDescriptor && //Check if the trigger belongs to the file descriptor specified above
			   epollTriggerCount > 1)                  //Ignore the first trigger.
			{
				#ifdef DEBUG
					bytesRead = read(epollEvent.data.fd, readBuffer,MAX_BYTES_TO_READ);

					if(bytesRead == -1)
					{
						perror("GPIO::pollEdge - Error reading file descriptor. read()");
					}
					else
					{
						cout << "Reading " << bytesRead << " bytes: ";
						for(unsigned int i = 0; i < (unsigned int)bytesRead; ++i)
						{
							cout << readBuffer[i];
						}
						cout << endl;
					}

					/* ************************************************************************
					 * Reposition the file offset of the open file descriptor.
					 * ************************************************************************
					 * Call to read() reads through the entire file and leaves the read cursor
					 * at the end of the file. Call seek() to return the read cursor to the start
					 * of the file.
					 */
					lseek(epollEvent.data.fd, 0, SEEK_SET);
				#endif

				callback();
			}
		}
	}

    close(fileDescriptor);
}

/*
 * Description:
 *	Writes specified value to a file.
 *
 * Args:
 *	FILE_NAME The file to write to
 *	VALUE The value to write into the file
 *
 * Return
 * 	True if the file was opened
 */
bool GPIO::writeToFile(const string FILE_NAME, const string VALUE) const
{
	//Open the file /sys/class/gpio/gpio<PinNumber>/<FILE_NAME> for writing
	ofstream gpioAttributeFile;
	gpioAttributeFile.open((this->gpioPinPath + FILE_NAME).c_str(), ios_base::out);

	bool fileIsOpen = gpioAttributeFile.is_open();

	if(fileIsOpen)
	{
		gpioAttributeFile << VALUE; // same as: echo <VALUE> > <FILE_NAME>
		gpioAttributeFile.close();
	}
	else
	{
		perror(("FailedTo open file for writing: " + this->gpioPinPath + FILE_NAME).c_str());
	}

	return fileIsOpen;
}

/*
 * Destructor
 */
GPIO::~GPIO()
{
}
