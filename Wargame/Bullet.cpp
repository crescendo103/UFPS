// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "SoundComponent.h"
#include "MyCharacter.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 콜리전 (루트)
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Bull"));
	//CollisionComponent->SetSimulatePhysics(true); 물리 시뮬?
	RootComponent = CollisionComponent;

	// 메쉬
	StaticBulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	StaticBulletMesh->SetupAttachment(RootComponent);
	StaticBulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Projectile
	PojectileCompo = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	PojectileCompo->InitialSpeed = 1000.f;
	PojectileCompo->MaxSpeed = 1000.f;
	PojectileCompo->bRotationFollowsVelocity = false;
	PojectileCompo->bShouldBounce = false;

	PojectileCompo->SetUpdatedComponent(CollisionComponent);
	

	
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnOverlapSphere);

	// Audio
	BulletAudio = CreateDefaultSubobject<USoundComponent>(TEXT("BulletAudio"));
	BulletAudio->SetupAttachment(RootComponent);
	BulletAudio->bAutoActivate = false; // 자동 재생 끔

	
}

void ABullet::SetBulletOwner(int32 owner)
{
	BulletOwner = owner;
}
int32 ABullet::GetBulletOwner()
{
	return BulletOwner;
}
/*
void ABullet::ActiveBulletOverlap()
{
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnOverlapSphere);
}*/

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	/*
	GetWorld()->GetTimerManager().SetTimer(
		OverlapTimerHandle,
		this,
		&ABullet::ActiveBulletOverlap,
		0.000001f,
		false
	);*/

	SetLifeSpan(5.0f);


}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugSphere(
		GetWorld(),
		CollisionComponent->GetComponentLocation(), // 콜리전 중심
		CollisionComponent->GetScaledSphereRadius(), // 콜리전 반경
		12, // 세그먼트 수 (원형 매끄럽게)
		FColor::Red, // 색상
		false, // 지속시간 (false = 1프레임만, true = Persistent)
		-1.f, // 지속시간
		0, // 깊이 우선 표시 옵션
		2.f // 선 두께
	);
}


void ABullet::ShootBullet(const FVector& Forward)
{
	
	PojectileCompo->Velocity = Forward * PojectileCompo->InitialSpeed;

}



void ABullet::SetBulletId(int32 id)
{
	BulletId = id;
}

void ABullet::OnOverlapSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetOwner())
		return;
		

	// 1. Owner와 겹치면 무조건 무시
	/*
	if (OtherActor == GetOwner()) {
		UE_LOG(LogTemp, Warning, TEXT("ignore Owner"));
		return;
	}*/
		



	UClass* Class = OtherActor->GetClass();
	FString ClassName = Class->GetName();

	FString ParentName = Class->GetSuperClass()
		? Class->GetSuperClass()->GetName()
		: TEXT("None");

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Cyan,
		FString::Printf(TEXT("Actor: %s | Class: %s | Parent: %s"),
			*OtherActor->GetName(),
			*ClassName,
			*ParentName)
	);

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		BulletImpactEffect,
		SweepResult.ImpactPoint,
		SweepResult.ImpactNormal.Rotation()
	);

	UGameplayStatics::ApplyDamage(
		OtherActor,
		10,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass()
	);

	if (bPlayed) return;

	

	if (OtherActor) //&& OtherActor != GetOwner())
	{
		/*
		BulletAudio->PlaySound();
		bPlayed = true;
		*/
		
		AMyEnemy* mc = Cast<AMyEnemy>(OtherActor);

		if (mc && mc->GetIgnoreCharacterId() != BulletOwner) {
			BulletAudio->PlaySound();
			bPlayed = true;

		}
		
	}
	
}



void ABullet::Throw(const FVector& Direction, float Power)
{
	ShootBullet(Direction);
	UE_LOG(LogTemp, Warning, TEXT("Dir: %s"), *Direction.ToString());
}