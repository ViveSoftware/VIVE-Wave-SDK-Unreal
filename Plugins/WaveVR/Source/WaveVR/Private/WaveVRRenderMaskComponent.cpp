// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRRenderMaskComponent.h"
#include "WaveVRPrivatePCH.h"
#include "WaveVRHMD.h"
#include "Engine.h"

#include "Platforms/WaveVRAPIWrapper.h"
#include "Platforms/WaveVRLogWrapper.h"

#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "Logging/LogMacros.h"
#include "Math/UnrealMathUtility.h"
#include "IXRTrackingSystem.h"
#include "IpdUpdateEvent.h"

DEFINE_LOG_CATEGORY_STATIC(RenderMask, Display, All);

#if PLATFORM_ANDROID
#define PLATFORM_CHAR(str) TCHAR_TO_UTF8(str)
#else
#define PLATFORM_CHAR(str) str
#endif

UWaveVRRenderMaskComponent::UWaveVRRenderMaskComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), Color(), UseDebugMesh(false), UseEyeSpecifiedMesh(false) {
	CastShadow = 0;
	bReceiveMobileCSMShadows = 0;
}

// Right hand rule
static FMatrix MakeGLProjection(float Left, float Right, float Top, float Bottom, float ZNear, float ZFar = 1000) {
	float SumRL = Right + Left;
	float SumTB = Top + Bottom;
	float SubRL = Right - Left;
	float SubTB = Top - Bottom;

	auto matrix = FMatrix(
		FPlane(2 * ZNear / SubRL, 0.0f, 0.0f, 0.0f),
		FPlane(0.0f, 2 * ZNear / SubTB, 0.0f, 0.0f),
		FPlane(SumRL / SubRL, SumTB / SubTB, -(ZFar + ZNear) / (ZFar - ZNear), -1.0f),
		FPlane(0.0f, 0.0f, -2 * ZFar * ZNear / (ZFar - ZNear), 0.0f)
	);
	auto tostr = matrix.ToString();
	LOGD(RenderMask, "Projection Matrix %s", PLATFORM_CHAR(*tostr));
	return matrix;
}

static void MakeTriangle(const float * verticesF, const int verticeFloatCount, const int * indicesI, const int indiceIntCount, int indexOffset, TArray<FVector> &vertices, TArray<FVector> &normals, TArray<int32> &triangles, TArray<FVector2D> &uvs, TArray<FLinearColor> & colors, TArray<FProcMeshTangent> tangents);

void UWaveVRRenderMaskComponent::PostLoad()
{
	Super::PostLoad();
	PrimaryComponentTick.bCanEverTick = true;
}

// Called every frame
void UWaveVRRenderMaskComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//LOGI(RenderMask, "TickComponent once.");

	bool isRenderInitialized = false;
	bool bLateUpdate = false;
	FWaveVRHMD* hmd = FWaveVRHMD::GetInstance();
	if (hmd != nullptr) {
		isRenderInitialized = hmd->IsRenderInitialized();
		bLateUpdate = hmd->DoesSupportLateUpdate();
	}

	if (!isRenderInitialized)
		return;

	if (GetStencilMesh())
	{
		auto parent = GetAttachParent();
		FString parentName;
		if (parent != nullptr)
			parentName = parent->GetName();
		//LOGI(RenderMask, "CreateMesh: Set RelativeLocation (%f, %f, %f) to %s", RelativeLocation.X, RelativeLocation.Y, RelativeLocation.Z, PLATFORM_CHAR(*parentName));

		CreateMesh();

		UIpdUpdateEvent::onIpdUpdateNative.AddDynamic(this, &UWaveVRRenderMaskComponent::OnIpdBroadcast);

		delete[] vertexDataL;
		delete[] indexDataL;
		delete[] vertexDataR;
		delete[] indexDataR;
		vertexDataL = nullptr;
		indexDataL = nullptr;
		vertexDataR = nullptr;
		indexDataR = nullptr;

		LOGI(RenderMask, "CreateMesh: RelativeLocation (%f, %f, %f) to %s", GetRelativeLocation().X, GetRelativeLocation().Y, GetRelativeLocation().Z, PLATFORM_CHAR(*parentName));
	}

	if (bLateUpdate && IsVisible())
		SetVisibility(false, true);

	// Disable this component's tick on function
	PrimaryComponentTick.bCanEverTick = false;
	SetComponentTickEnabled(false);
	PrimaryComponentTick.SetTickFunctionEnable(false);
}

