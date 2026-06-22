// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "InformationWidget.generated.h"


/**
 * 
 */

class UTextBlock;
UCLASS()
class FPS_API UInformationWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateInformationUI(FString information);

		
	UPROPERTY(meta = (BindWidget))
	UTextBlock* InformaitonText;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeAnim;
protected:
	virtual void NativeConstruct() override;
};
