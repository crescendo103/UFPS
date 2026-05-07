// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Minimap.generated.h"

/**
 * 
 */
class UImage;
UCLASS()
class FPS_API UMinimap : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void SetRenderTarget(class UTextureRenderTarget2D* RT);

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UImage* MinimapImage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UMaterialInterface* MinimapMaterial;

private:
    UMaterialInstanceDynamic* MID;
};
