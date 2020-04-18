#include <CamUtilities.h>
#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>
#include <math.h>

#include <Simulation.h>
#include <Config.h>


using namespace sl;
using namespace std;

const float GRAVITY_ACCEL = 9.006;
const float MAX_DISTANCE = 40.0;

const float GAS_CONSTANT_DRY_AIR = 287.058; //J / (kg*K).
const float GAS_CONSTANT_WATER_VAPOR = 461.495; //J / (kg*K).

Simulation::Simulation(sl::Camera *zed)
{
	currentZED = zed;

	sl::CameraInformation info = currentZED->getCameraInformation();
	projectionMatrix = CamUtilities::GetProjectionMatrix(info);

	//barrelOffset = barreloffset; //TODO: Remove, as we'll be calculating this based on the settings. 
}

bool Simulation::Simulate(sl::Mat depthmat, float speedmps, float distbetweensamples, bool applyphysics,
	int2& collisionpoint, float& collisiondepth, float& totaltime, bool drawline, cv::Mat& drawlinetomat, cv::Scalar linecolor)
{
	float downspeed = 0;
	totaltime = 0;


	sl::float3 camPosOffset(0, 0, 0);
	camPosOffset.x = Config::camXPos();
	camPosOffset.y = Config::camYPos();
	camPosOffset.z = Config::camZPos();

	//cout << camPosOffset.x << endl;

	//sl::float3 lastvalidpoint = barrelOffset;
	//sl::float3 currentpoint = barrelOffset;
	sl::float3 lastvalidpoint = camPosOffset;
	sl::float3 currentpoint = camPosOffset;

	sl::float3 forwardnormal(0, 0, 1); //TODO: Account for rotation using Config. 

	//I'm handling the rotations without full-on 3D math because it's relative to the forward direction of the barrel,
	//where Z rot does not (yet) matter (because the IMU should account for that when we add gravity). 
	//Y rot first. 
	sl::float3 yrotnormal = forwardnormal;
	float yanglerad = Config::camYRot() * 3.14159267 / 180.0;
	yrotnormal.z = forwardnormal.z * cos(yanglerad) - forwardnormal.x * sin(yanglerad);
	yrotnormal.x = forwardnormal.z * sin(yanglerad) + forwardnormal.x * cos(yanglerad);

	sl::float3 finalrotnormal = yrotnormal;
	float xanglerad = Config::camXRot() * 3.14159267 / 180.0;
	finalrotnormal.z = yrotnormal.z * cos(xanglerad) - yrotnormal.y * sin(xanglerad);
	finalrotnormal.y = yrotnormal.z * sin(xanglerad) - yrotnormal.y * cos(xanglerad);


	//Calculate the hop-up angle, which is upward relative to the gun itself. 

	sl::float3 vectorup(0, 1, 0); //Could just put 0 and 1 in the math directly, but this makes code easier to read. 
	sl::float3 hopupnormal = vectorup;
	float zanglerad = Config::camZRot() * 3.14159267 / 180.0;
	hopupnormal.x = vectorup.x * cos(zanglerad) - vectorup.y * sin(zanglerad);
	hopupnormal.y = vectorup.x * sin(zanglerad) + vectorup.y * cos(zanglerad);

	//Temporarily, the inverse of hop normal will be used for the gravity angle, until the ZED2 arrives and we have an IMU. 
	sl::float3 gravitynormal(-hopupnormal.x, -hopupnormal.y, -hopupnormal.z);

	//For drawing. 
	int2 lastscreenpos;
	bool drawthistime = false; //If we're drawing a line, we'll use this to alternate when we're drawing and not.


	//Downward acceleration to add each sample due to gravity.
	float timebetweendots = distbetweensamples / speedmps; //How long does it take for the projectile to travel between two dots? Currently assuming forward speed doesn't change.

	//sl::float3 fvelchange = finalrotnormal * distbetweensamples; //Change in position from forward velocity each frame. 
	sl::float3 velocityps = finalrotnormal * distbetweensamples; //Startingvelocity
	float downspeedaddpersample = GRAVITY_ACCEL * timebetweendots * timebetweendots; //GRAVITY_ACCEL * timebetweendots is downward accel added per dot but STILL IN MPS. Multiply again to get how much it should change. 

	//Upward acceleration to add each sample due to hop-up. 
	//TODO: Eventually take into effect rotational drag. 
	float hopupaddpersample = Config::hopUpSpeedMPS() * timebetweendots * timebetweendots; //Same concept as gravity. 

	//cout << "Start. tbd: " <<  timebetweendots << ", dsaps: " << downspeedaddpersample << ", vel: " << velocityps << ", frn: " << finalrotnormal << ", gravnormal: " << gravitynormal <<  endl;

	//Numbers needed for drag. 
	float crosssectionalarea = 3.14159267 * pow(Config::bbDiameterMM() / 1000 * 0.5, 2);
	//float airdensity = CalculateAirDensity(1013.25, 50, 0); //Temp values - one atmosphere, 50°C, no humidity.
	float airdensity = 122.5; //Temp. In hectapascals. 
	float bbmasskg = Config::bbMassGrams() / 1000.0;

	//DEBUG
	//float testdragforce = CalculateDragForce(0.47, 122.5, crosssectionalarea, 65);
	//cout << "Test Drag Force: " << testdragforce << "N" << endl;

	//for (float d = 0; d < MAX_DISTANCE; d += fvelchange.z) //Must only add the Z component of forward normal. 
	while (currentpoint.z < MAX_DISTANCE) //Sliiiightly concerned this will be infinite. 
	{
		//Catch potential infinite loop. 
		if (velocityps.z <= 0)
		{
			//cout << "Velocity z factor is less than or equal to zero. Aborting simulation." << endl;
			break;
		}

		if (applyphysics)
		{
			//Drag. 
			float speedps = sqrt(pow(velocityps.x, 2) + pow(velocityps.y, 2) + pow(velocityps.z, 2));
			float dragforcenewtons = CalculateDragForce(0.47, airdensity, crosssectionalarea, speedps);
			//cout <<"Air Density: " << airdensity <<  " Newtons: " << dragforcenewtons << endl;
			//velocityps -= velocityps.norm() * (dragforcenewtons / bbmasskg);// *timebetweendots;
			float speedchange = (dragforcenewtons / bbmasskg);

			velocityps -= velocityps / speedps * speedchange * timebetweendots;

			//Modify velocity to account for physical forces. 
			velocityps += gravitynormal * downspeedaddpersample;;
			//Modify velocity to account for hop-up.
			velocityps += hopupnormal * hopupaddpersample;
		}

		currentpoint += velocityps;

		float pointdepth = currentpoint.z; //Shorthand. 
		totaltime += timebetweendots;

		//If it's behind the camera, don't bother calculating the rest. 
		if (pointdepth < 0)
		{
			continue;
		}

		int swidth = (int)depthmat.getWidth();
		int sheight = (int)depthmat.getHeight();

		int2 screenpos = CamUtilities::CameraToScreenPos(currentpoint, projectionMatrix, depthmat.getWidth(), depthmat.getHeight());

		//If it's outside view of the screen, we won't be able to calculate depth. 
		if (screenpos.x < 0.0 || screenpos.y < 0.0 || screenpos.x > swidth || screenpos.y > sheight)
		{
			continue;
		}

		float zeddepth;
		depthmat.getValue(screenpos.x, screenpos.y, &zeddepth);

		bool hit = pointdepth > zeddepth; //ZED actually reports positive depth. Who woulda thinkitt? 
		if (hit)
		{
			//collisionpoint = &lastvalidpoint;
			int2 lastscreenpos = CamUtilities::CameraToScreenPos(lastvalidpoint, projectionMatrix, swidth, sheight);
			collisionpoint = lastscreenpos;
			collisiondepth = lastvalidpoint.z;
			return true;
		}
		else
		{
			if (drawline)
			{
				if (drawthistime)
				{
					cv::line(drawlinetomat, cv::Point(lastscreenpos.x, lastscreenpos.y), cv::Point(screenpos.x, screenpos.y), linecolor);
				}
				drawthistime = !drawthistime;
			}

			lastvalidpoint = currentpoint;
			lastscreenpos = screenpos;
		}
	}

	//collisionpoint = &lastvalidpoint;
	collisionpoint = CamUtilities::CameraToScreenPos(lastvalidpoint, projectionMatrix, depthmat.getWidth(), depthmat.getHeight());
	collisiondepth = lastvalidpoint.z;
	//cout << "Failed. Downspeed at failure: " << downspeed << endl;
	return false; //We didn't hit anything. 
}



