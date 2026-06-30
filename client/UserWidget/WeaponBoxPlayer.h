// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "WeaponBox.h"
#include "WeaponBoxPlayer.generated.h"

/**
 * 플레이어가 직접 장착하는 무기 슬롯.
 * UWeaponBox(보관용 무기 박스)를 상속하지만 장착 로직(WeaponAttach, setCurrentWeapon)이
 * 추가로 들어가므로 OnItemDropped만 override한다.
 */
class AMyCharacter;
class UMyDragDropOperation;

UCLASS()
class FPS_API UWeaponBoxPlayer : public UWeaponBox
{
    GENERATED_BODY()

public:
    UPROPERTY()
    AMyCharacter* Owner;

    void SetOwner(AMyCharacter* actor);

    UPROPERTY()
    FWeaponData PlayerWeaponData;

protected:
    virtual bool OnItemDropped(UMyDragDropOperation* DragOp) override;
};