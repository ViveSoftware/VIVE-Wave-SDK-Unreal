// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRBlueprintFunctionLibrary.h"
#include "WaveVRPrivatePCH.h"
#include "WaveVRHMD.h"
#include "PoseManagerImp.h"
#include "WaveVRSplash.h"
#include "Platforms/Editor/WaveVRDirectPreview.h"
#include "Runtime/ImageWrapper/Public/IImageWrapper.h"
#include "Runtime/ImageWrapper/Public/IImageWrapperModule.h"
#include "WaveVRScreenshot.h"
#include "WaveVRUtils.h"

using namespace wvr::utils;

DEFINE_LOG_CATEGORY_STATIC(WVRBPFunLib, Display, All);


#if PLATFORM_ANDROID
extern FString GExternalFilePath;
#endif

UWaveVRBlueprintFunctionLibrary::UWaveVRBlueprintFunctionLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

WVR_DeviceType GetType(EWVR_DeviceType Type)
{
	WVR_DeviceType ResType = WVR_DeviceType_HMD;
	switch(Type) {
		case EWVR_DeviceType::DeviceType_HMD:
			ResType = WVR_DeviceType_HMD;
			break;
		case EWVR_DeviceType::DeviceType_Controller_Right:
			ResType = WVR_DeviceType_Controller_Right;
			break;
		case EWVR_DeviceType::DeviceType_Controller_Left:
			ResType = WVR_DeviceType_Controller_Left;
			break;
	}
	return ResType;
}

bool UWaveVRBlueprintFunctionLibrary::GetDevicePose(FVector& OutPosition, FRotator& OutOrientation, EWVR_DeviceType Type)
{
	PoseManagerImp* PoseMngr = PoseManagerImp::GetInstance();
	PoseManagerImp::Device* device = PoseMngr->GetDevice(GetType(Type));
	OutOrientation = device->rotation_CompoundBase;
	OutPosition = device->position_CompoundBase;
	return device->pose.pose.isValidPose;
}

void UWaveVRBlueprintFunctionLibrary::SetPosePredictEnabled(EWVR_DeviceType Type, bool enabled_position_predict, bool enabled_rotation_predict)
{
	FWaveVRAPIWrapper::GetInstance()->SetPosePredictEnabled(GetType(Type), enabled_position_predict, enabled_rotation_predict);
}

FVector UWaveVRBlueprintFunctionLibrary::GetDeviceVelocity(EWVR_DeviceType type)
{
	PoseManagerImp* PoseMngr = PoseManagerImp::GetInstance();
	PoseManagerImp::Device* device = PoseMngr->GetDevice(GetType(type));
	WVR_Vector3f_t velocity = device->pose.pose.velocity;
	return FVector(velocity.v[0], velocity.v[1], velocity.v[2]);
}

FVector UWaveVRBlueprintFunctionLibrary::GetDeviceAngularVelocity(EWVR_DeviceType type)
{
	PoseManagerImp* PoseMngr = PoseManagerImp::GetInstance();
	PoseManagerImp::Device* device = PoseMngr->GetDevice(GetType(type));
	WVR_Vector3f_t angularv = device->pose.pose.angularVelocity;
	return FVector(angularv.v[0], angularv.v[1], angularv.v[2]);
}

bool UWaveVRBlueprintFunctionLibrary::IsDevicePoseValid(EWVR_DeviceType Type)
{

#if WITH_EDITOR
	if (GIsEditor && !WaveVRDirectPreview::IsDirectPreview())
		return true;
#endif // WITH_EDITOR

	PoseManagerImp* PoseMngr = PoseManagerImp::GetInstance();
	return PoseMngr->IsDevicePoseValid(GetType(Type));
}

EWVR_DOF UWaveVRBlueprintFunctionLibrary::GetSupportedNumOfDoF(EWVR_DeviceType Type)
{
	PoseManagerImp* PoseMngr = PoseManagerImp::GetInstance();
	return PoseMngr->GetSupportedNumOfDoF(GetType(Type));
}

EWVR_DOF UWaveVRBlueprintFunctionLibrary::GetNumOfDoF(EWVR_DeviceType Type)
{
	PoseManagerImp* PoseMngr = PoseManagerImp::GetInstance();
	return PoseMngr->GetNumOfDoF(GetType(Type));
}

bool UWaveVRBlueprintFunctionLibrary::SetNumOfDoF(EWVR_DeviceType Type, EWVR_DOF DOF)
{
	PoseManagerImp* PoseMngr = PoseManagerImp::GetInstance();
	return PoseMngr->SetNumOfDoF(GetType(Type), DOF);
}

