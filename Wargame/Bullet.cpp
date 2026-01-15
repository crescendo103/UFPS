// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

	// 구체를 단순 콜리전 표현으로 사용합니다.
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	// 구체의 콜리전 반경을 설정합니다.
	CollisionComponent->InitSphereRadius(15.0f);
	// 루트 컴포넌트를 콜리전 컴포넌트로 설정합니다.
	RootComponent = CollisionComponent;
	CollisionComponent->SetCollisionProfileName(TEXT("Bull"));//이거 작동안하는거같은데...


	StaticBulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	StaticBulletMesh->SetupAttachment(CollisionComponent);
	StaticBulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 중요!
	StaticBulletMesh->SetMobility(EComponentMobility::Movable);

	// 이 컴포넌트를 사용하여 이 프로젝타일의 무브먼트를 구동시킵니다.

	PojectileCompo = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	PojectileCompo->InitialSpeed = 3000.f;
	PojectileCompo->MaxSpeed = 3000.f;
	
	//PojectileCompo->ProjectileGravityScale = 0.f; // 중력 무시
	/** 속도에 따른 로테이션 변화 X */
	PojectileCompo->bRotationFollowsVelocity = false;
	/** 바운스 X */
	PojectileCompo->bShouldBounce = false;
	PojectileCompo->Bounciness = 0.3f;
	//CollisionComponent->SetCollisionProfileName(TEXT("BlockAll"));
	PojectileCompo->SetUpdatedComponent(CollisionComponent);
	PojectileCompo->Activate();//이거 무조건있어야하나봐

	CollisionComponent->OnComponentHit.AddDynamic(this, &ABullet::OnHitSphere); //일단 임시로 차단...

}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
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

void ABullet::OnHitSphere(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	//디버그
	FString ActorName = OtherActor ? OtherActor->GetName() : TEXT("None");
	FString CompName = OtherComp ? OtherComp->GetName() : TEXT("None");

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Red,
		FString::Printf(TEXT("Hit Actor: %s | Component: %s"), *ActorName, *CompName)
	);




	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		BulletImpactEffect,
		Hit.ImpactPoint,                    //  맞은 지점
		Hit.ImpactNormal.Rotation()          //  표면 방향에 맞게 회전
	);

	// 콜리전 채널 체크
	ECollisionChannel HitChannel = OtherComp->GetCollisionObjectType();

	if (HitChannel == ECC_GameTraceChannel2) // Player 채널
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Green,
			TEXT("Player 채널에 맞음!")
		);
		if (Hit.GetActor() != nullptr) {
			//되는지 모르겟네..
			GEngine->AddOnScreenDebugMessage(
				-1,
				3.f,
				FColor::Green,
				TEXT("Player null!")
			);
			/* 서버라서 일단 비활성화
			UGameplayStatics::ApplyDamage(Hit.GetActor(), 10, GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
			*/
		}

		
		
		
	}


	GEngine->AddOnScreenDebugMessage(
		-1,            // -1 = 새 줄 (BP Print String 기본 동작)
		2.0f,          // 화면에 표시될 시간
		FColor::Yellow,
		TEXT("펑")
	);

	//OnBulletHit.Broadcast(BulletId);
	
	if (OnBulletHit.IsBound())
	{
		OnBulletHit.Execute(BulletId);
	}

	//Destroy();
}
