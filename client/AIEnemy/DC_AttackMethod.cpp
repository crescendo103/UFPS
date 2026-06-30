// Fill out your copyright notice in the Description page of Project Settings.


#include "DC_AttackMethod.h"
#include "BehaviorTree/BlackboardComponent.h"

UDC_AttackMethod::UDC_AttackMethod()
{
    NodeName = TEXT("Have Gun?");
}

bool UDC_AttackMethod::CalculateRawConditionValue(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory
) const
{
    Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB)
        return false;

    // 블랙보드 Bool 값 읽기
    const bool bHaveGun = BB->GetValueAsBool(TEXT("HaveGun"));

    return bHaveGun;
}