#pragma once

#include <chrono>
using namespace std;
using namespace chrono;

class Time
{
public:
	//Time();

	static void LogNewFrame();

	static float deltaTime();

	static float fps();

	//static float fps_smoothed();


private:

	//std::vector<float>
};