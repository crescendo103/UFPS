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
    if (!OverlayMaterial)
    {
        UE_LOG(LogTemp, Warning, TEXT("OverlayMaterial is null"));
        return;
    }
    if (!WeaponMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("weaponMesh is null"));
        return;
    }

    // 1️⃣ Create Dynamic Material Instance
    DynamicOverlay = UMaterialInstanceDynamic::Create(
        OverlayMaterial,
        this
    );

    if (!DynamicOverlay)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create DynamicOverlay"));
        return;
    }

    // 2️⃣ Set Overlay Material (Mesh)
    WeaponMesh->SetOverlayMaterial(DynamicOverlay);

    // 3️⃣ Set Scalar Parameter Value (Opacity = 0)
    DynamicOverlay->SetScalarParameterValue(
        TEXT("Opacity"),
        0.0f
    );
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



void AWeaponBase::ItemOutlineOn_Implementation()
{
    UE_LOG(LogTemp, Warning,
        TEXT("ItemOutlineOn_Implementation"));
    // 3️⃣ Set Scalar Parameter Value (Opacity = 0)
    if (DynamicOverlay) {
        DynamicOverlay->SetScalarParameterValue(
            TEXT("Opacity"),
            1.0f
        );

        UE_LOG(LogTemp, Warning,
            TEXT("Opacity 1.0"));
    }
    
}

void AWeaponBase::ItemOutlineOff_Implementation()
{
    UE_LOG(LogTemp, Warning,
        TEXT("ItemOutlineOff_Implementation"));
    if (DynamicOverlay) {
        // 3️⃣ Set Scalar Parameter Value (Opacity = 0)
        DynamicOverlay->SetScalarParameterValue(
            TEXT("Opacity"),
            0.0f
        );

        UE_LOG(LogTemp, Warning,
            TEXT("Opacity 0.0"));
    }
    
}





