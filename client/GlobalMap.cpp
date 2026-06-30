// GlobalMap.cpp

#include "GlobalMap.h"

#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/TextureRenderTarget2D.h"

void UGlobalMap::NativeConstruct()
{
	Super::NativeConstruct();

	if (!MinimapMaterial)
	{
		UE_LOG(LogTemp, Error,
			TEXT("MinimapMaterial NULL"));

		return;
	}

	if (!MapImage)
	{
		UE_LOG(LogTemp, Error,
			TEXT("MapImage NULL"));

		return;
	}

	// Dynamic Material 생성
	MID = UMaterialInstanceDynamic::Create(
		MinimapMaterial,
		this);

	if (!MID)
	{
		UE_LOG(LogTemp, Error,
			TEXT("MID Create Failed"));

		return;
	}

	// Image에 Material 적용
	MapImage->SetBrushFromMaterial(MID);

	UE_LOG(LogTemp, Warning,
		TEXT("MID Created And Applied"));

	// 이미 RT가 들어와 있었다면 다시 연결
	if (CurrentRT)
	{
		MID->SetTextureParameterValue(
			TEXT("Param_MinimapRT"),
			CurrentRT);

		UE_LOG(LogTemp, Warning,
			TEXT("RT Applied In NativeConstruct"));
	}
}

void UGlobalMap::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime)
{
	Super::NativeTick(
		MyGeometry,
		InDeltaTime);

	// UI Size 초기화
	if (MapSizeX == 0 && MapImage)
	{
		FVector2D Size =
			MapImage->GetCachedGeometry().GetLocalSize();

		MapSizeX = Size.X;
		MapSizeY = Size.Y;
	}
}

void UGlobalMap::SetRenderTarget(
	UTextureRenderTarget2D* RT)
{
	if (!RT)
	{
		UE_LOG(LogTemp, Error,
			TEXT("SetRenderTarget : RT NULL"));

		return;
	}

	CurrentRT = RT;

	UE_LOG(LogTemp, Warning,
		TEXT("RenderTarget Received"));

	// MID가 이미 생성되어 있으면 바로 적용
	if (MID)
	{
		MID->SetTextureParameterValue(
			TEXT("Param_MinimapRT"),
			RT);

		UE_LOG(LogTemp, Warning,
			TEXT("RT Applied To MID"));
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("MID Not Ready Yet"));
	}
}