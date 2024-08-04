#pragma once

namespace hps
{
#pragma pack(push, 1)
	struct SampleStats
	{
		float3 HopUpCross;
		float3 HopUpNormal;
		float3 Velocity;
		float3 ChangeDueToGravity;
		float3 ChangeDueToDrag;
		float3 ChangeDueToBackspin;

		float CurrentSpinRPM;
		float SpinDragTorque;
		float AngularDragAccel;
		float MomentOfInertia;
		float SampleDecayRPM;

		float DragCoefficient;
		float DragForceNewtons;
		float LiftCoefficient;
		float LiftForceNewtons;

		float ReynoldsDragCoef;
		float ReynoldsSpinDragTorque;
	};
#pragma pack(pop)
}