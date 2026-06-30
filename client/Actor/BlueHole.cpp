// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueHole.h"
#include <Kismet/GameplayStatics.h>
#include "MyCharacter.h"
#include "DrawDebugHelpers.h"
#include "CustomPlayerController.h"
#include "StartMenuPawn.h"


// Sets default values
ABlueHole::ABlueHole()
{
    PrimaryActorTick.bCanEverTick = true;

    CurrentRadius = 3000.f;
    TargetRadius = 3000.f;

    ZoneSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ZoneSphere"));
    RootComponent = ZoneSphere;

    ZoneSphere->InitSphereRadius(CurrentRadius);

    ZoneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZoneMesh"));
    ZoneMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABlueHole::BeginPlay()
{
	Super::BeginPlay();
	
    Player = Cast<AMyCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

    GetWorldTimerManager().SetTimer(
        CheckTimerHandle,
        this,
        &ABlueHole::CheckPlayersInZone,
        1.0f,
        true
    );
    MeshBaseRadius = ZoneMesh->GetStaticMesh()->GetBounds().SphereRadius;
    SetSize();
    
    


    //ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    
}

// Called every frame
void ABlueHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    float ShrinkSpeed = 10.f; // 초당 10 유닛
    CurrentRadius = FMath::Max(
        TargetRadius,
        CurrentRadius - ShrinkSpeed * DeltaTime
    );
    SetSize();       

    DrawDebugSphere(
        GetWorld(),
        GetActorLocation(),
        CurrentRadius,
        32,
        FColor::Red,
        false,
        -1.f,
        0,
        5.f
    );
    UE_LOG(LogTemp, Warning,
        TEXT("Sphere=%f Mesh=%f"),
        ZoneSphere->GetScaledSphereRadius(),
        ZoneMesh->Bounds.SphereRadius);
}


void ABlueHole::CheckPlayersInZone()
{
    if (!Player)
    {
        //UE_LOG(LogTemp, Error, TEXT("플레이어를 못찾음"));
        return;
    }
        

    float Distance = FVector::Dist(
        Player->GetActorLocation(),
        GetActorLocation()
    );

    if (Distance <= CurrentRadius)
    {
        Player->InZone();
    }
    else
    {
        Player->OutZone();
    }
        
    
}
void ABlueHole::SetSize()
{
    if (!ZoneMesh || !ZoneSphere)
        return;

    ZoneSphere->SetSphereRadius(CurrentRadius, true);

    float Scale = CurrentRadius / MeshBaseRadius;

    ZoneMesh->SetRelativeScale3D(
        FVector(Scale)
    );

    // 3. 피벗 보정
    // 메시의 피벗이 바닥(Z=0)에 있으므로, 정중앙으로 맞추기 위해 
    // 계산된 현재 반지름(CurrentRadius)만큼 Z축을 아래로(-Z) 내려줍니다.
    ZoneMesh->SetRelativeLocation(FVector(0.f, 0.f, -CurrentRadius));
}

void ABlueHole::SetNextZone()
{
    TargetRadius *= 0.7f; // 30% 감소

    if (TargetRadius < 500.f)
    {
        TargetRadius = 500.f;
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("새 목표 반지름: %f"), TargetRadius);

        FBrodcastMessage msgPacket;

        msgPacket.Header.Size = sizeof(FBrodcastMessage);
        msgPacket.Header.Type = static_cast<uint16>(EPacketType::BoradCast);

        strcpy_s(
            msgPacket.Information,
            "경기장 크기가 줄어듭니다."
        );

        UMyServer* MyServer = GetGameInstance()->GetSubsystem<UMyServer>();
        if (MyServer) {
            MyServer->MoveBroadCast(msgPacket);
        }
    }
    
    
}

void ABlueHole::SetOverlapActive(bool state)
{
    if (state) {
        if (Player && ZoneSphere->IsOverlappingActor(Player))
        {
            // 자기장 안 → 정상 화면
            Player->InZone();
        }

        GetWorld()->GetTimerManager().SetTimer(
            timerHandle,
            this,
            &ABlueHole::SetNextZone,
            60.0f,
            true
        );
    }  
    
}


