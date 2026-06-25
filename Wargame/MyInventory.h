// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/CanvasPanel.h>
#include <Components/VerticalBox.h>
#include <Components/ScrollBox.h>
#include "TypeDefine.h"
#include "Components/SizeBox.h"
#include "TrashBorder.h"
//#include "RemoveItemInterface.h"
//#include "HaveItemBox.h"

#include "MyInventory.generated.h"





/**
 * 
 */
class UMyItem; // 전방 선언
class UHaveItemBox;
class UWeaponBox;
class AMyCharacter;
class UWeaponBoxPlayer;
class UInventoryGunCompoBox;
UCLASS()
class FPS_API UMyInventory : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

public:
    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* RootCanvas;

    UPROPERTY(meta = (BindWidget))
    UScrollBox* FloorItemBox;

   
    // 🔹 레이아웃 컨테이너들
    UPROPERTY()
    TArray<USizeBox*> ItemBoxes;
    
    // 🔹 슬롯 위젯들
    /*
    UPROPERTY()
    TArray<UMyItem*> ItemWidgets;
    */
    UPROPERTY()
    TMap<int32, UMyItem*> ItemWidgets;
    /*
    UPROPERTY()
    TArray<AActor*> HaveActor;
    */
    UPROPERTY()
    TMap<int32, AActor*> HaveActors;
    UPROPERTY()
    TMap<AActor*, int32> HaveActorsWithHaveBox;

    // 🔹 데이터
    UPROPERTY()
    TArray<int32> ItemIDs;



    UPROPERTY()
    TMap<int32, FInventorySlot> Slots;

    UPROPERTY()
    TMap<AActor*, UMyItem*> FloorActorWidgets;

    int32 NextSlotIndex = 0;

    void RemoveFloorItem(AActor* Actor);
    /*
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class UMyItem> ItemSlotClass;
    */
    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TSubclassOf<UMyItem> ItemSlotClass;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TSubclassOf<UTrashBorder> TrashWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    UTrashBorder* TrashWidget;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TSubclassOf<UHaveItemBox> HaveWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    UHaveItemBox* HaveWidget;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TSubclassOf<UWeaponBox> WeaponBoxWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    UWeaponBox* WeaponBoxWidget;


    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    UWeaponBoxPlayer* WeaponBoxPlayerWidget;
    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TSubclassOf<UWeaponBoxPlayer> WeaponBoxPlayerWidgetClass;

    /*
    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    UInventoryGunCompoBox* GunCompoWidget;
    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TSubclassOf<UInventoryGunCompoBox> GunCompoWidgetClass;
    */

    UPROPERTY()
    AMyCharacter* Owner;

    
    // 아이템 정적 데이터
    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    UDataTable* ItemTable;

    int32 itemid;//바닥전용 아이템

    
    void SwapItems(int32 From, int32 To);
    void RefreshSlot(int32 SlotIdx);
    void AddHaveItemBox(int32 itemId, AActor* actor, int32 itemSpawnID);
    void AddFloorItemBox(int32 itemId, AActor* actor, int32 ItemSpanwID);
    void AddWeaponBox(int32 itemId, AActor* actor, int32 ItemSpanwID);
    void AddWeaponBoxPlayer(int32 itemId, AActor* actor, int32 ItemSpanwID);
    void RemoveActorHaveActorsWithHaveBox(AActor* actor);
    bool IsHaveHaveActorsWithHaveBox(AActor* actor);

    FItemStaticData* GetItemStaticData(int32 ItemID);

    //virtual void RemoveItem_Implementation(int32 SlotIndex) override;
    void RemoveItemWidget(int32 itemindex);
    void RemoveItemWidgetNoItemSpawn(int32 itemindex);
    int32 GetItemIndexFromHaveItem(AActor* TargetActor);
    
    void SetMyOwner(AMyCharacter* character);
    void OnOffPZarts();
    bool partsOn;
    

    void WhosGunCompoToTrash(int32 itemidCard);
    void AttachGunCompo(int32 itemidCard, AActor* weaponCompo);
    

    void ActiveSecondWeaponToPlayer(AActor* actor);
    int32 GetItemIndexFromActor(AActor* TargetActor);
    AActor* GetActorFromSlot(int32 SlotIndex);

    void ClearSubWeaponIfMatches(int32 itemSpawnID);

    // ===== 새로 추가: 공통 헬퍼 =====
protected:
    // 위젯 생성 + Init + Slots 등록까지 공통 처리. 실패 시 nullptr.
    // bRegisterSlot=false면 Slots 맵에 등록하지 않음(바닥 아이템처럼 슬롯 개념이 없는 경우)
    UMyItem* CreateAndRegisterSlotWidget(
        int32 SlotIndex,
        int32 ItemId,
        AActor* Actor,
        int32 ItemSpawnID,
        bool bRegisterSlot = true
    );

    // SizeBox 계열(WeaponBox, WeaponBoxPlayer)에 단일 위젯을 끼워 넣는 공통 처리
    void AttachWidgetToSizeBox(USizeBox* Box, UMyItem* SlotWidget);

    // ScrollBox 계열(HaveItemBox, FloorItemBox)에 위젯을 추가하는 공통 처리
    void AttachWidgetToScrollBox(UScrollBox* Scroll, UMyItem* SlotWidget);
};
