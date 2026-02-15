#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Field/FieldSystemComponent.h"
#include "Field/FieldSystemObjects.h"
#include "MyFractureActor.generated.h"

UCLASS()
class FPS_API AMyFractureActor : public AActor
{
    GENERATED_BODY()

public:
    AMyFractureActor();


protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(VisibleAnywhere)
    UGeometryCollectionComponent* GeometryCollection;

    UPROPERTY(VisibleAnywhere)
    UFieldSystemComponent* FieldSystem;

    UFUNCTION()
    void ApplyFracture(FVector HitLocation);

    UFUNCTION()
    void DelayApplyFracture(FVector HitLocation);

    FTimerHandle ExplosionTimerHandle;
};
