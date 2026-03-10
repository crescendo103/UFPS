// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "WatingRoom.h"

#include "StartMenuWidget.h"
#include "CustomPlayerController.generated.h"

/**
 * 
 */
class AMyCharacter;
class UWD_HPBar;
class UAimWidget;
UCLASS()
class FPS_API ACustomPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// 위젯 클래스 (블루프린트 지정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UWatingRoom> WatingRoomWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UStartMenuWidget> StartMenuWidgetClass;

	UPROPERTY()
	UWatingRoom* WatingRoomWidget;

	UPROPERTY()
	UStartMenuWidget* StartingMenuWidget;


	//총기 조준점
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> AimWidgetClass;//블루프린트 위젯 설계도
	UPROPERTY()
	class UAimWidget* AimWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UWD_HPBar> HPBarClass;//블루프린트 위젯 설계도
	UPROPERTY()
	class UWD_HPBar* HpBarWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<AMyCharacter> PlayerCharacterClass;

protected:
	virtual void BeginPlay() override;
	bool isWatingRoom;
public:
	void ShowWaitingRoom();
	void HiddenWaitingRoom();
	void ShowHiddenStartMenu();
	void HiddenStartMenu();
	void SetTextTime(int time);
	void SwitchWidget(bool startwidget, bool watingwidget);

	void SpawnAndPossessMyCharacter();
};
