// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRArena.h"
#include "WaveVRPrivatePCH.h"

// class ArenaImpl
ArenaImpl* ArenaImpl::mInstance = nullptr;

ArenaImpl::ArenaImpl() {
}

ArenaImpl* ArenaImpl::getInstance() {

	if (mInstance == nullptr) {
		mInstance = new ArenaImpl();
	}

	return mInstance;
}

// Set Arena State
void ArenaImpl::ArenaState ( EWVR_ArenaState type ) {

	WVR_ArenaVisible aVisible = WVR_ArenaVisible_Auto;

	switch(type){
		case EWVR_ArenaState::ArenaVisible_Auto:
			aVisible = WVR_ArenaVisible_Auto;
			break;
		case EWVR_ArenaState::ArenaVisible_ForceOn:
			aVisible = WVR_ArenaVisible_ForceOn;
			break;
		case EWVR_ArenaState::ArenaVisible_ForceOff:
			aVisible = WVR_ArenaVisible_ForceOff;
			break;
		default:
			break;
	}
	FWaveVRAPIWrapper::GetInstance()->SetArenaVisible( aVisible );
}

// UE4 Plugin
void UWaveVRArena::WaveVR_ArenaState( EWVR_ArenaState type ) {

	ArenaImpl::ArenaState( type );
}
