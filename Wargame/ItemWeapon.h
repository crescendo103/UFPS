// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyItem.h"
#include <Components/Image.h>

#include "ItemWeapon.generated.h"


/**
 * 
 */
UCLASS()
class FPS_API UItemWeapon : public UMyItem
{
	GENERATED_BODY()
	
public:
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	UImage* Icon;
};
