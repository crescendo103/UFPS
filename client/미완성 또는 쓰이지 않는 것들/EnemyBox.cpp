// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBox.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AEnemyBox::AEnemyBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	// 콜리전 설정
	TriggerSphere->InitSphereRadius(50.f); // 구체 반지름
	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//TriggerSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	TriggerSphere->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel5);

	RootComponent = TriggerSphere;
	
	
}

// Called when the game starts or when spawned
void AEnemyBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AEnemyBox::fillBox(AActor* actor)
{
	dropBoxItems.Add(actor);
}

TArray<AActor*> AEnemyBox::GetDropBoxItems()
{
	return dropBoxItems;
}

