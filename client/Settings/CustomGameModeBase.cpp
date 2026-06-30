// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameModeBase.h"
#include "MyServer.h"
#include "GameConfigData.h"
#include "GameConfigData2.h"
#include "GameFramework/GameUserSettings.h"
#include "PawnManager.h"
#include "AudioManager.h"

ACustomGameModeBase::ACustomGameModeBase()
{
}

void ACustomGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    if (ConfigData)
    {
        if (UMyServer* Server = GetGameInstance()->GetSubsystem<UMyServer>())
        {
            Server->Init(ConfigData);
        }
    }

    if (ConfigData2)
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            if (UPawnManager* PawnManager = PC->GetLocalPlayer()->GetSubsystem<UPawnManager>())
            {
                PawnManager->Init(ConfigData2);
            }
        }
    }

    if (ConfigData3)
    {      
        UAudioManager* AudioMgr = GetGameInstance()->GetSubsystem<UAudioManager>();
        if (AudioMgr) {
            AudioMgr->Init(ConfigData3);
        }
    }


    if (UGameUserSettings* Settings = GEngine->GetGameUserSettings())
    {
        Settings->SetScreenResolution(FIntPoint(1920, 1080));
        Settings->SetFullscreenMode(EWindowMode::Windowed);
        Settings->ApplyResolutionSettings(false);
        Settings->ConfirmVideoMode();
    }
}