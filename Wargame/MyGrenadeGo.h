// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"


#include <GameFramework/ProjectileMovementComponent.h>
#include "MyGrenadeGo.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WARGAME_API UMyGrenadeGo : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyGrenadeGo();
	void GoGrenade(const FVector Start, const FVector Forward, float Power);
	void SetUpdateProjectile(UStaticMeshComponent* compo);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grenade")
	UProjectileMovementComponent* PojectileCompo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grenade")
	UStaticMeshComponent* GrenadeMesh2;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	
	UFUNCTION()
	void OnHitStaticMesh(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);
		
};
