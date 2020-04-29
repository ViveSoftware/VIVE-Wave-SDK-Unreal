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
#include "WaveVRBlueprintFunctionLibrary.h"
#include "SoftwareIpd.generated.h"


/**
 *
 */

UENUM(BlueprintType)
enum class EWVR_SoftwareIpd_value : uint8
{
	EWVR_IPD_VALUE_Invalid = 0,
	EWVR_IPD_VALUE_56 = 1,
	EWVR_IPD_VALUE_63 = 2,
	EWVR_IPD_VALUE_70 = 3,
	EWVR_IPD_VALUE_ZERO   = 4
};

UCLASS(ClassGroup = (WaveVR))
class WAVEVR_API USoftwareIpd : public UObject
{
	GENERATED_BODY()

	public:

	USoftwareIpd();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SoftwareIpd",
		meta = (ToolTip = "To read the current software IPD configurations."))
	static bool do_read_ipd(FString &mComponentName, FString &isEnable, FString &value);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|SoftwareIpd",
		meta = (ToolTip = "To write the software IPD option to the configurations. The options include EWVR_IPD_VALUE_56 (0.056m), EWVR_IPD_VALUE_63 (0.063m), EWVR_IPD_VALUE_70 (0.07m) and EWVR_IPD_VALUE_ZERO (0m)."))
	static bool do_write_ipd(EWVR_SoftwareIpd_value value, bool isEnable);
};
