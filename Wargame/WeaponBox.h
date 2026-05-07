// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SizeBox.h"
#include "Components/ScrollBox.h"
#include "WeaponBox.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FWeaponData
{
    GENERATED_BODY()

    UPROPERTY()
    int32 itemid;

    UPROPERTY()
    AActor* WeaponActor;
};

class UMyInventory;
class UMyItem;

UCLASS()
class FPS_API UWeaponBox : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual bool NativeOnDragOver(
        const FGeometry& InGeometry,
        const FDragDropEvent& InDragDropEvent,
        UDragDropOperation* InOperation) override;

    virtual bool NativeOnDrop(
        const FGeometry& InGeometry,
        const FDragDropEvent& InDragDropEvent,
        UDragDropOperation* InOperation) override;    


    void SetOwnerInventory(UMyInventory* InInventory);
    USizeBox* GetSizeBox();
    

    UPROPERTY(meta = (BindWidget))
    USizeBox* WeaponBox;

    UPROPERTY()
    UMyInventory* OwnerInventory;
    
    UPROPERTY()
    FWeaponData WeaponData;
};
