// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/Border.h>

#include <Components/Image.h>
#include "MyItem.generated.h"




/**
 * 
 */
class UMyInventory;

UCLASS()
class FPS_API UMyItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(int32 InSlotIndex, int32 InItemID,AActor* actor);
    void Init(int32 InSlotIndex, int32 InItemID);
    virtual void NativeConstruct() override;

    virtual FReply NativeOnMouseButtonDown(
        const FGeometry& InGeometry,
        const FPointerEvent& InMouseEvent) override;

    virtual void NativeOnDragDetected(
        const FGeometry& InGeometry,
        const FPointerEvent& InMouseEvent,
        UDragDropOperation*& OutOperation) override;

    virtual bool NativeOnDrop(
        const FGeometry& InGeometry,
        const FDragDropEvent& InDragDropEvent,
        UDragDropOperation* InOperation) override;

    void SetOwnerInventory(UMyInventory* InInventory);

    void SetItemIcon(UTexture2D* Icon);
    void Refresh();

    UPROPERTY()
    class UMyInventory* OwnerInventory;

    UPROPERTY(meta = (BindWidget))
    UImage* ItemImage;



    int32 SlotIndex;
    int32 ItemID;
    AActor* Actor;
    
};
