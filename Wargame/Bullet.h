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


#include "Bullet.generated.h"

DECLARE_DELEGATE_TwoParams(FBulletHitDelegate, int32 BulletId, int32 CharacterId);

class USoundComponent;

UCLASS()
class FPS_API ABullet : public AActor
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
	//UFUNCTION()
	//void OnHitSphere(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

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
	// 구체 콜리전 컴포넌트입니다.
	UPROPERTY(VisibleDefaultsOnly, Category = "Bullet")
	USphereComponent* CollisionComponent;
	UPROPERTY(VisibleDefaultsOnly, Category = "Bullet")
	UStaticMeshComponent* StaticBulletMesh;

	UPROPERTY(EditDefaultsOnly, Category = "EffectNiagara")
	UNiagaraSystem* BulletImpactEffect;

	// 오디오 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USoundComponent* BulletAudio;

	// 1회 재생용
	bool bPlayed = false;
	

	int32 BulletOwner;
	FTimerHandle OverlapTimerHandle;

	void SetBulletOwner(int32 owner);
	int32 GetBulletOwner();
	void ActiveBulletOverlap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Throw(const FVector& Direction, float Power);
};
