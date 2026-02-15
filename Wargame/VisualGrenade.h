// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GrenadeThrowCalculateCompo.h"
#include "VisualGrenade.generated.h"


UCLASS()
class FPS_API AVisualGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVisualGrenade();

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	//UGrenadeThrowCalculateCompo* GrenadeCalComponent;
	UPROPERTY()
	float AccumulateWheelVal;
	UPROPERTY()
	bool dirty;

	void AddThrowAngle(float val);
	void SetDirty(bool state);
	bool GetDirty();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
