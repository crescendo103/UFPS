// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AudioManager.generated.h"

class USoundBase;
class UAudioComponent;
class UGameConfigData3;

UCLASS()
class FPS_API UAudioManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void Init(UGameConfigData3* Config3);

	void PlayIntroBGM();
	void PlayMenuBGM();
	void PlayGameBGM();

private:
	void CrossfadeBGM(USoundBase* NewSound, float FadeDuration = 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* IntroBGM;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* MenuBGM;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* GameBGM;

	UPROPERTY()
	UAudioComponent* BGMComponent;
};