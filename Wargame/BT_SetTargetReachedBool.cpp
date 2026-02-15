// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_SetTargetReachedBool.h"
#include "AIEnemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBT_SetTargetReachedBool::UBT_SetTargetReachedBool()
{
	NodeName = "Set TargetReachedBool";
}



EBTNodeResult::Type UBT_SetTargetReachedBool::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    AAIController* AI = OwnerComp.GetAIOwner();
    if (!AI) return EBTNodeResult::Failed;

    UBlackboardComponent* BB =
        OwnerComp.GetBlackboardComponent();
    if (!BB) return EBTNodeResult::Failed;
    
    AAIEnemy* Enemy = Cast<AAIEnemy>(AI->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    Enemy->RequestNewPos(0,0,0,true,false);

    return EBTNodeResult::Succeeded;
}