// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"
#include "Components/SphereComponent.h"

// Sets default values
AWeaponActor::AWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;
    /*
    // ✅ 2. Mobility
    Mesh->SetMobility(EComponentMobility::Movable);

    // 🔹 물리 설정
    Mesh->SetSimulatePhysics(true);
    Mesh->SetEnableGravity(true);
    Mesh->SetNotifyRigidBodyCollision(true);
    */

    TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
    // 콜리전 설정
    TriggerSphere->InitSphereRadius(50.f); // 구체 반지름
    TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    TriggerSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    TriggerSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 플레이어만 감지

    TriggerSphere->SetupAttachment(RootComponent);
    

    bPickedUp = false;
}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
    if (!OverlayMaterial)
    {
        UE_LOG(LogTemp, Warning, TEXT("OverlayMaterial is null"));
        return;
    }
    if (!Mesh)
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
    Mesh->SetOverlayMaterial(DynamicOverlay);

    // 3️⃣ Set Scalar Parameter Value (Opacity = 0)
    DynamicOverlay->SetScalarParameterValue(
        TEXT("Opacity"),
        0.0f
    );
}

// Called every frame
void AWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if (TriggerSphere)
    {
        DrawDebugSphere(
            GetWorld(),
            TriggerSphere->GetComponentLocation(),
            TriggerSphere->GetScaledSphereRadius(),
            16,
            FColor::Red,
            false,   // false = 매 프레임 갱신
            -1.f,
            0,
            1.5f
        );
    }
}

void AWeaponActor::ItemOutlineOn_Implementation()
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

void AWeaponActor::ItemOutlineOff_Implementation()
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

void AWeaponActor::SpawnItem_Implementation(FVector playerPos)
{
}

int32 AWeaponActor::GetItemName()
{
    return ItemRowName;
}

void AWeaponActor::SetMyGunCompo(AWeaponCompo* compo)
{
    myGunCompo = compo;
}

AWeaponCompo* AWeaponActor::GetMyGunCompo()
{
    return myGunCompo;
}

