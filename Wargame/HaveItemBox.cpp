// Fill out your copyright notice in the Description page of Project Settings.


#include "HaveItemBox.h"
#include "MyDragDropOperation.h"
#include "MyItem.h"
#include <Components/ScrollBoxSlot.h>
#include "MyInventory.h"

void UHaveItemBox::SetOwnerInventory(UMyInventory* InInventory)
{
    OwnerInventory = InInventory;
}


UScrollBox* UHaveItemBox::GetHaveScrollBox()
{
    return HaveItemBox;
}

void UHaveItemBox::NativeOnDragEnter(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    UE_LOG(LogTemp, Warning, TEXT("Drag Enter Trash"));
    //TrashBorder->SetBrushColor(FLinearColor::Red);

    //TrashBorder->SetBrushColor(FLinearColor::Red);
}

void UHaveItemBox::NativeOnDragLeave(
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    //TrashBorder->SetBrushColor(FLinearColor::White);
}

bool UHaveItemBox::NativeOnDragOver(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    return true; // ⭐ 핵심
}

bool UHaveItemBox::NativeOnDrop(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    UE_LOG(LogTemp, Warning, TEXT("Drop On HaveItem"));


    UMyDragDropOperation* DragOp =
        Cast<UMyDragDropOperation>(InOperation);

    if (!DragOp || !OwnerInventory)
        return false;

    

    //
    OwnerInventory->AddHaveItemBox(DragOp->ItemID, DragOp->ItemActor);
   
    //아이템 좌표 이동

    FVector TargetLocation(100.f, 200.f, 300.f); // ← 원하는 좌표

    DragOp->ItemActor->SetActorLocation(
        TargetLocation,
        false,   // sweep ❌ (충돌 무시)
        nullptr,
        ETeleportType::TeleportPhysics
    );

    // ⭐⭐⭐ 핵심 ⭐⭐⭐
    // 2️⃣ FloorItemBox 쪽 아이템 제거
    UE_LOG(LogTemp, Warning,
        TEXT("[Drop] FromSlotIndex=%d, ItemID=%d, Actor=%s"),
        DragOp->FromSlotIndex,
        DragOp->ItemID,
        DragOp->ItemActor ? *DragOp->ItemActor->GetName() : TEXT("NULL")
    );

    
    //OwnerInventory->RemoveItemWidget(DragOp->FromSlotIndex);    
    OwnerInventory->RemoveItemWidgetNoItemSpawn(DragOp->FromSlotIndex);

    UE_LOG(LogTemp, Log,
        TEXT("추가 영역"));
    return true;
}

