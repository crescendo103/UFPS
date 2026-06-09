// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameModeBase.h"
#include "MyServer.h"
#include "GameConfigData.h"

ACustomGameModeBase::ACustomGameModeBase()
{
    Super::BeginPlay();

    if (ConfigData)
    {
        if (UMyServer* Server =
            GetGameInstance()->GetSubsystem<UMyServer>())
        {
            Server->Init(ConfigData);
        }
    }
}
