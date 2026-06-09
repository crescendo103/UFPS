// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include <Components/SphereComponent.h>
#include "MyGrenadeGo.h"
#include "TimerManager.h"
#include <Kismet/GameplayStatics.h>
#include <NiagaraFunctionLibrary.h>

#include "VoxelWorld.h"
#include "VoxelTools/Gen/VoxelSphereTools.h"

#include "GeometryCollection/GeometryCollectionComponent.h"

#include "ItemEffectComponent.h"

#include "MyGrenadeComponent.h"
#include "MyGrenade.generated.h"





UCLASS(Blueprintable)
class FPS_API AMyGrenade : public AActor
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

	void SetOwnerID(int32 owner);
	int32 GetOwnerID();
	int32 Owner;

	void ExplodeVoxel(FVector ExplosionCenter, float ExplosionRadius);
	UFUNCTION()
	void Explode();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grenade")
	UMyGrenadeComponent* grenadeCompo;
	
	
	// 구체 콜리전 컴포넌트입니다.
	UPROPERTY(VisibleDefaultsOnly, Category = "Grenade")
	USphereComponent* CollisionComponent;

	//부모에서 관리하는 수류탄 정보
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "feature")
	UItemEffectComponent* EffectComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "feature")
	float Damage = 20.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "feature")
	float ExplosionRadius = 200.f;
	//------
	// 
	//new
	UPROPERTY(VisibleDefaultsOnly, Category = "Grenade")
	UStaticMeshComponent* StaticGrenadeMesh;

	FTimerHandle ExplosionTimerHandle;

	
	// 수정할 VoxelWorld (블루프린트에서 지정 가능)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel")
	AVoxelWorld* TargetVoxelWorld;

	
	UFUNCTION()
	void OnHitSphere(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	bool bSticked = false;
};
