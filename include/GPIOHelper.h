#if SPI_OUTPUT || GPIO_HOTKEYS 

#pragma once
class GPIOHelper
{
public:
	static void GPIOSetup_Mem(const int gpio);
	static void UnexportGPIO_Mem(const int gpio);
};



#endif
