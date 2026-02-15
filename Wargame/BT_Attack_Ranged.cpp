// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Attack_Ranged.h"
#include "Bullet.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIEnemy.h"
#include "MyCharacter.h"
#include "WeaponActor.h"
#include <Components/BoxComponent.h>
#include "NavigationSystem.h"

UBT_Attack_Ranged::UBT_Attack_Ranged()
{
}

EBTNodeResult::Type UBT_Attack_Ranged::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIEnemy* Enemy = Cast<AAIEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Enemy)
		return EBTNodeResult::Failed;

	Enemy->AttackRanged();
	return EBTNodeResult::Succeeded;
}
