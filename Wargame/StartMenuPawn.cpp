// Fill out your copyright notice in the Description page of Project Settings.


#include "StartMenuPawn.h"
#include "GameFramework/Controller.h"
#include <Kismet/KismetMathLibrary.h>

// Sets default values
AStartMenuPawn::AStartMenuPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
}

// Called when the game starts or when spawned
void AStartMenuPawn::BeginPlay()
{
	Super::BeginPlay();
    
    MyServer = GetGameInstance()->GetSubsystem<UMyServer>();
    //MyServer->StartPlayers.Add(this);    

    Iswalking = false;
    
    if (EnemyClass)
    {     

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


        EnemyActor = GetWorld()->SpawnActor<ALoadingEnemy2>(
            EnemyClass,
            FVector(-126704.051001, -3572.33055, 62.20919),
            FRotator(0.f, 90.f, 0.f), // Pitch, Yaw, Roll
            Params
        );

        if (!EnemyActor)
        {
            UE_LOG(LogTemp, Error, TEXT("Enemy spawn failed"));
        }
    }
   
}

// Called every frame
void AStartMenuPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

   
    
}

// Called to bind functionality to input
void AStartMenuPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}



