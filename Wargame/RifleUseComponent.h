// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponUseComponent.h"
#include "MyCharacter.h"


#include "RifleUseComponent.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API URifleUseComponent : public UWeaponUseComponent
{
	GENERATED_BODY()
	
public:
	void UseWeapon(AMyCharacter* Owner, FVector StartPos, FVector Dir);
	void SendBulletPacket(FVector startPos, FVector dir);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ABullet> BulletClass;


};
