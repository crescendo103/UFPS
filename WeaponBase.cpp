// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("K2C1"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));


	//총구방향 설정
	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("muzzlepoint"));
	
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector AWeaponBase::GetGunStartLocation()
{
	return WeaponMesh->GetSocketLocation(TEXT("muzzleSocket"));
}

FVector AWeaponBase::GetGunFoward()
{
	return WeaponMesh->GetSocketRotation(TEXT("muzzleSocket")).Vector();
}

