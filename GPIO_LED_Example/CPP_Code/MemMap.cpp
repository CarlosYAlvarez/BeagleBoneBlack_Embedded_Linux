#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <fstream>
#include <sys/mman.h>

#include "MemMap.h"

/*
 * Description:
 */
MemMap::MemMap()
{
}

void MemMap::memSetup(ulong* &pinconf, const ulong REGISTER)
{
	int fd = open("/dev/mem", O_RDWR | O_SYNC);

	/* ************************************************************************
		* "Map the memory" at the physical location GPIO1_MEM_MAP_ADDR.
		* ************************************************************************
		* Creates a new mapping in the virtual address space of the calling process.
		* This will then return the address of the new mapping.
		*
		* Parameters:
		*	*addr - The starting address for the new mapping.
		*		if NULL - the kernel chooses the (page-aligned) address at which to
		*				  create the mapping. This is the most portable method of
		*				  creating a new mapping.
		*		if !NULL -the kernel takes it as a hint about where to place the
		*				  mapping.
		*
		*		NOTE: A "normal" pointer cna only point to the memory allocated to the
		*			  process, so we have to use memory mapping. NULL specifies that
		*			  LINUX is free to put the memory map wherever it wants in the
		*			  memory of the process.
		*
		*	length - Specifies the length of the mapping (must be > 0).
		*
		*		NOTE: In this case the lenght is 4KB
		*
		*	prot - Describes the desired memory protection of the mapping (and must
		*		   not conflict with the open mode of the file). Must be PROT_NONE,
		*		   or the bitwise OR of one or more of: PROT_EXEC, PROT_READ
		*		   PROT_WRITE.
		*
		*	flags - Determins whether updates to the mapping are visible to other
		*			processes mapping the same region, and whether updates are
		*			carried throught to the underlying file.
		*		MAP_SHARED - Share this mapping. Update to the mapping are visible to
		*					 other processes mapping the same region, and are carried
		*					 throught to the underlying file.
		*
		*		NOTE: MAP_SHARED says that we actually access the underlying memory and
		*			  not just the map in the memory of the process when we write to the
		*			  memory map.
		*
		*	file descriptor - This is where the contents of a file mapping are initialized.
		*
		*		NOTE: /dev/mem is the place where LINUX abstracts the physical memory as a file
		*
		*	offset - The offset starting at (argument above) length, where to initialize the
		*			 contents of the file mapping.
		*
		*		NOTE: Specifies where the memory map starts, so in this case it will start at
		*			  GPIO1_MEM_MAP_ADDR + GPIO_MAP_SIZE.
		*
		* ADDITIONAL NOTES:
		* The pointer is of type ulong because this type evalutes to 32 bits on the BBB, and
		* registers are of size 32 bits.
		*/
	pinconf = (ulong*)mmap(NULL, //*addr
		GPIO_MAP_SIZE, //length
		PROT_READ | PROT_WRITE, //prot
		MAP_SHARED, //flags
		fd, //file descriptor
		REGISTER); //offset
}

void MemMap::registerWrite(const ulong REGISTER, const unsigned int OFFSET, const unsigned int VALUE)
{
	ulong* pinconf = NULL;

	memSetup(pinconf, REGISTER);

	if (pinconf == NULL)
	{
		cout << "ERROR: pinconf not initialized" << endl;
		return;
	}

	/*
	 Because pinconf1 is of type ulong, everytime a read/write is performed, reading/writing
	 is done for 32 bits. The offset address of the various registers is in bytes, therefore
	 divide by the size of ulong (which is 32 bits or 4 bytes). E.g:
		GPIO_SYSCONFIG_OFFSET = 0x10 bytes = 16 bytes (note this is the offset from the base address)
		=> 16 bytes/4 bytes = 4.

		HENCE when we do pinconf1[4], we'll be accessing the address: BASE ADDRESS + (4 * 4 bytes) = BASE ADDRESS + 16 bytes

		pinconf1 now points to the address GPIO1_MEM_MAP_ADDR, => BASE ADDRESS (above) is GPIO1_MEM_MAP_ADDR
	*/
	const unsigned int U_LONG_SIZE = sizeof(ulong);

	const unsigned int GPIO_OE_BYTE_OFFSET = GPIO_OE_OFFSET / U_LONG_SIZE;
	const unsigned int BYTE_OFFSET = OFFSET / U_LONG_SIZE;

	pinconf[GPIO_OE_BYTE_OFFSET] &= (0xFFFFFFFF ^ VALUE); //Set the bit specified by VALUE as an output (all other bits are inputs)
	pinconf[BYTE_OFFSET] = VALUE; //Write the value specified by VALUE into REGISTER at BYTE_OFFSET
}

void MemMap::registerRead(void)
{
	/*
	 Because pinconf1 is of type ulong, everytime a read/write is performed, reading/writing
	 is done for 32 bits. The offset address of the various registers is in bytes, therefore
	 divide by the size of ulong (which is 32 bits or 4 bytes). E.g:
		GPIO_SYSCONFIG_OFFSET = 0x10 bytes = 16 bytes (note this is the offset from the base address)
		=> 16 bytes/4 bytes = 4.

		HENCE when we do pinconf1[4], we'll be accessing the address: BASE ADDRESS + (4 * 4 bytes) = BASE ADDRESS + 16 bytes

		pinconf1 now points to the address GPIO1_MEM_MAP_ADDR, => BASE ADDRESS (above) is GPIO1_MEM_MAP_ADDR
	*/
	//const unsigned int U_LONG_SIZE = sizeof(ulong);

	//const unsigned int GPIO_OE_BYTE_OFFSET = GPIO_OE_OFFSET / U_LONG_SIZE;

	//cout << "GPIO_OE_BYTE_OFFSET: " << std::hex << pinconf[GPIO_OE_BYTE_OFFSET] << endl;
}

/*
 * Destructor
 */
MemMap::~MemMap()
{
}
