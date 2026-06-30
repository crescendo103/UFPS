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

    // PlayerWeaponData 저장 총기 스코프 같은거랑 연계해서 사용하려고 했는데 미완성
    PlayerWeaponData.itemid = DragOp->ItemID;
    PlayerWeaponData.WeaponActor = ItemActor;    
    //----------------------------------------------
    return true;
}