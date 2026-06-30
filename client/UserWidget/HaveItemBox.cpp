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

    OwnerInventory->ClearSubWeaponIfMatches(DragOp->ItemSpawnID);//보조무기에서 넘어왔을경우 보조무기 해제
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
    weaponActor->sendItemPacket();//서버로 다른 클라는 아이템이 안보이게 지정된 위치로 이동하게 함 내가 아이템을 획득했으니까

    OwnerInventory->RemoveItemWidgetNoItemSpawn(DragOp->FromSlotIndex);
    OwnerInventory->RemoveFloorItem(DragOp->ItemActor); // 있다면    
    return true;
}