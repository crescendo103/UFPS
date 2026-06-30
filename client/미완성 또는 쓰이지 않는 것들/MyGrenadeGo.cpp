// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGrenadeGo.h"

// Sets default values for this component's properties
UMyGrenadeGo::UMyGrenadeGo()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	//PojectileCompo = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	//PojectileCompo->Deactivate();
	
	// ...
}


// Called when the game starts
void UMyGrenadeGo::BeginPlay()
{
	Super::BeginPlay();
	

	

	// ...
	
}


// Called every frame
void UMyGrenadeGo::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMyGrenadeGo::GoGrenade(const FVector Start, const FVector Forward, float Power)
{
	// 충돌 활성화
	if (GrenadeMesh2)
	{
		GrenadeMesh2->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GrenadeMesh2->SetCollisionProfileName(TEXT("PhysicsActor"));
		GrenadeMesh2->SetNotifyRigidBodyCollision(true);
	}


	
	PojectileCompo->ProjectileGravityScale = 1.0f;

	PojectileCompo->bShouldBounce = true;
	PojectileCompo->Bounciness = 0.5f; // 튕길수록 감속
	PojectileCompo->Friction = 0.3f;   // 굴러가며 감속

	PojectileCompo->BounceVelocityStopSimulatingThreshold = 50.f;

	PojectileCompo->Velocity = Forward * Power;
	PojectileCompo->Activate(true);


	GrenadeMesh2->OnComponentHit.AddDynamic(this, &UMyGrenadeGo::OnHitStaticMesh);
	

}

void UMyGrenadeGo::SetUpdateProjectile(UStaticMeshComponent* compo)
{
	PojectileCompo->SetUpdatedComponent(compo);
	GrenadeMesh2 = compo;
}



void UMyGrenadeGo::OnHitStaticMesh(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (PojectileCompo->Velocity.Size() < 5.f)
	{
		//Destroy();
		GetOwner()->Destroy();
	}
}