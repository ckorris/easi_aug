#include <HotkeyBindings.h>
#include <vector>
#include <memory>
#pragma once

using namespace std;

#if SPI_OUTPUT
//To find more of these, refer to the TRM page 247 as of writing.

#define NANO_GPIO_ADDRESS_PIN16 0x6000d704 //GPIO3 PDD.
#define NANO_GPIO_BIT_PIN16 0x01 //00, first bit.
#define NANO_GPIO_BCM_PIN16 232

#define NANO_GPIO_ADDRESS_PIN18 0x6000d004 //GPIO3 PB.
#define NANO_GPIO_BIT_PIN18 0x80 //07, last bit.
#define NANO_GPIO_BCM_PIN18 15

#define NANO_GPIO_ADDRESS_PIN26 0x6000d008 //GPIO3 PC.
#define NANO_GPIO_BIT_PIN26 0x10 //04, fifth bit.
#define NANO_GPIO_BCM_PIN26 20

#define NANO_GPIO_ADDRESS_PIN31 0x6000d604 //GPIO3 PZ
#define NANO_GPIO_BIT_PIN31 0x01 //00, first bit.
#define NANO_GPIO_BCM_PIN31 200

#define NANO_GPIO_ADDRESS_PIN33 0x6000d100 //GPIO3 PE
#define NANO_GPIO_BIT_PIN33 0x40 //06, seventh bit.
#define NANO_GPIO_BCM_PIN33 38

#define NANO_GPIO_ADDRESS_PIN35 0x6000d204 //GPIO3 PJ.
#define NANO_GPIO_BIT_PIN35 0x10 //04, fifth bit.
#define NANO_GPIO_BCM_PIN35 76

#define NANO_GPIO_ADDRESS_PIN36 0x6000d108 //GPIO3 PG.
#define NANO_GPIO_BIT_PIN36 0x08 //03, fourth bit.
#define NANO_GPIO_BCM_PIN36 51

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
	void CleanUpGPIOBindings();
#endif


	void Process();
private:
	vector<shared_ptr<HotkeyBinding>> _bindings;
	vector<KeyBinding> _keyBindings;
#if SPI_OUTPUT
	vector<GPIOBinding> _gpioBindings;
#endif
};
