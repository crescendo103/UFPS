// Fill out your copyright notice in the Description page of Project Settings.


#include "TrashBorder.h"
#include "Components/Border.h"
#include "MyDragDropOperation.h"
#include "MyInventory.h"
#include "WeaponActor.h"
#include "Components/SphereComponent.h"

void UTrashBorder::SetOwnerInventory(UMyInventory* InInventory)
{
    OwnerInventory = InInventory;
}

void UTrashBorder::NativeOnDragEnter(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    UE_LOG(LogTemp, Warning, TEXT("Drag Enter Trash"));
    //TrashBorder->SetBrushColor(FLinearColor::Red);

    //TrashBorder->SetBrushColor(FLinearColor::Red);
}

void UTrashBorder::NativeOnDragLeave(
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    //TrashBorder->SetBrushColor(FLinearColor::White);
}

bool UTrashBorder::NativeOnDragOver(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    return true; // ⭐ 핵심
}

bool UTrashBorder::NativeOnDrop(
    const FGeometry& InGeometry,
    const FDragDropEvent& InDragDropEvent,
    UDragDropOperation* InOperation)
{
    UE_LOG(LogTemp, Warning, TEXT("Drop On Trash"));
    

    UMyDragDropOperation* DragOp =
        Cast<UMyDragDropOperation>(InOperation);

    if (!DragOp || !OwnerInventory)
        return false;
    UE_LOG(LogTemp, Warning, TEXT("ItemID : %d"), DragOp->ItemID);
    UE_LOG(LogTemp, Warning, TEXT("FromSlotIndex : %d"), DragOp->FromSlotIndex);
    UE_LOG(LogTemp, Warning, TEXT("ItemActor : %s"),
        DragOp->ItemActor ? *DragOp->ItemActor->GetName() : TEXT("NULL"));
    OwnerInventory->WhosGunCompoToTrash(DragOp->FromSlotIndex);
    
    OwnerInventory->RemoveActorHaveActorsWithHaveBox(DragOp->ItemActor);

    OwnerInventory->RemoveItemWidget(DragOp->FromSlotIndex);

    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    FVector PlayerLocation = Player->GetActorLocation();
    FVector Forward = Player->GetActorForwardVector();
    FVector NewLocation = PlayerLocation + Forward * 200.f;

    FItemPacket packet;
    //FVector ItemPos = GetActorLocation();
    packet.Header.Type = (int32)EPacketType::Item;
    packet.Header.Size = sizeof(FItemPacket);
    packet.X = NewLocation.X;// ItemPos.X;
    packet.Y = NewLocation.Y;//ItemPos.Y;
    packet.Z = NewLocation.Z;//ItemPos.Z;
    packet.OwnerId = -1;//임시
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

    UE_LOG(LogTemp, Log,
        TEXT("쓰레기 영역"));
    return true;
}