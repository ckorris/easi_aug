#include <HotkeyBindings.h>
#include <vector>
#include <memory>
#pragma once

using namespace std;

#if SPI_OUTPUT
#define NANO_GPIO_ADDRESS_PIN16 0x6000d704 //GPIO3 PDD.
#define NANO_GPIO_BIT_PIN16 0x01 //00, first bit.
#define NANO_GPIO_BCM_PIN16 232

#define NANO_GPIO_ADDRESS_PIN18 0x6000d004 //GPIO3 PB.
#define NANO_GPIO_BIT_PIN18 0x80 //07, last bit.
#define NANO_GPIO_BCM_PIN18 15

#define NANO_GPIO_ADDRESS_PIN26 0x6000d008 //GPIO3 PC.
#define NANO_GPIO_BIT_PIN26 0x10 //04, fifth bit.
#define NANO_GPIO_BCM_PIN26 20

#define NANO_GPIO_ADDRESS_PIN38 0x6000d204 //GPIO3 PJ.
#define NANO_GPIO_BIT_PIN38 0x20 //05, sixth bit.
#define NANO_GPIO_BCM_PIN38 77


#define NANO_GPIO_ADDRESS_PIN40 0x6000d204 //GPIO3 PJ.
#define NANO_GPIO_BIT_PIN40 0x40 //06, seventh bit.
#define NANO_GPIO_BCM_PIN40 78
#endif

class HotkeyManager
{
public:
	shared_ptr<HotkeyBinding> RegisterKeyBinding(char key, void(*onTrue)());
#if SPI_OUTPUT
	shared_ptr<HotkeyBinding> RegisterGPIOBinding(int memoryAddress, int bit, int bcmNumber, void(*onTrue)());
#endif


	void Process();
private:
	vector<shared_ptr<HotkeyBinding>> _bindings;
	vector<KeyBinding> _keyBindings;
#if SPI_OUTPUT
	vector<GPIOBinding> _gpioBindings;
#endif
};
