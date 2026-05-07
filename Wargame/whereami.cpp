#include "whereami.h"
#include "ItemManagerSubsystem.h"

AWhereami::AWhereami()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AWhereami::BeginPlay()
{
    auto* Manager = GetGameInstance()->GetSubsystem<UItemManagerSubsystem>();
    Manager->InitItems(GetWorld());
}
