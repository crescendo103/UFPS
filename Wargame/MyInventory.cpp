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

    GunCompoWidget = CreateWidget<UInventoryGunCompoBox>(GetWorld(), GunCompoWidgetClass);
    GunCompoWidget->SetOwnerInventory(this);
    //GunCompoWidget->SetOwner(Owner);
    GunCompoWidget->AddToViewport(); // ZOrder 높게
    GunCompoWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UMyInventory::SwapItems(int32 From, int32 To)
{
    if (!ItemIDs.IsValidIndex(From) ||
        !ItemIDs.IsValidIndex(To) ||
        From == To)
        return;

    // 데이터 스왑
    Swap(ItemIDs[From], ItemIDs[To]);

   
    // 2️⃣ 위젯 스왑
    UMyItem* FromWidget = nullptr;
    UMyItem* ToWidget = nullptr;

    ItemWidgets.RemoveAndCopyValue(From, FromWidget);
    ItemWidgets.RemoveAndCopyValue(To, ToWidget);

    if (FromWidget)
        ItemWidgets.Add(To, FromWidget);

    if (ToWidget)
        ItemWidgets.Add(From, ToWidget);

    // 3️⃣ 위젯 내부 데이터 갱신
    if (FromWidget)
        FromWidget->Init(To, ItemIDs[To]);

    if (ToWidget)
        ToWidget->Init(From, ItemIDs[From]);

    // ✅ UI 갱신
    RefreshSlot(From);
    RefreshSlot(To);

    UE_LOG(LogTemp, Log,
        TEXT("슬롯 스왑 완료: %d <-> %d"),
        From, To);
}

void UMyInventory::RefreshSlot(int32 SlotIdx)
{
    /*
    if (!ItemWidgets.Find(SlotIdx) ||
        !ItemIDs.IsValidIndex(SlotIdx))
        return;

    ItemWidgets[SlotIdx]->Init(
        SlotIdx,
        ItemIDs[SlotIdx]
    );
    */

    if (!ItemIDs.IsValidIndex(SlotIdx))
        return;

    UMyItem** WidgetPtr = ItemWidgets.Find(SlotIdx);
    if (!WidgetPtr || !(*WidgetPtr))
        return;

    const FItemStaticData* StaticData = ItemTable->FindRow<FItemStaticData>(
        FName(*FString::FromInt(ItemIDs[SlotIdx])),
        TEXT("Item Lookup")
    );
        //ItemTable->FindRow<FItemStaticData>(ItemIDs[SlotIdx]);

    ItemWidgets[SlotIdx]->SetItemIcon(StaticData->Icon);
    
}

void UMyInventory::AddHaveItemBox(int32 itemId, AActor* actor)
{

    //HaveWidget->ItemHave(itemId,itemid);
    UMyItem* slot = nullptr;

    slot = CreateWidget<UMyItem>(GetWorld(), ItemSlotClass);
    if (slot != nullptr) {
        slot->Init(itemid, itemId, actor); // 슬롯번호, 아이템ID ??
        slot->SetOwnerInventory(this);
        ItemIDs.Add(itemId);//진짜 아이템 아이디
        ///HaveActor.Add(actor);
        HaveActors.Add(itemid, actor);
        HaveActorsWithHaveBox.Add(actor,0);//0은 쓰레기값
    }

    
    
    
    // ScrollBox에 바로 추가
    //UPanelSlot* PanelSlot = FloorItemBox->AddChild(slot);
    UPanelSlot* PanelSlot = HaveWidget->GetHaveScrollBox()->AddChild(slot);
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
}

void UMyInventory::AddFloorItemBox(int32 itemId, AActor* actor)
{
    //HaveWidget->ItemHave(itemId,itemid);
    UMyItem* slot = nullptr;

    slot = CreateWidget<UMyItem>(GetWorld(), ItemSlotClass);
    if (slot != nullptr) {
        slot->Init(itemid, itemId, actor); // 슬롯번호, 아이템ID ??
        slot->SetOwnerInventory(this);
        ItemIDs.Add(itemId);//진짜 아이템 아이디
        HaveActors.Add(itemid, actor);
    }




    // ScrollBox에 바로 추가
    UPanelSlot* PanelSlot = FloorItemBox->AddChild(slot);
    //UPanelSlot* PanelSlot = HaveWidget->GetHaveScrollBox()->AddChild(slot);
    if (UScrollBoxSlot* ScrollSlot = Cast<UScrollBoxSlot>(PanelSlot))
    {
        ScrollSlot->SetPadding(FMargin(0.f, 6.f));
        ScrollSlot->SetHorizontalAlignment(HAlign_Fill);
        //ScrollSlot->SetHorizontalAlignment(HAlign_Center);
    }

    ItemWidgets.Add(itemid, slot);

    slot->Refresh();

    itemid++;
}

void UMyInventory::AddWeaponBox(int32 itemId, AActor* actor)
{
    //HaveWidget->ItemHave(itemId,itemid);
    UMyItem* slot = nullptr;

    slot = CreateWidget<UMyItem>(GetWorld(), ItemSlotClass);
    if (slot != nullptr) {
        slot->Init(itemid, itemId, actor); // 슬롯번호, 아이템ID ??
        slot->SetOwnerInventory(this);
        ItemIDs.Add(itemId);//진짜 아이템 아이디
        
        HaveActors.Add(itemid, actor);
        HaveActorsWithHaveBox.Add(actor, 0);//0은 쓰레기값
    }




    // ScrollBox에 바로 추가
    //UPanelSlot* PanelSlot = FloorItemBox->AddChild(slot);
    UPanelSlot* PanelSlot = WeaponBoxWidget->GetScrollBox()->AddChild(slot);
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
}

