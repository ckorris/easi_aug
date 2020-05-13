#include <Stats.h>

//TODO: Apparently static initializers are the C++ equivalent of putting ketchup on a hotdog (ugh).
//But I use them all over here to avoid getting empty values. As I'm newer to C++, I'd like
//to research this more and see if this situation appl;ies. 

//TODO: Verify all units. 
//Speed at impact. 
static float speedMPS = 0;
float Stats::GetLatestSpeedImpact()
{
	return speedMPS;
}
void Stats::UpdateSpeedImpact(float newspeed)
{
	speedMPS = newspeed;
}

//Air pressure.
static float airPressureHPA = 0;
float Stats::GetLatestAirPressure()
{
	return airPressureHPA;
}
void Stats::UpdateAirPressure(float newpressure)
{
	airPressureHPA = newpressure;
}

//Air density.
static float airDensityKGM3 = 0;
float Stats::GetLatestAirDensity()
{
	return airDensityKGM3;
}
void Stats::UpdateAirDensity(float newdensity)
{
	airDensityKGM3 = newdensity;
}

//Air viscosity.
static float airViscosityPoise = 0; //May not be poise but I didn't comment that function well. -.-
float Stats::GetLatestAirViscosity()
{
	return airViscosityPoise;
}
void Stats::UpdateAirViscosity(float newviscosity)
{
	airViscosityPoise = newviscosity;
}

//Drag coefficient at the end of the barrel.
static float dragCoefBarrel = 0;
float Stats::GetDragCoefficientBarrel()
{
	return dragCoefBarrel;
}
void Stats::UpdateDragCoefficientBarrel(float newcoef)
{
	dragCoefBarrel = newcoef;
}

//Drag coefficient immediately before impact.
static float dragCoefImpact = 0;
float Stats::GetDragCoefficientImpact()
{
	return dragCoefImpact;
}
void Stats::UpdateDragCoefficientImpact(float newcoef)
{
	dragCoefImpact = newcoef;
}

//Drag force at the end of the barrel.
static float dragForceBarrel = 0;
float Stats::GetDragForceBarrel()
{
	return dragForceBarrel;
}
void Stats::UpdateDragForceBarrel(float newforce)
{
	dragForceBarrel = newforce;
}

//Drag force immediately before impact. 
static float dragForceImpact = 0;
float Stats::GetDragForceImpact()
{
	return dragForceImpact;
}
void Stats::UpdateDragForceImpact(float newforce)
{
	dragForceImpact = newforce;
}

//Magnus coefficient at the end of the barrel.
static float magnusCoefBarrel = 0;
float Stats::GetMagnusCoefficientBarrel()
{
	return magnusCoefBarrel;
}
void Stats::UpdateMagnusCoefficientBarrel(float newcoef)
{
	magnusCoefBarrel = newcoef;
}

//Magnus coefficient immediately before impact. 
static float magnusCoefImpact = 0;
float Stats::GetMagnusCoefficientImpact()
{
	return magnusCoefImpact;
}
void Stats::UpdateMagnusCoefficientImpact(float newcoef)
{
	magnusCoefImpact = newcoef;
}

//Magnus force at the end of the barrel.
static float magnusForceBarrel = 0;
float Stats::GetMagnusForceBarrel()
{
	return magnusForceBarrel;
}
void Stats::UpdateMagnusForceBarrel(float newforce)
{
	magnusForceBarrel = newforce;
}

//Magnus force immediately before impact. 
static float magnusForceImpact = 0;
float Stats::GetMagnusForceImpact()
{
	return magnusForceImpact;
}
void Stats::UpdateMagnusForceImpact(float newforce)
{
	magnusForceImpact = newforce; 
}