// Fill out your copyright notice in the Description page of Project Settings.

// Add default functionality here for any IHighlightInterface functions that are not pure virtual.
// Fill out your copyright notice in the Description page of Project Settings.

#include "HighlightInterface.h"
/*
#include "GameFramework/Actor.h"
#include "Components/MeshComponent.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
    // MT_ItemHighlight 머티리얼 경로 (실제 프로젝트 경로로 수정 필요)
    static const TCHAR* OutlineMaterialPath = TEXT("/Script/Engine.Material'/Game/Mycontent/Material/MT_ItemHighlight.MT_ItemHighlight'");
}

void IHighlightInterface::ItemOutlineOn_Implementation()
{
    AActor* OwnerActor = Cast<AActor>(this);
    if (!OwnerActor)
    {
        return;
    }

    UMaterialInterface* OutlineMaterial = LoadObject<UMaterialInterface>(nullptr, OutlineMaterialPath);
    if (!OutlineMaterial)
    {
        return;
    }

    TArray<UMeshComponent*> MeshComponents;
    OwnerActor->GetComponents<UMeshComponent>(MeshComponents);

    for (UMeshComponent* MeshComp : MeshComponents)
    {
        if (MeshComp)
        {
            MeshComp->SetOverlayMaterial(OutlineMaterial);
        }
    }
}

void IHighlightInterface::ItemOutlineOff_Implementation()
{
    AActor* OwnerActor = Cast<AActor>(this);
    if (!OwnerActor)
    {
        return;
    }

    TArray<UMeshComponent*> MeshComponents;
    OwnerActor->GetComponents<UMeshComponent>(MeshComponents);

    for (UMeshComponent* MeshComp : MeshComponents)
    {
        if (MeshComp)
        {
            MeshComp->SetOverlayMaterial(nullptr);
        }
    }
}*/