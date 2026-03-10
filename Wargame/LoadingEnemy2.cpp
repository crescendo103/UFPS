// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingEnemy2.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ALoadingEnemy2::ALoadingEnemy2()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	/*
	PrimaryActorTick.bCanEverTick = true;
	bIsWalking = false;
	// SkeletalMesh 생성
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SetRootComponent(SkeletalMeshComp);*/
	// ❗ Capsule은 절대 건드리지 말 것
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	// ✅ BP에서 설정한 Mesh 위치 그대로
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -89.f));

	// ✅ BP에서 설정한 Mesh 회전 그대로
	GetMesh()->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));

	
}

// Called when the game starts or when spawned
void ALoadingEnemy2::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALoadingEnemy2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALoadingEnemy2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

