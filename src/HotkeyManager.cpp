#include <HotkeyManager.h>


shared_ptr<HotkeyBinding> HotkeyManager::RegisterKeyBinding(char key, void(*onTrue)())
{
	KeyBinding newBinding(key, onTrue);
	_keyBindings.emplace_back(newBinding);
	shared_ptr<HotkeyBinding> sharedBinding = make_shared<KeyBinding>(newBinding);
	_bindings.push_back(sharedBinding);
	return sharedBinding;
}

#if SPI_OUTPUT

shared_ptr<HotkeyBinding> HotkeyManager::RegisterGPIOBinding(int memoryAddress, int bit, int bcmNumber, void(*onTrue)())
{
	GPIOBinding newBinding(memoryAddress, bit, bcmNumber, onTrue);
	_gpioBindings.emplace_back(newBinding);
	shared_ptr<HotkeyBinding> sharedBinding = make_shared<GPIOBinding>(newBinding);
	_bindings.push_back(sharedBinding);
	return sharedBinding;
}

#endif

void HotkeyManager::Process()
{
	//Call the static pre-process function for the known types, to do things that need to be done once
	//for all bindings.
	//TODO: Would be better to do procedurally without needing to explicitly list each type.
	KeyBinding::PreProcess();

	for (int i = 0; i < _keyBindings.size(); i++)
	{
		_bindings[i]->Process();
	}
}

