// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChasePlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

UBTTask_ChasePlayer::UBTTask_ChasePlayer()
{
	NodeName = "Chase Player";
}

EBTNodeResult::Type UBTTask_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AActor* Player =
        UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (!Player) return EBTNodeResult::Failed;

    OwnerComp.GetBlackboardComponent()
        ->SetValueAsObject("TargetActor", Player);

    // 1️⃣ AIController
    AAIController* AICon = OwnerComp.GetAIOwner();
    if (!AICon)
        return EBTNodeResult::Failed;

    // 2️⃣ Blackboard
    UBlackboardComponent* BB = AICon->GetBlackboardComponent();
    if (!BB)
        return EBTNodeResult::Failed;

    // 3️⃣ Navigation System
    UNavigationSystemV1* NavSys =
        FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

    if (!NavSys)
        return EBTNodeResult::Failed;

    // 4️⃣ 랜덤 NavMesh 좌표
    FNavLocation RandomLocation;
    FVector Origin = AICon->GetPawn()->GetActorLocation();
    float Radius = 1000.f; // 원하는 반경

    if (!NavSys->GetRandomReachablePointInRadius(
        Origin,
        Radius,
        RandomLocation))
    {
        return EBTNodeResult::Failed;
    }

    // 5️⃣ Blackboard에 세팅
    BB->SetValueAsVector(
        TEXT("TargetLocation"),
        RandomLocation.Location
    );

    return EBTNodeResult::Succeeded;
}
