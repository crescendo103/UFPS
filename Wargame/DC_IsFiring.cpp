// Fill out your copyright notice in the Description page of Project Settings.


#include "DC_IsFiring.h"
#include "BehaviorTree/BlackboardComponent.h"

UDC_IsFiring::UDC_IsFiring()
{
	NodeName = TEXT("Is Firing?");
}

bool UDC_IsFiring::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB)
        return false;

    // 블랙보드 Bool 값 읽기
    const bool bHaveGun = BB->GetValueAsBool(TEXT("IsFireing"));
    
    return bHaveGun;
}
