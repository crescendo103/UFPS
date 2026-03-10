// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "TimerManager.h"

#include "CP_BloodEffect.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_API UCP_BloodEffect : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCP_BloodEffect();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* HitPPBaseMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* HitPPMaterial;

	// 자기장 베이스 머티리얼
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* ZonePPBaseMaterial;

	// 동적 머티리얼
	UMaterialInstanceDynamic* ZonePPMaterial;

	// 섬광탄 베이스 머티리얼
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* WhitePPBaseMaterial;
	// 동적 머티리얼
	UMaterialInstanceDynamic* WhitePPMaterial;

	void TriggerHitEffect(float val);
	void TriggerZoneEffect(float val);
	void TriggerWhiteEffect(float val);

	void ResetEffect();

	FTimerHandle HitEffectTimer;
};
