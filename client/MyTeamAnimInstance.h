// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyTeamAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API UMyTeamAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

    UFUNCTION()
    void AnimNotify_PunchStart();

    UFUNCTION()
    void AnimNotify_PunchEnd();
};
