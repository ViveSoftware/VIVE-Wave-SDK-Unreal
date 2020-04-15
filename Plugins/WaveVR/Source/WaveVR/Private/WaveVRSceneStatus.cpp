// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRPrivatePCH.h"
#include "WaveVRHMD.h"
#include "PoseManagerImp.h"

#include "WaveVRBlueprintFunctionLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(WVRSceneStatus, Display, All);

void TraverseAssignedActor() {

	UWorld* const World = GWorld->GetWorld();

	if (World)
	{
		FString WaveVRPawnName = FString(TEXT("WaveVR_Pawn_C"));
		FString ControllerLoaderName = FString(TEXT("ControllerLoader_Blueprint_C"));
		bool IsWaveVRPawnExist = false;
		bool IsWaveVRControllerLoader = false;
		for(TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if(!Actor->IsPendingKill())
			{
				FString str = Actor->GetClass()->GetName();

				if (str == WaveVRPawnName) {
					IsWaveVRPawnExist = true;
					FName InteractionMode_Current = FName(TEXT("InteractionMode_Current"));
					UProperty* Property = Actor->GetClass()->FindPropertyByName(InteractionMode_Current);
					if(Property)
					{
						uint8* ptrValue = Property->ContainerPtrToValuePtr<uint8>(Actor);
						if(ptrValue)
						{
							switch (*ptrValue)
							{
								case 0: //EWVR_InteractionMode::InteractionMode_SystemDefault:
									LOGD(WVRSceneStatus, "InteractionMode_SystemDefault");
									break;
								case 1: //EWVR_InteractionMode::InteractionMode_Gaze:
									LOGD(WVRSceneStatus, "InteractionMode_Gaze");
									break;
								case 2: //EWVR_InteractionMode::InteractionMode_Controller:
									LOGD(WVRSceneStatus, "InteractionMode_Controller");
									break;
								default:
									LOGD(WVRSceneStatus, "Error : Current InteractionMode Type error.");
							}
						}
					}
				}

				if (str == ControllerLoaderName) {
					IsWaveVRControllerLoader = true;
					FName device = FName(TEXT("device"));

					UProperty* Property = Actor->GetClass()->FindPropertyByName(device);
					if(Property)
					{
						EWVR_DeviceType* ptrValue = Property->ContainerPtrToValuePtr<EWVR_DeviceType>(Actor);
						if(ptrValue)
						{
							switch (*ptrValue)
							{
								case EWVR_DeviceType::DeviceType_Controller_Right:
									LOGD(WVRSceneStatus, "Controller Right hand");
									break;
								case EWVR_DeviceType::DeviceType_Controller_Left:
									LOGD(WVRSceneStatus, "Controller Left hand");
									break;
								default:
									LOGD(WVRSceneStatus, "Error : Device Type setting error.");
							}
						}
					}
				}
			}
		}
		if(!IsWaveVRPawnExist) {
			LOGD(WVRSceneStatus, "WaveVRPawn NOT found");
		}
		if(!IsWaveVRControllerLoader) {
			LOGD(WVRSceneStatus, "WaveVRControllerLoader NOT found");
		}
	}
}

void FWaveVRHMD::SceneStatusInfo(bool IsFirstGameFrame) const {

	if(!IsFirstGameFrame || PoseMngr == nullptr) return;

	LOGD(WVRSceneStatus, "Scene Info:");

	//Check WaveVR key object Actors
	TraverseAssignedActor();

	//Tracking Origin Mogel.
	switch (PoseMngr->GetTrackingOriginModelInternal())
	{
		case WVR_PoseOriginModel::WVR_PoseOriginModel_OriginOnHead:
			LOGD(WVRSceneStatus, "Tracking Origin : WVR_PoseOriginModel_OriginOnHead");
			break;
		case WVR_PoseOriginModel::WVR_PoseOriginModel_OriginOnGround:
			LOGD(WVRSceneStatus, "Tracking Origin : WVR_PoseOriginModel_OriginOnGround");
			break;
		case WVR_PoseOriginModel::WVR_PoseOriginModel_OriginOnHead_3DoF:
			LOGD(WVRSceneStatus, "Tracking Origin : WVR_PoseOriginModel_OriginOnHead_3DoF");
			break;
		default:
			LOGD(WVRSceneStatus, "Tracking Origin : Error. Not in Category");
	}

	//SupportDOF.
	const EWVR_DeviceType DeviceTypes[3] = {
		EWVR_DeviceType::DeviceType_HMD,
		EWVR_DeviceType::DeviceType_Controller_Right,
		EWVR_DeviceType::DeviceType_Controller_Left
	};

	for(auto type : DeviceTypes) {
		int dof = UWaveVRBlueprintFunctionLibrary::GetSupportedNumOfDoF(type) == EWVR_DOF::DOF_6 ? 6 : 3;
		if (type == EWVR_DeviceType::DeviceType_HMD) {
			LOGD(WVRSceneStatus, "Supported Dof of HMD: %d", dof);
		} else if (type == EWVR_DeviceType::DeviceType_Controller_Right) {
			LOGD(WVRSceneStatus, "Supported Dof of Controller_Right: %d", dof);
		} else if (type == EWVR_DeviceType::DeviceType_Controller_Left) {
			LOGD(WVRSceneStatus, "Supported Dof of Controller_Left : %d", dof);
		}
	}

	//If tracking position or not.
	LOGD(WVRSceneStatus, "Is HDM tracking position? (%d)", UWaveVRBlueprintFunctionLibrary::IsTrackingHMDPosition());

	//Are devices connected or not.
	for(auto type : DeviceTypes) {
		bool IsConnected = UWaveVRBlueprintFunctionLibrary::IsDeviceConnected(type);
		if (type == EWVR_DeviceType::DeviceType_HMD) {
			LOGD(WVRSceneStatus, "Is HMD connected? (%d)", IsConnected);
		} else if (type == EWVR_DeviceType::DeviceType_Controller_Right) {
			LOGD(WVRSceneStatus, "Is Controller_Right connected?(%d)", IsConnected);
		} else if (type == EWVR_DeviceType::DeviceType_Controller_Left) {
			LOGD(WVRSceneStatus, "Is Controller_Left connected? (%d)", IsConnected);
		}
	}

	//Are poses of devices valid or not.
	for(auto type : DeviceTypes) {
		bool IsValid = UWaveVRBlueprintFunctionLibrary::IsDevicePoseValid(type);
		if (type == EWVR_DeviceType::DeviceType_HMD) {
			LOGD(WVRSceneStatus, "Is HMD pose valid? (%d)", IsValid);
		} else if (type == EWVR_DeviceType::DeviceType_Controller_Right) {
			LOGD(WVRSceneStatus, "Is Controller_Right pose valid?(%d)", IsValid);
		} else if (type == EWVR_DeviceType::DeviceType_Controller_Left) {
			LOGD(WVRSceneStatus, "Is Controller_Left pose valid? (%d)", IsValid);
		}
	}

	//Is left-hand mode or not.
	LOGD(WVRSceneStatus, "Is left-hand mode? (%d)", UWaveVRBlueprintFunctionLibrary::IsLeftHandedMode());
}
