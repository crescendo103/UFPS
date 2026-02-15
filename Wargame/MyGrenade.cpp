// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGrenade.h"
#include "Field/FieldSystemComponent.h"
#include "Field/FieldSystemObjects.h"
#include "MyFractureActor.h"


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
	//StaticGrenadeMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	//StaticGrenadeMesh->SetCollisionProfileName(TEXT("BlockAll"));//실험
	RootComponent = StaticGrenadeMesh;

	
	
	// 구체를 단순 콜리전 표현으로 사용합니다.
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	// 구체의 콜리전 반경을 설정합니다.
	CollisionComponent->InitSphereRadius(5.0f);

	CollisionComponent->SetSimulatePhysics(false);                 // 물리 시뮬 X
	CollisionComponent->SetNotifyRigidBodyCollision(true);        // Hit 이벤트 ON
	CollisionComponent->SetGenerateOverlapEvents(false);          // 필요 없으면 OFF
	CollisionComponent->SetupAttachment(StaticGrenadeMesh);

	grenadeCompo = CreateDefaultSubobject<UMyGrenadeComponent>(TEXT("gre"));
	

	
	//CollisionComponent->OnComponentHit.AddDynamic(this, &AMyGrenade::OnHitSphere);//부착할려고
	
	StaticGrenadeMesh->SetNotifyRigidBodyCollision(true);
	

}

// Called when the game starts or when spawned
void AMyGrenade::BeginPlay()
{
	Super::BeginPlay();
	
	TargetVoxelWorld = Cast<AVoxelWorld>(UGameplayStatics::GetActorOfClass(GetWorld(), AVoxelWorld::StaticClass()));
	UE_LOG(LogTemp, Warning, TEXT("VoxelWorld: %s"), *TargetVoxelWorld->GetName());

	StaticGrenadeMesh->OnComponentHit.AddDynamic(
		this,
		&AMyGrenade::OnHitSphere
	);

	UE_LOG(LogTemp, Warning, TEXT("Hit Bound: %s"),
		StaticGrenadeMesh->OnComponentHit.IsBound() ? TEXT("YES") : TEXT("NO"));

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
	ExplodeVoxel(GetActorLocation(), 200.f);
	
	

	
}


void AMyGrenade::ExplodeVoxel(FVector ExplosionCenter, float ExplosionRadius)
{
	if (!TargetVoxelWorld) return;

	UE_LOG(LogTemp, Warning, TEXT("ExplodeVoxel"));

	UVoxelSphereTools::RemoveSphere(
		TargetVoxelWorld,
		ExplosionCenter,
		ExplosionRadius,
		nullptr,   // ModifiedValues 필요 없으면 nullptr
		nullptr,   // EditedBounds 필요 없으면 nullptr
		true,      // MultiThreaded
		true,      // ConvertToVoxelSpace
		true       // UpdateRender
	);

	Destroy();
}


void AMyGrenade::OnHitSphere(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bSticked) return;

	bSticked = true;


	if (!OtherComp) {
		UE_LOG(LogTemp, Warning, TEXT("No compo!"));
		return;
	}
	//디버그
	FString ActorName = OtherActor ? OtherActor->GetName() : TEXT("None");
	FString CompName = OtherComp ? OtherComp->GetName() : TEXT("None");

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Red,
		FString::Printf(TEXT("Hit Actor: %s | Component: %s"), *ActorName, *CompName)
	);	

	// 콜리전 채널 체크
	ECollisionChannel HitChannel = OtherComp->GetCollisionObjectType();

	if (HitChannel == ECC_GameTraceChannel3) // FractureAble 채널
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Green,
			TEXT("FractureAble 채널에 맞음!")
		);

		UE_LOG(LogTemp, Warning, TEXT("FractureAble 채널에 맞음!"));
		if (Hit.GetActor() != nullptr) {
			
			USceneComponent* RootComp = Hit.GetActor()->GetRootComponent();
			// 1 맞은 액터에서 GC 컴포넌트 찾기
			
			
			// 충돌 지점으로 이동
			SetActorLocation(Hit.ImpactPoint + Hit.ImpactNormal * 2.f);
				/*
				// 부모에 스냅 부착
				AttachToComponent(
					RootComp,
					FAttachmentTransformRules::SnapToTargetNotIncludingScale
				);*/
				
			UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(GetRootComponent());
			if (Prim)
			{
				Prim->SetSimulatePhysics(false);
			}

			// 월드 위치 기준으로 먼저 이동
			SetActorLocation(Hit.ImpactPoint);

			// 맞은 컴포넌트에 스냅 Attach
			AttachToComponent(
				OtherComp,
				FAttachmentTransformRules::KeepWorldTransform
			);

			// 맞은 액터가 MyFractureActor인지 확인
			AMyFractureActor* FractureActor = Cast<AMyFractureActor>(OtherActor);

			if (FractureActor)
			{
				UE_LOG(LogTemp, Warning, TEXT("FractureActor 발견 → 파괴 실행"));

				FractureActor->DelayApplyFracture(Hit.ImpactPoint);
			}						
				
		}		
			/*
			OtherActor = GC를 포함한 전체 액터

			Hit.GetActor() = 실제 부서지는 컴포넌트가 속한 액터
			*/

	}
}

	

