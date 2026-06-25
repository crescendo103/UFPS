// WeaponBox.h
#pragma once
#include "CoreMinimal.h"
#include "InventoryDropTargetBase.h"
#include "Components/SizeBox.h"
#include "WeaponBox.generated.h"

USTRUCT(BlueprintType)
struct FWeaponData
{
    GENERATED_BODY()
    UPROPERTY()
    int32 itemid;
    UPROPERTY()
    AActor* WeaponActor;
    UPROPERTY()
    int32 ItemSpawnID = -1; // 필드에 놓인 아이템의 고유 SpawnID
};

class UMyItem;

UCLASS()
class FPS_API UWeaponBox : public UInventoryDropTargetBase
{
    GENERATED_BODY()

public:
    USizeBox* GetSizeBox();

    UPROPERTY(meta = (BindWidget))
    USizeBox* WeaponBox;

    UPROPERTY()
    FWeaponData WeaponData;

protected:
    virtual bool OnItemDropped(UMyDragDropOperation* DragOp) override;
};