void UWaveVRBlueprintFunctionLibrary::SetTrackingHMDPosition(bool IsTrackingPosition)
{
	PoseManagerImp* PoseMngr = PoseManagerImp::GetInstance();
	return PoseMngr->SetTrackingHMDPosition(IsTrackingPosition);
}

bool UWaveVRBlueprintFunctionLibrary::IsTrackingHMDPosition()
{
	PoseManagerImp* PoseMngr = PoseManagerImp::GetInstance();
	return PoseMngr->IsTrackingHMDPosition();
}

void UWaveVRBlueprintFunctionLibrary::SetTrackingHMDRotation(bool IsTrackingRotation)
{
	PoseManagerImp* PoseMngr = PoseManagerImp::GetInstance();
	return PoseMngr->SetTrackingHMDRotation(IsTrackingRotation);
}

void UWaveVRBlueprintFunctionLibrary::SetTrackingOrigin3Dof()
{
	PoseManagerImp* PoseMngr = PoseManagerImp::GetInstance();
	return PoseMngr->SetTrackingOrigin3Dof();
}

bool UWaveVRBlueprintFunctionLibrary::IsDeviceConnected(EWVR_DeviceType Type)
{

#if WITH_EDITOR
	if (GIsEditor && !WaveVRDirectPreview::IsDirectPreview())
		return true;
	if (GIsEditor && WaveVRDirectPreview::IsDirectPreview())
		return FWaveVRAPIWrapper::GetInstance()->IsDeviceConnected((WVR_DeviceType) Type);
#endif // WITH_EDITOR

	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	if (HMD == nullptr) return false;
	return HMD->IsDeviceConnected(GetType(Type));
}

bool UWaveVRBlueprintFunctionLibrary::IsInputFocusCapturedBySystem()
{
	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	if (HMD == nullptr) return false;
	return HMD->IsFocusCapturedBySystem();
}

void UWaveVRBlueprintFunctionLibrary::InAppRecenter(ERecenterType type)
{
	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	if (HMD != nullptr) {
		switch (type)
		{
		case ERecenterType::Disabled:
			LOGI(WVRBPFunLib, "RecenterSwitchType = %s", "WVR_RecenterType_Disabled");
			FWaveVRAPIWrapper::GetInstance()->InAppRecenter(WVR_RecenterType_Disabled);
			HMD->SetInAppRecenter(false);
			break;
		case ERecenterType::YawOnly:
			LOGI(WVRBPFunLib, "RecenterSwitchType = %s", "WVR_RecenterType_YawOnly");
			FWaveVRAPIWrapper::GetInstance()->InAppRecenter(WVR_RecenterType_YawOnly);
			HMD->SetInAppRecenter(true);
			break;
		case ERecenterType::YawAndPosition:
			LOGI(WVRBPFunLib, "RecenterSwitchType = %s", "WVR_RecenterType_YawAndPosition");
			FWaveVRAPIWrapper::GetInstance()->InAppRecenter(WVR_RecenterType_YawAndPosition);
			HMD->SetInAppRecenter(true);
			break;
		case ERecenterType::RotationAndPosition:
			LOGI(WVRBPFunLib, "RecenterSwitchType = %s", "WVR_RecenterType_RotationAndPosition");
			FWaveVRAPIWrapper::GetInstance()->InAppRecenter(WVR_RecenterType_RotationAndPosition);
			HMD->SetInAppRecenter(true);
			break;
		default:
			break;
		}
	}
}

void UWaveVRBlueprintFunctionLibrary::SetPoseSimulationOption(SimulatePosition Option)
{
	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	if (HMD != nullptr) {
		LOGD(WVRBPFunLib, "SetPoseSimulationOption() %d", (uint8)Option);
		if (Option == SimulatePosition::WhenNoPosition)
		{
			FWaveVRAPIWrapper::GetInstance()->SetArmModel(WVR_SimulationType::WVR_SimulationType_Auto);
		}
		else if (Option == SimulatePosition::ForceSimulation)
		{
			FWaveVRAPIWrapper::GetInstance()->SetArmModel(WVR_SimulationType::WVR_SimulationType_ForceOn);
		}
		else
		{
			FWaveVRAPIWrapper::GetInstance()->SetArmModel(WVR_SimulationType::WVR_SimulationType_ForceOff);
		}
	}
}

