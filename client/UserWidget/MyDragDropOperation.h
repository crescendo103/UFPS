// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "MyDragDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API UMyDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
public:
    UPROPERTY()
	int32 ItemID;// ItemDataTable의 ID, 같은 종류 아이템이면 같음

    UPROPERTY()
	int32 FromSlotIndex;// inventory 에서 아이템 슬롯 번호
    
    UPROPERTY()
    AActor* ItemActor=nullptr;// weaponActor
    UPROPERTY()
	int32 ItemSpawnID;// 월드의 고유 ID, 같은 종류 아이템이여도 다름
};
