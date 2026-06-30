// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingEnemy2.h"
#include "Components/CapsuleComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "MyServer.h"

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
    bIsWalking = false;
    UMyServer* MyServer = GetGameInstance()->GetSubsystem<UMyServer>();
    MyServer->StartPlayers.Add(this);
}

// Called every frame
void ALoadingEnemy2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (bIsWalking) {
        FVector CurrentLocation = GetActorLocation();
        FRotator CurrentRot = GetActorRotation();
        //  현재 위치와 목표 위치 사이 거리 계산
        float Distance = FVector::Dist(CurrentLocation, TargetLocation);
                
        
        //  일정 거리 이하이면 멈춤
        if (Distance <= 30.f)   // 10은 허용 오차 (원하는 값으로 조절)
        {
            bIsWalking = false;
            //SetActorLocation(TargetLocation);
            SetActorTickEnabled(false);  // 🔥 Tick 완전히 중지
            return;
        }

        FVector NewLocation = FMath::VInterpTo(
            CurrentLocation,
            TargetLocation,
            DeltaTime,
            MoveSpeed
        );

        SetActorLocation(NewLocation);

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

        SetActorRotation(SmoothRot);

    }
}

// Called to bind functionality to input
void ALoadingEnemy2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ALoadingEnemy2::MoveEnemy()
{
    bIsWalking = true;
}

int32 ALoadingEnemy2::getMyOrder()
{
    return MyOrder;
}
