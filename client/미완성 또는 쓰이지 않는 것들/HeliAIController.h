// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HeliAIController.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API AHeliAIController : public AAIController
{
	GENERATED_BODY()
public:
	AHeliAIController();
protected:

	// Pawn Possess 시 호출
	virtual void OnPossess(APawn* InPawn) override;

	// Behavior Tree 실행
	void StartBehaviorTree();

protected:

	// 사용할 Behavior Tree
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* BehaviorTreeAsset;

};
