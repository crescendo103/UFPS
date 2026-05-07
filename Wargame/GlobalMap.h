// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"

#include "GlobalMap.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API UGlobalMap : public UUserWidget
{
	GENERATED_BODY()

public:
	void CalPlayerPos();
	void UpdatePlayerDotPosition(float UI_X, float UI_Y);

	UPROPERTY(meta = (BindWidget))
	UImage* MapImage;

	UPROPERTY(meta = (BindWidget))
	UImage* PlayerDot;

	


protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	FVector MapCenter;
	float OrthoWidth;
	float MapSizeX;
	float MapSizeY;

	FVector PlayerLoc;


};
