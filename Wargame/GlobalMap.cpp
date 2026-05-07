// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalMap.h"
#include "Components/CanvasPanelSlot.h"

void UGlobalMap::NativeConstruct()
{
	Super::NativeConstruct();
	

	MapCenter = FVector(15403.913172, -42463.252373 ,3000);
	OrthoWidth = 10000.f;

	if (!MapImage) return;

	FVector2D Size = MapImage->GetCachedGeometry().GetLocalSize();

	MapSizeX = Size.X;
	MapSizeY = Size.Y;
}

void UGlobalMap::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

	Super::NativeTick(MyGeometry, InDeltaTime);

	if (MapSizeX == 0)
	{
		FVector2D Size = MapImage->GetCachedGeometry().GetLocalSize();
		MapSizeX = Size.X;
		MapSizeY = Size.Y;

		//UE_LOG(LogTemp, Warning, TEXT("MapSize: %f %f"), MapSizeX, MapSizeY);
	}



	if (GetOwningPlayer()->IsLocalController())
	{
		// 내 UI
		APawn* PlayerPawn = GetOwningPlayerPawn();
		if (!PlayerPawn) return;

		PlayerLoc = PlayerPawn->GetActorLocation();
		//UE_LOG(LogTemp, Warning, TEXT("PlayerLoc: %s"), *PlayerLoc.ToString());
	}

	CalPlayerPos();
}

void UGlobalMap::CalPlayerPos()
{
	FVector Delta = PlayerLoc - MapCenter;
	float Half = OrthoWidth * 0.5f;
	float WorldToUI = MapSizeX / OrthoWidth;
	//-1~1사이의 좌표
	float NX = Delta.X / Half;
	float NY = Delta.Y / Half;

	//UE_LOG(LogTemp, Warning, TEXT("NX: %f NY: %f"), NX, NY);
	// ⭐ 핵심: 직접 스케일 적용
	float UI_X = (Delta.Y * WorldToUI) + MapSizeX * 0.5f;
	float UI_Y = (-Delta.X * WorldToUI) + MapSizeY * 0.5f;
	//0~1사이의 좌표
	//float UI_X = (NY + 1.f) * 0.5f * MapSizeX;
	//float UI_Y = (1.f - ((NX + 1.f) * 0.5f)) * MapSizeY;

	UpdatePlayerDotPosition(UI_X, UI_Y);
}

void UGlobalMap::UpdatePlayerDotPosition(float UI_X, float UI_Y)
{
	if (!PlayerDot) return;

	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(PlayerDot->Slot);
	if (!CanvasSlot) return;

	// 🔥 점 중심 맞추기 (중요)
	FVector2D DotSize = CanvasSlot->GetSize();

	FVector2D NewPos;
	NewPos.X = UI_X - DotSize.X * 0.5f;
	NewPos.Y = UI_Y - DotSize.Y * 0.5f;

	CanvasSlot->SetPosition(NewPos);
}
