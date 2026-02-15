// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorBox.h"
#include "MyDragDropOperation.h"
#include <Components/ScrollBoxSlot.h>
#include "MyInventory.h"

void UFloorBox::SetOwnerInventory(UMyInventory* InInventory)
{
    OwnerInventory = InInventory;
}

void UFloorBox::NativeOnDragEnter(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    UE_LOG(LogTemp, Warning, TEXT("Drag Enter Trash"));
    //TrashBorder->SetBrushColor(FLinearColor::Red);

    //TrashBorder->SetBrushColor(FLinearColor::Red);
}

void UFloorBox::NativeOnDragLeave(
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    //TrashBorder->SetBrushColor(FLinearColor::White);
}

bool UFloorBox::NativeOnDragOver(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    return true; // ⭐ 핵심
}

bool UFloorBox::NativeOnDrop(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    UE_LOG(LogTemp, Warning, TEXT("Drop On floorbox"));


    UMyDragDropOperation* DragOp =
        Cast<UMyDragDropOperation>(InOperation);

    if (!DragOp || !OwnerInventory)
        return false;
    OwnerInventory->AddFloorItemBox(DragOp->ItemID, DragOp->ItemActor);
    
    UE_LOG(LogTemp, Warning,
        TEXT("[Drop] FromSlotIndex=%d, ItemID=%d, Actor=%s"),
        DragOp->FromSlotIndex,
        DragOp->ItemID,
        DragOp->ItemActor ? *DragOp->ItemActor->GetName() : TEXT("NULL")
    );

    // ⭐⭐⭐ 핵심 ⭐⭐⭐
    // 2️⃣ FloorItemBox 쪽 아이템 제거
    OwnerInventory->RemoveItemWidget(DragOp->FromSlotIndex);

    UE_LOG(LogTemp, Log,
        TEXT("추가 영역"));
    return true;
}