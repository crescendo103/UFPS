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

void AMyEnemy::SetSPD(float speed, FVector position, FVector direction, bool isjump, bool isfire, bool isdeath)
{
	CurrentSpeed = speed;
	Position = position;
	Direction = direction;
	IsJump = isjump;
	IsFire = isfire;	
	IsDeath = isdeath;
}
/*
float AMyEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UE_LOG(LogTemp, Warning,
		TEXT("hit Enemy bullet"));

	FDamagePacket packet;
	packet.Header.Type = (int32)EPacketType::Damage;
	packet.Header.Size = sizeof(FDamagePacket);
	packet.CharacterId = -1;
	packet.Damage = DamageAmount;

	if (Cast<AFragGrenade>(DamageCauser))
	{
		UE_LOG(LogTemp, Warning, TEXT("FragGrenade Damage"));
		packet.Effect= EPlayerEffect::Red;
	}
	else if(Cast<AFlashBang>(DamageCauser)) {
		UE_LOG(LogTemp, Warning, TEXT("FlashBangGrenade Damage"));
		packet.Effect = EPlayerEffect::White;
	}
	else {
		packet.Effect = EPlayerEffect::Blue;
	}

	MyServer->MoveDmg(packet);	

	
	return DamageAmount;
}
*/
void AMyEnemy::SetIgnoreCharacterId(int id)
{
	IgnoreCharacterID = id;
}

int AMyEnemy::GetIgnoreCharacterId()
{
	return IgnoreCharacterID;
}

