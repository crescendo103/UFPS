// Fill out your copyright notice in the Description page of Project Settings.
#include "FloorBox.h"
#include "MyDragDropOperation.h"
#include "MyInventory.h"

bool UFloorBox::OnItemDropped(UMyDragDropOperation* DragOp)
{
    UE_LOG(LogTemp, Warning, TEXT("Drop On floorbox"));

    OwnerInventory->ClearSubWeaponIfMatches(DragOp->ItemSpawnID);

    OwnerInventory->AddFloorItemBox(DragOp->ItemID, DragOp->ItemActor, DragOp->ItemSpawnID);

    UE_LOG(LogTemp, Warning,
        TEXT("[Drop] FromSlotIndex=%d, ItemID=%d, Actor=%s"),
        DragOp->FromSlotIndex,
        DragOp->ItemID,
        DragOp->ItemActor ? *DragOp->ItemActor->GetName() : TEXT("NULL")
    );

    OwnerInventory->RemoveItemWidget(DragOp->FromSlotIndex);
    OwnerInventory->RemoveFloorItem(DragOp->ItemActor); // 있다면

    UE_LOG(LogTemp, Log, TEXT("추가 영역"));
    return true;
}