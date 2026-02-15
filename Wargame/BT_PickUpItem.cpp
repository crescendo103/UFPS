// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_PickUpItem.h"
#include "WeaponActor.h"
#include "MyCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIEnemy.h"

UBT_PickUpItem::UBT_PickUpItem()
{
}

EBTNodeResult::Type UBT_PickUpItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	AAIEnemy* Enemy = Cast<AAIEnemy>(AICon->GetPawn());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!Enemy)
		return EBTNodeResult::Failed;
	
	Enemy->AttackMelee(); // 애니 + 데미지

	return EBTNodeResult::Succeeded;
}
