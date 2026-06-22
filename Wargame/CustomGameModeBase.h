// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CustomGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API ACustomGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ACustomGameModeBase();
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	class UGameConfigData* ConfigData;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	class UGameConfigData2* ConfigData2;
};
