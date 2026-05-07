// GameConfigData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameConfigData.generated.h"

UCLASS(BlueprintType)
class FPS_API UGameConfigData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<class ABomb> BombClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<class ABlueHole> BlueHoleClass;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<class AMyGrenade> GrenadeClass;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<class ABullet> BulletClass;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<class AMyEnemy> EnermyClass;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<class AAIEnemy> AIClass;
};