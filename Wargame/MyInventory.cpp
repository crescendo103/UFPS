// Fill out your copyright notice in the Description page of Project Settings.


#include "MyInventory.h"
#include "MyItem.h"
#include <Components/Image.h>
#include <Components/ScrollBoxSlot.h>
#include "Components/VerticalBoxSlot.h"
#include "GameFramework/Character.h"
#include "HaveItemBox.h"
#include "Components/Image.h"
#include <Kismet/GameplayStatics.h>
#include "WeaponBox.h"
#include "WeaponBoxPlayer.h"
#include "InventoryGunCompoBox.h"
#include "WeaponActor.h"
#include "WeaponCompo.h"
#include "MyCharacter.h"

void UMyInventory::NativeConstruct()
{
    Super::NativeConstruct();

    ItemBoxes.Empty();
    itemid = 0;   
    partsOn = false;

    TrashWidget = CreateWidget<UTrashBorder>(GetWorld(), TrashWidgetClass);
    TrashWidget->SetOwnerInventory(this);
    TrashWidget->AddToViewport(); // ZOrder 높게
    TrashWidget->SetVisibility(ESlateVisibility::Hidden);

    HaveWidget = CreateWidget<UHaveItemBox>(GetWorld(), HaveWidgetClass);
    HaveWidget->SetOwnerInventory(this);
    HaveWidget->AddToViewport(); // ZOrder 높게
    HaveWidget->SetVisibility(ESlateVisibility::Hidden);

    WeaponBoxWidget = CreateWidget<UWeaponBox>(GetWorld(), WeaponBoxWidgetClass);
    WeaponBoxWidget->SetOwnerInventory(this);
    WeaponBoxWidget->AddToViewport(); // ZOrder 높게
    WeaponBoxWidget->SetVisibility(ESlateVisibility::Hidden); 

    WeaponBoxPlayerWidget = CreateWidget<UWeaponBoxPlayer>(GetWorld(), WeaponBoxPlayerWidgetClass);
    WeaponBoxPlayerWidget->SetOwnerInventory(this);
    WeaponBoxPlayerWidget->SetOwner(Owner);
    WeaponBoxPlayerWidget->AddToViewport(); // ZOrder 높게
    WeaponBoxPlayerWidget->SetVisibility(ESlateVisibility::Hidden);

    
    /*606
    GunCompoWidget = CreateWidget<UInventoryGunCompoBox>(GetWorld(), GunCompoWidgetClass);
    GunCompoWidget->SetOwnerInventory(this);
    //GunCompoWidget->SetOwner(Owner);
    
    GunCompoWidget->AddToViewport(); // ZOrder 높게
    GunCompoWidget->SetVisibility(ESlateVisibility::Hidden);*/
}

void UMyInventory::RemoveFloorItem(AActor* Actor)
{
    if (!Actor)
        return;

    UMyItem** FoundWidget = FloorActorWidgets.Find(Actor);

    if (!FoundWidget)
        return;

    if (*FoundWidget)
    {
        (*FoundWidget)->RemoveFromParent();
    }

    FloorActorWidgets.Remove(Actor);
}

void UMyInventory::SwapItems(int32 From, int32 To)
{
    FInventorySlot* SlotA = Slots.Find(From);
    FInventorySlot* SlotB = Slots.Find(To);

    if (!SlotA || !SlotB) return;

    Swap(*SlotA, *SlotB);

    if (SlotA->Widget)
        SlotA->Widget->Init(From, SlotA->ItemID, SlotA->Actor, SlotA->ItemSpanwID);

    if (SlotB->Widget)
        SlotB->Widget->Init(To, SlotB->ItemID, SlotB->Actor, SlotB->ItemSpanwID);

    RefreshSlot(From);
    RefreshSlot(To);
}