static void MakeTriangle(const float * verticesF, const int verticeFloatCount, const int * indicesI, const int indiceIntCount, int indexOffset, TArray<FVector> &vertices, TArray<FVector> &normals, TArray<int32> &triangles, TArray<FVector2D> &uvs, TArray<FLinearColor> & colors, TArray<FProcMeshTangent> tangents)
{
	auto tangent = FProcMeshTangent(0, 1, 0);
	const int verticesCount = verticeFloatCount / 3;
	for (int i = 0; i < verticesCount; i++) {
		// Set z to -1.0f which is the near plane value in GL right hand rule clipping space.
		vertices.Add(FVector(verticesF[3 * i + 0], verticesF[3 * i + 1], -1.0f));
		normals.Add(FVector(-1, 0, 0));
		uvs.Add(FVector2D(0, 0));
		colors.Add(FLinearColor::Black);
		tangents.Add(tangent);
	}

	for (int i = 0; i < indiceIntCount; i++) {
		triangles.Add(indicesI[i] + indexOffset);
	}

	return;
}

static void MakeTriangle(int shape, float offsetY, float offsetZ, float size, TArray<FVector> &vertices, TArray<FVector> &normals, TArray<int32> &triangles, int & index, TArray<FVector2D> &uvs, TArray<FLinearColor> & colors, FLinearColor Color, TArray<FProcMeshTangent> tangents)
{
	/**
	 *  A B
	 *  C D
	 *  0-2   2  0   0-2
	 *  |o   o|  |o   o|
	 *  1   0-1  1-2   1
	 */

	FVector A(0, offsetY - size, offsetZ + size);
	FVector B(0, offsetY + size, offsetZ + size);
	FVector C(0, offsetY - size, offsetZ - size);
	FVector D(0, offsetY + size, offsetZ - size);

	switch (shape) {
	case 0:
		vertices.Add(A);
		vertices.Add(C);
		vertices.Add(B);
		break;
	case 1:
		vertices.Add(C);
		vertices.Add(D);
		vertices.Add(B);
		break;
	case 2:
		vertices.Add(A);
		vertices.Add(C);
		vertices.Add(D);
		break;
	case 3:
		vertices.Add(A);
		vertices.Add(D);
		vertices.Add(B);
		break;
	}
	normals.Add(FVector(-1, 0, 0));
	normals.Add(FVector(-1, 0, 0));
	normals.Add(FVector(-1, 0, 0));

	triangles.Add(index);
	triangles.Add(index + 1);
	triangles.Add(index + 2);
	index += 3;

	uvs.Add(FVector2D(1, 0));
	uvs.Add(FVector2D(1, 1));
	uvs.Add(FVector2D(0, 0));

	colors.Add(Color);
	colors.Add(Color);
	colors.Add(Color);

	auto tangent = FProcMeshTangent(0, 1, 0);
	tangents.Add(tangent);
	tangents.Add(tangent);
	tangents.Add(tangent);
}

#define RM_R 1.04f
#define RM_P0 0.52
#define RM_P1 0.90666
#define RM_Z -1  // near plane value in right hand rule clipping space

