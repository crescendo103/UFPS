// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartMenuWidget.generated.h"

/**
 * 
 */
class UButton;
class UTextBlock;
class AStartMenuPawn;
UCLASS()
class FPS_API UStartMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// 버튼
	UPROPERTY(meta = (BindWidget))
	UButton* StartButton;

	//titel
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TitleTitleText;
	// 텍스트
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TitleText;


	AStartMenuPawn* StartPawn;
	void SetStartMenuPawn(AStartMenuPawn* pawn);
	void SetStartButtonDisabled();
	void UpdateTitleText(bool state);
private:

	UFUNCTION()
	void OnStartClicked();
	
	void SendTimePacket(int32 TimeValue);

	int32 CountdownValue;

	FTimerHandle CountdownTimerHandle;
public:

	void SetInformationText(const FText& InText);
	
};
