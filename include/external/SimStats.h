#pragma once

namespace hps
{
	struct SimStats
	{
		float SpeedImpact;

		float AirPressureHPa;

		float AirDensityKGM3;

		float AirViscosityPaS;

		float DragCoefficientBarrel;

		float DragCoefficientImpact;

		float DragForceBarrel;

		float DragForceImpact;

		float MagnusCoefficientBarrel;

		float MagnusCoefficientImpact;

		float MagnusForceBarrel;

		float MagnusForceImpact;
	};
}