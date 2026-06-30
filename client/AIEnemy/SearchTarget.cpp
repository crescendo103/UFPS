// Fill out your copyright notice in the Description page of Project Settings.


#include "SearchTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIEnemy.h"
#include "MyCharacter.h"
#include "WeaponActor.h"
#include <Components/BoxComponent.h>
#include "NavigationSystem.h"

USearchTarget::USearchTarget()
{
	NodeName = "Search Target";
}

EBTNodeResult::Type USearchTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // 1️⃣ AIController
    AAIController* AICon = OwnerComp.GetAIOwner();
    if (!AICon)
        return EBTNodeResult::Failed;

    // 2️⃣ Pawn
    AAIEnemy* Enemy = Cast<AAIEnemy>(AICon->GetPawn());
    if (!Enemy || !Enemy->BoxCollision)
        return EBTNodeResult::Failed;

    // 3️⃣ Blackboard
    UBlackboardComponent* BB = AICon->GetBlackboardComponent();
    if (!BB)
        return EBTNodeResult::Failed;

    // 4️⃣ Box 안에 있는 액터들
    TArray<AActor*> OverlappingActors;
    Enemy->BoxCollision->GetOverlappingActors(OverlappingActors);

    AActor* FoundItem = nullptr;

    for (AActor* Actor : OverlappingActors)
    {
        if (!Actor || Actor == Enemy)
            continue;

        // Player 우선
        if (Actor->IsA(AMyCharacter::StaticClass()))
        {            
            FVector pos = Actor->GetActorLocation();
            Enemy->RequestNewPos(pos.X, pos.Y, pos.Z, false, true);
            UE_LOG(LogTemp, Warning, TEXT("BTTask: Player detected"));
            
            return EBTNodeResult::Succeeded;
        }

        // Item은 후보로만 저장
        if (!FoundItem && Actor->IsA(AWeaponActor::StaticClass()))
        {
            FoundItem = Actor;
        }
    }

    // Player 없고 Item만 있을 때
    if (FoundItem)
    {      
        FVector pos = FoundItem->GetActorLocation();
        Enemy->RequestNewPos(pos.X, pos.Y, pos.Z, false, true);

        UE_LOG(LogTemp, Warning, TEXT("BTTask: Item detected"));
        return EBTNodeResult::Succeeded;
    }    
    
    return EBTNodeResult::Succeeded;
}
