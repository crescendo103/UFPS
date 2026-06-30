// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "DC_AttackMethod.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API UDC_AttackMethod : public UBTDecorator
{
	GENERATED_BODY()
public:
    UDC_AttackMethod();

protected:
    // 조건 체크 핵심 함수
    virtual bool CalculateRawConditionValue(
        UBehaviorTreeComponent& OwnerComp,
        uint8* NodeMemory
    ) const override;

};
