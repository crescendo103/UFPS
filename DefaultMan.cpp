// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultMan.h"
#include "Engine/Engine.h" // GEngine->AddOnScreenDebugMessage를 위해 포함


// Sets default values
ADefaultMan::ADefaultMan()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// UStaticMeshComponent 생성 및 Root Component로 설정
	m_StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyMesh"));
	RootComponent = m_StaticMesh;
	// 변수 기본값 설정
	m_Message = TEXT("Hello from C++!");
}

// Called when the game starts or when spawned
void ADefaultMan::BeginPlay()
{
	Super::BeginPlay();
	PrintMessage();
}

// Called every frame
void ADefaultMan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADefaultMan::PrintMessage()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, m_Message);
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *m_Message);
}

