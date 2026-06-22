// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerController.h"
#include "MyServer.h"
#include "MyCharacter.h"
#include "WD_HPBar.h"
#include "AimWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "PawnManager.h"
#include "killAccountWidget.h"
#include "MinimapCaptureActor.h"
#include <Kismet/KismetRenderingLibrary.h>
#include "MinimapCaptureActor.h"
#include "EndGameUIWidget.h"
#include "RoomWidget.h"

UkillAccountWidget* ACustomPlayerController::GetDeathWidget()
{
    return DeathLogWidget;
}
UInformationWidget* ACustomPlayerController::GetInformationWidget()
{
    return InformationWidget;
}
UEndGameUIWidget* ACustomPlayerController::GetUEndGameUIWidget()
{
    return EndGameWidget;
}
UMinimap* ACustomPlayerController::GetMiniMapWidget()
{
    return MiniMapWidget;
}
UGlobalMap* ACustomPlayerController::GetGlobalMapWidget()
{
    return GlobalMapWidget;
}
void ACustomPlayerController::BeginPlay()
{
    Super::BeginPlay();

    

    if (!IsLocalController())
        return;
    
    //지울요망
    if (ConfigData)
    {
        UMyServer* Server = GetGameInstance()->GetSubsystem<UMyServer>();
        if (Server)
        {
            Server->Init(ConfigData);
        }
    }

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

    if (DeathLogWidgetClass)
    {
        DeathLogWidget = CreateWidget<UkillAccountWidget>(
            this,
            DeathLogWidgetClass
        );
    }
    if (DeathLogWidget)
    {
        DeathLogWidget->AddToViewport();
        DeathLogWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
    }



    if (EndGameWidgetClass)
    {
        EndGameWidget = CreateWidget<UEndGameUIWidget>(
            this,
            EndGameWidgetClass
        );
    }
    if (EndGameWidget)
    {
        EndGameWidget->AddToViewport();
        EndGameWidget->SetVisibility(ESlateVisibility::Hidden);
    }


    if (InformationWidgetClass)
    {
        InformationWidget = CreateWidget<UInformationWidget>(
            this,
            InformationWidgetClass
        );
    }
    if (InformationWidget)
    {
        InformationWidget->AddToViewport();
        InformationWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
    }


    // 처음에는 스타트 메뉴만 표시
    isWatingRoom = false;

    // UI 입력 모드로 전환
    FInputModeUIOnly InputMode;
    //InputMode.SetWidgetToFocus(CurrentInventoryWidget->TakeWidget()); // 포커스할 위젯 지정 (선택 사항)
    //InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 마우스 커서 뷰포트 잠금 해제 (선택 사항)
    SetInputMode(InputMode);
    bShowMouseCursor = true; // 마우스 커서 표시
    
    

    if (GlobalMapWidgetClass)
    {
        GlobalMapWidget = CreateWidget<UGlobalMap>(
            this,
            GlobalMapWidgetClass
        );
    }

    if (GlobalMapWidget)
    {
        GlobalMapWidget->AddToViewport();
        GlobalMapWidget->SetVisibility(ESlateVisibility::Hidden);

        // 1. 로컬 RT 생성
        LocalRT =
            UKismetRenderingLibrary::CreateRenderTarget2D(
                this,
                1024,
                1024);

        // 2. 로컬 CaptureActor 생성
        FVector MapLocation(-43064.267979, -3694.705605, 136314.579742);

        FRotator MapRotation(-90.f, 0.f, 0.f);

        LocalCaptureActor =
            GetWorld()->SpawnActor<AMinimapCaptureActor>(
                AMinimapCaptureActor::StaticClass(),
                MapLocation,
                MapRotation);

        // 3. RT 연결
        LocalCaptureActor->SetRenderTarget(LocalRT);

        // 4. Widget 연결
        GlobalMapWidget->SetRenderTarget(LocalRT);
    }
    //---

    if (LoginWidgetClass)
    {
        LoginWidget = CreateWidget<ULoginWidget>(
            this,
            LoginWidgetClass
        );
    }
    if (LoginWidget)
    {
        LoginWidget->AddToViewport();
        LoginWidget->SetVisibility(ESlateVisibility::Visible);
    }

    if (RoomWidgetClass)
    {
        RoomWidget = CreateWidget<URoomWidget>(
            this,
            RoomWidgetClass
        );
    }
    if (RoomWidget)
    {
        RoomWidget->AddToViewport();
        RoomWidget->SetVisibility(ESlateVisibility::Hidden);
    }

}


void ACustomPlayerController::SetTextTime(int time)
{
    // 10초 카운트 시작 → WaitingRoom
    if (time == 10)
    {
        isWatingRoom = true;
        ShowWaitingRoom();
        HiddenStartMenu();
        ShowMiniMap();
        ShowInformationText();
        SpawnAndPossessMyCharacter();
    }

    // 🔹 5초 카운트 시작 → StartMenu
    if (time == 5 && !isWatingRoom)
    {
        StartingMenuWidget->UpdateTitleText(true);
        ShowHiddenStartMenu();
        HiddenWaitingRoom();
    }

    if (time == 0 && isWatingRoom)
    {

        //두번째 카운트다운
        if (isWatingRoom) {                
            WatingRoomWidget->ClearInformationText();

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
            PM->SpawnHeli();//헬기 여기요
            */

            return;
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
        //FVector NewLocation = FVector(1000, 500, 200); // 이동할 위치
        FVector NewLocation = FVector(15386.461119, -42848.227375, 550.232971); // 이동할 위치
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

void ACustomPlayerController::InitMinimap(UTextureRenderTarget2D* RT)
{
    if (MinimapWidgetClass)
    {
        MiniMapWidget = CreateWidget<UMinimap>(this, MinimapWidgetClass);
        if (MiniMapWidget)
        {
            MiniMapWidget->AddToViewport();
            MiniMapWidget->SetRenderTarget(RT); // ✅ 확실히 값 있음
            HiddendMiniMap();
        }
    }
}

void ACustomPlayerController::UpdateGlobalMap(bool state)
{
    if (state) {
        GlobalMapWidget->SetVisibility(ESlateVisibility::Visible);
    }
    else {
        GlobalMapWidget->SetVisibility(ESlateVisibility::Hidden);
    }    
}

void ACustomPlayerController::ShowWaitingRoom()
{
    WatingRoomWidget->SetVisibility(ESlateVisibility::Visible);//
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
void ACustomPlayerController::SetDisableStartButton()
{
    StartingMenuWidget->SetStartButtonDisabled();
}
void ACustomPlayerController::ShowMiniMap()
{
    MiniMapWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);//
}

void ACustomPlayerController::HiddendMiniMap()
{
    MiniMapWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ACustomPlayerController::ShowInformationText()
{
    InformationWidget->SetVisibility(ESlateVisibility::Visible);//
}

void ACustomPlayerController::HiddenInformationText()
{
    InformationWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ACustomPlayerController::ShowEndGameUI()
{
    EndGameWidget->SetVisibility(ESlateVisibility::Visible);
}

void ACustomPlayerController::HiddenEndGameUI()
{
    EndGameWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ACustomPlayerController::ShowRoomUI()
{
    RoomWidget->SetVisibility(ESlateVisibility::Visible);
}

void ACustomPlayerController::HiddenRoomUI()
{
    RoomWidget->SetVisibility(ESlateVisibility::Hidden);
}
