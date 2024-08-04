#pragma once

namespace hps
{
	struct PhysicsArgs
	{
		float BBDiameterMM;

		float BBMassGrams;

		float StartSpeedMPS;

		float SpinRPM;

		float TemperatureCelsius;

		float RelativeHumidity01;

		float PressureHPa;

		float BBToAirFrictionCoefficient;
	};
}