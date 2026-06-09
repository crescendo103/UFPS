// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemManagerSubsystem.h"

void UItemManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWeaponActor::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        AWeaponActor* Item = Cast<AWeaponActor>(Actor);
        if (Item)
        {
            ItemMap.Add(Item->ItemSpawnID, Item);
        }
    }
}

AWeaponActor* UItemManagerSubsystem::GetItem(int32 SpawnId)
{
    {
        if (AWeaponActor** Found = ItemMap.Find(SpawnId))
        {
            return *Found;
        }
        return nullptr;
    }
}

void UItemManagerSubsystem::SetItem(int32 SpawnId, AWeaponActor* Item)
{
    ItemMap.Add(SpawnId, Item);
}

void UItemManagerSubsystem::InitItems(UWorld* World)
{
    if (!ItemMap.IsEmpty())
    {
        return;
    }

    if (!IsValid(World))
    {
        return;
    }

    //ItemMap.Empty();

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(World, AWeaponActor::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        AWeaponActor* Item = Cast<AWeaponActor>(Actor);
        if (Item)
        {
            ItemMap.Add(Item->ItemSpawnID, Item);
        }
    }
}
