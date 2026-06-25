// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/EditableTextBox.h>
#include <Components/Button.h>

#include "Components/Image.h"
#include <Runtime/MediaAssets/Public/MediaPlayer.h>
#include <Runtime/MediaAssets/Public/MediaSoundComponent.h>
#include "LoginWidget.generated.h"


/**
 * 
 */
UCLASS()
class FPS_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* NameTextBox;

	UPROPERTY(meta = (BindWidget))
	UButton* LoginButton;

	UPROPERTY(meta = (BindWidget))
	UImage* HandImg;

	UFUNCTION()
	void OnLoginClicked();

	UPROPERTY(meta = (BindWidget))
	UImage* Image_IntroVideo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	UMediaPlayer* introPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings")
	UMediaSource* introMediaSource;


	UPROPERTY(EditAnywhere, Category = "MySettings")
	UMediaPlayer* shareMediaPlayer;

	UPROPERTY()
	UMediaSoundComponent* introSoundComponent;

	UFUNCTION()
	void OnIntroFinished();


	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeInUI;

protected:
	virtual void NativeConstruct() override;
};
