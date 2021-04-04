#if SPI_OUTPUT || GPIO_HOTKEYS 

#pragma once
class GPIOHelper
{
public:
	static void GPIOHelper::GPIOSetup_Mem(const int gpio);
	static void GPIOHelper::UnexportGPIO_Mem(const int gpio);
};



#endif