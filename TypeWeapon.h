#pragma once

#include "CoreMinimal.h"
#include "TypeWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Rifle   UMETA(DisplayName = "Rifle"),
    Shotgun UMETA(DisplayName = "Shotgun"),
    Grenade UMETA(DisplayName = "Grenade")
};


