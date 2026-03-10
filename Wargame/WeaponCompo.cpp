// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponCompo.h"

AWeaponCompo::AWeaponCompo()
{
    PrimaryActorTick.bCanEverTick = true;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMesh;

    ScopeCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("ScopeCapture"));
    ScopeCapture->SetupAttachment(WeaponMesh);

    // 기본 설정
    ScopeCapture->bCaptureEveryFrame = false;
    ScopeCapture->bCaptureOnMovement = false;
    ScopeCapture->FOVAngle = 20.f; // 확대 FOV
    ScopeCapture->SetVisibility(false);

    bScopeEnabled = false;
}

void AWeaponCompo::BeginPlay()
{
    if (ScopeRenderTarget)
    {
        ScopeCapture->TextureTarget = ScopeRenderTarget;
    }

    if (ScopeLensMaterial)
    {
        ScopeMID = UMaterialInstanceDynamic::Create(ScopeLensMaterial, this);

        if (ScopeMID)
        {
            ScopeMID->SetTextureParameterValue(TEXT("ScopeTexture"), ScopeRenderTarget);

            // 렌즈 메쉬 머티리얼 슬롯 번호 맞게 설정
            WeaponMesh->SetMaterial(1, ScopeMID);
        }
    }

}

void AWeaponCompo::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWeaponCompo::EnableScope(bool bEnable)
{
    bScopeEnabled = bEnable;

    if (bScopeEnabled)
    {
        ScopeCapture->SetVisibility(true);
        ScopeCapture->bCaptureEveryFrame = true;
    }
    else
    {
        ScopeCapture->SetVisibility(false);
        ScopeCapture->bCaptureEveryFrame = false;
    }
}
