// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "InventoryDropTargetBase.h"
#include "TrashBorder.generated.h"

/**
 *
 */
class UBorder;
class UMyDragDropOperation;

UCLASS()
class FPS_API UTrashBorder : public UInventoryDropTargetBase
{
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    UBorder* TrashBorder;

protected:
    virtual bool OnItemDropped(UMyDragDropOperation* DragOp) override;
};