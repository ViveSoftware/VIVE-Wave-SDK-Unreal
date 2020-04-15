// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRUtils.h"
#include "Platforms/WaveVRLogWrapper.h"

DEFINE_LOG_CATEGORY_STATIC(WVRUtils, Display, All);

using namespace wvr::utils;

void WaveVRUtils::MatrixToQuat(WVR_Quatf * q, const WVR_Matrix4f_t &mx) {
	MatrixToQuat(mx, *q);
}

void WaveVRUtils::MatrixToQuat(const WVR_Matrix4f_t &mx, WVR_Quatf& q) {
	q = MatrixToQuat(mx);
}

WVR_Quatf WaveVRUtils::MatrixToQuat(const WVR_Matrix4f_t &mx) {
	WVR_Quatf q;
	float trace = mx.m[0][0] + mx.m[1][1] + mx.m[2][2];
	if (trace > 0) {
		float s = 0.5f / sqrtf(trace + 1.0f);
		q.w = 0.25f / s;
		q.x = (mx.m[2][1] - mx.m[1][2]) * s;
		q.y = (mx.m[0][2] - mx.m[2][0]) * s;
		q.z = (mx.m[1][0] - mx.m[0][1]) * s;
	}
	else {
		if (mx.m[0][0] > mx.m[1][1] && mx.m[0][0] > mx.m[2][2]) {
			float s = 2.0f * sqrtf(1.0f + mx.m[0][0] - mx.m[1][1] - mx.m[2][2]);
			if (s > 0) {
				q.w = (mx.m[2][1] - mx.m[1][2]) / s;
				q.x = 0.25f * s;
				q.y = (mx.m[0][1] + mx.m[1][0]) / s;
				q.z = (mx.m[0][2] + mx.m[2][0]) / s;
}
		}
		else if (mx.m[1][1] > mx.m[2][2]) {
			float s = 2.0f * sqrtf(1.0f + mx.m[1][1] - mx.m[0][0] - mx.m[2][2]);
			if (s > 0) {
				q.w = (mx.m[0][2] - mx.m[2][0]) / s;
				q.x = (mx.m[0][1] + mx.m[1][0]) / s;
				q.y = 0.25f * s;
				q.z = (mx.m[1][2] + mx.m[2][1]) / s;
			}
		}
		else {
			float s = 2.0f * sqrtf(1.0f + mx.m[2][2] - mx.m[0][0] - mx.m[1][1]);
			if (s > 0) {
				q.w = (mx.m[1][0] - mx.m[0][1]) / s;
				q.x = (mx.m[0][2] + mx.m[2][0]) / s;
				q.y = (mx.m[1][2] + mx.m[2][1]) / s;
				q.z = 0.25f * s;
			}
		}
	}
	return q;
}

void WaveVRUtils::ConvertWVRPosePairToUnrealPose(const WVR_DevicePosePair_t& pose, float meterToWorldUnit, FQuat& OutOrientation, FVector& OutPosition) {
	ConvertWVRMatrixToUnrealPose(pose.pose.poseMatrix, meterToWorldUnit, OutOrientation, OutPosition);
}

bool WaveVRUtils::VerifyMatrixQuality(const WVR_Matrix4f_t& InPoseMatrix) {
	FMatrix VerifiedMatrix = ToFMatrix(InPoseMatrix);
	// Make sure the Rotation part of the Matrix is unit length.
	if (!ensure((FMath::Abs(1.f - VerifiedMatrix.GetScaledAxis(EAxis::X).SizeSquared()) <= KINDA_SMALL_NUMBER)
		&& (FMath::Abs(1.f - VerifiedMatrix.GetScaledAxis(EAxis::Y).SizeSquared()) <= KINDA_SMALL_NUMBER)
		&& (FMath::Abs(1.f - VerifiedMatrix.GetScaledAxis(EAxis::Z).SizeSquared()) <= KINDA_SMALL_NUMBER)))
	{
		LOGD(WVRUtils, "Error: Bad pose quality. The Rotation part of Matrix is not close enough to unit length. This is for develop verify, shipping build should not print.");
		return false;  // BAD
	}
	return true;  // OK
}

void WaveVRUtils::ConvertWVRMatrixToUnrealPose(const WVR_Matrix4f_t& InPoseMatrix, float meterToWorldUnit, FQuat& OutOrientation, FVector& OutPosition) {
#if 0 //If the Rotation part of the Matrix is not near unit length enough, this will make Quat as Identity(no rotate).
	FMatrix Pose = ToFMatrix(InPoseMatrix);
	FQuat Orientation(Pose);

	OutOrientation.X = -Orientation.Z;
	OutOrientation.Y = Orientation.X;
	OutOrientation.Z = Orientation.Y;
	OutOrientation.W = -Orientation.W;

	OutOrientation.Normalize();

	FVector position = FVector(Pose.M[3][0], Pose.M[3][1], Pose.M[3][2]) * meterToWorldUnit;
#endif

#if 1 //!UE_BUILD_SHIPPING //Do not verify for performance purpose.
	VerifyMatrixQuality(InPoseMatrix);
#endif
	WVR_Quatf Quat = MatrixToQuat(InPoseMatrix);

	// GL space to Unreal space
	OutOrientation.X = -Quat.z;
	OutOrientation.Y = Quat.x;
	OutOrientation.Z = Quat.y;
	OutOrientation.W = -Quat.w;

	OutOrientation.Normalize();

	FVector position = FVector(InPoseMatrix.m[0][3], InPoseMatrix.m[1][3], InPoseMatrix.m[2][3]);

	// GL space to Unreal space
	OutPosition = CoordinateUtil::GetVector3(position, meterToWorldUnit);
}

void WaveVRUtils::CoordinatTransform(const WVR_Matrix4f_t& InPoseMatrix, FQuat& OutOrientation, FVector& OutPosition) {
	ConvertWVRMatrixToUnrealPose(InPoseMatrix, 1, OutOrientation, OutPosition);
}

void WaveVRUtils::CoordinateTransform(const WVR_Matrix4f_t& InPoseMatrix, float meterToWorldUnit, FQuat& OutOrientation, FVector& OutPosition) {
	ConvertWVRMatrixToUnrealPose(InPoseMatrix, meterToWorldUnit, OutOrientation, OutPosition);
}

// Teleport
// Where variablae BasePosition has old name CompensationOffset.
void WaveVRUtils::ApplyGamePose(
	const FQuat& InOrientation, const FVector& InPosition,
	const FQuat& BaseOrientation, const FVector& BasePosition,
	FQuat& OutOrientation, FVector& OutPosition) {

	if (!BasePosition.IsZero())
		OutPosition = InPosition - BasePosition;
	else
		OutPosition = InPosition;

	if (!BaseOrientation.IsIdentity()) {
		OutOrientation = BaseOrientation * InOrientation;
		OutPosition = BaseOrientation.RotateVector(OutPosition);
	} else {
		OutOrientation = InOrientation;
	}
}
