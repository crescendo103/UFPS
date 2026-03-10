// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerController.h"
#include "MyServer.h"
#include "MyCharacter.h"
#include "WD_HPBar.h"
#include "AimWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "PawnManager.h"

void ACustomPlayerController::BeginPlay()
{
    Super::BeginPlay();

    

    if (!IsLocalController())
        return;

    if (StartMenuWidgetClass)
    {
        StartingMenuWidget = CreateWidget<UStartMenuWidget>(
            this,
            StartMenuWidgetClass
        );
    }

    if (StartingMenuWidget)
    {
        StartingMenuWidget->AddToViewport();
        StartingMenuWidget->SetVisibility(ESlateVisibility::Visible);
    }

    if (WatingRoomWidgetClass)
    {
        WatingRoomWidget = CreateWidget<UWatingRoom>(
            this,
            WatingRoomWidgetClass
        );
    }

    if (WatingRoomWidget)
    {
        WatingRoomWidget->AddToViewport();
        WatingRoomWidget->SetVisibility(ESlateVisibility::Hidden);
    }
    // 처음에는 스타트 메뉴만 표시
    isWatingRoom = false;

    // UI 입력 모드로 전환
    FInputModeUIOnly InputMode;
    //InputMode.SetWidgetToFocus(CurrentInventoryWidget->TakeWidget()); // 포커스할 위젯 지정 (선택 사항)
    //InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 마우스 커서 뷰포트 잠금 해제 (선택 사항)
    SetInputMode(InputMode);
    bShowMouseCursor = true; // 마우스 커서 표시
    
}


void ACustomPlayerController::SetTextTime(int time)
{
    // 10초 카운트 시작 → WaitingRoom
    if (time == 10)
    {
        isWatingRoom = true;
        ShowWaitingRoom();
        HiddenStartMenu();
        SpawnAndPossessMyCharacter();
    }

    // 🔹 5초 카운트 시작 → StartMenu
    if (time == 5 && !isWatingRoom)
    {
        ShowHiddenStartMenu();
        HiddenWaitingRoom();
    }

    if (time == 0 && isWatingRoom)
    {

        //두번째 카운트다운
        if (isWatingRoom) {                        
            ULocalPlayer* LP = this->GetLocalPlayer();
            if (!LP) return;

            UPawnManager* PM = LP->GetSubsystem<UPawnManager>();
            if (!PM) return;

            /*
            FInputModeGameOnly InputMode;
            SetInputMode(InputMode);
            bShowMouseCursor = false;
            FSlateApplication::Get().SetAllUserFocusToGameViewport();
            */

            FInputModeGameAndUI InputMode;
            InputMode.SetHideCursorDuringCapture(false); // 마우스 숨기지 않음
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

            SetInputMode(InputMode);
            bShowMouseCursor = true;
            /*
            PM->SpawnHeli();*/
        }
        else {
            SpawnAndPossessMyCharacter();
        }
    }

    if (StartingMenuWidget &&
        StartingMenuWidget->GetVisibility() == ESlateVisibility::Visible)
    {
        StartingMenuWidget->SetInformationText(FText::AsNumber(time));
    }
    else if (WatingRoomWidget &&
        WatingRoomWidget->GetVisibility() == ESlateVisibility::Visible)
    {
        WatingRoomWidget->SetInformationText(FText::AsNumber(time));
    }
}

void ACustomPlayerController::SwitchWidget(bool startwidget, bool watingwidget)
{
    if (startwidget) {
        ShowHiddenStartMenu();
        HiddenWaitingRoom();
    }
    else if (watingwidget) {
        ShowWaitingRoom();
        HiddenStartMenu();
    }

}

void ACustomPlayerController::SpawnAndPossessMyCharacter()
{
    APawn* CurrentPawn = GetPawn();

    if (CurrentPawn)
    {
        FVector NewLocation = FVector(1000, 500, 200); // 이동할 위치
        CurrentPawn->SetActorLocation(NewLocation);

        UE_LOG(LogTemp, Warning, TEXT("Pawn moved to: %s"), *NewLocation.ToString());
    }

    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
    bShowMouseCursor = false;
    FSlateApplication::Get().SetAllUserFocusToGameViewport();
    if (AimWidgetClass)
    {
        AimWidget = CreateWidget<UAimWidget>(this, AimWidgetClass);

        if (AimWidget)
        {
            AimWidget->AddToViewport(-10);
            AimWidget->SetVisibility(ESlateVisibility::Visible);
        }
    }

    if (HPBarClass)
    {
        HpBarWidget = CreateWidget<UWD_HPBar>(this, HPBarClass);

        if (HpBarWidget)
        {
            HpBarWidget->AddToViewport(-10);
            HpBarWidget->SetVisibility(ESlateVisibility::Visible);
        }
    }
    
}

void ACustomPlayerController::ShowWaitingRoom()
{
    WatingRoomWidget->SetVisibility(ESlateVisibility::Visible);
}

void ACustomPlayerController::HiddenWaitingRoom()
{
    WatingRoomWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ACustomPlayerController::ShowHiddenStartMenu()
{
    StartingMenuWidget->SetVisibility(ESlateVisibility::Visible);
}

void ACustomPlayerController::HiddenStartMenu()
{
    StartingMenuWidget->SetVisibility(ESlateVisibility::Hidden);
}