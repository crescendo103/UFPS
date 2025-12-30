// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grenade")
	UStaticMesh* GrenadeAsset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grenade")
	UStaticMeshComponent* GrenadeMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grenade")
	UMyGrenadeComponent* grenadeCompo;

	

	

};
