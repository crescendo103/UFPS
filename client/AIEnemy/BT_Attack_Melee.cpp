// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Attack_Melee.h"
#include "AIEnemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBT_Attack_Melee::UBT_Attack_Melee()
{
}

EBTNodeResult::Type UBT_Attack_Melee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	AAIEnemy* Enemy = Cast<AAIEnemy>(AICon->GetPawn());	

	if (!Enemy)
		return EBTNodeResult::Failed;	
	Enemy->AttackMelee(); // 애니 + 데미지

	return EBTNodeResult::Succeeded;
}
