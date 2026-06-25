// Fill out your copyright notice in the Description page of Project Settings.
#include "HaveItemBox.h"
#include "MyDragDropOperation.h"
#include "MyItem.h"
#include <Components/ScrollBoxSlot.h>
#include "MyInventory.h"
#include "MyCharacter.h"
#include "WeaponActor.h"

UScrollBox* UHaveItemBox::GetHaveScrollBox()
{
    return HaveItemBox;
}

bool UHaveItemBox::OnItemDropped(UMyDragDropOperation* DragOp)
{
    UE_LOG(LogTemp, Warning, TEXT("Drop On HaveItem"));

    OwnerInventory->ClearSubWeaponIfMatches(DragOp->ItemSpawnID);

    OwnerInventory->AddHaveItemBox(DragOp->ItemID, DragOp->ItemActor, DragOp->ItemSpawnID);

    // 아이템 좌표 이동
    FVector TargetLocation(100.f, 200.f, 300.f); // ← 원하는 좌표
    DragOp->ItemActor->SetActorLocation(
        TargetLocation,
        false,   // sweep ❌ (충돌 무시)
        nullptr,
        ETeleportType::TeleportPhysics
    );

    AWeaponActor* weaponActor = Cast<AWeaponActor>(DragOp->ItemActor);
    weaponActor->sendItemPacket();

    //OwnerInventory->Owner->setCurrentWeapon(EWeaponType::Rifle);?
    OwnerInventory->RemoveItemWidgetNoItemSpawn(DragOp->FromSlotIndex);
    OwnerInventory->RemoveFloorItem(DragOp->ItemActor); // 있다면

    UE_LOG(LogTemp, Log, TEXT("추가 영역"));
    return true;
}