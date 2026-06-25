// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "InventoryDropTargetBase.h"
#include "MyItem.h"
#include "Components/ScrollBox.h"
#include "FloorBox.generated.h"

/**
 *
 */
class UMyDragDropOperation;

UCLASS()
class FPS_API UFloorBox : public UInventoryDropTargetBase
{
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    UScrollBox* HaveItemBox;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TSubclassOf<UMyItem> ItemSlotClass;

    UPROPERTY()
    TArray<AActor*> HaveActor;

    // 🔹 데이터
    UPROPERTY()
    TArray<int32> ItemIDs;

    UPROPERTY()
    TArray<UMyItem*> ItemWidgets;

protected:
    virtual bool OnItemDropped(UMyDragDropOperation* DragOp) override;
};