// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"


#include "SpawnItemActor.h"
#include "HighlightInterface.h"
#include "WeaponActor.generated.h"

UCLASS(Blueprintable)
class FPS_API AWeaponActor : public AActor, public IHighlightInterface, public ISpawnItemActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshy")
	USkeletalMeshComponent* Mesh;

	// 구체 콜리전 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
	class USphereComponent* TriggerSphere;

	// 부모 머티리얼 (M_Overlay)
	UPROPERTY(EditDefaultsOnly, Category = "Overlay")
	UMaterialInterface* OverlayMaterial;
	// 동적 머티리얼 인스턴스
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicOverlay;

	UPROPERTY(EditDefaultsOnly, Category = "ItemName")
	int32 ItemRowName;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ItemOutlineOn_Implementation() override;
	virtual void ItemOutlineOff_Implementation() override;
	
	virtual void SpawnItem_Implementation(FVector playerPos) override;

	int32 GetItemName();


	UPROPERTY()
	bool bPickedUp;

	
	USphereComponent* GetTriggerSphere() const { return TriggerSphere; }
	
};