void UMyInventory::AddWeaponBoxPlayer(int32 itemId, AActor* actor)
{
    //HaveWidget->ItemHave(itemId,itemid);
    UMyItem* slot = nullptr;

    slot = CreateWidget<UMyItem>(GetWorld(), ItemSlotClass);
    if (slot != nullptr) {
        slot->Init(itemid, itemId, actor); // 슬롯번호, 아이템ID ??
        slot->SetOwnerInventory(this);
        ItemIDs.Add(itemId);//진짜 아이템 아이디

        HaveActors.Add(itemid, actor);
        HaveActorsWithHaveBox.Add(actor, 0);//0은 쓰레기값
    }




    // ScrollBox에 바로 추가
    //UPanelSlot* PanelSlot = FloorItemBox->AddChild(slot);
    UPanelSlot* PanelSlot = WeaponBoxPlayerWidget->GetScrollBox()->AddChild(slot);
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
    UE_LOG(LogTemp, Warning,
        TEXT("===== RemoveItemWidget 호출 ===== Index = %d"),
        itemindex);

    UMyItem** resultItem = ItemWidgets.Find(itemindex);
    AActor** reusltActor = HaveActors.Find(itemindex);

    UE_LOG(LogTemp, Warning, TEXT("ItemWidgets Contains Index : %s"),
        resultItem ? TEXT("TRUE") : TEXT("FALSE"));

    UE_LOG(LogTemp, Warning, TEXT("HaveActors Contains Index : %s"),
        reusltActor ? TEXT("TRUE") : TEXT("FALSE"));


    if (!resultItem)
        return;
    if (!reusltActor)
        return;

    //UMyItem* Item = ItemWidgets[itemindex];
    //if (Item)
    //{
        //Item->RemoveFromParent(); // ✅ UI에서 제거
        (*resultItem)->RemoveFromParent();
    //}

    //ItemWidgets.RemoveAt(itemindex); // ✅ 배열에서 제거
    ItemWidgets.Remove(itemindex);

    //AActor* TargetActor = HaveActor[itemindex];
    AActor* TargetActor = *reusltActor;
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    if (TargetActor && Player)
    {
        FVector PlayerLocation = Player->GetActorLocation();
        FRotator PlayerRotation = Player->GetActorRotation();

        // 플레이어가 바라보는 방향
        FVector Forward = Player->GetActorForwardVector();

        // 원하는 거리만큼 앞쪽으로 오프셋
        float Distance = 200.f;
        FVector NewLocation = PlayerLocation + Forward * Distance;

        // 위치와 회전 적용
        TargetActor->SetActorLocation(NewLocation);
        TargetActor->SetActorRotation(PlayerRotation);
    }

    //HaveActor.RemoveAt(itemindex); // ✅ 배열에서 제거
    HaveActors.Remove(itemindex);
}

void UMyInventory::RemoveItemWidgetNoItemSpawn(int32 itemindex)
{
    UE_LOG(LogTemp, Warning,
        TEXT("===== RemoveItemWidgetNoItemSpawn 호출 ===== Index = %d"),
        itemindex);

    UMyItem** resultItem = ItemWidgets.Find(itemindex);
    AActor** reusltActor = HaveActors.Find(itemindex);

    UE_LOG(LogTemp, Warning, TEXT("ItemWidgets Contains Index : %s"),
        resultItem ? TEXT("TRUE") : TEXT("FALSE"));

    UE_LOG(LogTemp, Warning, TEXT("HaveActors Contains Index : %s"),
        reusltActor ? TEXT("TRUE") : TEXT("FALSE"));

    if (!resultItem)
        return;
    if (!reusltActor)
        return;

    //UMyItem* Item = ItemWidgets[itemindex];
    //if (Item)
    //{
        //Item->RemoveFromParent(); // ✅ UI에서 제거
    (*resultItem)->RemoveFromParent();
    //}

    //ItemWidgets.RemoveAt(itemindex); // ✅ 배열에서 제거
    ItemWidgets.Remove(itemindex);

    
    

    //HaveActor.RemoveAt(itemindex); // ✅ 배열에서 제거
    HaveActors.Remove(itemindex);
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
        GunCompoWidget->SetVisibility(ESlateVisibility::Hidden);
        //WeaponBoxWidget->SetVisibility(ESlateVisibility::Visible);
    }
    else {
        TrashWidget->SetVisibility(ESlateVisibility::Visible);
        //HaveWidget->SetVisibility(ESlateVisibility::Visible);
        HaveWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        WeaponBoxWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        WeaponBoxPlayerWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        GunCompoWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        //WeaponBoxWidget->SetVisibility(ESlateVisibility::Visible);
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

void UMyInventory::AddWeaponCompoBox(int32 itemId, AActor* actor)
{
    //HaveWidget->ItemHave(itemId,itemid);
    UMyItem* slot = nullptr;

    slot = CreateWidget<UMyItem>(GetWorld(), ItemSlotClass);
    if (slot != nullptr) {
        slot->Init(itemid, itemId, actor); // 슬롯번호, 아이템ID ??
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
}

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

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = Owner;
        //SpawnParams.Instigator = Owner->GetInstigator();
        SpawnParams.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        GetWorld()->SpawnActor<AWeaponActor>(StaticData->ItemActorClass, SocketTransform, SpawnParams);
        Owner->SubItemClass = StaticData->RealActiveActorClass;
    }
}


