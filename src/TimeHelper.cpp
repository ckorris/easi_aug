#include <TimeHelper.h>

#include <chrono>
#include <iostream> //Just for cout debugging. 

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


static int framesToSmooth;
static float smoothedFPSTimer;
static float lastSmoothedFPS;

static const float SECONDS_BETWEEN_SMOOTHED_FPS = 0.2;

void Time::LogNewFrame()
{
	if (isInit != true)
	//if (true)
	{
		//clock = steady_clock();
		lastPoint = steady_clock::now();
		lastSmoothedFPS = 0.0;
		smoothedFPSTimer = 0.0;
		framesToSmooth = 0;

		isInit = true;
	}

	steady_clock::time_point currentpoint = steady_clock::now();
	duration<double> timediff = duration_cast<duration<double>>(currentpoint - lastPoint);
	lastFrameDelta = timediff.count();

	lastPoint = currentpoint;

	framesToSmooth++;

	smoothedFPSTimer += lastFrameDelta;
	if (smoothedFPSTimer >= SECONDS_BETWEEN_SMOOTHED_FPS) 
	{
		lastSmoothedFPS = smoothedFPSTimer / framesToSmooth;
		//std::cout << "Timer: " << smoothedFPSTimer << " frames: " << framesToSmooth << " last: " << lastSmoothedFPS << endl;
		smoothedFPSTimer = 0.0; //Doesn't handle overshooting the timer, resulting in missing the mark a bit, but no real consequence to that. 
		framesToSmooth = 0;
	}
}

float Time::deltaTime()
{
	return lastFrameDelta;
}

float Time::fps()
{
	if (lastFrameDelta != 0)
	{
		return 1.0 / lastFrameDelta;
	}
	else
	{
		return 0;
	}

}

///Returns the average FPS over the last X seconds defined by a const in TimeHelper (0.2 seconds as of writing). 
///Useful for displaying the FPS without it jumping around too quickly and not being readable. 
float Time::smoothedFPS()
{
	if (lastSmoothedFPS != 0)
	{
		return 1.0 / lastSmoothedFPS;
	}
	else
	{
		return 0;
	}
}
