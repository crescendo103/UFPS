// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTeamAnimInstance.h"
#include "MyCharacter.h"

void UMyTeamAnimInstance::AnimNotify_PunchStart()
{
    AMyCharacter* Character = Cast<AMyCharacter>(TryGetPawnOwner());

    if (Character)
    {
        Character->SpawnPunchCollision();
    }
}

void UMyTeamAnimInstance::AnimNotify_PunchEnd()
{
    AMyCharacter* Character = Cast<AMyCharacter>(TryGetPawnOwner());

    if (Character)
    {
        Character->EndPunchCollision();
    }
}