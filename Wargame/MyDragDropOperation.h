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
    int32 ItemID;

    UPROPERTY()
    int32 FromSlotIndex;
    
    UPROPERTY()
    AActor* ItemActor=nullptr;
    UPROPERTY()
    int32 ItemSpawnID;
};
