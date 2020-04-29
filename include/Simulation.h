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

	bool Simulate(sl::Mat depthmat, float speedmps, float distbetweensamples, bool applyphysics, sl::SensorsData sensordata,
		int2& collisionpoint, float& collisiondepth, float& totaltime, bool drawline, cv::Mat& drawlinetomat, cv::Scalar linecolor);

	static float CalculateAirDensity(float totalairpressurehpa, float tempcelsius, float relhumidity01);

	static float CalculateAirViscosity(float tempcelsius);

	static float CalculateDragCoefficient(float spinrpm, float linearspeedmps, float bbdiametermeters,
		float airdensity, float airviscosity);

	static float CalculateLiftCoefficient(float spinrpm, float linearspeedmps, float bbdiametermeters);

	static float CalculateDragForce(float dragcoefficient, float airdensitykgm3, float surfaream2, float speedms);

	static float CalculateLiftForce(float liftcoefficient, float airdensitykgm3, float surfaream2, float speedms);

private:
	sl::Camera *currentZED;
	cv::Mat projectionMatrix;
	sl::float3 CrossProduct(sl::float3 v1, sl::float3 v2);
	sl::float3 RotateVectorAroundAxis(sl::float3 vectortorot, sl::float3 axis, float angledegrees);

};