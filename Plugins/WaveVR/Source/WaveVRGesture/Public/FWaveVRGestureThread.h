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
#include "HAL/Runnable.h"
#include "WaveVRHMD.h"
#include "WaveVRGestureEnums.h"

DEFINE_LOG_CATEGORY_STATIC(LogWaveVRGestureThread, Log, All);

class WAVEVRGESTURE_API FWaveVRGestureThread : public FRunnable
{
	FWaveVRHMD* GetWaveVRHMD() const {
		if (GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetSystemName() == TEXT("WaveVR"))
		{
			return static_cast<FWaveVRHMD*>(GEngine->XRSystem->GetHMDDevice());
		}
		return nullptr;
	}

	/** Singleton instance, can access the thread any time via static accessor, if it is active! */
	static  FWaveVRGestureThread* Runnable;

	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread;

	/** Stop this thread? Uses Thread Safe Counter */
	FThreadSafeCounter StopTaskCounter;

	enum Actions
	{
		None,
		StartGesture,
		StopGesture,
		StartTracking,
		StopTracking
	};

public:
	FWaveVRGestureThread();
	virtual ~FWaveVRGestureThread();

	// Begin FRunnable interface.
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	// End FRunnable interface

	/** Makes sure this thread has stopped properly */
	void EnsureCompletion();


	//~~~ Starting and Stopping Thread ~~~


	/*
		Start the thread and the worker from static (easy access)!
		This code ensures only 1 Prime Number thread will be able to run at a time.
		This function returns a handle to the newly started instance.
	*/
	static FWaveVRGestureThread* JoyInit();

	/** Shuts down the thread. Static so it can easily be called from outside the thread context */
	static void Shutdown();


	// ~~~ WaveVR related components ~~~
	void StartHandGesture();
	void StopHandGesture();
	void RestartHandGesture();
	bool IsHandGestureAvailable() { return handGestureStatus == EWaveVRHandGestureStatus::AVAILABLE ? true : false; }
	EWaveVRHandGestureStatus GetHandGestureStatus() { return handGestureStatus; }

	void StartHandTracking();
	void StopHandTracking();
	void RestartHandTracking();
	bool IsHandTrackingAvailable() { return handTrackingStatus == EWaveVRHandTrackingStatus::AVAILABLE ? true : false; }
	EWaveVRHandTrackingStatus GetHandTrackingStatus() { return handTrackingStatus; }

private:
	FCriticalSection m_mutex;


	// ~~~ WaveVR related components ~~~
	bool shutDownThread;
	FWaveVRHMD* HMD;
	bool getSupportedFeature;
	volatile EWaveVRHandGestureStatus handGestureStatus;
	volatile EWaveVRHandTrackingStatus handTrackingStatus;

	Actions action = Actions::None;
	TQueue<Actions> qActions;
};
