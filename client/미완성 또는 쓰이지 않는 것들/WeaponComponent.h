// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"


#include "WeaponComponent.generated.h"



class UNiagaraSystem;
class UNiagaraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
		
	UWeaponComponent();

	UPROPERTY(EditDefaultsOnly, Category = "EffectBullet")
	UNiagaraSystem* BulletImpactEffect;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FHitResult RayBullet(const FVector Start, const FVector Forward, float Distance);

	/*
	헤더 순환참조..
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Parents")
	AMyCharacter* MyChar;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Parents")
	AWeaponBase* MyWeapon;
	*/
		
};
