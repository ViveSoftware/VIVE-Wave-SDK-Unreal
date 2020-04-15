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
#include "Framework/Commands/Commands.h"
#include "WaveVRStyle.h"

class FWaveVRCommands : public TCommands<FWaveVRCommands>
{
public:

	FWaveVRCommands()
		: TCommands<FWaveVRCommands>(TEXT("WaveVR"), NSLOCTEXT("Contexts", "WaveVR", "WaveVR Plugin"), NAME_None, FWaveVRStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	//TSharedPtr< FUICommandInfo > PluginAction;
	TSharedPtr< FUICommandInfo > StartDPServer;
	TSharedPtr< FUICommandInfo > StopDPServer;
	TSharedPtr< FUICommandInfo > InstallDeviceAPK;
	TSharedPtr< FUICommandInfo > StartDeviceAPK;
	TSharedPtr< FUICommandInfo > StopDeviceAPK;
};