void UWaveVRBlueprintFunctionLibrary::SetFollowHead(bool follow)
{
	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	if (HMD != nullptr) {
		LOGD(WVRBPFunLib, "SetFollowHead() %d", (uint8)follow);
		FWaveVRAPIWrapper::GetInstance()->SetArmSticky(follow);
	}
}

FVector UWaveVRBlueprintFunctionLibrary::ConvertToUnityVector(float x, float y, float z, float WorldToMetersScale)
{
	return CoordinateUtil::ConvertToUnityVector(x, y, z, WorldToMetersScale);
}

FVector UWaveVRBlueprintFunctionLibrary::ConvertToUnityVectorInVector(FVector vec, float WorldToMetersScale)
{
	return CoordinateUtil::ConvertToUnityVector(vec, WorldToMetersScale);
}

FVector UWaveVRBlueprintFunctionLibrary::ConvertToUnrealVector(float x, float y, float z, float WorldToMetersScale)
{
	return CoordinateUtil::ConvertToUnrealVector(x, y, z, WorldToMetersScale);
}

FVector UWaveVRBlueprintFunctionLibrary::ConvertToUnrealVectorInVector(FVector vec, float WorldToMetersScale)
{
	return CoordinateUtil::ConvertToUnrealVector(vec, WorldToMetersScale);
}

FQuat UWaveVRBlueprintFunctionLibrary::ConvertToUnityQuaternion(float w, float x, float y, float z)
{
	return CoordinateUtil::ConvertToUnityQuaternion(w, x, y, z);
}

FQuat UWaveVRBlueprintFunctionLibrary::ConvertToUnityQuaternionInQuat(FQuat quat)
{
	return CoordinateUtil::ConvertToUnityQuaternion(quat);
}

FQuat UWaveVRBlueprintFunctionLibrary::ConvertToUnityQuaternionInRotator(FRotator rotator)
{
	return CoordinateUtil::ConvertToUnityQuaternion(rotator);
}

FRotator UWaveVRBlueprintFunctionLibrary::ConvertToUnrealRotator(float w, float x, float y, float z)
{
	return CoordinateUtil::ConvertToUnrealRotator(w, x, y, z);
}

FRotator UWaveVRBlueprintFunctionLibrary::ConvertToUnrealRotatorInQuat(FQuat quat)
{
	return CoordinateUtil::ConvertToUnrealRotator(quat);
}

FRotator UWaveVRBlueprintFunctionLibrary::ConvertToUnrealRotatorInRotator(FRotator rotator)
{
	return CoordinateUtil::ConvertToUnrealRotator(rotator);
}

float UWaveVRBlueprintFunctionLibrary::getDeviceBatteryPercentage(EWVR_DeviceType type) {
	float ret = FWaveVRAPIWrapper::GetInstance()->GetDeviceBatteryPercentage((WVR_DeviceType) type);
	LOGD(WVRBPFunLib, "getDeviceBatteryPercentage() %d -> %f", (uint8)type, ret);
	return ret;
}

bool UWaveVRBlueprintFunctionLibrary::IsLeftHandedMode()
{
	bool _lefthanded = false;

	WVR_DeviceType _type = FWaveVRAPIWrapper::GetInstance()->GetDefaultControllerRole();
	_lefthanded = _type == WVR_DeviceType::WVR_DeviceType_Controller_Left ? true : false;

	return _lefthanded;
}

int UWaveVRBlueprintFunctionLibrary::GetHoveredWidgetSeqId(UUserWidget* ParentWidget, TArray<UWidget*>& ChildWidgets)
{

	int index = 0;
	ChildWidgets.Empty();

	if (ParentWidget)
	{
		UWidgetTree::GetChildWidgets(ParentWidget->GetRootWidget(), ChildWidgets);
		for (UWidget* Widget : ChildWidgets)
		{
			index += 1;
			if (Widget->IsHovered())
			{
				return index;
			}
		}
	}
	return index;
}

void UWaveVRBlueprintFunctionLibrary::EnableNeckModel(bool enable)
{
	LOGD(WVRBPFunLib, "EnableNeckModel() %d", (uint8)enable);
	FWaveVRAPIWrapper::GetInstance()->SetNeckModelEnabled(enable);
}

