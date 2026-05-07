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
	CurrentSpeed = EnemyData.Speed;
	Position = EnemyData.Position;
	Direction = EnemyData.Direction;
	IsJump = EnemyData.isjump;
	IsFire = EnemyData.isfire;
	IsDeath = EnemyData.isdeath;
	IsHeal = EnemyData.isHeal;
	IsHaveGun = EnemyData.isHaveGun;
}

void AMyEnemy::SetIgnoreCharacterId(int id)
{
	IgnoreCharacterID = id;
}

int AMyEnemy::GetIgnoreCharacterId()
{
	return IgnoreCharacterID;
}

