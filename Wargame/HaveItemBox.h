// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
 // UMyItem 클래스 정의 헤더

#include "HaveItemBox.generated.h"

/**
 * 
 */

class UMyInventory;
class UMyItem;
UCLASS()
class FPS_API UHaveItemBox : public UUserWidget
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

    void SetOwnerInventory(UMyInventory* InInventory);
   
    UScrollBox* GetHaveScrollBox();
    void SwapItems(int32 From, int32 To);
    void RefreshSlot(int32 SlotIdx);

    UPROPERTY(meta = (BindWidget))
    UScrollBox* HaveItemBox;

    UPROPERTY() 
    UMyInventory* OwnerInventory;

    

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TSubclassOf<UMyItem> ItemSlotClass;

    
    UPROPERTY()
    TArray<AActor*> HaveActor;
    
    // 🔹 데이터
    UPROPERTY()
    TArray<int32> ItemIDs;

    UPROPERTY()
    TArray<UMyItem*> ItemWidgets;
};
