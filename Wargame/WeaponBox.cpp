// WeaponBox.cpp
#include "WeaponBox.h"
#include "MyDragDropOperation.h"
#include "MyInventory.h"
#include "MyCharacter.h"

bool UWeaponBox::OnItemDropped(UMyDragDropOperation* DragOp)
{
    OwnerInventory->AddWeaponBox(DragOp->ItemID, DragOp->ItemActor, DragOp->ItemSpawnID);

    OwnerInventory->RemoveItemWidgetNoItemSpawn(DragOp->FromSlotIndex);
    OwnerInventory->RemoveFloorItem(DragOp->ItemActor);
    OwnerInventory->ActiveSecondWeaponToPlayer(DragOp->ItemActor);
    WeaponData = { DragOp->FromSlotIndex, DragOp->ItemActor, DragOp->ItemSpawnID }; // ItemSpawnID 추가


    UE_LOG(LogTemp, Log, TEXT("WeaponBox: item dropped"));
    return true;
}

USizeBox* UWeaponBox::GetSizeBox()
{
    return WeaponBox;
}