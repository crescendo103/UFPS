// Fill out your copyright notice in the Description page of Project Settings.


#include "CP_BloodEffect.h"
#include "MyCharacter.h"
#include "Camera/CameraComponent.h"        // UPROPERTY로 선언한 컴포넌트의 헤더를 여기서 포함


// Sets default values for this component's properties
UCP_BloodEffect::UCP_BloodEffect()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCP_BloodEffect::BeginPlay()
{
	Super::BeginPlay();

	// ...
    if (!HitPPBaseMaterial) return;

    HitPPMaterial = UMaterialInstanceDynamic::Create(HitPPBaseMaterial, this);

    AActor* Owner = GetOwner();
    if (!Owner) return;

    // 👇 여기 핵심
    AMyCharacter* Character = Cast<AMyCharacter>(Owner);
    if (!Character) return;

    if (Character->FollowCamera && HitPPMaterial)
    {
        Character->FollowCamera->AddOrUpdateBlendable(HitPPMaterial);
    }


    if (!WhitePPBaseMaterial) return;

    WhitePPMaterial = UMaterialInstanceDynamic::Create(WhitePPBaseMaterial, this);


    if (Character->FollowCamera && WhitePPMaterial)
    {
        Character->FollowCamera->AddOrUpdateBlendable(WhitePPMaterial);
    }

    if (ZonePPBaseMaterial)
    {
        ZonePPMaterial = UMaterialInstanceDynamic::Create(ZonePPBaseMaterial, this);

        if (Character->FollowCamera && ZonePPMaterial)
        {
            Character->FollowCamera->AddOrUpdateBlendable(ZonePPMaterial);
        }
    }

    ResetEffect();
}


// Called every frame
void UCP_BloodEffect::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCP_BloodEffect::TriggerHitEffect(float val)
{
    if (!HitPPMaterial) return;

    HitPPMaterial->SetScalarParameterValue(
        TEXT("Intensity"), val
    );

    // 일정시간 후 끄기
    GetWorld()->GetTimerManager().SetTimer(
        HitEffectTimer,
        this,
        &UCP_BloodEffect::ResetEffect,
        0.3f,   // 지속시간
        false
    );
}

void UCP_BloodEffect::TriggerZoneEffect(float val)
{
    if (!ZonePPMaterial) return;

    ZonePPMaterial->SetScalarParameterValue(
        TEXT("Intensity"), val
    );

    // 일정시간 후 끄기
    GetWorld()->GetTimerManager().SetTimer(
        HitEffectTimer,
        this,
        &UCP_BloodEffect::ResetEffect,
        0.3f,   // 지속시간
        false
    );
}

void UCP_BloodEffect::TriggerWhiteEffect(float val)
{
    if (!WhitePPMaterial) return;

    WhitePPMaterial->SetScalarParameterValue(
        TEXT("Intensity"), val
    );

    // 일정시간 후 끄기
    GetWorld()->GetTimerManager().SetTimer(
        HitEffectTimer,
        this,
        &UCP_BloodEffect::ResetEffect,
        1.0f,   // 지속시간
        false
    );
}

void UCP_BloodEffect::ResetEffect()
{
    if (!HitPPMaterial) return;

    HitPPMaterial->SetScalarParameterValue(TEXT("Intensity"), 0.0f);

    if (!ZonePPMaterial) return;

    ZonePPMaterial->SetScalarParameterValue(TEXT("Intensity"), 0.0f);

    if (!WhitePPMaterial) return;

    WhitePPMaterial->SetScalarParameterValue(TEXT("Intensity"), 0.0f);
}

