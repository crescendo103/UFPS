// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingEnemyActor.h"

// Sets default values
ALoadingEnemyActor::ALoadingEnemyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bIsWalking = false;
	// SkeletalMesh »ý¼º
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SetRootComponent(SkeletalMeshComp);
}

// Called when the game starts or when spawned
void ALoadingEnemyActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALoadingEnemyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