UWaveVRRenderMaskComponent::RenderMaskData UWaveVRRenderMaskComponent::GetRenderMaskData(int eye) {
	bool useDebugMesh = UseDebugMesh;

#if WITH_EDITOR
	useDebugMesh = true;
#endif

	if (useDebugMesh)
	{
		/**
		 *   15    11   0    1     12
		 *   +----------+----------+
		 *   |    _+    |    +_    |
		 *   |  -   -   |   -   -  |
		 * 10| + _   -  |  -   _ + |2
		 *   |-    -  - | -  -     |
		 *  9+----------*----------+3
		 *   |-   _ - - | - - _   -|
		 *  8| +     -  |  -     + |4
		 *   |  -_  -   |   -  _-  |
		 *   |    -+_   |   _+-    |
		 *   +----------+----------+
		 *   14    7    6    5     13
		 *
		**/
		static float verticesF[] {
			// XYZ
			// Circle start from Top
			0,			RM_R,	RM_Z,  // 0
			RM_P0,		RM_P1,	RM_Z,  // 1
			RM_P1,		RM_P0,	RM_Z,  // 2
			RM_R,		0,		RM_Z,  // 3
			RM_P1,		-RM_P0,	RM_Z,  // 4
			RM_P0,		-RM_P1,	RM_Z,  // 5
			0,			-RM_R,	RM_Z,  // 6
			-RM_P0,		-RM_P1,	RM_Z,  // 7
			-RM_P1,		-RM_P0,	RM_Z,  // 8
			-RM_R,		0,		RM_Z,  // 9
			-RM_P1,		RM_P0,	RM_Z,  // 10
			-RM_P0,		RM_P1,	RM_Z,  // 11

			// Corner
			 RM_R,   RM_R, RM_Z,  // 12
			 RM_R,  -RM_R, RM_Z,  // 13
			-RM_R,  -RM_R, RM_Z,  // 14
			-RM_R,   RM_R, RM_Z,  // 15

			0, 0, RM_Z  // 16 center
		};

		static int indexI[] {
			0, 1, 12,
			1, 2, 12,
			2, 3, 12,
			3, 4, 13,
			4, 5, 13,
			5, 6, 13,
			6, 7, 14,
			7, 8, 14,
			8, 9, 14,
			9, 10, 15,
			10, 11, 15,
			11, 0, 15,

			16, 16, 16
		};

		if (UseEyeSpecifiedMesh) {
			if (eye == 0) {
				indexI[36] = 16;
				indexI[37] = 5;
				indexI[38] = 4;
			}
			else {
				indexI[36] = 16;
				indexI[37] = 8;
				indexI[38] = 7;
			}
		}

		RenderMaskData data;
		data.vertexFloatCount = 16 * 3 + (UseEyeSpecifiedMesh ? 3 : 0);
		data.indexIntCount = 12 * 3 + (UseEyeSpecifiedMesh ? 3 : 0);
		data.vertices = verticesF;
		data.indices = indexI;

		return data;
	}
	else
	{
		RenderMaskData data;
		if (eye == 0)
		{
			data.vertexFloatCount = vertexCountL * 3;
			data.indexIntCount = triangleCountL * 3;
			data.vertices = vertexDataL;
			data.indices = indexDataL;
		}
		else if (eye == 1)
		{
			data.vertexFloatCount = vertexCountR * 3;
			data.indexIntCount = triangleCountR * 3;
			data.vertices = vertexDataR;
			data.indices = indexDataR;
		}
		else
		{
			data.vertexFloatCount = 0;
			data.indexIntCount = 0;
			data.vertices = nullptr;
			data.indices = nullptr;
		}
		return data;
	}
}

bool UWaveVRRenderMaskComponent::GetStencilMesh()
{
#if WITH_EDITOR
	if (GIsEditor)
		return true;
#endif
	if (UseDebugMesh)
		return true;

	// Left
	FWaveVRAPIWrapper::GetInstance()->GetStencilMesh(WVR_Eye_Left, &vertexCountL, &triangleCountL, 0, NULL, 0, NULL);
	if (vertexCountL <= 0 || vertexCountL > 0xFF || triangleCountL <= 0 || triangleCountL > 0xFF)
	{
		return false;
	}
	LOGD(RenderMask, "GetStencilMesh() Left VCount = %d, TCount = %d", vertexCountL, triangleCountL);

	vertexDataL = new float[vertexCountL * 3];
	indexDataL = new int[triangleCountL * 3];

	FWaveVRAPIWrapper::GetInstance()->GetStencilMesh(WVR_Eye_Left, &vertexCountL, &triangleCountL, vertexCountL * 3, vertexDataL, triangleCountL * 3, indexDataL);


	// Right
	FWaveVRAPIWrapper::GetInstance()->GetStencilMesh(WVR_Eye_Left, &vertexCountR, &triangleCountR, 0, NULL, 0, NULL);
	if (vertexCountR <= 0 || vertexCountR > 0xFF || triangleCountR <= 0 || triangleCountR > 0xFF)
	{
		return false;
	}
	LOGD(RenderMask, "GetStencilMesh() Right VCount = %d, TCount = %d", vertexCountR, triangleCountR);

	vertexDataR = new float[vertexCountR * 3];
	indexDataR = new int[triangleCountR * 3];

	FWaveVRAPIWrapper::GetInstance()->GetStencilMesh(WVR_Eye_Right, &vertexCountR, &triangleCountR, vertexCountR * 3, vertexDataR, triangleCountR * 3, indexDataR);
	return true;
}

