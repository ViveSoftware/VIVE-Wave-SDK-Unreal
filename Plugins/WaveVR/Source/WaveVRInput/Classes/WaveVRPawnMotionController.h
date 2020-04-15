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
#include "GameFramework/Pawn.h"
#include "WaveVRPawnMotionController.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogWaveVRPawnMotionController, Log, All);

UCLASS()
class WAVEVRINPUT_API AWaveVRPawnMotionController : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AWaveVRPawnMotionController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(EditAnywhere)
	USceneComponent* OurVisibleComponent;

	// Input APIs
	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);
	void StartGrowing();
	void StopGrowing();

	// Input Parameters
	FVector CurrentPosition;
	bool bGrowing;
	//FRotator CurrentRotation;
};
