// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include <Kismet/GameplayStatics.h>
#include "TimerManager.h"
#include <Components/SphereComponent.h>


#include "Bomb.generated.h"

class UItemEffectComponent;
UCLASS()
class FPS_API ABomb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABomb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
	void OnOverlapSphere(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	UProjectileMovementComponent* PojectileCompo;
	UPROPERTY(VisibleDefaultsOnly, Category = "Bomb")
	class UStaticMeshComponent* BombMesh;
	UPROPERTY(VisibleDefaultsOnly, Category = "Bomb")
	USphereComponent* CollisionComponent;
	/*
	UPROPERTY(EditDefaultsOnly, Category = "EffectNiagara")
	UNiagaraSystem* BulletImpactEffect;
	*/
	//ÄÄÆ÷³ÍÆ®
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UItemEffectComponent* ItemEffectComponent;

	bool bPlayed = false;
};
