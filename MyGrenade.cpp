// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGrenade.h"

// Sets default values
AMyGrenade::AMyGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.


	PrimaryActorTick.bCanEverTick = true;
	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
	SetRootComponent(GrenadeMesh);

	if (GrenadeAsset)
	{
		GrenadeMesh->SetStaticMesh(GrenadeAsset);
	}

	

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
	/*
	if (bShow && grenadeCompo)
	{
		grenadeCompo->RayGrenade(
			GetActorLocation(),
			GetActorForwardVector(),
			100.f
		);
	}*/
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