void UWaveVRRenderMaskComponent::CreateMesh() {
	//LOGI(RenderMask, "CreateMesh");

	// We need the projection used by HMD.  Invert it and multiply with vertices.
	FMatrix InvProj[2];

	const float WorldUnitToMeter = GWorld->GetWorldSettings()->WorldToMeters;
	const float MeterToWorldUnit = 1 / WorldUnitToMeter;

	const float ZNear = GNearClippingPlane / WorldUnitToMeter;  // in meter

	// For editor
	const float FOV = 90;
	const float Tangent = ZNear * FMath::Tan(FMath::DegreesToRadians(FOV / 2));

	float L = -Tangent, R = Tangent, T = Tangent, B = -Tangent;

	const WVR_Eye eyes[] = {WVR_Eye_Left, WVR_Eye_Right};

	// Find the vertices not on the clipping space boundary.
	const float ZDistance = ZNear + 0.01f;  // Near clipping plane plus 1cm.  Where the WaveVRRenderMaskComponent place at front of head.

	SetRelativeLocation(FVector(ZDistance * WorldUnitToMeter, 0, 0));
	SetRelativeRotation(FRotator::ZeroRotator);
	SetRelativeScale3D(FVector::OneVector);
	// If use SetRelativeLocationAndRotation, the value will not be set exactly.  Some shift may happen when level is first level.
	//SetRelativeLocationAndRotation(FVector(ZDistance * WorldUnitToMeter, 0, 0), FQuat::Identity);

	LOGI(RenderMask, "WorldUnitToMeter %f, ZNear %f, ZDistance %f", WorldUnitToMeter, ZNear, ZDistance);

	// Transform from right hand rule to Unreal World
	FMatrix AxisChangeMatrix = FMatrix::Identity;
	AxisChangeMatrix.M[0][0] = 0, AxisChangeMatrix.M[0][1] = 1, AxisChangeMatrix.M[0][2] = 0;
	AxisChangeMatrix.M[1][0] = 0, AxisChangeMatrix.M[1][1] = 0, AxisChangeMatrix.M[1][2] = 1;
	AxisChangeMatrix.M[2][0] = -1, AxisChangeMatrix.M[2][1] = 0, AxisChangeMatrix.M[2][2] = 0;

	UMaterialInterface * MaterialMask = (UMaterialInterface*)LoadObject<UMaterial>(nullptr, TEXT("Material'/WaveVR/Materials/WaveVRRenderMask.WaveVRRenderMask'"));
	if (MaterialMask == nullptr)
	{
		LOGE(RenderMask, "Can't find WaveVRRenderMask.WaveVRRenderMask");
		return;
	}

	IXRTrackingSystem* XRSystem = GEngine->XRSystem.Get();
	EHMDTrackingOrigin::Type dofType = XRSystem->GetTrackingOrigin();

	for (int e = 0; e < 2; e++)
	{
		WVR()->GetClippingPlaneBoundary(eyes[e], &L, &R, &T, &B);
		L *= ZNear; R *= ZNear; T *= ZNear; B *= ZNear;  // In WVR_GetClippingPlaneBoundary, the near value is assumed as 1.
		LOGI(RenderMask, "eye%d LRTB (%f, %f, %f, %f)", e, L, R, T, B);

		InvProj[e] = MakeGLProjection(L, R, T, B, ZNear, 1000).Inverse();

		// Load RenderMask Mesh from API
		TArray<FVector> vertices;
		TArray<FVector> normals;
		TArray<int32> triangles;
		TArray<FVector2D> uvs;
		TArray<FLinearColor> colors;
		TArray<FProcMeshTangent> tangents;

		// Put array to TArray
		const RenderMaskData data = GetRenderMaskData(e);
		if (data.vertexFloatCount <= 0 || data.indexIntCount < 0)
			continue;
		//LOGD(RenderMask, "MakeTriangle");
		MakeTriangle(data.vertices, data.vertexFloatCount, data.indices, data.indexIntCount, 0, vertices, normals, triangles, uvs, colors, tangents);


		//LOGD(RenderMask, "GL world to Unreal world");
		// Convert from
		TArray<FVector> newVertices;
		int N = data.vertexFloatCount / 3;
		for (int i = 0; i < N; i++) {
			auto v = InvProj[e].TransformFVector4(FVector4(vertices[i].X, vertices[i].Y, vertices[i].Z, 1));
			v = v / v.W;

			// v will be a vector in rhr world space.  convert to unreal world.  The mesh will put to ZDistance from camera.  no need the z.
			// World axis change & No need Z
			auto newVertex = AxisChangeMatrix.TransformVector((FVector(v.X, v.Y, 0) / ZNear * ZDistance) * WorldUnitToMeter);

			newVertices.Add(newVertex);
		}

		//LOGD(RenderMask, "Generate Mesh in ProceduralMeshComponent");
		CreateMeshSection_LinearColor(e, newVertices, triangles, normals, uvs, colors, tangents, false);
		UMaterialInstanceDynamic * dynamic = UMaterialInstanceDynamic::Create(MaterialMask, this);
		if (dynamic == nullptr)
		{
			LOGE(RenderMask, "Can't create MaterialInstanceDynamic");
			return;
		}

		//LOGD(RenderMask, "Set IPD");
		FQuat e2hOrientation;
		FVector e2hPosition;
		XRSystem->GetRelativeEyePose(IXRTrackingSystem::HMDDeviceId, e == 0 ? eSSP_LEFT_EYE : eSSP_RIGHT_EYE, e2hOrientation, e2hPosition);
		// The camera space is used in EyeToHeadOffset.  The camera space is in left hand rule.  X is not nagative.
		dynamic->SetVectorParameterValue("EyeToHeadOffset", FLinearColor(e2hPosition.Y, e2hPosition.Z, e2hPosition.X, 1));
		LOGI(RenderMask, "EyeToHeadOffsetPos%d (X,Y,Z)= (%f, %f, %f)", e, e2hPosition.Y, e2hPosition.Z, e2hPosition.X);

		dynamic->SetScalarParameterValue("EyeSpecify", e == 0 ? -1 : 1);
		dynamic->SetVectorParameterValue("Color", Color);

		//LOGD(RenderMask, "Set Material");
		SetMaterial(e, dynamic);
	}
	ContainsPhysicsTriMeshData(false);
}

