// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBoxPlayer.h"
#include "MyCharacter.h"
#include "MyDragDropOperation.h"
#include "TypeWeapon.h"

void UWeaponBoxPlayer::SetOwner(AMyCharacter* actor)
{
	Owner = actor;
}

bool UWeaponBoxPlayer::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    UE_LOG(LogTemp, Warning, TEXT("Drop On PlayerWeaponBox"));

    UMyDragDropOperation* DragOp = Cast<UMyDragDropOperation>(InOperation);

    if (!DragOp || !OwnerInventory)
        return false;

    UE_LOG(LogTemp, Warning, TEXT("ItemID : %d"), DragOp->ItemID);
    UE_LOG(LogTemp, Warning, TEXT("FromSlotIndex : %d"), DragOp->FromSlotIndex);

    // 🔹 슬롯에서 Actor 가져오기
    AActor* ItemActor = DragOp->ItemActor;
        //OwnerInventory->GetActorFromSlot(DragOp->FromSlotIndex);

    if (!ItemActor)
        return false;

    // 🔹 인벤토리에서 제거
    //OwnerInventory->RemoveItemWidgetNoItemSpawn(DragOp->FromSlotIndex);

    // 🔹 플레이어 무기 슬롯 UI 생성
    OwnerInventory->AddWeaponBoxPlayer(DragOp->ItemID, ItemActor, DragOp->ItemSpawnID);
    OwnerInventory->RemoveItemWidgetNoItemSpawn(DragOp->FromSlotIndex);
    OwnerInventory->RemoveFloorItem(DragOp->ItemActor);//있다면

    // 🔹 무기 장착
    if (OwnerInventory->Owner)
    {
        OwnerInventory->Owner->WeaponAttach(ItemActor, "RightHandPinky4Socket");

        OwnerInventory->Owner->CurrentWeapon =
            static_cast<EWeaponType>(DragOp->ItemID);
    }

    // 🔹 PlayerWeaponData 저장
    PlayerWeaponData.itemid = DragOp->ItemID;
    PlayerWeaponData.WeaponActor = ItemActor;

    UE_LOG(LogTemp, Warning, TEXT("Weapon Equipped"));
    UE_LOG(LogTemp, Warning, TEXT("PlayerWeaponData.itemid = %d"), PlayerWeaponData.itemid);

    UE_LOG(LogTemp, Warning, TEXT("PlayerWeaponData.WeaponActor = %s"),
        PlayerWeaponData.WeaponActor ? *PlayerWeaponData.WeaponActor->GetName() : TEXT("NULL"));

    return true;
    /*
    if (DragOp->ItemID == 1) {
        OwnerInventory->RemoveItemWidgetNoItemSpawn(DragOp->FromSlotIndex); ???
    }
    */
}
