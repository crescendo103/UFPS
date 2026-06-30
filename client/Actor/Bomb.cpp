// Fill out your copyright notice in the Description page of Project Settings.


#include "Bomb.h"
#include "ItemEffectComponent.h"

// Sets default values
ABomb::ABomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMesh"));	
	RootComponent = BombMesh;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->InitSphereRadius(5.0f);	
	CollisionComponent->SetupAttachment(RootComponent);

	// Projectile
	PojectileCompo = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	PojectileCompo->InitialSpeed = 1000.f;
	PojectileCompo->MaxSpeed = 1000.f;
	PojectileCompo->bRotationFollowsVelocity = false;
	PojectileCompo->bShouldBounce = false;
	PojectileCompo->SetUpdatedComponent(CollisionComponent);

	// Audio
	ItemEffectComponent = CreateDefaultSubobject<UItemEffectComponent>(TEXT("EffectComponent"));
	ItemEffectComponent->SetupAttachment(RootComponent);
	
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABomb::OnOverlapSphere);
}

// Called when the game starts or when spawned
void ABomb::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(5.0f);

}

// Called every frame
void ABomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABomb::OnOverlapSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	

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
	/*
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		BulletImpactEffect,
		SweepResult.ImpactPoint,
		SweepResult.ImpactNormal.Rotation()
	);*/

	UGameplayStatics::ApplyDamage(
		OtherActor,
		100,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass()
	);

	ItemEffectComponent->PlayDestroySound();
	ItemEffectComponent->PlayDestroyEffect();

	Destroy();
}

