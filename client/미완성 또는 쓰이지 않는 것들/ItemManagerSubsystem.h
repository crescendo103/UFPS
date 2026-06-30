// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WeaponActor.h"

#include "ItemManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API UItemManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    TMap<int32, AWeaponActor*> ItemMap;

    AWeaponActor* GetItem(int32 SpawnId);
    void SetItem(int32 SpawnId, AWeaponActor* Item);
    void InitItems(UWorld* World);
};