EWVR_InputId UWaveVRBlueprintFunctionLibrary::GetInputMappingPair(EWVR_DeviceType type, EWVR_InputId button)
{
	EWVR_InputId _id = EWVR_InputId::NoUse;
	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	if (HMD == nullptr) return _id;

	WVR_DeviceType _type = (WVR_DeviceType)type;
	WVR_InputId _btn = (WVR_InputId)button;
	if (HMD->isInputRequested(_type))
	{
		LOGD(WVRBPFunLib, "GetInputMappingPair() dev %d destination %d", (uint8)_type, (uint8)_btn);
		WVR_InputMappingPair _pair;
		bool _ret = FWaveVRAPIWrapper::GetInstance()->GetInputMappingPair(_type, _btn, &_pair);
		if (_ret)
		{
			unsigned int _capability = (unsigned int)_pair.source.capability;
			if (_capability != 0)
			{
				_id = (EWVR_InputId)_pair.source.id;
				LOGD(WVRBPFunLib, "GetInputMappingPair() WVR_GetInputMappingPair: source %d", (uint8)_id);
			}
		}
		/*
		uint32_t _size = 10;
		WVR_InputMappingPair _table[_size];
		uint32_t _count = FWaveVRAPIWrapper::GetInstance()->GetInputMappingTable(_type, _table, _size);
		for (int _i = 0; _i < (int)_count; _i++)
		{
			//LOGD(WVRBPFunLib, "GetInputMappingPair() FWaveVRAPIWrapper::GetInstance()->GetInputMappingTable: source %d, dest %d", (uint8)_table[_i].source.id, (uint8)_table[_i].destination.id);
			if (_btn == _table[_i].destination.id)
			{
				_id = (EWVR_InputId)_table[_i].source.id;
				break;
			}
		}
		*/
	}
	return _id;
}

bool UWaveVRBlueprintFunctionLibrary::IsButtonAvailable(EWVR_DeviceType type, EWVR_InputId button)
{
	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	if (HMD == nullptr) return false;

	return HMD->IsButtonAvailable((WVR_DeviceType)type, (WVR_InputId)button);
}

bool UWaveVRBlueprintFunctionLibrary::GetRenderTargetSize(FIntPoint & OutSize)
{
	LOG_FUNC();

	uint32 width = 1600, height = 900;
	FWaveVRAPIWrapper::GetInstance()->GetRenderTargetSize(&width, &height);
	OutSize.X = width;
	OutSize.Y = height;
	return true;
}

FString UWaveVRBlueprintFunctionLibrary::GetRenderModelName(EWVR_Hand hand) {
	FString retString = FString(TEXT("Generic"));

#if PLATFORM_ANDROID
	uint32_t num2 = 0;
	bool bIsLeftHanded = UWaveVRBlueprintFunctionLibrary::IsLeftHandedMode();
	char *str = new char[128];
	FString str_name = FString(TEXT("GetRenderModelName")); //Controller getParameter
	if (bIsLeftHanded) {
		if (hand == EWVR_Hand::Hand_Controller_Right) {
			num2 = FWaveVRAPIWrapper::GetInstance()->GetParameters(WVR_DeviceType_Controller_Left, TCHAR_TO_ANSI(*str_name), str, 128);
		}
		else if (hand == EWVR_Hand::Hand_Controller_Left) {
			num2 = FWaveVRAPIWrapper::GetInstance()->GetParameters(WVR_DeviceType_Controller_Right, TCHAR_TO_ANSI(*str_name), str, 128);
		}
	}
	else {
		if (hand == EWVR_Hand::Hand_Controller_Right) {
			num2 = FWaveVRAPIWrapper::GetInstance()->GetParameters(WVR_DeviceType_Controller_Right, TCHAR_TO_ANSI(*str_name), str, 128);
		}
		else if (hand == EWVR_Hand::Hand_Controller_Left) {
			num2 = FWaveVRAPIWrapper::GetInstance()->GetParameters(WVR_DeviceType_Controller_Left, TCHAR_TO_ANSI(*str_name), str, 128);
		}
	}

	retString = FString(ANSI_TO_TCHAR(str));
	delete[] str;
#endif

	return retString;
}

