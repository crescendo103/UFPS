// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GlobalMap.h"
#include "LoginWidget.h"
#include "Minimap.h"
#include "WatingRoom.h"
#include "PaperSprite.h"
#include "InformationWidget.h"
#include "StartMenuWidget.h"
#include "CustomPlayerController.generated.h"

/**
 * 
 */
class AMyCharacter;
class UWD_HPBar;
class UAimWidget;
class UkillAccountWidget;
class UGameConfigData;
class AMinimapCaptureActor;
class UEndGameUIWidget;
class URoomWidget;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UkillAccountWidget> DeathLogWidgetClass;
	UPROPERTY()
	UkillAccountWidget* DeathLogWidget;
	UkillAccountWidget* GetDeathWidget();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UInformationWidget> InformationWidgetClass;
	UPROPERTY()
	UInformationWidget* InformationWidget;
	UInformationWidget* GetInformationWidget();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULoginWidget> LoginWidgetClass;//블루프린트 위젯 설계도
	UPROPERTY()
	class ULoginWidget* LoginWidget;

	//총기 조준점
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> AimWidgetClass;//블루프린트 위젯 설계도
	UPROPERTY()
	class UAimWidget* AimWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UWD_HPBar> HPBarClass;//블루프린트 위젯 설계도
	UPROPERTY()
	class UWD_HPBar* HpBarWidget;


	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMinimap> MinimapWidgetClass;//블루프린트 위젯 설계도

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGlobalMap> GlobalMapWidgetClass;//블루프린트 위젯 설계도

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UEndGameUIWidget> EndGameWidgetClass;//블루프린트 위젯 설계도
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<URoomWidget> RoomWidgetClass;//블루프린트 위젯 설계도

	UPROPERTY()
	class UGlobalMap* GlobalMapWidget;

	UPROPERTY()
	AMinimapCaptureActor* LocalCaptureActor;

	UPROPERTY()
	UTextureRenderTarget2D* LocalRT;

	UPROPERTY()
	UMaterialInstanceDynamic* LocalMID;


	UPROPERTY()
	UMinimap* MiniMapWidget;

	UPROPERTY()
	class URoomWidget* RoomWidget;

	UPROPERTY()
	UEndGameUIWidget* EndGameWidget;
	UEndGameUIWidget* GetUEndGameUIWidget();

	UMinimap* GetMiniMapWidget();

	UGlobalMap* GetGlobalMapWidget();
protected:
	virtual void BeginPlay() override;
	bool isWatingRoom;
public:
	void ShowWaitingRoom();
	void HiddenWaitingRoom();
	void ShowHiddenStartMenu();
	void HiddenStartMenu();
	void SetDisableStartButton();
	void ShowMiniMap();
	void HiddendMiniMap();
	void ShowInformationText();
	void HiddenInformationText();
	void ShowEndGameUI();
	void HiddenEndGameUI();
	void ShowRoomUI();
	void HiddenRoomUI();
	void SetTextTime(int time);
	void SwitchWidget(bool startwidget, bool watingwidget);

	void SpawnAndPossessMyCharacter();

	void InitMinimap(UTextureRenderTarget2D* RT);
	void UpdateGlobalMap(bool state);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	class UGameConfigData* ConfigData;

};
