#include <TimeHelper.h>

#include <chrono>
using namespace std;
using namespace chrono;

/*
Time::Time()
{
	clock = steady_clock();
	lastPoint = steady_clock::now();
}
*/

static bool isInit;// = false; //Apparently static initializers are the devil so we won't for now. 
//static steady_clock clock;
static steady_clock::time_point lastPoint;
static double lastFrameTime;
static float lastFrameDelta;

void Time::LogNewFrame()
{
	if (isInit != true)
	//if (true)
	{
		//clock = steady_clock();
		lastPoint = steady_clock::now();
		isInit = true;
	}

	steady_clock::time_point currentpoint = steady_clock::now();
	duration<double> timediff = duration_cast<duration<double>>(currentpoint - lastPoint);
	lastFrameDelta = timediff.count();

	lastPoint = currentpoint;
}

float Time::deltaTime()
{
	return lastFrameDelta;
}

float Time::fps()
{
	return 1.0 / lastFrameDelta;
}
