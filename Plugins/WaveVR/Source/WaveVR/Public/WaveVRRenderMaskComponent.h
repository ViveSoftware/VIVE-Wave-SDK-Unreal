// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "WaveVRRenderMaskComponent.generated.h"

/**
 *  RenderMask help to reduce rendering on a part of Eye texture pixels which 
 *  can not be seen by player.  If HMD support the LateUpdate, this RenderMask
 *  will be disabled because the LateUpdate may cause RenderMask's position
 *  abnormal.
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class WAVEVR_API UWaveVRRenderMaskComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UWaveVRRenderMaskComponent(const FObjectInitializer& ObjectInitializer);
	virtual void PostLoad() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// This is used for debug.  Setting a color instead of black will help developer to see if it is working.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	FLinearColor Color;

	// This is used for debug.  On Editor or some device didn't have RenderMask, set UseDebugMesh true to show a debug mesh.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	bool UseDebugMesh;

	// This is work if UseDebugMesh is true.  This will indicate the left and right eye's mesh, and make them more obviously.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	bool UseEyeSpecifiedMesh;

public:
	class RenderMaskData {
	public:
		int vertexFloatCount;
		int indexIntCount;

		float * vertices;
		int * indices;
	};

public:
	UFUNCTION()
	void OnIpdBroadcast();

private:
	RenderMaskData GetRenderMaskData(int e);

private:
	float* vertexDataL;
	int* indexDataL;
	float* vertexDataR;
	int* indexDataR;
	uint32_t vertexCountL = 0, triangleCountL = 0;
	uint32_t vertexCountR = 0, triangleCountR = 0;
	bool GetStencilMesh();
	void CreateMesh();
};
