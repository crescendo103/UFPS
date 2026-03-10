// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGrenade.h"
#include "FlashBang.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API AFlashBang : public AMyGrenade
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "EffectNiagara")
	UNiagaraSystem* GrenadeImpactEffect;

	void ExplodeVoxel(FVector ExplosionCenter, float ExplosionRadius) override;
	void Explode() override;
};
