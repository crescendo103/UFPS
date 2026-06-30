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
class UItemEffectComponent;
class UMyAudioComponent;
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

	UFUNCTION()
	void OnNearPlayer(
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
	USphereComponent* NearSoundSphere;
	UPROPERTY(VisibleDefaultsOnly, Category = "Bullet")
	UStaticMeshComponent* StaticBulletMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "feature")
	UItemEffectComponent* EffectComponent;

	// 맞았는지 여부
	bool bHitTarget = false;

	// 휘익 소리 중복 방지
	bool bPlayedWhizSound = false;

	// 휘익 소리
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* BulletWhizSound;
	//피격음
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* BulletHitSound;

	int32 Owner;
	FTimerHandle OverlapTimerHandle;

	void SetOwnerID(int32 owner);
	int32 GetOwnerID();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Throw(const FVector& Direction, float Power);
};
