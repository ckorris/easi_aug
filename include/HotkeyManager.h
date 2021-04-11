#include <HotkeyBindings.h>
#include <vector>
#include <memory>
#pragma once

using namespace std;

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