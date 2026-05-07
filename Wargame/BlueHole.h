// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"


#include "BlueHole.generated.h"

class AMyCharacter;
UCLASS()
class FPS_API ABlueHole : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlueHole();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	float CurrentRadius;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* ZoneMesh;

    UPROPERTY(VisibleAnywhere)
    USphereComponent* ZoneSphere;

    UPROPERTY(VisibleAnywhere)
    AMyCharacter* Player;

    
  
    

    // 1초 타이머
    FTimerHandle CheckTimerHandle;
    void CheckPlayersInZone();
    void SetSize();

    float MeshBaseRadius;
    bool isDone;

    float TargetRadius;
    FTimerHandle timerHandle;
    void SetTargetRadius();
};
