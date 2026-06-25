// Fill out your copyright notice in the Description page of Project Settings.
#include "WeaponBoxPlayer.h"
#include "MyCharacter.h"
#include "MyInventory.h"
#include "MyDragDropOperation.h"
#include "TypeWeapon.h"

void UWeaponBoxPlayer::SetOwner(AMyCharacter* actor)
{
    Owner = actor;
}

bool UWeaponBoxPlayer::OnItemDropped(UMyDragDropOperation* DragOp)
{
    UE_LOG(LogTemp, Warning, TEXT("Drop On PlayerWeaponBox"));
    UE_LOG(LogTemp, Warning, TEXT("ItemID : %d"), DragOp->ItemID);
    UE_LOG(LogTemp, Warning, TEXT("FromSlotIndex : %d"), DragOp->FromSlotIndex);

    AActor* ItemActor = DragOp->ItemActor;
    if (!ItemActor)
        return false;

    // 플레이어 무기 슬롯 UI 생성
    OwnerInventory->AddWeaponBoxPlayer(DragOp->ItemID, ItemActor, DragOp->ItemSpawnID);
    OwnerInventory->RemoveItemWidgetNoItemSpawn(DragOp->FromSlotIndex);
    OwnerInventory->RemoveFloorItem(DragOp->ItemActor); // 있다면

    // 무기 장착
    if (OwnerInventory->Owner)
    {
        OwnerInventory->Owner->WeaponAttach(ItemActor, "RightHandPinky4Socket");
        OwnerInventory->Owner->setCurrentWeapon(static_cast<EWeaponType>(DragOp->ItemID));
    }

    // PlayerWeaponData 저장
    PlayerWeaponData.itemid = DragOp->ItemID;
    PlayerWeaponData.WeaponActor = ItemActor;

    UE_LOG(LogTemp, Warning, TEXT("Weapon Equipped"));
    UE_LOG(LogTemp, Warning, TEXT("PlayerWeaponData.itemid = %d"), PlayerWeaponData.itemid);
    UE_LOG(LogTemp, Warning, TEXT("PlayerWeaponData.WeaponActor = %s"),
        PlayerWeaponData.WeaponActor ? *PlayerWeaponData.WeaponActor->GetName() : TEXT("NULL"));

    return true;
}