void UMyInventory::RefreshSlot(int32 SlotIdx)
{
    FInventorySlot* FoundSlot = Slots.Find(SlotIdx);

    if (!FoundSlot) return;

    if (!FoundSlot->Widget) return;

    const FItemStaticData* StaticData =
        ItemTable->FindRow<FItemStaticData>(
            FName(*FString::FromInt(FoundSlot->ItemID)),
            TEXT("Item Lookup")
        );

    if (!StaticData) return;

    FoundSlot->Widget->SetItemIcon(StaticData->Icon);
}
void UMyInventory::AddHaveItemBox(int32 itemId, AActor* actor, int32 itemSpawnID)
{
    int32 SlotIndex = NextSlotIndex++;

    UMyItem* SlotWidget = CreateAndRegisterSlotWidget(SlotIndex, itemId, actor, itemSpawnID);
    if (!SlotWidget || !HaveWidget)
        return;

    AttachWidgetToScrollBox(HaveWidget->GetHaveScrollBox(), SlotWidget);
}
void UMyInventory::AddFloorItemBox(int32 itemId, AActor* actor, int32 ItemSpanwID)
{
    if (!actor || FloorActorWidgets.Contains(actor))
        return;

    // 바닥 아이템은 슬롯 인덱스(-1)와 Slots 등록이 필요 없음
    UMyItem* SlotWidget = CreateAndRegisterSlotWidget(-1, itemId, actor, ItemSpanwID, /*bRegisterSlot=*/false);
    if (!SlotWidget || !FloorItemBox)
        return;

    AttachWidgetToScrollBox(FloorItemBox, SlotWidget);

    FloorActorWidgets.Add(actor, SlotWidget);
}

void UMyInventory::AddWeaponBox(int32 itemId, AActor* actor, int32 ItemSpanwID)
{
    int32 SlotIndex = NextSlotIndex++;

    UMyItem* SlotWidget = CreateAndRegisterSlotWidget(SlotIndex, itemId, actor, ItemSpanwID);
    if (!SlotWidget || !WeaponBoxWidget)
        return;

    AttachWidgetToSizeBox(WeaponBoxWidget->GetSizeBox(), SlotWidget);
}

void UMyInventory::AddWeaponBoxPlayer(int32 itemId, AActor* actor, int32 ItemSpanwID)
{
    int32 SlotIndex = NextSlotIndex++;

    UMyItem* SlotWidget = CreateAndRegisterSlotWidget(SlotIndex, itemId, actor, ItemSpanwID);
    if (!SlotWidget || !WeaponBoxPlayerWidget)
        return;

    AttachWidgetToSizeBox(WeaponBoxPlayerWidget->GetSizeBox(), SlotWidget);
}



void UMyInventory::RemoveActorHaveActorsWithHaveBox(AActor* actor)
{
    if (HaveActorsWithHaveBox.Contains(actor)) {
        HaveActorsWithHaveBox.Remove(actor);
    }    
}

bool UMyInventory::IsHaveHaveActorsWithHaveBox(AActor* actor)
{
    return (HaveActorsWithHaveBox.Contains(actor));
}


FItemStaticData* UMyInventory::GetItemStaticData(int32 ItemID)
{
    FItemStaticData* StaticData = ItemTable->FindRow<FItemStaticData>(
        FName(*FString::FromInt(ItemID)),
        TEXT("Item Lookup")
    );

    return StaticData;
}


void UMyInventory::RemoveItemWidget(int32 itemindex)
{
    FInventorySlot* FoundSlot = Slots.Find(itemindex);

    if (!FoundSlot) return;

    if (FoundSlot->Widget)
    {
        FoundSlot->Widget->RemoveFromParent();
    }

    AActor* TargetActor = FoundSlot->Actor;

    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    if (TargetActor && Player)
    {
        FVector PlayerLocation = Player->GetActorLocation();
        FVector Forward = Player->GetActorForwardVector();

        FVector NewLocation = PlayerLocation + Forward * 200.f;

        TargetActor->SetActorLocation(NewLocation);
        TargetActor->SetActorRotation(Player->GetActorRotation());
    }

    Slots.Remove(itemindex);
}

