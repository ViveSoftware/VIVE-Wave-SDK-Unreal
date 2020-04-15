// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "Engine.h"

// Settings
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"
#include "WaveVRStyle.h"
#include "WaveVRCommands.h"
#include "WaveVRSettings.h"
#include "LevelEditor.h"
#include "WaveVRItemActions.h"

#define LOCTEXT_NAMESPACE "WaveVRSettings"

class FWaveVRSettingsModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
		RegisterSettings();

		FWaveVRStyle::Initialize();
		FWaveVRStyle::ReloadTextures();
		FWaveVRCommands::Register();

		PluginCommands = MakeShareable(new FUICommandList);

		// Example to map action and UI Command
		// PluginCommands->MapAction(
		//	FWaveVRCommands::Get().PluginAction,
		//	FExecuteAction::CreateRaw(this, &FWaveVRSettingsModule::PluginButtonClicked),
		//	FCanExecuteAction());

		PluginCommands->MapAction(
			FWaveVRCommands::Get().StartDPServer,
			FExecuteAction::CreateRaw(this, &FWaveVRSettingsModule::StartDPServer),
			FCanExecuteAction());

		PluginCommands->MapAction(
			FWaveVRCommands::Get().StopDPServer,
			FExecuteAction::CreateRaw(this, &FWaveVRSettingsModule::StopDPServer),
			FCanExecuteAction());

		PluginCommands->MapAction(
			FWaveVRCommands::Get().InstallDeviceAPK,
			FExecuteAction::CreateRaw(this, &FWaveVRSettingsModule::InstallDeviceAPK),
			FCanExecuteAction());

		PluginCommands->MapAction(
			FWaveVRCommands::Get().StartDeviceAPK,
			FExecuteAction::CreateRaw(this, &FWaveVRSettingsModule::StartDeviceAPK),
			FCanExecuteAction());

		PluginCommands->MapAction(
			FWaveVRCommands::Get().StopDeviceAPK,
			FExecuteAction::CreateRaw(this, &FWaveVRSettingsModule::StopDeviceAPK),
			FCanExecuteAction());
		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

		{
			TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
			// Added a MenuBar extension
			MenuExtender->AddMenuBarExtension(
				"Help",
				EExtensionHook::After,
				PluginCommands,
				FMenuBarExtensionDelegate::CreateRaw(this, &FWaveVRSettingsModule::AddMenu)
			);

			LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);

			// Example of adding a menu extension
			/*
			MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FWaveVRSettingsModule::AddMenuExtension));
			LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
		    */
		}

		// Example of adding a toolbar extension
		/*
		{
			TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
			ToolbarExtender->AddToolBarExtension("Game", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FWaveVRSettingsModule::AddToolbarExtension));

			LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
		}
		*/
	}

	virtual void ShutdownModule() override
	{
		if (UObjectInitialized())
		{
			UnregisterSettings();
		}
		// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
		// we call this function before unloading the module.
		FWaveVRStyle::Shutdown();

		FWaveVRCommands::Unregister();
	}

	virtual bool SupportsDynamicReloading() override
	{
		return true;
	}

	/* Example of implement menu click callback
	void PluginButtonClicked() {
		// Put your "OnButtonClicked" stuff here
		FText DialogText = FText::Format(
								LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
								FText::FromString(TEXT("FWaveVRSettingsModule::PluginButtonClicked()")),
								FText::FromString(TEXT("WaveVRSettingsModule.cpp"))
							);
		FMessageDialog::Open(EAppMsgType::Ok, DialogText);
	}
	 */

	void StartDPServer() {
		UWaveVRItemActions::StartDPServer();
	}

	void StopDPServer() {
		UWaveVRItemActions::StopDPServer();
	}

	void InstallDeviceAPK() {
		UWaveVRItemActions::InstallDeviceAPK();
	}

	void StartDeviceAPK() {
		UWaveVRItemActions::StartDeviceAPK();
	}

	void StopDeviceAPK() {
		UWaveVRItemActions::StopDeviceAPK();
	}

