// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TrashBorder.generated.h"

/**
 * 
 */
class UBorder;
class UMyInventory;

UCLASS()
class FPS_API UTrashBorder : public UUserWidget
{
	GENERATED_BODY()
	

public:
    virtual bool NativeOnDrop(
        const FGeometry& InGeometry,
        const FDragDropEvent& InDragDropEvent,
        UDragDropOperation* InOperation) override;

    virtual void NativeOnDragEnter(
        const FGeometry& InGeometry,
        const FDragDropEvent& InDragDropEvent,
        UDragDropOperation* InOperation) override;

    virtual void NativeOnDragLeave(
        const FDragDropEvent& InDragDropEvent,
        UDragDropOperation* InOperation) override;

    virtual bool NativeOnDragOver(
        const FGeometry& InGeometry,
        const FDragDropEvent& InDragDropEvent,
        UDragDropOperation* InOperation) override;
    

    UPROPERTY(meta = (BindWidget))
    UBorder* TrashBorder;

    void SetOwnerInventory(UMyInventory* InInventory);

    UPROPERTY()
    UMyInventory* OwnerInventory;
};
