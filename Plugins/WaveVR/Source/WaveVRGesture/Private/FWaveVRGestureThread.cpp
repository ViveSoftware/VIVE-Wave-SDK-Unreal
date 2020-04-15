// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "FWaveVRGestureThread.h"

//***********************************************************
//Thread Worker Starts as NULL, prior to being instanced
//		This line is essential! Compiler error without it
FWaveVRGestureThread* FWaveVRGestureThread::Runnable = NULL;
//***********************************************************

FWaveVRGestureThread::FWaveVRGestureThread()
	: shutDownThread(false)
	, HMD(nullptr)
	, getSupportedFeature(false)
	, handGestureStatus(EWaveVRHandGestureStatus::UNSUPPORT)
	, handTrackingStatus(EWaveVRHandTrackingStatus::UNSUPPORT)
{
	Thread = FRunnableThread::Create(this, TEXT("FWaveVRGestureThread"));
}

FWaveVRGestureThread::~FWaveVRGestureThread()
{
	delete Thread;
	Thread = NULL;
}

//Init
bool FWaveVRGestureThread::Init()
{
	return true;
}

//Run
uint32 FWaveVRGestureThread::Run()
{
	//Initial wait before starting
	FPlatformProcess::Sleep(0.03);

	while (!shutDownThread)
	{
		if (HMD == nullptr)
		{
			HMD = GetWaveVRHMD();
		}

		if (HMD == nullptr)
			continue;

		if (!getSupportedFeature)
		{
			uint64_t supportedFeatures = HMD->GetSupportedFeatures();
			if ((supportedFeatures & (uint64_t)WVR_SupportedFeature::WVR_SupportedFeature_HandGesture) != 0)
				handGestureStatus = EWaveVRHandGestureStatus::NOT_START;
			if ((supportedFeatures & (uint64_t)WVR_SupportedFeature::WVR_SupportedFeature_HandTracking) != 0)
				handTrackingStatus = EWaveVRHandTrackingStatus::NOT_START;

			UE_LOG(LogWaveVRGestureThread, Log, TEXT("Run() supportedFeatures %d, handGestureStatus %d, handTrackingStatus %d"), (int)supportedFeatures, (int)handGestureStatus, (int)handTrackingStatus);
			getSupportedFeature = true;
		}

		bool has_action = qActions.Dequeue(action);
		if (has_action)
		{
			m_mutex.Lock();
			switch (action)
			{
			case Actions::StartGesture:
				if (handGestureStatus == EWaveVRHandGestureStatus::NOT_START ||
					handGestureStatus == EWaveVRHandGestureStatus::START_FAILURE)
				{
					handGestureStatus = EWaveVRHandGestureStatus::STARTING;
					UE_LOG(LogWaveVRGestureThread, Log, TEXT("Run() Start hand gesture."));
					WVR_Result result = HMD->StartHandGesture();
					handGestureStatus = result == WVR_Result::WVR_Success ? EWaveVRHandGestureStatus::AVAILABLE : EWaveVRHandGestureStatus::START_FAILURE;
					UE_LOG(LogWaveVRGestureThread, Log, TEXT("Run() Start hand gesture result: %d"), (uint8)result);
				}
				break;
			case Actions::StopGesture:
				if (handGestureStatus == EWaveVRHandGestureStatus::AVAILABLE)
				{
					handGestureStatus = EWaveVRHandGestureStatus::STOPING;
					UE_LOG(LogWaveVRGestureThread, Log, TEXT("Run() Stop hand gesture."));
					HMD->StopHandGesture();
					handGestureStatus = EWaveVRHandGestureStatus::NOT_START;
					UE_LOG(LogWaveVRGestureThread, Log, TEXT("Run() Hand gesture stopped."));
				}
				break;
			case Actions::StartTracking:
				if (handTrackingStatus == EWaveVRHandTrackingStatus::NOT_START ||
					handTrackingStatus == EWaveVRHandTrackingStatus::START_FAILURE)
				{
					handTrackingStatus = EWaveVRHandTrackingStatus::STARTING;
					UE_LOG(LogWaveVRGestureThread, Log, TEXT("Run() Start hand tracking."));
					WVR_Result result = HMD->StartHandTracking();
					handTrackingStatus = result == WVR_Result::WVR_Success ? EWaveVRHandTrackingStatus::AVAILABLE : EWaveVRHandTrackingStatus::START_FAILURE;
					UE_LOG(LogWaveVRGestureThread, Log, TEXT("Run() Start hand tracking result: %d"), (uint8)result);
				}
				break;
			case Actions::StopTracking:
				if (handTrackingStatus == EWaveVRHandTrackingStatus::AVAILABLE)
				{
					handTrackingStatus = EWaveVRHandTrackingStatus::STOPING;
					UE_LOG(LogWaveVRGestureThread, Log, TEXT("Run() Stop hand tracking."));
					HMD->StopHandTracking();
					handTrackingStatus = EWaveVRHandTrackingStatus::NOT_START;
					UE_LOG(LogWaveVRGestureThread, Log, TEXT("Run() Hand tracking stopped."));
				}
				break;
			default:
				break;
			}
			m_mutex.Unlock();
		}
		FPlatformProcess::Sleep(0.1);
	}

	return 0;
}

//stop
void FWaveVRGestureThread::Stop()
{
}

FWaveVRGestureThread* FWaveVRGestureThread::JoyInit()
{
	//Create new instance of thread if it does not exist
	//		and the platform supports multi threading!
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new FWaveVRGestureThread();
		UE_LOG(LogWaveVRGestureThread, Log, TEXT("JoyInit() Create new thread."));
	}
	return Runnable;
}

void FWaveVRGestureThread::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
}

void FWaveVRGestureThread::Shutdown()
{
	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = NULL;
	}
}

void FWaveVRGestureThread::StartHandGesture()
{
	qActions.Enqueue(Actions::StartGesture);
}

void FWaveVRGestureThread::StopHandGesture()
{
	qActions.Enqueue(Actions::StopGesture);
}

void FWaveVRGestureThread::RestartHandGesture()
{
	qActions.Enqueue(Actions::StopGesture);
	qActions.Enqueue(Actions::StartGesture);
}

void FWaveVRGestureThread::StartHandTracking()
{
	qActions.Enqueue(Actions::StartTracking);
}

void FWaveVRGestureThread::StopHandTracking()
{
	qActions.Enqueue(Actions::StopTracking);
}

void FWaveVRGestureThread::RestartHandTracking()
{
	qActions.Enqueue(Actions::StopTracking);
	qActions.Enqueue(Actions::StartTracking);
}
