#include <HotkeyBindings.h>
#include <vector>
#include <memory>
#pragma once

using namespace std;

#if SPI_OUTPUT
#define NANO_GPIO_ADDRESS_PIN16 0x6000d704 //GPIO3 PDD.
#define NANO_GPIO_BIT_PIN16 0x01 //00, first bit.
#define NANO_GPIO_BCM_PIN16 232

#define NANO_GPIO_ADDRESS_PIN18 0x6000d704 //GPIO3 PDD.
#define NANO_GPIO_BIT_PIN18 0x80 //07, last bit.
#define NANO_GPIO_BCM_PIN18 15

#endif

class HotkeyManager
{




public:
	void RegisterKeyBinding(char key, void(*onTrue)());
#if SPI_OUTPUT
	void RegisterGPIOBinding(int memoryAddress, int bit, int bcmNumber, void(*onTrue)());
#endif


	void Process();
private:
	vector<unique_ptr<HotkeyBinding>> _bindings;
};