// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "InventoryDropTargetBase.h"
#include "Components/ScrollBox.h"
#include "HaveItemBox.generated.h"

/**
 *
 */
class UMyItem;
class UMyDragDropOperation;

UCLASS()
class FPS_API UHaveItemBox : public UInventoryDropTargetBase
{
    GENERATED_BODY()

public:
    UScrollBox* GetHaveScrollBox();
    void SwapItems(int32 From, int32 To);
    void RefreshSlot(int32 SlotIdx);

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