AActor * UWaveVRBlueprintFunctionLibrary::LoadCustomControllerModel(EWVR_DeviceType device, EWVR_DOF dof, FTransform transform) {
	// auto cls = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("/WaveVR/Blueprints/Controller_BP_3DoF_Finch"));
	// asyc load asset (materials, texture ...)
	//auto cls = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("/WaveVR/Materials/Touch_Icon_Sprite"));
	FString GenericControllerModel = FString(TEXT("WVR_CONTROLLER_GENERIC"));
	FString ControllerName = FString(TEXT(""));
	char *str = new char[30];
	FString str_name = FString(TEXT("GetRenderModelName")); //Controller getParameter
	FString BPClassName = FString(TEXT("/WaveVR/Blueprints/"));

#if PLATFORM_ANDROID
	bool bIsLeftHanded = UWaveVRBlueprintFunctionLibrary::IsLeftHandedMode();

	if (bIsLeftHanded) {
		LOGI(WVRBPFunLib, "is left hand mode");
		if (device == EWVR_DeviceType::DeviceType_Controller_Right) {
			uint32_t num2 = FWaveVRAPIWrapper::GetInstance()->GetParameters(WVR_DeviceType_Controller_Left, TCHAR_TO_ANSI(*str_name), str, 30);
		}
		else if (device == EWVR_DeviceType::DeviceType_Controller_Left) {
			uint32_t num2 = FWaveVRAPIWrapper::GetInstance()->GetParameters(WVR_DeviceType_Controller_Right, TCHAR_TO_ANSI(*str_name), str, 30);
		}
	} else {
		LOGI(WVRBPFunLib, "is not  left hand mode");
		if (device == EWVR_DeviceType::DeviceType_Controller_Right) {
			uint32_t num2 = FWaveVRAPIWrapper::GetInstance()->GetParameters(WVR_DeviceType_Controller_Right, TCHAR_TO_ANSI(*str_name), str, 30);
		}
		else if (device == EWVR_DeviceType::DeviceType_Controller_Left) {
			uint32_t num2 = FWaveVRAPIWrapper::GetInstance()->GetParameters(WVR_DeviceType_Controller_Left, TCHAR_TO_ANSI(*str_name), str, 30);
		}
	}

	ControllerName.Append(FString(ANSI_TO_TCHAR(str)));

	if (dof == EWVR_DOF::DOF_3) {
		ControllerName.Append(FString(TEXT("_3DOF_MC_")));
	}
	else if (dof == EWVR_DOF::DOF_6) {
		ControllerName.Append(FString(TEXT("_6DOF_MC_")));
	}
	else {
		WVR_NumDoF tmp = WVR_NumDoF::WVR_NumDoF_3DoF;
		if (device == EWVR_DeviceType::DeviceType_Controller_Right) {
			tmp = FWaveVRAPIWrapper::GetInstance()->GetDegreeOfFreedom(WVR_DeviceType_Controller_Right);
		}
		else if (device == EWVR_DeviceType::DeviceType_Controller_Left) {
			tmp = FWaveVRAPIWrapper::GetInstance()->GetDegreeOfFreedom(WVR_DeviceType_Controller_Left);
		}
		if (tmp == WVR_NumDoF::WVR_NumDoF_3DoF) {
			ControllerName.Append(FString(TEXT("_3DOF_MC_")));
		}
		else {
			ControllerName.Append(FString(TEXT("_6DOF_MC_")));
		}
	}

	if (device == EWVR_DeviceType::DeviceType_Controller_Right) {
		ControllerName.Append(FString(TEXT("R")));
	}
	else if (device == EWVR_DeviceType::DeviceType_Controller_Left) {
		ControllerName.Append(FString(TEXT("L")));
	}

	FString tmpName = ControllerName;

	ControllerName.Append(FString(TEXT(".")));
	ControllerName.Append(tmpName);
	ControllerName.Append(FString(TEXT("_C")));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, *ControllerName);
	LOGI(WVRBPFunLib, "ControllerName = %s", TCHAR_TO_ANSI(*ControllerName));

	BPClassName.Append(ControllerName);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, *BPClassName);
	LOGI(WVRBPFunLib, "BPClassName = %s", TCHAR_TO_ANSI(*BPClassName));
#else
	GenericControllerModel.Append(FString(TEXT("_3DOF_MC_R")));
	FString tmpName = GenericControllerModel;
	GenericControllerModel.Append(FString(TEXT(".")));
	GenericControllerModel.Append(tmpName);
	GenericControllerModel.Append(FString(TEXT("_C")));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, *GenericControllerModel);
	LOGI(WVRBPFunLib, "ControllerName = %s", TCHAR_TO_ANSI(*GenericControllerModel));

	BPClassName.Append(GenericControllerModel);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, *BPClassName);
	LOGI(WVRBPFunLib, "BPClassName = %s", TCHAR_TO_ANSI(*BPClassName));
