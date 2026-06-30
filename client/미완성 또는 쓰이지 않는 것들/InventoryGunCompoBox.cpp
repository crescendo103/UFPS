// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryGunCompoBox.h"
#include "MyDragDropOperation.h"
#include "MyInventory.h"

bool UInventoryGunCompoBox::NativeOnDragOver(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    return true; // ⭐ 핵심
}

bool UInventoryGunCompoBox::NativeOnDrop(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    UE_LOG(LogTemp, Warning, TEXT("Drop On Compobox"));


    UMyDragDropOperation* DragOp =
        Cast<UMyDragDropOperation>(InOperation);

    if (!DragOp || !OwnerInventory)
        return false;
    
    
    OwnerInventory->AttachGunCompo(DragOp->FromSlotIndex, DragOp->ItemActor);

    //OwnerInventory->RemoveItemWidget(DragOp->FromSlotIndex);    
    OwnerInventory->RemoveItemWidgetNoItemSpawn(DragOp->FromSlotIndex);
    
    
    return true;
}


void UInventoryGunCompoBox::SetOwnerInventory(UMyInventory* InInventory)
{
    OwnerInventory = InInventory;
}

USizeBox* UInventoryGunCompoBox::GetScrollBox()
{
    return WeaponBox;
}
