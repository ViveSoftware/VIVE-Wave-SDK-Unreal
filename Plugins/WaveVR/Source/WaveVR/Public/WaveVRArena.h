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
#include "UObject/NoExportTypes.h"
#include "WaveVRArena.generated.h"


/**
 * @brief To decide whether vitual wall should be showed.
 */
UENUM(BlueprintType)
enum class EWVR_ArenaState : uint8
{
	ArenaVisible_Auto     = 0, /**< Show Arena while HMD is near boundaries */
	ArenaVisible_ForceOn  = 1, /**< Always show Arena */
	ArenaVisible_ForceOff = 2 /**< Never show Arena */
};

class ArenaImpl {
public:
	ArenaImpl();
	// New ArenaImpl GetInstance
	static ArenaImpl* getInstance();
	// Set Arena State
	static void ArenaState(EWVR_ArenaState type);
private:
	static ArenaImpl* mInstance;
};

UCLASS()
class WAVEVR_API UWaveVRArena : public UObject
{
	GENERATED_BODY()

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Arena",
		meta = (ToolTip = "Auto: To show the virtual wall automatically when close to the virtual wall. ForceOn: The virtual wall is forced to show. ForceOff: Do not show the virtuall wall."))
	static void WaveVR_ArenaState( EWVR_ArenaState type = EWVR_ArenaState::ArenaVisible_Auto );
};
