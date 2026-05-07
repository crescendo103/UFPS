// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueHole.h"
#include <Kismet/GameplayStatics.h>
#include "MyCharacter.h"
#include "DrawDebugHelpers.h"


// Sets default values
ABlueHole::ABlueHole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CurrentRadius = 3000.f; // 초기 반지름

    ZoneSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ZoneSphere"));
    RootComponent = ZoneSphere;

    ZoneSphere->InitSphereRadius(3000.f);
    //ZoneSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    ZoneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZoneMesh"));
    ZoneMesh->SetupAttachment(RootComponent);
    //ZoneMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 🔥 여기서 크기 맞추기
    const float DefaultMeshRadius = 50.f; // Shape_Sphere 기본 반지름
    float Scale = CurrentRadius / DefaultMeshRadius;
    ZoneMesh->SetWorldScale3D(FVector(Scale));
    TargetRadius = 3000.f;
}

// Called when the game starts or when spawned
void ABlueHole::BeginPlay()
{
	Super::BeginPlay();
	
    /*
    ZoneSphere->OnComponentBeginOverlap.AddDynamic(
        this,
        &ABlueHole::OnOverlapBegin
    );

    ZoneSphere->OnComponentEndOverlap.AddDynamic(
        this,
        &ABlueHole::OnOverlapEnd
    );
    */
    Player = Cast<AMyCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

    GetWorldTimerManager().SetTimer(
        CheckTimerHandle,
        this,
        &ABlueHole::CheckPlayersInZone,
        1.0f,
        true
    );
    MeshBaseRadius = ZoneMesh->GetStaticMesh()->GetBounds().SphereRadius;
    isDone = false;
    


    //ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    if (Player && ZoneSphere->IsOverlappingActor(Player))
    {
        // 자기장 안 → 정상 화면
        Player->InZone();
    }

    GetWorld()->GetTimerManager().SetTimer(
        timerHandle,
        this,
        &ABlueHole::SetTargetRadius,
        60.0f,
        true
    );
}

// Called every frame
void ABlueHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    CurrentRadius = FMath::FInterpTo(
        CurrentRadius,
        1000.f,
        DeltaTime,
        0.1f
    );

    CurrentRadius = FMath::Max(CurrentRadius, 1000.f);

    ZoneSphere->SetSphereRadius(CurrentRadius, true);

    if (CurrentRadius <= TargetRadius) {
        isDone = true;
    }

    if (!isDone) {
        float Scale = CurrentRadius / MeshBaseRadius;
        ZoneMesh->SetWorldScale3D(FVector(Scale));
    }
    
    

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



    
}

/*
void ABlueHole::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    
    // 자기장 안 → 정상 화면
    if (Player == Cast<AMyCharacter>(OtherActor)) {
        Player->InZone();
    }
    
}

void ABlueHole::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    
    if (Player == Cast<AMyCharacter>(OtherActor)) {
        // 자기장 밖 → 파란 화면
        Player->OutZone();
    }
    
}
*/
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
    ZoneSphere->SetSphereRadius(CurrentRadius, true);

    // 메시 실제 반지름 가져오기
    float MeshRadius = ZoneMesh->GetStaticMesh()->GetBounds().SphereRadius;

    float Scale = CurrentRadius / MeshRadius;
    ZoneMesh->SetWorldScale3D(FVector(Scale));
}

void ABlueHole::SetTargetRadius()
{
    TargetRadius -= TargetRadius;
}


