#pragma once

#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>

using namespace sl;
using namespace std;

class Simulation
{
public:
	//Simulation(sl::Camera *zed, sl::float3 barreloffset);
	Simulation(sl::Camera *zed);

	bool Simulate(sl::Mat depthmat, float speedmps, float distbetweensamples, bool applygravity, 
		int2& collisionpoint, float& collisiondepth, float& totaltime, bool drawline, cv::Mat& drawlinetomat, cv::Scalar linecolor);

	static float CalculateAirDensity(float totalairpressurehpa, float tempcelsius, float relhumidity01);

	static float CalculateDragForce(float dragcoefficient, float airdensitykgm3, float surfaream2, float speedms);

private:
	sl::Camera *currentZED;
	cv::Mat projectionMatrix;
	//sl::float3 barrelOffset; //For now, we assumt it always points in the same direction as the camera. 


};