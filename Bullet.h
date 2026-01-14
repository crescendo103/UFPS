// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include <Kismet/GameplayStatics.h>

#include <Components/SphereComponent.h>

#include "Bullet.generated.h"

DECLARE_DELEGATE_OneParam(FBulletHitDelegate, int32 BulletId);


UCLASS()
class WARGAME_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();


	void ShootBullet(const FVector& Forward);

	FBulletHitDelegate OnBulletHit;
	UPROPERTY()
	int32 BulletId;
	
	void SetBulletId(int32 id);
	UFUNCTION()
	void OnHitSphere(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	UProjectileMovementComponent* PojectileCompo; 
	// 구체 콜리전 컴포넌트입니다.
	UPROPERTY(VisibleDefaultsOnly, Category = "Bullet")
	USphereComponent* CollisionComponent;
	UPROPERTY(VisibleDefaultsOnly, Category = "Bullet")
	UStaticMeshComponent* StaticBulletMesh;

	UPROPERTY(EditDefaultsOnly, Category = "EffectNiagara")
	UNiagaraSystem* BulletImpactEffect;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
