// Fill out your copyright notice in the Description page of Project Settings.


#include "HeliAIController.h"
#include "AudioManager.h"

AHeliAIController::AHeliAIController()
{
	// Tick 필요 없으면 끔 (성능)
	PrimaryActorTick.bCanEverTick = false;
}

void AHeliAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UAudioManager* AudioMgr = GetGameInstance()->GetSubsystem<UAudioManager>();
	if (AudioMgr) {
		AudioMgr->PlayGameBGM();
	}

	StartBehaviorTree();
}

void AHeliAIController::StartBehaviorTree()
{
	if (!BehaviorTreeAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("BehaviorTreeAsset is NULL"));
		return;
	}

	RunBehaviorTree(BehaviorTreeAsset);

	UE_LOG(LogTemp, Warning, TEXT("Helicopter BehaviorTree Started"));
}