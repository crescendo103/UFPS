// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGrenade.h"


// Sets default values
AMyGrenade::AMyGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.


	PrimaryActorTick.bCanEverTick = true;
	
	StaticGrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("hi"));
	//  Physics 설정
	StaticGrenadeMesh->SetSimulatePhysics(true);
	StaticGrenadeMesh->SetEnableGravity(true);

	//  Collision 설정
	StaticGrenadeMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticGrenadeMesh->SetCollisionProfileName(TEXT("PhysicsActor"));

	RootComponent = StaticGrenadeMesh;

	
	
	// 구체를 단순 콜리전 표현으로 사용합니다.
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	// 구체의 콜리전 반경을 설정합니다.
	CollisionComponent->InitSphereRadius(15.0f);

	CollisionComponent->SetSimulatePhysics(false);                 // 물리 시뮬 X
	CollisionComponent->SetNotifyRigidBodyCollision(true);        // Hit 이벤트 ON
	CollisionComponent->SetGenerateOverlapEvents(false);          // 필요 없으면 OFF
	CollisionComponent->SetupAttachment(StaticGrenadeMesh);

	grenadeCompo = CreateDefaultSubobject<UMyGrenadeComponent>(TEXT("gre"));
	

	
	
	
}

// Called when the game starts or when spawned
void AMyGrenade::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void AMyGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	

}

void AMyGrenade::ThrowGrenade()
{
	//grenadeCompo->RayGrenade(GetActorLocation(), GetActorForwardVector(), 100);
	grenadeCompo->SetBshow(true);
}

void AMyGrenade::ThrowWheelVal(float val)
{

	grenadeCompo->SetWheelVal(val);
	GEngine->AddOnScreenDebugMessage(
		-1,            // -1 = 새 줄 (BP Print String 기본 동작)
		2.0f,          // 화면에 표시될 시간
		FColor::Yellow,
		TEXT("Wheel")
	);
}






void AMyGrenade::Throw(const FVector& Direction, float Power)
{
	FVector Velocity = Direction * Power;

	//UStaticMeshComponent* Mesh = GetMesh();
	///StaticGrenadeMesh->SetSimulatePhysics(true);

	// 속도 직접 설정
	StaticGrenadeMesh->SetPhysicsLinearVelocity(Velocity);

	// 또는 임펄스
	// Mesh->AddImpulse(Velocity * Mesh->GetMass());

	// 3초 후 폭발 예약
	GetWorldTimerManager().SetTimer(
		ExplosionTimerHandle,
		this,
		&AMyGrenade::Explode,
		3.0f,
		false
	);

}

void AMyGrenade::Explode()
{
	if (GrenadeImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			GrenadeImpactEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	Destroy();
}

