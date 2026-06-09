// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"
#include "Components/SphereComponent.h"
#include "MyCharacter.h"
//#include "../../../../../../../Program Files/Epic Games/UE_5.4/Engine/Plugins/Experimental/Mutable/Source/MutableRuntime/Public/MuR/System.h"
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
    TriggerSphere->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel4);
    TriggerSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    TriggerSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 플레이어만 감지

    TriggerSphere->SetupAttachment(RootComponent);
    

    bPickedUp = false;
    EffectComponent = CreateDefaultSubobject<UItemEffectComponent>(TEXT("NIEF"));
    EffectComponent->SetupAttachment(Mesh, TEXT("EffectSocket"));

    SpawnComponent = CreateDefaultSubobject<UWeaponUseComponent>(TEXT("SPItem"));
    SpawnComponent->SetupAttachment(Mesh, TEXT("SpawnPoint"));
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

void AWeaponActor::SpawnItem(FVector startPos, FVector startRot)
{
    if (SpawnComponent)
    {
        FVector startedPos = Mesh->GetSocketLocation(TEXT("SpawnPoint"));
        //FVector startRot = Mesh->GetSocketRotation(TEXT("SpawnPoint")).Vector();
        SpawnComponent->UseWeapon(Owner, startedPos, startRot);
        SpawnComponent->SendBulletPacket(startPos, startRot);//서버 전송
    }

    if (EffectComponent)
    {
        EffectComponent->PlayStartSound();
        EffectComponent->PlayStartEffect();
    }
}

void AWeaponActor::SetMyOwner(AMyCharacter* myowner)
{
    Owner = myowner;
}

void AWeaponActor::sendItemPacket()
{
    FItemPacket packet;
    FVector ItemPos = GetActorLocation();
    packet.Header.Type = (int32)EPacketType::Item;
    packet.Header.Size = sizeof(FItemPacket);
    packet.X = ItemPos.X;
    packet.Y = ItemPos.Y;
    packet.Z = ItemPos.Z;
    packet.OwnerId = -1;//임시
    packet.ItemRow = ItemRowName;
    packet.ItemSpawnID = ItemSpawnID;
    packet.ShouldRemove = false;

    UMyServer* MyServer = GetGameInstance()->GetSubsystem<UMyServer>();
    MyServer->MoveItem(packet);
}

int32 AWeaponActor::GetItemSpawnID()
{
    return ItemSpawnID;
}

FVector AWeaponActor::GetGunStartLocation()
{
    return Mesh->GetSocketLocation(TEXT("muzzleSocket"));
}