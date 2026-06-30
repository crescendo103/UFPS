// GlobalMap.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GlobalMap.generated.h"

class UImage;
class UTextureRenderTarget2D;
class UMaterialInterface;
class UMaterialInstanceDynamic;

UCLASS()
class FPS_API UGlobalMap : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	virtual void NativeTick(
		const FGeometry& MyGeometry,
		float InDeltaTime) override;

	void SetRenderTarget(
		UTextureRenderTarget2D* RT);

public:

	UPROPERTY(meta = (BindWidget))
	UImage* MapImage;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "Minimap")
	UMaterialInterface* MinimapMaterial;

	UPROPERTY()
	UTextureRenderTarget2D* CurrentRT;

private:

	UPROPERTY()
	UMaterialInstanceDynamic* MID;

public:

	FVector MapCenter;

	float OrthoWidth = 0.f;

	float MapSizeX = 0.f;
	float MapSizeY = 0.f;

	FVector PlayerLoc;
};