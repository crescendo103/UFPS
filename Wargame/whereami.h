#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "whereami.generated.h"

UCLASS()
class FPS_API AWhereami : public AActor
{
    GENERATED_BODY()

public:
    AWhereami();
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
};
