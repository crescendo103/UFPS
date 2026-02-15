// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnManager.h"
#include "Helicopter.h"

void UPawnManager::RegisterPlayer(AMyCharacter* player)
{
	LocalPlayer = player;
}

void UPawnManager::PossesHelicopter()
{
	
	APlayerController* PC =
		UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (!PC) return;

	PC->Possess(Helicopter);
    /*
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC || !Helicopter) return;

    // ⭐⭐⭐ 이게 핵심 중 핵심 ⭐⭐⭐
    PC->bAutoManageActiveCameraTarget = false;

    // ⭐ 헬기 카메라 강제 활성화
    Helicopter->ActivateCamera();
    FViewTargetTransitionParams Params;
    Params.BlendTime = 0.5f;
    Params.BlendFunction = VTBlend_Cubic;

    AActor* CurrentTarget = PC->GetViewTarget();
    UE_LOG(LogTemp, Warning, TEXT("Before SetViewTarget: %s"),
        *GetNameSafe(CurrentTarget));

    PC->SetViewTarget(Helicopter, Params);

    UE_LOG(LogTemp, Warning, TEXT("After SetViewTarget: %s"),
        *GetNameSafe(PC->GetViewTarget()));
	//PC->SetViewTargetWithBlend(Helicopter, 0.5f);*/
    
}

void UPawnManager::PossesLocalPlayer()
{
	
	APlayerController* PC =
		UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (!PC) return;

	PC->Possess(LocalPlayer);
	
    FVector HeliPos = Helicopter->GetActorLocation();
    FVector HeliDownPos = HeliPos + FVector(0.f, 0.f, -10.f);
    LocalPlayer->SetActorLocation(HeliDownPos);

    if (!ParachuteClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ParachuteClass is NULL"));
        return;
    }

    FVector ParachutePos = HeliPos + FVector(0.f, 0.f, -2.f);
    FRotator Rotation = FRotator::ZeroRotator;

    Parachute = GetWorld()->SpawnActor<AActor>(
        ParachuteClass,
        ParachutePos,
        Rotation
    );
    /*
    Parachute->AttachToComponent(
        LocalPlayer->GetRootComponent(),
        FAttachmentTransformRules::SnapToTargetNotIncludingScale
    );*/

    FAttachmentTransformRules AttachRules(
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::KeepWorld,
        false // ⭐ bWeldSimulatedBodies = false (제일 중요)
    );

    Parachute->AttachToComponent(
        LocalPlayer->GetRootComponent(),
        AttachRules
    );
    LocalPlayer->SetParachuteActor(Parachute);

    /*
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC || !LocalPlayer) return;

    PC->bAutoManageActiveCameraTarget = true;

    FViewTargetTransitionParams Params;
    Params.BlendTime = 0.3f;
    Params.BlendFunction = VTBlend_Cubic;

    PC->SetViewTarget(LocalPlayer, Params);
    
    FVector HeliPos = Helicopter->GetActorLocation();
    FVector HeliDownPos = HeliPos + FVector(0.f, 0.f, -10.f);
    LocalPlayer->SetActorLocation(HeliDownPos);
    */
}
void UPawnManager::SpawnHeli()
{
    if (!HeliClass)
    {
        UE_LOG(LogTemp, Error, TEXT("HeliClass is NULL"));
        return;
    }

    FVector Location = FVector(-131172.288122, -3401.966239, 1925.888404);
    FRotator Rotation = FRotator::ZeroRotator;

    Helicopter = GetWorld()->SpawnActor<AHelicopter>(
        HeliClass,
        Location,
        Rotation
    );

    if (!Helicopter)
    {
        UE_LOG(LogTemp, Error, TEXT("Helicopter Spawn Failed"));
        return;
    }

    // 매니저 먼저 등록
    Helicopter->RegisterManager(this);

    // 카메라 전환
    PossesHelicopter();
}
void UPawnManager::SetHeliClass(TSubclassOf<AHelicopter> heli)
{
	HeliClass = heli;
}

void UPawnManager::SetParachuteClass(TSubclassOf<AActor> actor)
{
    ParachuteClass = actor;
}
