// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include <Components/SphereComponent.h>
#include "MyGrenadeGo.h"
#include "TimerManager.h"
#include <Kismet/GameplayStatics.h>
#include <NiagaraFunctionLibrary.h>

#include "MyGrenadeComponent.h"
#include "MyGrenade.generated.h"




UCLASS(Blueprintable)
class WARGAME_API AMyGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyGrenade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ThrowGrenade();
	
	void ThrowWheelVal(float val);

	void Throw(const FVector& Direction, float Power);

	UFUNCTION()
	void Explode();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grenade")
	UMyGrenadeComponent* grenadeCompo;
	
	
	// 구체 콜리전 컴포넌트입니다.
	UPROPERTY(VisibleDefaultsOnly, Category = "Grenade")
	USphereComponent* CollisionComponent;


	//new
	UPROPERTY(VisibleDefaultsOnly, Category = "Grenade")
	UStaticMeshComponent* StaticGrenadeMesh;

	FTimerHandle ExplosionTimerHandle;


	UPROPERTY(EditDefaultsOnly, Category = "EffectNiagara")
	UNiagaraSystem* GrenadeImpactEffect;

};