void UMyInventory::RemoveItemWidgetNoItemSpawn(int32 itemindex)
{
    FInventorySlot* FoundSlot = Slots.Find(itemindex);

    if (!FoundSlot) return;

    if (FoundSlot->Widget)
    {
        FoundSlot->Widget->RemoveFromParent();
    }

    Slots.Remove(itemindex);
}

int32 UMyInventory::GetItemIndexFromHaveItem(AActor* TargetActor)
{
    /*
    if (!TargetActor) return INDEX_NONE;

    // HaveActor 배열에서 찾기
    int32 Index = HaveActors.Find(TargetActor);

    return Index; // 없으면 INDEX_NONE(-1) 반환
    */

    if (!TargetActor) return INDEX_NONE;

    for (const TPair<int32, AActor*>& Pair : HaveActors)
    {
        if (Pair.Value == TargetActor)
        {
            return Pair.Key;
        }
    }

    return INDEX_NONE;

}

void UMyInventory::SetMyOwner(AMyCharacter* character)
{
    Owner = character;
}

void UMyInventory::OnOffPZarts()
{
    
    if (partsOn) {
        TrashWidget->SetVisibility(ESlateVisibility::Hidden);
        HaveWidget->SetVisibility(ESlateVisibility::Hidden);
        WeaponBoxWidget->SetVisibility(ESlateVisibility::Hidden);
        WeaponBoxPlayerWidget->SetVisibility(ESlateVisibility::Hidden);
        //GunCompoWidget->SetVisibility(ESlateVisibility::Hidden);
        
    }
    else {
        TrashWidget->SetVisibility(ESlateVisibility::Visible);
        //HaveWidget->SetVisibility(ESlateVisibility::Visible);
        HaveWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        WeaponBoxWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        WeaponBoxPlayerWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        //GunCompoWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        
    }
    partsOn = !partsOn;

}

void UMyInventory::WhosGunCompoToTrash(int32 itemidCard)
{
    if (WeaponBoxPlayerWidget->PlayerWeaponData.itemid == itemidCard) {
        // Actor 먼저 제거 (있다면)
        if (WeaponBoxPlayerWidget->PlayerWeaponData.WeaponActor)
        {
            //WeaponBoxPlayerWidget->WeaponData.WeaponActor->Destroy();
            WeaponBoxPlayerWidget->PlayerWeaponData.itemid = -119;
            WeaponBoxPlayerWidget->PlayerWeaponData.WeaponActor = nullptr;
        }        
    }


    if (WeaponBoxWidget->WeaponData.itemid == itemidCard) {
        // Actor 먼저 제거 (있다면)
        if (WeaponBoxWidget->WeaponData.WeaponActor)
        {
            //WeaponBoxPlayerWidget->WeaponData.WeaponActor->Destroy();
            WeaponBoxWidget->WeaponData.itemid = -119;
            WeaponBoxWidget->WeaponData.WeaponActor = nullptr;
        }
    }
}

