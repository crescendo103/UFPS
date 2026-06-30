// Fill out your copyright notice in the Description page of Project Settings.


#include "Minimap.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"

void UMinimap::NativeConstruct()
{
    Super::NativeConstruct();

    if (MinimapMaterial)
    {
        MID = UMaterialInstanceDynamic::Create(MinimapMaterial, this);
        MinimapImage->SetBrushFromMaterial(MID);
    }
}

void UMinimap::SetRenderTarget(UTextureRenderTarget2D* RT)
{
    if (MID && RT)
    {
        MID->SetTextureParameterValue(TEXT("Param_MinimapRT"), RT);
    }
}