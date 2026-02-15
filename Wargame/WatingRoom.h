// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/TextBlock.h>

#include "WatingRoom.generated.h"


/**
 * 
 */
UCLASS()
class FPS_API UWatingRoom : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* InformationText;

public:
	void SetInformationText(const FText& InText);
};
