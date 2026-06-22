// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameConfigData2.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API UGameConfigData2 : public UDataAsset
{
	GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<class AHelicopter> HeliClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<class AActor> ParachuteClass;
};