void UMyInventory::AttachGunCompo(int32 itemidCard, AActor* weaponCompo)
{
    /*
    if (WeaponBoxPlayerWidget->PlayerWeaponData.itemid == itemidCard) {
        // Actor 먼저 제거 (있다면)
        if (WeaponBoxPlayerWidget->PlayerWeaponData.WeaponActor)
        {
            USkeletalMeshComponent* Mesh =
                WeaponBoxPlayerWidget->PlayerWeaponData.WeaponActor->FindComponentByClass<USkeletalMeshComponent>();

            if (weaponCompo)
            {
                weaponCompo->AttachToComponent(
                    Mesh,
                    FAttachmentTransformRules::SnapToTargetIncludingScale,
                    TEXT("GunCompo")
                );
            }
        }
    }    */
    UE_LOG(LogTemp, Warning, TEXT("AttachGunCompo 처음"));
    

    if (!WeaponBoxPlayerWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("WeaponBoxPlayerWidget is NULL"));
        return;
    }

    

    //AActor* WeaponActor = WeaponBoxPlayerWidget->PlayerWeaponData.WeaponActor;
    AWeaponActor* WeaponActor = Cast<AWeaponActor>(WeaponBoxPlayerWidget->PlayerWeaponData.WeaponActor);
    if (!WeaponActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("WeaponActor is NULL"));
        return;
    }

    AWeaponCompo* WeaponCompo = Cast<AWeaponCompo>(weaponCompo);
    if (!WeaponCompo)
    {
        UE_LOG(LogTemp, Warning, TEXT("WeaponCompo is NULL"));
        return;
    }
    WeaponActor->SetMyGunCompo(WeaponCompo);  

    if (!weaponCompo)
    {
        UE_LOG(LogTemp, Warning, TEXT("WeaponCompo is NULL"));
        return;
    }

    USkeletalMeshComponent* Mesh =
        WeaponActor->FindComponentByClass<USkeletalMeshComponent>();

    if (!Mesh)
    {
        UE_LOG(LogTemp, Error, TEXT("SkeletalMeshComponent NOT found in WeaponActor"));
        return;
    }

    if (!Mesh->DoesSocketExist(TEXT("GunCompo")))
    {
        UE_LOG(LogTemp, Error, TEXT("Socket 'GunCompo' does NOT exist"));
        return;
    }

    // Attach 시도
    weaponCompo->AttachToComponent(
        Mesh,
        FAttachmentTransformRules::SnapToTargetIncludingScale,
        TEXT("GunCompo")
    );

    // ✅ Attach 성공 여부 확인
    if (weaponCompo->GetAttachParentActor() == WeaponActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Attach SUCCESS to GunCompo socket"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Attach FAILED"));
    }



    UE_LOG(LogTemp, Warning, TEXT("AttachGunCompo 끝"));
}
/*
void UMyInventory::AddWeaponCompoBox(int32 itemId, AActor* actor, int32 ItemSpanwID)
{
    //HaveWidget->ItemHave(itemId,itemid);
    UMyItem* slot = nullptr;

    slot = CreateWidget<UMyItem>(GetWorld(), ItemSlotClass);
    if (slot != nullptr) {
        slot->Init(itemid, itemId, actor, ItemSpanwID); // 슬롯번호, 아이템ID ??
        slot->SetOwnerInventory(this);
        ItemIDs.Add(itemId);//진짜 아이템 아이디

        HaveActors.Add(itemid, actor);
        HaveActorsWithHaveBox.Add(actor, 0);//0은 쓰레기값
    }




    // ScrollBox에 바로 추가
    //UPanelSlot* PanelSlot = FloorItemBox->AddChild(slot);
    UPanelSlot* PanelSlot = GunCompoWidget->GetScrollBox()->AddChild(slot);
    if (UScrollBoxSlot* ScrollSlot = Cast<UScrollBoxSlot>(PanelSlot))
    {
        ScrollSlot->SetPadding(FMargin(0.f, 6.f));
        ScrollSlot->SetHorizontalAlignment(HAlign_Fill);
        //ScrollSlot->SetHorizontalAlignment(HAlign_Center);
    }

    ///ItemWidgets.Add(slot);
    ItemWidgets.Add(itemid, slot);

    slot->Refresh();

    itemid++;
}606*/

void UMyInventory::ActiveSecondWeaponToPlayer(AActor* actor)
{
    if (AWeaponActor* setItem = Cast<AWeaponActor>(actor)) {

        FItemStaticData* StaticData = ItemTable->FindRow<FItemStaticData>(
            FName(*FString::FromInt(setItem->ItemRowName)),
            TEXT("Item Lookup")
        );

        if (!StaticData) return;

        FTransform SocketTransform;

        if (Owner && Owner->GetMesh())
        {
            SocketTransform =
                Owner->GetMesh()->GetSocketTransform(
                    TEXT("RightHandPinky4Socket"),
                    RTS_World
                );
        }
        /* 5-22 왜 스폰하고 있었지..? 부착하기만하면 되는데
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = Owner;
        //SpawnParams.Instigator = Owner->GetInstigator();
        SpawnParams.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        GetWorld()->SpawnActor<AWeaponActor>(StaticData->ItemActorClass, SocketTransform, SpawnParams);
        */
        //Owner->SubItemClass = StaticData->RealActiveActorClass;5-23
        FSubItemData subData = { StaticData->RealActiveActorClass, StaticData->ItemID };
        Owner->SetSubItemData(subData);
    }
}