#endif
	//load blueprints
	//UClass* cls = LoadObject< UClass >(nullptr, TEXT("/WaveVR/Blueprints/Controller_BP_3DoF_Finch.Controller_BP_3DoF_Finch_C"));
	//BPClassName = FString(TEXT("/WaveVR/Blueprints/WVR_CONTROLLER_GENERIC_3DOF_MC_R.WVR_CONTROLLER_GENERIC_3DOF_MC_R_C"));
	UClass* cls1 = LoadObject< UClass >(nullptr, *BPClassName);

	if (cls1 != nullptr) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Controller model loaded "));
		LOGI(WVRBPFunLib, "UWaveVRBlueprintFunctionLibrary::BeginPlay load model BP");
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Controller model is null, try load generic controller model "));
		LOGI(WVRBPFunLib, "Controller model is null, try load generic controller model");

		BPClassName = FString(TEXT("/WaveVR/Blueprints/"));
		GenericControllerModel = FString(TEXT("WVR_CONTROLLER_GENERIC_3DOF_MC_"));
		if (device == EWVR_DeviceType::DeviceType_Controller_Right) {
			GenericControllerModel.Append(FString(TEXT("R")));
		}
		else if (device == EWVR_DeviceType::DeviceType_Controller_Left) {
			GenericControllerModel.Append(FString(TEXT("L")));
		}
		FString tmpName2 = GenericControllerModel;
		GenericControllerModel.Append(FString(TEXT(".")));
		GenericControllerModel.Append(tmpName2);
		GenericControllerModel.Append(FString(TEXT("_C")));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, *GenericControllerModel);
		LOGI(WVRBPFunLib, "ControllerName = %s", TCHAR_TO_ANSI(*GenericControllerModel));

		BPClassName.Append(GenericControllerModel);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, *BPClassName);
		LOGI(WVRBPFunLib, "BPClassName = %s", TCHAR_TO_ANSI(*BPClassName));

		cls1 = LoadObject< UClass >(nullptr, *BPClassName);
		if (cls1 == nullptr) {
			LOGI(WVRBPFunLib, "Generic controller model is null");
			return nullptr;
		}
		else {
			LOGI(WVRBPFunLib, "Generic controller model is loaded");
		}
	}

	//UBlueprint * bp = Cast<UBlueprint>(cls);
	/*
	if (!bp) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Failed to load UClass 2  "));
		return nullptr;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("UClass LOADED!!!! 2 " + bp->GetName()));
	}*/

	//TSubclassOf<class UObject> MyItemBlueprint;

	//MyItemBlueprint = (UClass*)bp->GeneratedClass;
	UWorld* const World = GWorld->GetWorld();
	UObject* SpawnObject = nullptr;
	AActor* SpawnActor = nullptr;
	if (World) {
		FActorSpawnParameters SpawnParams;
		//SpawnParams.Instigator = this;
		SpawnActor = World->SpawnActor<AActor>(cls1, transform.GetLocation(), transform.Rotator(), SpawnParams);
		//SpawnObject = World->SpawnActor<UObject>(cls, { 0,0,0 }, { 0,0,0 }, SpawnParams);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("NO WORLD!!!!"));
	}
	delete[] str;
	return SpawnActor;
}

UTexture2D* UWaveVRBlueprintFunctionLibrary::GetTexture2DFromImageFile(
	FString imageFileName,
	FString imagePath,
	EUTex2DFmtType type)
{
	TArray<uint8> RawFileData;
	UTexture2D* UImageTexture2D = nullptr;
	FString FFName;

	LOGI(WVRBPFunLib, "Oz,(in) image file name: %s, image file path: %s", PLATFORM_CHAR(*imageFileName), PLATFORM_CHAR(*imagePath));

	if (type == EUTex2DFmtType::BMP) {
		FFName = imageFileName + "." + "bmp";
	}
	else if (type == EUTex2DFmtType::PNG) {
		FFName = imageFileName + "." + "png";
	}
	else if (type == EUTex2DFmtType::JPEG) {
		FFName = imageFileName + "." + "jpeg";
	}
	else
	{
		;;
	}

	FString androidRootDirPath = "";

#if PLATFORM_ANDROID
	TArray<FString> Folders;
	//Get external root dir path:
	GExternalFilePath.ParseIntoArray(Folders, TEXT("/"));
	for (FString Folder : Folders) {
		androidRootDirPath += FString("/..");
	}
#endif

	const FString FilePath = androidRootDirPath + imagePath + FFName;

	LOGI(WVRBPFunLib, "Oz, (out) image androidRootDirPath: %s, image file name: %s, image file path: %s", PLATFORM_CHAR(*androidRootDirPath), PLATFORM_CHAR(*imageFileName), PLATFORM_CHAR(*imagePath));

	if (FFileHelper::LoadFileToArray(RawFileData, *FilePath /*"<path to file>"*/))
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

		TSharedPtr<IImageWrapper> ImageWrapper;

		if ( type == EUTex2DFmtType::BMP ) {
			ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
		}
		else if ( type == EUTex2DFmtType::PNG ){
			ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
		}
		else if ( type == EUTex2DFmtType::JPEG ){
			ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
		}
		else {
			;;
		}

		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
		{
			const TArray<uint8>* UncompressedBGRA = NULL;
			if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
			{
				UImageTexture2D = UTexture2D::CreateTransient(  ImageWrapper->GetWidth(),
										ImageWrapper->GetHeight(),
										PF_B8G8R8A8);

				void* TextureData = UImageTexture2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedBGRA->GetData(), UncompressedBGRA->Num());
				UImageTexture2D->PlatformData->Mips[0].BulkData.Unlock();

				UImageTexture2D->UpdateResource();
			}
		}
	}
	return UImageTexture2D;
}

