#pragma once

class Stats
{
public:
	static float GetLatestSpeedImpact();
	static void UpdateSpeedImpact(float newspeed);
	
	static float GetLatestAirPressure();
	static void UpdateAirPressure(float newpressure);

	static float GetLatestAirDensity();
	static void UpdateAirDensity(float newdensity);

	static float GetLatestAirViscosity();
	static void UpdateAirViscosity(float newviscosity);

	static float GetDragCoefficientBarrel();
	static void UpdateDragCoefficientBarrel(float newcoef);

	static float GetDragCoefficientImpact();
	static void UpdateDragCoefficientImpact(float newcoef);

	static float GetDragForceBarrel();
	static void UpdateDragForceBarrel(float newforce);

	static float GetDragForceImpact();
	static void UpdateDragForceImpact(float newforce);

	static float GetMagnusCoefficientBarrel();
	static void UpdateMagnusCoefficientBarrel(float newcoef);

	static float GetMagnusCoefficientImpact();
	static void UpdateMagnusCoefficientImpact(float newcoef);

	static float GetMagnusForceBarrel();
	static void UpdateMagnusForceBarrel(float newforce);

	static float GetMagnusForceImpact();
	static void UpdateMagnusForceImpact(float newforce);
};