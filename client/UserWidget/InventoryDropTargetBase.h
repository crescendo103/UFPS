// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryDropTargetBase.generated.h"

class UMyInventory;
class UMyDragDropOperation;

/**
 * 인벤토리 드래그-드롭 타겟 위젯들의 공통 베이스.
 * TrashBorder, HaveItemBox, FloorBox, WeaponBox가 공유하던
 * NativeOnDrag* 보일러플레이트를 한 곳에 모은다.
 */
UCLASS(Abstract)
class FPS_API UInventoryDropTargetBase : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetOwnerInventory(UMyInventory* InInventory);

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

    // 공통 캐스팅/널체크를 처리하고, 실제 로직은 OnItemDropped로 위임
    virtual bool NativeOnDrop(
        const FGeometry& InGeometry,
        const FDragDropEvent& InDragDropEvent,
        UDragDropOperation* InOperation) override;

    UPROPERTY()
    UMyInventory* OwnerInventory;

protected:
    // 각 박스(Trash/Have/Floor/Weapon)별 고유 로직만 여기에 구현
    // 반환값 false면 NativeOnDrop도 false를 반환
    virtual bool OnItemDropped(UMyDragDropOperation* DragOp) PURE_VIRTUAL(UInventoryDropTargetBase::OnItemDropped, return false;);
};