void UWaveVRBlueprintFunctionLibrary::SetFocusController(EWVR_DeviceType focusedController) {
		FWaveVRAPIWrapper::GetInstance()->SetFocusedController(static_cast<WVR_DeviceType>(focusedController));
}

EWVR_DeviceType UWaveVRBlueprintFunctionLibrary::GetFocusController() {
	WVR_DeviceType ret = WVR_DeviceType_Invalid;
	ret = FWaveVRAPIWrapper::GetInstance()->GetFocusedController();
	return static_cast<EWVR_DeviceType>(ret);
}

bool UWaveVRBlueprintFunctionLibrary::GetInputButtonState(EWVR_DeviceType type, EWVR_InputId id)
{
	bool _ret = false;
	_ret = FWaveVRAPIWrapper::GetInstance()->GetInputButtonState(static_cast<WVR_DeviceType>(type), static_cast<WVR_InputId>(id));
	return _ret;
}

bool UWaveVRBlueprintFunctionLibrary::GetInputTouchState(EWVR_DeviceType type, EWVR_InputId id)
{
	bool _ret = false;
	_ret = FWaveVRAPIWrapper::GetInstance()->GetInputTouchState(static_cast<WVR_DeviceType>(type), static_cast<WVR_InputId>(id));
	return _ret;
}

bool UWaveVRBlueprintFunctionLibrary::IsRenderFoveationSupport() {
	return FWaveVRAPIWrapper::GetInstance()->IsRenderFoveationSupport();
}

void UWaveVRBlueprintFunctionLibrary::EnableFoveation(bool bEnable) {
	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	if (HMD == nullptr) return;
	HMD->EnableFoveation(bEnable);
}

void UWaveVRBlueprintFunctionLibrary::SetFoveationParams(EEye Eye, float Focal_X, float Focal_Y, float FOV, EWVR_PeripheralQuality PeripheralQuality) {

	WVR_RenderFoveationParams_t FoveatParam;

	FoveatParam.focalX = Focal_X;
	FoveatParam.focalY = Focal_Y;
	switch (PeripheralQuality) {
	case EWVR_PeripheralQuality::Low:
		FoveatParam.periQuality = WVR_PeripheralQuality::WVR_PeripheralQuality_Low;
		break;
	case EWVR_PeripheralQuality::Medium:
		FoveatParam.periQuality = WVR_PeripheralQuality::WVR_PeripheralQuality_Medium;
		break;
	case EWVR_PeripheralQuality::High:
		FoveatParam.periQuality = WVR_PeripheralQuality::WVR_PeripheralQuality_High;
		break;
	}
	FoveatParam.fovealFov = FOV;

	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	if (HMD == nullptr) return;
	EStereoscopicPass eSSPEye = (Eye == EEye::LEFT) ? EStereoscopicPass::eSSP_LEFT_EYE : EStereoscopicPass::eSSP_RIGHT_EYE;
	HMD->SetFoveationParams(eSSPEye, FoveatParam);
}

bool UWaveVRBlueprintFunctionLibrary::IsAdaptiveQualityEnabled() {
	return FWaveVRAPIWrapper::GetInstance()->IsAdaptiveQualityEnabled();
}
bool UWaveVRBlueprintFunctionLibrary::EnableAdaptiveQuality(bool enable) {
	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	if (HMD == nullptr) return false;
	HMD->SetAdaptiveQualityState(enable);
	return FWaveVRAPIWrapper::GetInstance()->EnableAdaptiveQuality(enable);
}

