// Fill out your copyright notice in the Description page of Project Settings.


#include "MyItem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "MyDragDropOperation.h"
#include "MyInventory.h"

void UMyItem::Init(int32 InSlotIndex, int32 InItemID, AActor* actor, int32 itemSpawnID)
{
	SlotIndex = InSlotIndex;
	ItemID = InItemID;
    Actor = actor;
    ItemSpanwID = itemSpawnID;
}

void UMyItem::Init(int32 InSlotIndex, int32 InItemID)
{
    SlotIndex = InSlotIndex;
    ItemID = InItemID;
}

void UMyItem::NativeConstruct()
{
    Super::NativeConstruct();    
}

//예약
FReply UMyItem::NativeOnMouseButtonDown(
    const FGeometry& InGeometry,
    const FPointerEvent& InMouseEvent)
{
    return UWidgetBlueprintLibrary::DetectDragIfPressed(
        InMouseEvent, this, EKeys::LeftMouseButton
    ).NativeReply;
}

//실제 실행
void UMyItem::NativeOnDragDetected(
    const FGeometry& InGeometry,
    const FPointerEvent& InMouseEvent,
    UDragDropOperation*& OutOperation)
{
    UMyDragDropOperation* DragOp = NewObject<UMyDragDropOperation>();

    DragOp->ItemID = ItemID;
    DragOp->FromSlotIndex = SlotIndex;
    DragOp->ItemActor = Actor;
    DragOp->ItemSpawnID = ItemSpanwID;
    // 드래그 시 보여줄 위젯 생성
    UMyItem* DragVisual =
        CreateWidget<UMyItem>(GetWorld(), GetClass());

    DragVisual->Init(SlotIndex, ItemID, Actor, ItemSpanwID);
    DragVisual->SetOwnerInventory(OwnerInventory);//
    
    DragVisual->Refresh();//
    // 핵심
    DragVisual->SetVisibility(ESlateVisibility::HitTestInvisible);


    DragOp->DefaultDragVisual = DragVisual;
    DragOp->Pivot = EDragPivot::MouseDown;

    //
    OutOperation = DragOp;

    UE_LOG(LogTemp, Warning,
        TEXT("[DragStart] SlotIndex=%d, ItemID=%d, Actor=%s"),
        SlotIndex,
        ItemID,
        Actor ? TEXT("alive") : TEXT("NULL")
    );
}

bool UMyItem::NativeOnDrop(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    UMyDragDropOperation* DragOp =
        Cast<UMyDragDropOperation>(InOperation);

    if (!DragOp)
        return false;

    UE_LOG(LogTemp, Log,
        TEXT("아이템 이동: %d -> %d"),
        DragOp->FromSlotIndex,
        SlotIndex);

    if (!OwnerInventory)
        return false;

    // 인벤토리에 스왑 요청
    OwnerInventory->SwapItems(
        DragOp->FromSlotIndex,
        SlotIndex
    );

    return true;
}

void UMyItem::SetOwnerInventory(UMyInventory* InInventory)
{
    OwnerInventory = InInventory;
}

void UMyItem::SetItemIcon(UTexture2D* Icon)
{
   
    if (Icon)
    {
        ItemImage->SetVisibility(ESlateVisibility::Visible);
        ItemImage->SetBrushFromTexture(Icon);
    }
    else
    {
        ItemImage->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UMyItem::Refresh()
{
    
    UE_LOG(LogTemp, Warning, TEXT("ItemID = %d"), ItemID);

    if (!ItemImage || ItemID < 0 || !OwnerInventory)
    {
        ItemImage->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    const FItemStaticData* StaticData =
        OwnerInventory->GetItemStaticData(ItemID);

    if (!StaticData)
    {
        UE_LOG(LogTemp, Error, TEXT("StaticData NOT FOUND for ItemID %d"), ItemID);
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("Icon = %s"),
        StaticData->Icon ? *StaticData->Icon->GetName() : TEXT("ICON NULL"));

    ItemImage->SetVisibility(ESlateVisibility::Visible);
    ItemImage->SetBrushFromTexture(StaticData->Icon);
    
}