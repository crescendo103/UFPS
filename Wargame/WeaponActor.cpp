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
    // 1. 머티리얼 에셋 로드 (정확한 경로 확인 필수)
    static const TCHAR* OutlineMaterialPath = TEXT("/Game/Mycontent/Material/MT_ItemHighlight.MT_ItemHighlight");
    UMaterialInterface* OutlineMaterial = LoadObject<UMaterialInterface>(nullptr, OutlineMaterialPath);

    if (!OutlineMaterial)
    {
        UE_LOG(LogTemp, Warning, TEXT("[WeaponActor] ItemOutlineOn: 머티리얼 로드 실패! 경로를 확인하세요: %s"), OutlineMaterialPath);
        return;
    }

    // 2. 이 액터가 가진 모든 메시 컴포넌트(SkeletalMesh, StaticMesh 등)를 가져옴
    TArray<UMeshComponent*> MeshComponents;
    GetComponents<UMeshComponent>(MeshComponents);

    if (MeshComponents.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[WeaponActor] ItemOutlineOn: %s 에서 메시 컴포넌트를 찾을 수 없습니다."), *GetName());
        return;
    }

    // 성공 로그 (Warning으로 출력하여 확실히 보이게 설정)
    UE_LOG(LogTemp, Warning, TEXT("[WeaponActor] ItemOutlineOn 호출됨! 대상: %s (메시 개수: %d)"), *GetName(), MeshComponents.Num());

    // 3. 모든 메시 컴포넌트에 오버레이 머티리얼 적용
    for (UMeshComponent* MeshComp : MeshComponents)
    {
        if (MeshComp)
        {
            MeshComp->SetOverlayMaterial(OutlineMaterial);
        }
    }
}

void AWeaponActor::ItemOutlineOff_Implementation()
{
    TArray<UMeshComponent*> MeshComponents;
    GetComponents<UMeshComponent>(MeshComponents);

    UE_LOG(LogTemp, Warning, TEXT("[WeaponActor] ItemOutlineOff 호출됨! 대상: %s"), *GetName());

    // 오버레이 머티리얼 제거 (nullptr 대입)
    for (UMeshComponent* MeshComp : MeshComponents)
    {
        if (MeshComp)
        {
            MeshComp->SetOverlayMaterial(nullptr);
        }
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
        FVector startedPos = Mesh->GetSocketLocation(TEXT("SpawnPoint"));//총구에서 안쏠듯  
        SpawnComponent->UseWeapon(Owner, startPos, startRot);
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