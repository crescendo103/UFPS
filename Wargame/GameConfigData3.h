#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameConfigData3.generated.h"

class USoundBase;

UCLASS(BlueprintType)
class FPS_API UGameConfigData3 : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* IntroBGM;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* MenuBGM;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* GameBGM;
};