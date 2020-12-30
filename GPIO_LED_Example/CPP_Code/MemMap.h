#ifndef H_MEM_MAP_H_
#define H_MEM_MAP_H_

#include <iostream>

/* Memmory map addresses for GPIO: See TRM */
#define GPIO0_MEM_MAP_ADDR 0x44E07000
#define GPIO1_MEM_MAP_ADDR 0x4804C000
#define GPIO2_MEM_MAP_ADDR 0x481AC000
#define GPIO3_MEM_MAP_ADDR 0x481AE000

/* The lenght of each GPIO section is 4KB: See TRM*/
#define GPIO_MAP_SIZE 4096UL

/* The list of GPIO registers and their offset (in bytes) from the GPIO base address: See TRM*/
#define GPIO_REVISION_OFFSET        0x0
#define GPIO_SYSCONFIG_OFFSET       0x10
#define GPIO_EOI_OFFSET             0x20
#define GPIO_IRQSTATUS_RAW_0_OFFSET 0x24
#define GPIO_IRQSTATUS_RAW_1_OFFSET 0x28
#define GPIO_IRQSTATUS_0_OFFSET     0x2C
#define GPIO_IRQSTATUS_1_OFFSET     0x30
#define GPIO_IRQSTATUS_SET_0_OFFSET 0x34
#define GPIO_IRQSTATUS_SET_1_OFFSET 0x38
#define GPIO_IRQSTATUS_CLR_0_OFFSET 0x3C
#define GPIO_IRQSTATUS_CLR_1_OFFSET 0x40
#define GPIO_IRQWAKEN_0_OFFSET      0x44
#define GPIO_IRQWAKEN_1_OFFSET      0x48
#define GPIO_SYSSTATUS_OFFSET       0x114
#define GPIO_CTRL_OFFSET            0x130
#define GPIO_OE_OFFSET              0x134
#define GPIO_DATAIN_OFFSET          0x138
#define GPIO_DATAOUT_OFFSET         0x13C
#define GPIO_LEVELDETECT0_OFFSET    0x140
#define GPIO_LEVELDETECT1_OFFSET    0x144
#define GPIO_RISINGDETECT_OFFSET    0x148
#define GPIO_FALLINGDETECT_OFFSET   0x14C
#define GPIO_DEBOUNCENABLE_OFFSET   0x150
#define GPIO_DEBOUNCINGTIME_OFFSET  0x154
#define GPIO_CLEARDATAOUT_OFFSET    0x190
#define GPIO_SETDATAOUT_OFFSET      0x194

using namespace std;


class MemMap
{
public:
	MemMap();
	~MemMap();

	void registerWrite(const ulong REGISTER, const unsigned int OFFSET, const unsigned int VALUE);
	void registerRead(void);

private:
	void memSetup(ulong* &pinconf, const ulong REGISTER);
};

#endif /* H_MEM_MAP_H_ */
