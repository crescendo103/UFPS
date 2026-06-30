// WeaponBox.cpp
#include "WeaponBox.h"
#include "MyDragDropOperation.h"
#include "MyInventory.h"
#include "MyCharacter.h"

bool UWeaponBox::OnItemDropped(UMyDragDropOperation* DragOp)
{
    /*
    // 1. 인벤토리(보유 슬롯) UI에서 먼저 제거
    OwnerInventory->RemoveItemWidgetNoItemSpawn(DragOp->FromSlotIndex);
    // 2. 바닥(FloorBox) UI에서도 제거 — 혹시 거기 있었다면
    OwnerInventory->RemoveFloorItem(DragOp->ItemActor);
    // 3. WeaponBox 슬롯에 등록
    OwnerInventory->AddWeaponBox(DragOp->ItemID, DragOp->ItemActor, DragOp->ItemSpawnID);
    // 4. 실제 캐릭터에 보조무기로 부착 (소켓 부착까지 처리됨)
    OwnerInventory->ActiveSecondWeaponToPlayer(DragOp->ItemActor);
    WeaponData = { DragOp->FromSlotIndex, DragOp->ItemActor, DragOp->ItemSpawnID };
    */
    OwnerInventory->AddWeaponBox(DragOp->ItemID, DragOp->ItemActor, DragOp->ItemSpawnID);

    OwnerInventory->RemoveItemWidgetNoItemSpawn(DragOp->FromSlotIndex);
    OwnerInventory->RemoveFloorItem(DragOp->ItemActor);
    OwnerInventory->ActiveSecondWeaponToPlayer(DragOp->ItemActor);
    WeaponData = { DragOp->FromSlotIndex, DragOp->ItemActor, DragOp->ItemSpawnID };


    UE_LOG(LogTemp, Log, TEXT("WeaponBox: item dropped"));
    return true;
}

USizeBox* UWeaponBox::GetSizeBox()
{
    return WeaponBox;
}