// Fill out your copyright notice in the Description page of Project Settings.


#include "StartMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "StartMenuPawn.h"
#include "MyServer.h"

void UStartMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UStartMenuWidget::OnStartClicked);
	}
}

void UStartMenuWidget::SetStartMenuPawn(AStartMenuPawn* pawn)
{
	StartPawn = pawn;
}

void UStartMenuWidget::OnStartClicked()
{
	int32 TimeValue = 5;

	SendTimePacket(TimeValue);	
	
	//StartPawn->MoveEnemy(1);
}

void UStartMenuWidget::SendTimePacket(int32 TimeValue)
{
	UMyServer* MyServer = GetGameInstance()->GetSubsystem<UMyServer>();
	if (MyServer)
	{
		MyServer->MoveTime(TimeValue);
		//MyServer->SetMyCharacterId();//게임 접속할때 안전하게 서버에서 받은 캐릭터 id를 캐릭터클래스에 연결
	}

	
}

void UStartMenuWidget::SetInformationText(const FText& InText)
{
	UE_LOG(LogTemp, Warning, TEXT("StartMenu SetInformationText: %s"), *InText.ToString());

	if (!TitleText)
	{
		UE_LOG(LogTemp, Error, TEXT("TitleText is NULL"));
		return;
	}

	TitleText->SetText(InText);
}