private:
	// Example of adding a toolbar extension
	/*
	void AddToolbarExtension(FToolBarBuilder& Builder) {
		Builder.AddToolBarButton(FWaveVRCommands::Get().PluginAction);
		Builder.AddToolBarButton(FWaveVRCommands::Get().StartDPServer);
    }
	*/
	// Example of adding a menu extension
	/*
	void AddMenuExtension(FMenuBuilder& Builder) {
		Builder.AddMenuEntry(FWaveVRCommands::Get().PluginAction);
		Builder.AddMenuEntry(FWaveVRCommands::Get().StartDPServer);
    }
	*/

	void AddMenu(FMenuBarBuilder& MenuBuilder)
	{
		MenuBuilder.AddPullDownMenu(
			LOCTEXT("MenuLocKey", "WaveVR"),
			LOCTEXT("MenuTooltipKey", "Open WaveVR menu"),
			FNewMenuDelegate::CreateRaw(this, &FWaveVRSettingsModule::FillMenu),
			FName(TEXT("WaveVR")),
			FName(TEXT("WaveVRName")));
	}

	void OpenSettings(FName ContainerName, FName CategoryName, FName SectionName)
	{
		FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer(ContainerName, CategoryName, SectionName);
	}

	void FillMenu(FMenuBuilder& MenuBuilder) {
		// Example to add a new menu item
		// MenuBuilder.AddMenuEntry(FWaveVRCommands::Get().PluginAction);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("ProjectSettingsMenuLabel", "Plugin Settings..."),
			LOCTEXT("ProjectSettingsMenuToolTip", "Change the settings of the WaveVR."),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FWaveVRSettingsModule::OpenSettings, FName("Project"), FName("Plugins"), FName("WaveVR")))
		);

		MenuBuilder.BeginSection("DirectPreview", LOCTEXT("DirectPreviewKey", "DirectPreview"));
		MenuBuilder.AddMenuEntry(FWaveVRCommands::Get().StartDPServer);
		MenuBuilder.AddMenuEntry(FWaveVRCommands::Get().StopDPServer);
		MenuBuilder.AddMenuEntry(FWaveVRCommands::Get().InstallDeviceAPK);
		MenuBuilder.AddMenuEntry(FWaveVRCommands::Get().StartDeviceAPK);
		MenuBuilder.AddMenuEntry(FWaveVRCommands::Get().StopDeviceAPK);
		MenuBuilder.EndSection();
	}

private:
	TSharedPtr<class FUICommandList> PluginCommands;

private:
	// Callback for when the settings were saved.
	bool HandleSettingsSaved()
	{
		UWaveVRSettings* Settings = GetMutableDefault<UWaveVRSettings>();
		bool isValidate = false;
		// Do validate here
		isValidate = true;

		if (isValidate)
			Settings->SaveConfig();

		return true;
	}

	void RegisterSettings()
	{
		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			// Register the settings
			ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "WaveVR",
				LOCTEXT("RuntimeGeneralSettingsName", "Wave VR"),
				LOCTEXT("RuntimeGeneralSettingsDescription", "Base configuration for our game module"),
				GetMutableDefault<UWaveVRSettings>()
			);

			// Register the save handler to your settings, you might want to use it to
			// validate those or just act to settings changes.
			if (SettingsSection.IsValid())
			{
				SettingsSection->OnModified().BindRaw(this, &FWaveVRSettingsModule::HandleSettingsSaved);
			}
		}
	}

	void UnregisterSettings()
	{
		// Ensure to unregister all of your registered settings here, hot-reload would
		// otherwise yield unexpected results.

		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->UnregisterSettings("Project", "Plugins", "WaveVR");
		}
	}
};

IMPLEMENT_MODULE(FWaveVRSettingsModule, WaveVREditor);

#undef LOCTEXT_NAMESPACE
