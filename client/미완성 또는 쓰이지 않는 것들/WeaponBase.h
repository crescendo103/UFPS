// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "HighlightInterface.h" // 인터페이스 포함
//#include "Components/SkeletalMeshComponent.h" // SetRenderCustomDepth 사용

#include "HighlightInterface.h"
#include "WeaponBase.generated.h"

UCLASS(Blueprintable)
class FPS_API AWeaponBase : public AActor, public IHighlightInterface//, public IHighlightInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gun")
	USceneComponent* MuzzlePoint;
	// 부모 머티리얼 (M_Overlay)
	UPROPERTY(EditDefaultsOnly, Category = "Overlay")
	UMaterialInterface* OverlayMaterial;
	// 동적 머티리얼 인스턴스
    UPROPERTY()
    UMaterialInstanceDynamic* DynamicOverlay;
public:
	FVector GetGunStartLocation();
	FVector GetGunFoward();
	
	virtual void ItemOutlineOn_Implementation() override;
	virtual void ItemOutlineOff_Implementation() override;
	
};
