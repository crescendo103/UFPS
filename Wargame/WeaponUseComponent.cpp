// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponUseComponent.h"

// Sets default values for this component's properties
UWeaponUseComponent::UWeaponUseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UWeaponUseComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWeaponUseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponUseComponent::UseWeapon(AMyCharacter* Owner, FVector StartPos, FVector Dir)
{
	UE_LOG(LogTemp, Warning,
		TEXT("UseWeapon parent"));
}

void UWeaponUseComponent::SendBulletPacket(FVector startPos, FVector dir)
{
	UE_LOG(LogTemp, Warning,
		TEXT("UseWeapon parent"));
}
