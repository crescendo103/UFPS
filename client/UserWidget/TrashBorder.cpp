// Fill out your copyright notice in the Description page of Project Settings.
#include "TrashBorder.h"
#include "Components/Border.h"
#include "MyDragDropOperation.h"
#include "MyInventory.h"
#include "WeaponActor.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

bool UTrashBorder::OnItemDropped(UMyDragDropOperation* DragOp)
{        
    OwnerInventory->WhosGunCompoToTrash(DragOp->FromSlotIndex);
    OwnerInventory->ClearSubWeaponIfMatches(DragOp->ItemSpawnID);
    OwnerInventory->RemoveActorHaveActorsWithHaveBox(DragOp->ItemActor);
    OwnerInventory->RemoveItemWidget(DragOp->FromSlotIndex);

    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    FVector PlayerLocation = Player->GetActorLocation();
    FVector Forward = Player->GetActorForwardVector();
    FVector NewLocation = PlayerLocation + Forward * 200.f;

    FItemPacket packet;
    packet.Header.Type = (int32)EPacketType::Item;
    packet.Header.Size = sizeof(FItemPacket);
    packet.X = NewLocation.X;
    packet.Y = NewLocation.Y;
    packet.Z = NewLocation.Z;
    packet.OwnerId = -1; // 임시
    packet.ItemRow = DragOp->ItemID;
    packet.ItemSpawnID = DragOp->ItemSpawnID;
    packet.ShouldRemove = true;

    UMyServer* MyServer = GetGameInstance()->GetSubsystem<UMyServer>();
    MyServer->MoveItem(packet);

    OwnerInventory->Owner->setCurrentWeapon(EWeaponType::Melee);

    if (DragOp->ItemActor->GetAttachParentActor())
    {
        // 지금 다른 Actor에 붙어 있음
        DragOp->ItemActor->DetachFromActor(
            FDetachmentTransformRules::KeepWorldTransform
        );
        OwnerInventory->Owner->setCurrentWeapon(EWeaponType::Melee);
        OwnerInventory->Owner->resetCurrentGunWeapon();

        if (USphereComponent* Sphere =
            Cast<AWeaponActor>(DragOp->ItemActor)->GetTriggerSphere())
        {
            Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
            Sphere->SetGenerateOverlapEvents(true);
            Sphere->UpdateOverlaps();
        }
    }

    UE_LOG(LogTemp, Log, TEXT("TrashBorder"));
    return true;
}