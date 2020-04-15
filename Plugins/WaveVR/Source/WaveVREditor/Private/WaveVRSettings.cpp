// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRSettings.h"

#include "HAL/IConsoleManager.h"
#include "UObject/UnrealType.h"
#include "UObject/EnumProperty.h"
#include "UObject/PropertyPortFlags.h"

DEFINE_LOG_CATEGORY_STATIC(WVRSettings, Display, All);

// No, you just can't find its declaration in header.
UWaveVRSettings::UWaveVRSettings(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	bEyeTrackingEnable(false),
	bRenderMaskEnable(true),
	bFoveatedRenderingEnable(false),
	FoveatedRenderingPeripheralFOV(40),
	FoveatedRenderingPeripheralQuality(EFoveatedRenderingPeripheralQuality::High),
	DirectPreviewConnectType(EDirectPreviewConnectType::WiFi),
	sDirectPreviewDeviceWiFiIP(""),
	bDirectPreviewEnablePreviewImage(true),
	DirectPreviewUpdateFrequency(EDirectPreviewUpdateFrequency::FPS_60),
	bDirectPreviewEnableRegularlySaveImages(false),
	bAdaptiveQuality(true)
{
}


void UWaveVRSettings::PostInitProperties()
{
	Super::PostInitProperties();
}

#if WITH_EDITOR

static FName DeveloperSettingsConsoleVariableMetaFName(TEXT("ConsoleVariable"));

static void ImportConsoleVariableValues(UWaveVRSettings * settings)
{
	for (UProperty* Property = settings->GetClass()->PropertyLink; Property; Property = Property->PropertyLinkNext)
	{
		if (!Property->HasAnyPropertyFlags(CPF_Config))
		{
			continue;
		}

		const FString& CVarName = Property->GetMetaData(DeveloperSettingsConsoleVariableMetaFName);
		if (!CVarName.IsEmpty())
		{
			IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(*CVarName);
			if (CVar)
			{
				if (Property->ImportText(*CVar->GetString(), Property->ContainerPtrToValuePtr<uint8>(settings, 0), PPF_ConsoleVariable, settings) == NULL)
				{
					UE_LOG(WVRSettings, Error, TEXT("%s import failed for %s on console variable %s (=%s)"), *settings->GetClass()->GetName(), *Property->GetName(), *CVarName, *CVar->GetString());
				}
			}
			else
			{
				UE_LOG(WVRSettings, Fatal, TEXT("%s failed to find console variable %s for %s"), *settings->GetClass()->GetName(), *CVarName, *Property->GetName());
			}
		}
	}
}

static void ExportValuesToConsoleVariables(UWaveVRSettings * settings, UProperty* PropertyThatChanged)
{
	if (PropertyThatChanged)
	{
		const FString& CVarName = PropertyThatChanged->GetMetaData(DeveloperSettingsConsoleVariableMetaFName);
		if (!CVarName.IsEmpty())
		{
			IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(*CVarName);
			if (CVar && (CVar->GetFlags() & ECVF_ReadOnly) == 0)
			{
				UByteProperty* ByteProperty = Cast<UByteProperty>(PropertyThatChanged);
				if (ByteProperty != NULL && ByteProperty->Enum != NULL)
				{
					CVar->Set(ByteProperty->GetPropertyValue_InContainer(settings), ECVF_SetByProjectSetting);
				}
				else if (UEnumProperty* EnumProperty = Cast<UEnumProperty>(PropertyThatChanged))
				{
					UNumericProperty* UnderlyingProp = EnumProperty->GetUnderlyingProperty();
					void* PropertyAddress = EnumProperty->ContainerPtrToValuePtr<void>(settings);
					CVar->Set((int32)UnderlyingProp->GetSignedIntPropertyValue(PropertyAddress), ECVF_SetByProjectSetting);
				}
				else if (UBoolProperty* BoolProperty = Cast<UBoolProperty>(PropertyThatChanged))
				{
					CVar->Set((int32)BoolProperty->GetPropertyValue_InContainer(settings), ECVF_SetByProjectSetting);
				}
				else if (UIntProperty* IntProperty = Cast<UIntProperty>(PropertyThatChanged))
				{
					CVar->Set(IntProperty->GetPropertyValue_InContainer(settings), ECVF_SetByProjectSetting);
				}
				else if (UFloatProperty* FloatProperty = Cast<UFloatProperty>(PropertyThatChanged))
				{
					CVar->Set(FloatProperty->GetPropertyValue_InContainer(settings), ECVF_SetByProjectSetting);
				}
				else if (UStrProperty* StringProperty = Cast<UStrProperty>(PropertyThatChanged))
				{
					CVar->Set(*StringProperty->GetPropertyValue_InContainer(settings), ECVF_SetByProjectSetting);
				}
				else if (UNameProperty* NameProperty = Cast<UNameProperty>(PropertyThatChanged))
				{
					CVar->Set(*NameProperty->GetPropertyValue_InContainer(settings).ToString(), ECVF_SetByProjectSetting);
				}

			}
			else
			{
				UE_LOG(WVRSettings, Warning, TEXT("CVar named '%s' marked up in %s was not found or is set to read-only"), *CVarName, *settings->GetClass()->GetName());
			}
		}
	}
}

void UWaveVRSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		ExportValuesToConsoleVariables(this, PropertyChangedEvent.Property);
	}
}
#endif
