// Fill out your copyright notice in the Description page of Project Settings.
#include "InventoryDropTargetBase.h"
#include "MyDragDropOperation.h"
#include "MyInventory.h"

void UInventoryDropTargetBase::SetOwnerInventory(UMyInventory* InInventory)
{
    OwnerInventory = InInventory;
}

void UInventoryDropTargetBase::NativeOnDragEnter(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    // 필요 시 하위 클래스에서 시각 효과 등을 추가하려면
    // 이 함수를 virtual로 유지하고 Super:: 호출 후 확장
}

void UInventoryDropTargetBase::NativeOnDragLeave(
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
}

bool UInventoryDropTargetBase::NativeOnDragOver(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    return true;
}

bool UInventoryDropTargetBase::NativeOnDrop(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    UMyDragDropOperation* DragOp = Cast<UMyDragDropOperation>(InOperation);
    if (!DragOp || !OwnerInventory)
        return false;

    return OnItemDropped(DragOp);
}