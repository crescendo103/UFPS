// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualGrenade.h"

// Sets default values
AVisualGrenade::AVisualGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//GrenadeCalComponent = CreateDefaultSubobject<UGrenadeThrowCalculateCompo>(TEXT("Calculate"));
}

void AVisualGrenade::AddThrowAngle(float val)
{
	AccumulateWheelVal += val;
	dirty = true;
	//GrenadeCalComponent->SetMouseVal(val);
}

bool AVisualGrenade::GetDirty()
{
	return dirty;
}

void AVisualGrenade::SetDirty(bool state)
{
	dirty = state;
}

// Called when the game starts or when spawned
void AVisualGrenade::BeginPlay()
{
	Super::BeginPlay();
	/*
	StaticGrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("hi"));
	//  Physics 설정
	StaticGrenadeMesh->SetSimulatePhysics(true);
	StaticGrenadeMesh->SetEnableGravity(true);

	//  Collision 설정
	StaticGrenadeMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);	
	RootComponent = StaticGrenadeMesh;*/

	
	AccumulateWheelVal = 0;

}

// Called every frame
void AVisualGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

