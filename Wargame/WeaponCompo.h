// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponActor.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/TextureRenderTarget2D.h"

#include "WeaponCompo.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API AWeaponCompo : public AWeaponActor
{
	GENERATED_BODY()
	

public:
    AWeaponCompo();


    virtual void BeginPlay() override;


    virtual void Tick(float DeltaTime) override;

    void EnableScope(bool bEnable);

protected:

    /* 총 메쉬 */
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* WeaponMesh;

    /* 스코프 전용 캡처 카메라 */
    UPROPERTY(VisibleAnywhere)
    USceneCaptureComponent2D* ScopeCapture;

    /* Render Target */
    UPROPERTY(EditDefaultsOnly, Category = "Scope")
    UTextureRenderTarget2D* ScopeRenderTarget;

    /* 렌즈용 베이스 머티리얼 */
    UPROPERTY(EditDefaultsOnly, Category = "Scope")
    UMaterialInterface* ScopeLensMaterial;


    /* 런타임 머티리얼 인스턴스 */
    UMaterialInstanceDynamic* ScopeMID;

    bool bScopeEnabled;

};
