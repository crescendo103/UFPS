// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBox.h"
#include "WeaponBoxPlayer.generated.h"

/**
 * 
 */
class AMyCharacter;
UCLASS()
class FPS_API UWeaponBoxPlayer : public UWeaponBox
{
	GENERATED_BODY()

public:
	UPROPERTY()
	AMyCharacter* Owner;
	
	void SetOwner(AMyCharacter* actor);

	virtual bool NativeOnDrop(
		const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;
	
};