//Returns the air density in kg/m^3.
float Simulation::CalculateAirDensity(float totalairpressurehpa, float tempcelsius, float relhumidity01)
{
	//Calculated with: https://www.omnicalculator.com/physics/air-density#how-to-calculate-the-air-density
	float tempkelvins = tempcelsius + 273.15;
	float saturationvaporpressure = 6.1078 * pow(10, 7.5 * tempcelsius / (tempcelsius + 237.3));
	float vaporpressure = saturationvaporpressure * relhumidity01;
	float dryairpressure = totalairpressurehpa - vaporpressure;

	float dryairpressure_pa = dryairpressure * 100; //Hectopascals to Pascals.
	float vaporpressure_pa = vaporpressure * 100;

	//ρ = (pd / (Rd * T)) + (pv / (Rv * T))

	float airdensity = (dryairpressure_pa / (GAS_CONSTANT_DRY_AIR * tempkelvins)) + (vaporpressure_pa / (GAS_CONSTANT_WATER_VAPOR * tempkelvins));
	return airdensity;
}

//Returns the magnitude of the drag force vector in newtons. 
float Simulation::CalculateDragForce(float dragcoefficient, float airdensitykgm3, float surfaream2, float speedms)
{
	//Drag coefficient is estimated to be 0.47 with no spin, 0.43 with high spin.
	//Area for a 6mm BB is 0.000028274m^2.

	float dragforce = dragcoefficient * 0.5 * airdensitykgm3 * surfaream2 * pow(speedms, 2);
	return dragforce;
}