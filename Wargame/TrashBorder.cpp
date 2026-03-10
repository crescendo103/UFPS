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

    if (DragOp->ItemActor->GetAttachParentActor())
    {
        // 지금 다른 Actor에 붙어 있음
        DragOp->ItemActor->DetachFromActor(
            FDetachmentTransformRules::KeepWorldTransform
        );

        if (USphereComponent* Sphere =
            Cast<AWeaponActor>(DragOp->ItemActor)->GetTriggerSphere())
        {
            Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
            Sphere->SetGenerateOverlapEvents(true);
            

            Sphere->UpdateOverlaps();
            // ===== 🔍 디버그 로그 시작 =====
            UE_LOG(LogTemp, Warning, TEXT("===== TriggerSphere Debug ====="));
            UE_LOG(LogTemp, Warning, TEXT("CollisionEnabled : %d"),
                (int32)Sphere->GetCollisionEnabled());

            UE_LOG(LogTemp, Warning, TEXT("GenerateOverlapEvents : %s"),
                Sphere->GetGenerateOverlapEvents() ? TEXT("TRUE") : TEXT("FALSE"));

            UE_LOG(LogTemp, Warning, TEXT("CollisionObjectType : %d"),
                (int32)Sphere->GetCollisionObjectType());

            UE_LOG(LogTemp, Warning, TEXT("SphereLocation : %s"),
                *Sphere->GetComponentLocation().ToString());

            UE_LOG(LogTemp, Warning, TEXT("SphereRadius : %f"),
                Sphere->GetScaledSphereRadius());

            TArray<AActor*> OverlappingActors;
            Sphere->GetOverlappingActors(OverlappingActors);

            UE_LOG(LogTemp, Warning, TEXT("OverlappingActors Count : %d"),
                OverlappingActors.Num());

            for (AActor* Actor : OverlappingActors)
            {
                UE_LOG(LogTemp, Warning, TEXT(" - Overlap Actor : %s"),
                    *Actor->GetName());
            }
            UE_LOG(LogTemp, Warning, TEXT("=============================="));
            // ===== 🔍 디버그 로그 끝 =====
        }
    }

    UE_LOG(LogTemp, Log,
        TEXT("쓰레기 영역"));
    return true;
}