void UWaveVRRenderMaskComponent::OnIpdBroadcast() {

	LOGI(RenderMask, "OnIpdBroadcast");

	bool bLateUpdate = false;
	FWaveVRHMD* hmd = FWaveVRHMD::GetInstance();
	if (hmd != nullptr)
		bLateUpdate = hmd->DoesSupportLateUpdate();
	if (bLateUpdate && IsVisible())
		SetVisibility(false, true);

	IXRTrackingSystem* XRSystem = GEngine->XRSystem.Get();
	for (int e = 0; e < 2; e++) {
		FQuat e2hOrientation;
		FVector e2hPosition;
		XRSystem->GetRelativeEyePose(IXRTrackingSystem::HMDDeviceId, e == 0 ? eSSP_LEFT_EYE : eSSP_RIGHT_EYE, e2hOrientation, e2hPosition);
		auto material = dynamic_cast<UMaterialInstanceDynamic*>(GetMaterial(e));
		check(material);
		material->SetVectorParameterValue("EyeToHeadOffset", FLinearColor(e2hPosition.Y, e2hPosition.Z, -e2hPosition.X, 1));
		LOGI(RenderMask, "EyeToHeadOffsetPos%d (X,Y,Z)= (%f, %f, %f)", e, e2hPosition.Y, e2hPosition.Z, -e2hPosition.X);
	}
}