int32 UMyInventory::GetItemIndexFromActor(AActor* TargetActor)
{
    for (auto& Pair : Slots)
    {
        if (Pair.Value.Actor == TargetActor)
        {
            return Pair.Key;
        }
    }

    return INDEX_NONE;
}

AActor* UMyInventory::GetActorFromSlot(int32 SlotIndex)
{
    FInventorySlot* FoundSlot = Slots.Find(SlotIndex);

    if (!FoundSlot) return nullptr;

    return FoundSlot->Actor;
}

void UMyInventory::ClearSubWeaponIfMatches(int32 itemSpawnID)
{
    if (!WeaponBoxWidget) return;

    UE_LOG(LogTemp, Warning, TEXT("ClearSubWeaponIfMatches: comparing Dropped.SpawnID=%d vs WeaponData.SpawnID=%d"),
        itemSpawnID, WeaponBoxWidget->WeaponData.ItemSpawnID);

    if (WeaponBoxWidget->WeaponData.ItemSpawnID == itemSpawnID)
    {
        WeaponBoxWidget->WeaponData.itemid = -119;
        WeaponBoxWidget->WeaponData.WeaponActor = nullptr;
        WeaponBoxWidget->WeaponData.ItemSpawnID = -1;

        if (Owner)
        {
            Owner->ClearSubWeapon();
        }

        UE_LOG(LogTemp, Warning, TEXT("ClearSubWeaponIfMatches: MATCHED, cleared"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ClearSubWeaponIfMatches: NOT matched"));
    }
}

UMyItem* UMyInventory::CreateAndRegisterSlotWidget(
    int32 SlotIndex,
    int32 ItemId,
    AActor* Actor,
    int32 ItemSpawnID,
    bool bRegisterSlot)
{
    if (!ItemSlotClass)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateAndRegisterSlotWidget: ItemSlotClass NULL"));
        return nullptr;
    }

    UMyItem* SlotWidget = CreateWidget<UMyItem>(GetWorld(), ItemSlotClass);
    if (!SlotWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateAndRegisterSlotWidget: SlotWidget creation failed"));
        return nullptr;
    }

    SlotWidget->Init(SlotIndex, ItemId, Actor, ItemSpawnID);
    SlotWidget->SetOwnerInventory(this);

    if (bRegisterSlot)
    {
        FInventorySlot NewSlot;
        NewSlot.ItemID = ItemId;
        NewSlot.Actor = Actor;
        NewSlot.Widget = SlotWidget;
        NewSlot.ItemSpanwID = ItemSpawnID;

        Slots.Add(SlotIndex, NewSlot);
    }

    return SlotWidget;
}

void UMyInventory::AttachWidgetToScrollBox(UScrollBox* Scroll, UMyItem* SlotWidget)
{
    if (!Scroll || !SlotWidget)
        return;

    UPanelSlot* PanelSlot = Scroll->AddChild(SlotWidget);

    if (UScrollBoxSlot* ScrollSlot = Cast<UScrollBoxSlot>(PanelSlot))
    {
        ScrollSlot->SetPadding(FMargin(0.f, 6.f));
        ScrollSlot->SetHorizontalAlignment(HAlign_Fill);
    }

    SlotWidget->Refresh();
}

void UMyInventory::AttachWidgetToSizeBox(USizeBox* Box, UMyItem* SlotWidget)
{
    if (!Box || !SlotWidget)
        return;

    Box->ClearChildren();
    Box->SetContent(SlotWidget);

    SlotWidget->Refresh();
}
