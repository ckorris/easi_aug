#if SPI_OUTPUT || GPIO_HOTKEYS 

#pragma once

#include <stdint.h>

typedef struct {
    uint32_t CNF;
    uint32_t _padding1[3];
    uint32_t OE;
    uint32_t _padding2[3];
    uint32_t OUT;
    uint32_t _padding3[3];
    uint32_t IN;
    uint32_t _padding4[3];
    uint32_t INT_STA;
    uint32_t _padding5[3];
    uint32_t INT_ENB;
    uint32_t _padding6[3];
    uint32_t INT_LVL;
    uint32_t _padding7[3];
    uint32_t INT_CLR;
    uint32_t _padding8[3];
} gpio_t;

class GPIOHelper
{
public:
	enum GPIODirection { IN, OUT };
	static void* _base;
	static void GPIOSetup_Mem(const int gpio, GPIODirection direction);
	static void UnexportGPIO_Mem(const int gpio);
	static volatile gpio_t* InitPin_Out(int memaddress, int bit);
	static volatile gpio_t* InitPin_In(int memaddress, int bit);
	static bool GetValue_Mem(volatile gpio_t *pinLed, int bit);
	static volatile gpio_t* InitPin_Mem(void *base, int pagemask, int memaddress, int bit);
	static int _fd;
};



#endif
