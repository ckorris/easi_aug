#if SPI_OUTPUT || GPIO_HOTKEYS 

#pragma once
class GPIOHelper
{
public:
	enum GPIODirection { IN, OUT };
	static void GPIOSetup_Mem(const int gpio, GPIODirection direction);
	static void UnexportGPIO_Mem(const int gpio);
	
};



#endif
