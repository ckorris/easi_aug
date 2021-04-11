#include <HotkeyManager.h>


void HotkeyManager::RegisterKeyBinding(char key, void(*onTrue)())
{
	_bindings.emplace_back(new KeyBinding(key, onTrue));
}

void HotkeyManager::Process()
{
	//Call the static pre-process function for the known types, to do things that need to be done once
	//for all bindings.
	//TODO: Would be better to do procedurally without needing to explicitly list each type.
	KeyBinding::PreProcess();

	for (int i = 0; i < _bindings.size(); i++)
	{
		_bindings[i]->Process();
	}
}

#if SPI_OUTPUT

void HotkeyManager::RegisterGPIOBinding(int memoryAddress, int bit, int bcmNumber, void(*onTrue)())
{
	_bindings.emplace_back(new GPIOBinding(memoryAddress, bit, bcmNumber, onTrue));
}

#endif