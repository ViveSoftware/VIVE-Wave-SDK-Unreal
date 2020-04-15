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
//#include "WaveVR.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IpdUpdateEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FIpdUpdateDelNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FIpdUpdateDelToBP);

UCLASS( ClassGroup=(WaveVR), meta=(BlueprintSpawnableComponent) )
class WAVEVR_API UIpdUpdateEvent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UIpdUpdateEvent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void receiveStatusUpdateFromNative();

	static FIpdUpdateDelNative onIpdUpdateNative;

	UPROPERTY(BlueprintAssignable, Category = "WaveVR|Event")
	FIpdUpdateDelToBP onIpdUpdate;
};
