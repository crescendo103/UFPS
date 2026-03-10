// Fill out your copyright notice in the Description page of Project Settings.


#include "StartMenuPawn.h"
#include "GameFramework/Controller.h"
#include <Kismet/KismetMathLibrary.h>

// Sets default values
AStartMenuPawn::AStartMenuPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AStartMenuPawn::BeginPlay()
{
	Super::BeginPlay();
    
    MyServer = GetGameInstance()->GetSubsystem<UMyServer>();
    MyServer->StartPlayers.Add(this);    

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

    if (Iswalking) {
        FVector CurrentLocation = EnemyActor->GetActorLocation();
        FRotator CurrentRot = EnemyActor->GetActorRotation();
        //  현재 위치와 목표 위치 사이 거리 계산
        float Distance = FVector::Dist(CurrentLocation, TargetLocation);
        

        //  일정 거리 이하이면 멈춤
        if (Distance <= 5.f)   // 10은 허용 오차 (원하는 값으로 조절)
        {
            Iswalking = false;
            EnemyActor->bIsWalking = false;
            return;
        }

        FVector NewLocation = FMath::VInterpTo(
            CurrentLocation,
            TargetLocation,
            DeltaTime,
            MoveSpeed
        );

        EnemyActor->SetActorLocation(NewLocation);

        // 🔹 매 프레임 "나"를 바라보게
        FVector LookTarget = FVector(-127052.6f, -3110.548f, 106.14188f);//GetActorLocation(); // StartMenuPawn = 나

        FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(
            CurrentLocation,
            LookTarget
        );


        // Yaw만
        TargetRot.Pitch = 0.f;
        TargetRot.Roll = 0.f;

        FRotator SmoothRot = FMath::RInterpTo(
            CurrentRot,
            TargetRot,
            DeltaTime,
            0.5f // 회전 속도
        );

        EnemyActor->SetActorRotation(SmoothRot);

    }
    
}

// Called to bind functionality to input
void AStartMenuPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AStartMenuPawn::MoveEnemy(int order)
{
    if (!EnemyActor)
    {
        UE_LOG(LogTemp, Error, TEXT("EnemyActor is NULL in MoveEnemy"));
        return;
    }

    Iswalking = true;
    EnemyActor->bIsWalking = true;
}

