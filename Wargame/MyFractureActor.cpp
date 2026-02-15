#include "MyFractureActor.h"
#include "GameFramework/DamageType.h"  // FDamageEvent 포함
#include "Kismet/GameplayStatics.h"    // TakeDamage 사용 시
#include "Engine/DamageEvents.h"  // FDamageEvent 정의

AMyFractureActor::AMyFractureActor()
{
    PrimaryActorTick.bCanEverTick = false;

    GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
    RootComponent = GeometryCollection;

    FieldSystem = CreateDefaultSubobject<UFieldSystemComponent>(TEXT("FieldSystem"));
    FieldSystem->SetupAttachment(RootComponent);

    GeometryCollection->SetSimulatePhysics(true);
    GeometryCollection->SetEnableGravity(true);
    GeometryCollection->SetNotifyBreaks(true);
}

void AMyFractureActor::BeginPlay()
{
    Super::BeginPlay();
    //ApplyFracture(GetActorLocation());
}


void AMyFractureActor::ApplyFracture(FVector HitLocation)
{
   
    if (!GeometryCollection) return;

    // Apply Impulse on Damage 옵션 사용 시, TakeDamage로 파괴 가능
    const float DamageAmount = 1000.f; // 임의 데미지
    FDamageEvent DamageEvent;

    TakeDamage(DamageAmount, DamageEvent, GetInstigatorController(), this);

    // Optional: 위치 기반 추가 물리력
    GeometryCollection->AddRadialImpulse(
        HitLocation,     // 중심 위치
        300.f,           // 반경
        1000.f,          // 힘
        ERadialImpulseFalloff::RIF_Linear,
        true             // VelChange = true → 물리량 직접 적용
    );

}

void AMyFractureActor::DelayApplyFracture(FVector HitLocation)
{

    FTimerDelegate TimerDel;
    TimerDel.BindUFunction(this, FName("ApplyFracture"), HitLocation);

    GetWorldTimerManager().SetTimer(
        ExplosionTimerHandle,
        TimerDel,
        3.0f,
        false
    );
}
