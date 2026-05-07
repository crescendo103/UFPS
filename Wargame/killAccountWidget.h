// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "killAccountWidget.generated.h"


class UTextBlock;
/**
 * 
 */
UCLASS()
class FPS_API UkillAccountWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateDeathUI(FString killerName, FString victimName);

	//  UMG에서 BindWidget 체크해야 함
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DeathText;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeAnim;
protected:
	virtual void NativeConstruct() override;
};
