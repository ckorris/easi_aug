#pragma once

#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>

using namespace sl;
using namespace std;

class Simulation
{
public:
	Simulation(sl::Camera *zed, sl::float3 barreloffset);
	bool Simulate(sl::Mat depthmat, float speedmps, float distbetweensamples, bool applygravity, int2& collisionpoint, float& collisiondepth);

private:
	sl::Camera *currentZED;
	cv::Mat projectionMatrix;
	sl::float3 barrelOffset; //For now, we assumt it always points in the same direction as the camera. 

};