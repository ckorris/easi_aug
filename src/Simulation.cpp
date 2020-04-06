#include <CamUtilities.h>
#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>

#include <Simulation.h>

using namespace sl;
using namespace std;

const float GRAVITY_ACCEL = 9.00665;
const float MAX_DISTANCE = 40.0;

Simulation::Simulation(sl::Camera *zed, sl::float3 barreloffset)
{
	currentZED = zed;

	sl::CameraInformation info = currentZED->getCameraInformation();
	projectionMatrix = CamUtilities::GetProjectionMatrix(info);

	barrelOffset = barreloffset;
}

bool Simulation::Simulate(sl::Mat depthmat, float speedmps, float distbetweensamples, bool applygravity, int2& collisionpoint, float& collisiondepth)
{
	float downspeed = 0;

	float timebetweendots = distbetweensamples / speedmps; //How long does it take for the projectile to travel between two dots? Currently assuming forward speed doesn't change.
	float downspeedaddpersample = GRAVITY_ACCEL * timebetweendots;

	sl::float3 lastvalidpoint = barrelOffset;
	sl::float3 currentpoint = barrelOffset;

	for (float d = 0; d < MAX_DISTANCE; d += distbetweensamples)
	{
		//Add forward and down speeds to position. 
		currentpoint += sl::float3(0, 0, distbetweensamples);
		if (applygravity)
		{
			currentpoint += sl::float3(0, -downspeed * timebetweendots, 0);
			//currentpoint += sl::float3(0, -downspeedaddpersample, 0); //WRONG but testing. 
			downspeed += downspeedaddpersample;
		}

		float pointdepth = currentpoint.z; //Shorthand. 

		//If it's behind the camera, don't bother calculating the rest. 
		if (pointdepth < 0) 
		{
			continue;
		}

		int swidth = depthmat.getWidth();
		int sheight = depthmat.getHeight();

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
			lastvalidpoint = currentpoint;
		}
	}

	//collisionpoint = &lastvalidpoint;
	collisionpoint = CamUtilities::CameraToScreenPos(lastvalidpoint, projectionMatrix, depthmat.getWidth(), depthmat.getHeight());
	collisiondepth = lastvalidpoint.z;
	//cout << "Failed. Downspeed at failure: " << downspeed << endl;
	return false; //We didn't hit anything. 
}