#include <CamUtilities.h>
#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>
#include <math.h>

#include <Simulation.h>
#include <Config.h>
#include <Stats.h>


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

bool Simulation::Simulate(sl::Mat depthmat, float startspeedmps, float distbetweensamples, bool applyphysics, sl::SensorsData sensordata,
	int2& collisionpoint, float& collisiondepth, float& totaltime, bool drawline, cv::Mat& drawlinetomat, cv::Scalar linecolor)
{
	float downspeed = 0;
	totaltime = 0;

	sl::float3 camPosOffset(0, 0, 0);
	camPosOffset.x = Config::camXPos();
	camPosOffset.y = Config::camYPos();
	camPosOffset.z = Config::camZPos();

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


	//Calculate the gun upwards angle, which is the vector pointing upward from the gun.
	//This is used to calculate hop up, since the backspin of the bb will cause it to rise in this direction. 

	sl::float3 vectorup(0, 1, 0); //Could just put 0 and 1 in the math directly, but this makes code easier to read. 
	sl::float3 gunupnormal = vectorup;
	float zanglerad = Config::camZRot() * 3.14159267 / 180.0;
	gunupnormal.x = vectorup.x * cos(zanglerad) - vectorup.y * sin(zanglerad);
	gunupnormal.y = vectorup.x * sin(zanglerad) + vectorup.y * cos(zanglerad);


	//Temporarily, the inverse of gun up normal will be used for the gravity angle, until the ZED2 arrives and we have an IMU. 
	//sl::float3 gravitynormal(-gunupnormal.x, -gunupnormal.y, -gunupnormal.z);
	//Edit: I GOT MY ZED 2 MOFOS TIME FOR SUM REEL GUD FIZIKZ.
	sl::float3 gravitynormal = CamUtilities::IMUPoseToGravityVector(sensordata.imu.pose);

	//For drawing. 
	int2 lastscreenpos;
	bool drawthistime = false; //If we're drawing a line, we'll use this to alternate when we're drawing and not.


	//Downward acceleration to add each sample due to gravity.
	float timebetweendots = distbetweensamples / startspeedmps; //How long does it take for the projectile to travel between two dots? Currently assuming forward speed doesn't change.

	sl::float3 velocity = finalrotnormal * startspeedmps; //Startingvelocity
	//float downspeedaddpersample = GRAVITY_ACCEL * timebetweendots * timebetweendots; //GRAVITY_ACCEL * timebetweendots is downward accel added per dot but STILL IN MPS. Multiply again to get how much it should change. 

	//Cache the spin speed in rotations per second(rps). 
	float spinrpm = Config::hopUpRPM();


	//Upward acceleration to add each sample due to hop-up. 
	//TODO: Eventually take into effect rotational drag. 
	float hopupaddpersample = Config::hopUpRPM() * timebetweendots * timebetweendots; //Same concept as gravity. 

	//cout << "Start. tbd: " <<  timebetweendots << ", dsaps: " << downspeedaddpersample << ", vel: " << velocityps << ", frn: " << finalrotnormal << ", gravnormal: " << gravitynormal <<  endl;

	//Numbers needed for drag and hop-up/Magnus that don't change sample to sample. 
	//TODO: Don't calculate if not using physics - and also don't update stats. 
	float bbdiameterm = Config::bbDiameterMM() / 1000;
	//float crosssectionalarea = 3.14159267 * pow(Config::bbDiameterMM() / 1000 * 0.5, 2);
	float crosssectionalarea = 3.14159267 * pow(bbdiameterm * 0.5, 2);
	float tempcelsius = Config::temperatureC();
	float relhumidity01 = Config::relativeHumidity01();
	float airdensity = CalculateAirDensity(sensordata.barometer.pressure, tempcelsius, relhumidity01); //Temp values - one atmosphere, 50°C, no humidity.
	//float airdensity = 122.5; //Temp. In hectapascals. 
	float airviscosity = CalculateAirViscosity(tempcelsius);
	float bbmasskg = Config::bbMassGrams() / 1000.0;

	//DEBUG
	//float testdragforce = CalculateDragForce(0.47, 122.5, crosssectionalarea, 65);
	//cout << "Test Drag Force: " << testdragforce << "N" << endl;
	//float dragcoef = CalculateDragCoefficient(0, 120, 0.006, airdensity, 0.000198); //0.000185 using Dr's constant, poises. 
	//cout << "Drag coef: " << dragcoef << endl;
	//float airviscosity = CalculateAirViscosity(37.78); //Test at 50. 
	//cout << "Viscosity: " << airviscosity << endl;
	//sl::float3 rotangle = RotateVectorAroundAxis(sl::float3(0, 0, 1), sl::float3(1, 0, 0), 45.0);
	//cout << "New Angle: " << rotangle.x << ", " << rotangle.y << ", " << rotangle.z << endl;

	//Update stats with air values, which won't change between samples. 
	Stats::UpdateAirPressure(sensordata.barometer.pressure);
	Stats::UpdateAirDensity(airdensity);
	Stats::UpdateAirViscosity(airviscosity);


	bool isfirstsample = true;


	while (currentpoint.z < MAX_DISTANCE) //Sliiiightly concerned this will be infinite. 
	{
		//Catch potential infinite loop. 
		if (velocity.z <= 0)
		{
			//cout << "Velocity z factor is less than or equal to zero. Aborting simulation." << endl;
			break;
		}

		//Calculate the time that passes in order to travel the distance per sample. 
		float speed = sqrt(pow(velocity.x, 2) + pow(velocity.y, 2) + pow(velocity.z, 2));
		float sampletime = distbetweensamples / speed;

		//Declare physics values outside if statement so we can use them to update stats later if needed.
		//This does rely us on checking applyphysics again at that point to avoid passing inappropriate zeroes. 
		float dragcoefficient = 0;
		float dragforcenewtons = 0;
		float liftcoefficient = 0;
		float liftforcenewtons = 0;

		if (applyphysics)
		{
			sl::float3 velocitynorm = velocity / speed;

			//Gravity. Simplest first. 
			velocity += gravitynormal * GRAVITY_ACCEL * sampletime;

			//Drag. 
			//float dragcoefficient = CalculateDragCoefficient(spinrpm, speed / timebetweendots, bbdiameterm, airdensity, airviscosity); //Using per-sample speed. 
			dragcoefficient = CalculateDragCoefficient(spinrpm, speed, bbdiameterm, airdensity, airviscosity);
			dragforcenewtons = CalculateDragForce(dragcoefficient, airdensity, crosssectionalarea, speed);
			//cout <<"Air Density: " << airdensity <<  " Newtons: " << dragforcenewtons << endl;

			float dragspeedchange = dragforcenewtons * sampletime / bbmasskg;

			//velocityps -= velocityps / speedps * speedchange * timebetweendots;
			velocity -= velocitynorm * dragspeedchange;


			//Hop-up/Magnus.
			if (spinrpm != 0) //If it's not spinning, don't spend the cycles doing all this. 
			{
				//Calculate hop-up normal. It's orthogonal to the velocity. 
				//We get that by rotating around the cross product of the gun up normal and velocity by 90 degrees. 
				//Recall that the gun up normal is the upward direction of the gun, ie the direction the backspin will push the bb. 
				sl::float3 hopupcross = CrossProduct(velocity, gunupnormal);
				//Normalize it. 
				float hopupcrossmagnitude = sqrt(pow(hopupcross.x, 2) + pow(hopupcross.y, 2) + pow(hopupcross.z, 2));
				hopupcross = hopupcross / hopupcrossmagnitude;

				sl::float3 hopupnormal = RotateVectorAroundAxis(velocitynorm, hopupcross, 90); 


				liftcoefficient = CalculateLiftCoefficient(spinrpm, speed, bbdiameterm); //If change to speed works, apply to drag. 
				liftforcenewtons = CalculateLiftForce(liftcoefficient, airdensity, crosssectionalarea, speed); //Force across a second. 
				//That force is how much force will occur over a second. 
				float liftspeedchange = liftforcenewtons * sampletime / bbmasskg; //Speed change. 
				velocity += hopupnormal * liftspeedchange;

				//cout << "CL: " << liftcoefficient << " H Speed Change: " << liftspeedchange << endl;
			}

			if (isfirstsample)
			{
				Stats::UpdateDragCoefficientBarrel(dragcoefficient);
				Stats::UpdateDragForceBarrel(dragforcenewtons);
				Stats::UpdateMagnusCoefficientBarrel(liftcoefficient);
				Stats::UpdateMagnusForceBarrel(liftforcenewtons);


				isfirstsample = false;
			}

		}

		currentpoint += velocity * sampletime;

		float pointdepth = currentpoint.z; //Shorthand. 
		//totaltime += timebetweendots;
		totaltime += sampletime;

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

		bool hit = zeddepth > 0.0 && pointdepth > zeddepth; //ZED actually reports positive depth. Who woulda thinkitt? 
		if (hit)
		{
			//collisionpoint = &lastvalidpoint;
			int2 lastscreenpos = CamUtilities::CameraToScreenPos(lastvalidpoint, projectionMatrix, swidth, sheight);
			collisionpoint = lastscreenpos;
			collisiondepth = lastvalidpoint.z;

			//Update stats for the impact point. 
			Stats::UpdateSpeedImpact(speed);
			if (applyphysics)
			{
				Stats::UpdateDragCoefficientImpact(dragcoefficient);
				Stats::UpdateDragForceImpact(dragforcenewtons);
				Stats::UpdateMagnusCoefficientImpact(liftcoefficient);
				Stats::UpdateMagnusForceImpact(liftforcenewtons);
			}

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

	//Update stats to show that they are empty. 
	if (applyphysics)
	{
		Stats::UpdateDragCoefficientImpact(0);
		Stats::UpdateDragForceImpact(0);
		Stats::UpdateMagnusCoefficientImpact(0);
		Stats::UpdateMagnusForceImpact(0);
	}


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

float Simulation::CalculateAirViscosity(float tempcelsius)
{
	float tempkelvins = tempcelsius + 273.15;

	//We'll use Sutherland's formula for calculating this, which requires a reference temperature and viscosity. 
	//We'll use 0°C (273.15°L) where viscosity is 0.00001716.
	//Note we use the number 383.55, which is reference number 273.15 + the Sutherland temp of 110.4°K.
	//Used as reference: https://www.cfd-online.com/Wiki/Sutherland's_law and https://www.lmnoeng.com/Flow/GasViscosity.php

	//return 0.00001716 * pow(tempkelvins / 273.15, 1.5) * (383.55 / (tempkelvins + 110.4));
	return (0.000001458 * pow(tempkelvins, 1.5)) / (tempkelvins + 110.4);
}


float Simulation::CalculateDragCoefficient(float spinrpm, float linearspeedmps, float bbdiametermeters,
	float airdensity, float airviscosity) //Note speed in mps, not a smaller fraction. 
{
	//This equation is made to fit experimental data, from Dr. Dyrkacz's "The Physics of Paintball" which he based on 
	//Achenbach, E., J. Fluid Mech. 54,565 (1972). See his page here:
	//https://web.archive.org/web/20040617080904/http://home.comcast.net/~dyrgcmn/pball/pballIntro.html

	//Calculate the Reynolds number, which we'll need in a few places. 
	//double reynolds = bbdiametermeters * airdensity * linearspeedmps * airviscosity;
	double reynolds = bbdiametermeters * airdensity * linearspeedmps / airviscosity;
	//cout << "Reynolds: " << reynolds << endl;

	//First calculate without spin. We're gonna start using doubles from now on. 
	double spinlessCD = (0.4274794 + 0.000001146254 * reynolds - 7.559635 * pow(10, -12) * pow(reynolds, 2) - 3.817309 * pow(10, -18) *
		pow(reynolds, 3) + 2.389417 * pow(10, -23) * pow(reynolds, 4)) / (1 - 0.000002120623 * reynolds + 2.952772 * pow(10, -11) * pow(reynolds, 2) -
			1.914687 * pow(10, -16) * pow(reynolds, 3) + 3.125996 * pow(10, -22) * pow(reynolds, 4));

	//If we have any spin, we then take that and apply a new formula. 
	if (spinrpm == 0) return (float)spinlessCD; //If no spin, we can quit here. 

	//Calculate the surface speed, which to my understanding is the speed at which a point on the outside of the circle is moving. 
	float surfacespeed = bbdiametermeters * 3.14159267 * spinrpm / 60;

	//Calculate the ratio of spin velocity to linear velocity. Those two are represented as V and U mathematically, hence calling it vu.
	float vu = surfacespeed / linearspeedmps;

	//Aaaaand another crazy formula created via lots of experimentation by someone way better at the mathses than yours truly. 
	double spinCD = (spinlessCD + 2.2132291 * vu - 10.345178 * pow(vu, 2) + 16.15703 * pow(vu, 3) + -5.27306480 * pow(vu, 4)) /
		(1 + 3.1077276 * vu - 13.6598678 * pow(vu, 2) + 24.00539887 * pow(vu, 3) - 8.340493152 * pow(vu, 4));

	return (float)spinCD;
}

//Returns the magnitude of the drag force vector in Newtons. 
float Simulation::CalculateDragForce(float dragcoefficient, float airdensitykgm3, float surfaream2, float speedms)
{
	//Drag coefficient is estimated to be 0.47 with no spin, 0.43 with high spin.
	//Area for a 6mm BB is 0.000028274m^2.
	return dragcoefficient * 0.5 * airdensitykgm3 * surfaream2 * pow(speedms, 2);
}

float Simulation::CalculateLiftCoefficient(float spinrpm, float linearspeedmps, float bbdiametermeters)
{
	//This equation is made to fit experimental data, from Dr. Dyrkacz's "The Physics of Paintball" which he based on 
	//Achenbach, E., J. Fluid Mech. 54,565 (1972). See his page here:
	//https://web.archive.org/web/20040617080904/http://home.comcast.net/~dyrgcmn/pball/pballIntro.html

	//Calculate the surface speed, which to my understanding is the speed at which a point on the outside of the circle is moving. 
	float surfacespeed = bbdiametermeters * 3.14159267 * spinrpm / 60;

	//Calculate the ratio of spin velocity to linear velocity. Those two are represented as V and U mathematically, hence calling it vu.
	float vu = surfacespeed / linearspeedmps;

	//Aaaaand another crazy formula created via lots of experimentation by someone way better at the mathses than yours truly. 
	double liftcoef = (-0.0020907 - 0.208056226 * vu + 0.768791456 * pow(vu, 2) - 0.84865215 * pow(vu, 3) + 0.75365982 * pow(vu, 4)) /
		(1 - 4.82629033 * vu + 9.95459464 * pow(vu, 2) - 7.85649742 * pow(vu, 3) + 3.273765328 * pow(vu, 4));

	//cout << " VU: " << vu << " CL: " << liftcoef << endl;

	return (float)liftcoef;
}

//Returns the magnitute of the lift force from the Magnus effect (from hop-up) in Newtons.
float Simulation::CalculateLiftForce(float liftcoefficient, float airdensitykgm3, float surfaream2, float speedms)
{
	return liftcoefficient * airdensitykgm3 * pow(speedms, 2) * surfaream2;
}


sl::float3 Simulation::CrossProduct(sl::float3 v1, sl::float3 v2)
{
	sl::float3 cross;
	cross.x = v1.y * v2.z - v1.z * v2.y;
	cross.y = v1.z * v2.x - v1.x * v2.z;
	cross.z = v1.x * v2.y - v1.y * v2.x;

	return cross;
}

sl::float3 Simulation::RotateVectorAroundAxis(sl::float3 srcvec, sl::float3 axis, float angledegrees)
{
	//Convert to radians. 
	float anglerad = angledegrees * 3.14159267 / 180;

	float sinang = sin(anglerad); //Shorthand.
	float cosang = cos(anglerad); //Shorthand. 

	//Build the rotation matrix that does the job. 
	//Referencing: https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
	cv::Mat rotmatrix = cv::Mat(3, 3, CV_32FC1);

	rotmatrix.at<float>(0, 0) = cosang + pow(axis.x, 2) * (1 - cosang);
	rotmatrix.at<float>(0, 1) = axis.x * axis.y * (1 - cosang) - axis.z * cosang;
	rotmatrix.at<float>(0, 2) = axis.x * axis.z * (1 - cosang) + axis.y * sinang;

	rotmatrix.at<float>(1, 0) = axis.y * axis.x * (1 - cosang) + axis.z * sinang;
	rotmatrix.at<float>(1, 1) = cosang + pow(axis.y, 2) * (1 - cosang);
	rotmatrix.at<float>(1, 2) = axis.y * axis.z * (1 - cosang) - axis.x * sinang;

	rotmatrix.at<float>(2, 0) = axis.z * axis.x * (1 - cosang) - axis.y * sinang;
	rotmatrix.at<float>(2, 1) = axis.z * axis.y * (1 - cosang) + axis.x * sinang;
	rotmatrix.at<float>(2, 2) = cosang + pow(axis.z, 2) * (1 - cosang);

	//Multiply. 
	cv::Mat srcvecmat = cv::Mat(3, 1, CV_32FC1);
	srcvecmat.at<float>(0) = srcvec.x;
	srcvecmat.at<float>(1) = srcvec.y;
	srcvecmat.at<float>(2) = srcvec.z;

	cv::Mat multmat = rotmatrix * srcvecmat;

	return sl::float3(multmat.at<float>(0), multmat.at<float>(1), multmat.at<float>(2));


}