void UWaveVRBlueprintFunctionLibrary::SetSplashParam(UTexture2D* InSplashTexture, FLinearColor BackGroundColor, float ScaleFactor, FVector2D Shift, bool EnableAutoLoading) {
	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	if (HMD == nullptr) return;
	if (InSplashTexture == nullptr) {
		LOGD(WVRBPFunLib, "SetSplashParam() Splash Texture is nullptr!");
	} else if (!InSplashTexture->Resource) {
		LOGD(WVRBPFunLib, "SetSplashParam() Resource is not valid!");
	}
	if (HMD->GetSplashScreen().IsValid()) {
		HMD->GetSplashScreen()->SetSplashParam(InSplashTexture, BackGroundColor, ScaleFactor, Shift, EnableAutoLoading);
	}
}

void UWaveVRBlueprintFunctionLibrary::ShowSplashScreen() {
	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	if (HMD == nullptr) return;
	if (HMD->GetSplashScreen().IsValid()) {
		HMD->GetSplashScreen()->Show();
	}
}

void UWaveVRBlueprintFunctionLibrary::HideSplashScreen() {
	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	if (HMD == nullptr) return;
	if (HMD->GetSplashScreen().IsValid()) {
		HMD->GetSplashScreen()->Hide();
	}
}

int UWaveVRBlueprintFunctionLibrary::GetWaveVRRuntimeVersion() {
	return FWaveVRAPIWrapper::GetInstance()->GetWaveRuntimeVersion();
}

bool UWaveVRBlueprintFunctionLibrary::IsDirectPreview() {
#if WITH_EDITOR
	return WaveVRDirectPreview::IsDirectPreview();
#endif
	return false;
}

void UWaveVRBlueprintFunctionLibrary::SetParameters(EWVR_DeviceType type, FString pchValue) {
	char* chValue = TCHAR_TO_ANSI(*pchValue);
	FWaveVRAPIWrapper::GetInstance()->SetParameters(GetType(type), chValue);
}

#pragma region ScreenshotMode
bool UWaveVRBlueprintFunctionLibrary::ScreenshotMode(EScreenshotMode ScreenshotMode) {

	ScreenshotImpl* screenshotMode = ScreenshotImpl::GetInstance();

	bool ret = false;

	switch (ScreenshotMode) {
	case EScreenshotMode::DEFAULT:
		LOGI(WVRBPFunLib, "Oz Screenshot Mode: DEFAULT");
		break;
	case EScreenshotMode::RAW:
		LOGI(WVRBPFunLib, "Oz Screenshot Mode: RAW");
		break;
	case EScreenshotMode::DISTORTED:
		LOGI(WVRBPFunLib, "Oz Screenshot Mode: DISTORTED");
		break;
	default:
		break;
	}

	ret = screenshotMode->Screenshot(ScreenshotMode);
	return ret;
}

void UWaveVRBlueprintFunctionLibrary::GetScreenshotFileInfo(FString &ImageFileName, FString &ImagePath) {

	ScreenshotImpl* screenshotMode = ScreenshotImpl::GetInstance();

	ImageFileName = screenshotMode->ScreenshotFileName;
	ImagePath = screenshotMode->ScreenshotImagePath;

	LOGI(WVRBPFunLib	, "Oz (GetScreenshotFileInfo) imageFileName: %s, imagePath: %s", PLATFORM_CHAR(*ImageFileName), PLATFORM_CHAR(*ImagePath));
}
#pragma endregion

void UWaveVRBlueprintFunctionLibrary::SimulateCPULoading(int gameThreadLoading, int renderThreadLoading) {
	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	if (HMD != nullptr)
		HMD->SimulateCPULoading((unsigned int)gameThreadLoading, (unsigned int)renderThreadLoading);
}

void UWaveVRBlueprintFunctionLibrary::EnableLateUpdate(bool enable, bool doUpdateInGameThread, float predictTimeInGameThread) {
	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	if (HMD != nullptr)
		HMD->EnableLateUpdate(enable, doUpdateInGameThread, predictTimeInGameThread);
}

bool UWaveVRBlueprintFunctionLibrary::IsLateUpdateEnabled() {
	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	return HMD != nullptr && HMD->DoesSupportLateUpdate();
}
