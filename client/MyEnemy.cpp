// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyServer.h"
#include "FragGrenade.h"
#include "FlashBang.h"

// Sets default values
AMyEnemy::AMyEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bIsDeadFinal = false;
}

// Called when the game starts or when spawned
void AMyEnemy::BeginPlay()
{
	Super::BeginPlay();
	MyServer = GetGameInstance()->GetSubsystem<UMyServer>();
}

// Called every frame
void AMyEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMyEnemy::SetSPD(FEnemyState EnemyData)
{
	//이미 죽었으면 서버 값 무시
	if (bIsDeadFinal)
	{
		IsDeath = true;
		return;
	}

	bool bPrevDeath = IsDeath;

	CurrentSpeed = EnemyData.Speed;
	Position = EnemyData.Position;
	Direction = EnemyData.Direction;
	IsJump = EnemyData.isjump;
	IsFire = EnemyData.isfire;
	IsDeath = EnemyData.isdeath;
	IsHeal = EnemyData.isHeal;
	WeaponType = EnemyData.WeaponType;
	AimActive = EnemyData.AimActive;

	
	if (IsDeath) {
		bIsDeadFinal = true;
	}
	
}

void AMyEnemy::SetIgnoreCharacterId(int id)
{
	IgnoreCharacterID = id;
}

int AMyEnemy::GetIgnoreCharacterId()
{
	return IgnoreCharacterID;
}

void AMyEnemy::ActorStateInWorldByVehicle(bool hide)
{
	if (hide) {
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);		
	}
	else {
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
	}	
}

void AMyEnemy::AttachWeaponActor(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("AttachWeaponActor"));
	if (!Actor) return;
	UE_LOG(LogTemp, Warning, TEXT("AttachWeaponActor !actor"));
	Actor->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		TEXT("HandSocket")
	);
}
