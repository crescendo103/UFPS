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


    UMyDragDropOperation* DragOp =
        Cast<UMyDragDropOperation>(InOperation);

    if (!DragOp || !OwnerInventory)
        return false;

    UE_LOG(LogTemp, Warning, TEXT("ItemID : %d"), DragOp->ItemID);
    UE_LOG(LogTemp, Warning, TEXT("FromSlotIndex : %d"), DragOp->FromSlotIndex);
    UE_LOG(LogTemp, Warning, TEXT("ItemActor : %s"),
        DragOp->ItemActor ? *DragOp->ItemActor->GetName() : TEXT("NULL"));

    OwnerInventory->RemoveItemWidgetNoItemSpawn(DragOp->FromSlotIndex);
    OwnerInventory->AddWeaponBoxPlayer(DragOp->ItemID, DragOp->ItemActor);
    OwnerInventory->Owner->WeaponAttach(DragOp->ItemActor, "RightHandPinky4Socket");
    if (Owner == nullptr) {
        UE_LOG(LogTemp, Warning, TEXT("owner is nullptr!!"));
        
    }
    OwnerInventory->Owner->CurrentWeapon = static_cast<EWeaponType>(DragOp->ItemID);
    UE_LOG(LogTemp, Warning, TEXT("DragOp->ItemID = %d"), DragOp->ItemID);
    UE_LOG(LogTemp, Warning, TEXT("CurrentWeapon (int) = %d"),
        static_cast<int32>(OwnerInventory->Owner->CurrentWeapon));


    //수정해야할듯...
    if (DragOp->ItemID == 1) {
        OwnerInventory->RemoveItemWidgetNoItemSpawn(DragOp->FromSlotIndex);
    }
    //Owner->CurrentWeapon = static_cast<EWeaponType>(DragOp->ItemID);
    
    //OwnerInventory->RemoveItemWidgetNoItemSpawn(DragOp->FromSlotIndex);

    UE_LOG(LogTemp, Log,
        TEXT("추가 영역"));

	return true;
}
