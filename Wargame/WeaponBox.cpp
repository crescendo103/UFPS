// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBox.h"
#include "MyDragDropOperation.h"
#include "MyItem.h"
#include <Components/ScrollBoxSlot.h>
#include "MyInventory.h"
#include "MyCharacter.h"

bool UWeaponBox::NativeOnDragOver(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    return true; // ⭐ 핵심
}

bool UWeaponBox::NativeOnDrop(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    UE_LOG(LogTemp, Warning, TEXT("Drop On HaveItem"));


    UMyDragDropOperation* DragOp =
        Cast<UMyDragDropOperation>(InOperation);

    if (!DragOp || !OwnerInventory)
        return false;
    
    OwnerInventory->AddWeaponBox(DragOp->ItemID, DragOp->ItemActor, DragOp->ItemSpawnID);
    OwnerInventory->Owner->WeaponAttach(DragOp->ItemActor, "backSocket");

    //OwnerInventory->RemoveItemWidget(DragOp->FromSlotIndex);    
    OwnerInventory->RemoveItemWidgetNoItemSpawn(DragOp->FromSlotIndex);
    OwnerInventory->RemoveFloorItem(DragOp->ItemActor);//있다면

    OwnerInventory->ActiveSecondWeaponToPlayer(DragOp->ItemActor);
    //
    FWeaponData Data = { DragOp->FromSlotIndex, DragOp->ItemActor };
    WeaponData = Data;

    UE_LOG(LogTemp, Log,
        TEXT("추가 영역"));
    return true;
}

void UWeaponBox::SetOwnerInventory(UMyInventory* InInventory)
{
    OwnerInventory = InInventory;
}

USizeBox* UWeaponBox::GetSizeBox()
{
    return WeaponBox;
}
