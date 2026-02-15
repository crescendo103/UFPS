// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_SetTargetReachedBool.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API UBT_SetTargetReachedBool : public UBTTaskNode
{
	GENERATED_BODY()
public:

    UBT_SetTargetReachedBool();

    virtual EBTNodeResult::Type ExecuteTask(
        UBehaviorTreeComponent& OwnerComp,
        uint8* NodeMemory) override;

    // 목적지 벡터 키
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetKey;

    // 도착 여부 bool 키
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector ReachedKey;

};
