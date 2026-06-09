// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleCar.h"
#include "Components/BoxComponent.h"
#include "MyCharacter.h"
#include "GameFramework/PlayerController.h"
#include <Perception/AIPerceptionComponent.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "ChaosVehicleMovementComponent.h"
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>

AVehicleCar::AVehicleCar()
{
    PrimaryActorTick.bCanEverTick = true;
            

    CarMesh =
        CreateDefaultSubobject<UStaticMeshComponent>(
            TEXT("CarMesh"));
    // Root를 차량 메쉬로 설정
    RootComponent = CarMesh;

    // 물리 끄기
    CarMesh->SetSimulatePhysics(false);

    

    EnterTrigger =
        CreateDefaultSubobject<UBoxComponent>(
            TEXT("EnterTrigger"));

    EnterTrigger->SetupAttachment(
        RootComponent);

    EnterTrigger->SetBoxExtent(
        FVector(250, 250, 100));

    SpringArm =
        CreateDefaultSubobject<USpringArmComponent>(
            TEXT("SpringArm"));

    SpringArm->SetupAttachment(
        RootComponent);

    SpringArm->TargetArmLength = 500;

    SpringArm->SetRelativeLocation(
        FVector(0, 0, 100));

    SpringArm->bUsePawnControlRotation = true;



    Camera =
        CreateDefaultSubobject<UCameraComponent>(
            TEXT("Camera"));

    Camera->SetupAttachment(
        SpringArm);

    Camera->bUsePawnControlRotation = false;
}

void AVehicleCar::BeginPlay()
{
    Super::BeginPlay();

    EnterTrigger->OnComponentBeginOverlap.AddDynamic(
        this,
        &AVehicleCar::OnOverlapBegin
    );

    EnterTrigger->OnComponentEndOverlap.AddDynamic(
        this,
        &AVehicleCar::OnOverlapEnd
    );

    MyServer = GetGameInstance()->GetSubsystem<UMyServer>();
    /*
    if (MyServer) {
        MyServer->SpawnVehicles.Add(carId, this);
        UE_LOG(LogTemp, Warning, TEXT("car success"));
    }
    else {
        UE_LOG(LogTemp, Warning,
            TEXT("car fail"));
    }*/
}

void AVehicleCar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    DrawDebugBox(
        GetWorld(),
        EnterTrigger->GetComponentLocation(),
        EnterTrigger->GetScaledBoxExtent(),
        EnterTrigger->GetComponentQuat(),
        FColor::Red,
        false,
        0.f, // <- 여기
        0,
        5
    );
        /*
    if (!CurrentDriver)
        return;

    // 추가
    if (!IsLocallyControlled())
        return;
    */
    /*
    FVehiclePacket packet;
    packet.Header.Type = (int32)EPacketType::Vehicle;
    packet.Header.Size = sizeof(FVehiclePacket);    
    packet.carId = carId;
    packet.ownerId = -1;
    // 위치
    FVector Location =
        GetActorLocation();
    packet.X = Location.X;
    packet.Y = Location.Y;
    packet.Z = Location.Z;
    // 방향
    FVector Forward =
        GetActorForwardVector();
    packet.DirX = Forward.X;
    packet.DirY = Forward.Y;
    packet.DirZ = Forward.Z;

    packet.IsHaveCharacter = IsHaveCharacter;

    MyServer->MoveVehicle(packet);*/
    /*
    if (MyServer && IsHaveCharacter) {
        MyServer->MoveVehicle(packet);
    }*/
}

void AVehicleCar::SetupPlayerInputComponent(
    UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

    if (!EnhancedInputComponent)
        return;


    if (MoveForwardAction)
    {
        EnhancedInputComponent->BindAction(
            MoveForwardAction,
            ETriggerEvent::Triggered,
            this,
            &AVehicleCar::MoveForward
        );
    }

    if (LookAction)
    {
        EnhancedInputComponent->BindAction(
            LookAction,
            ETriggerEvent::Triggered,
            this,
            &AVehicleCar::MoveLook
        );
    }

    if (ExitAction)
    {
        EnhancedInputComponent->BindAction(
            ExitAction,
            ETriggerEvent::Started,
            this,
            &AVehicleCar::ExitPressed
        );
    }
}

void AVehicleCar::EnterVehicle(
    AMyCharacter* Character)
{
    if (!Character) return;

    CurrentDriver = Character;

    APlayerController* PC =
        Cast<APlayerController>(
            Character->GetController()
        );

    if (!PC) return;

    Character->SetActorHiddenInGame(true);

    Character->GetCharacterMovement()
        ->DisableMovement();

    Character->SetActorEnableCollision(false);

    PC->Possess(this);

    UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem
        <UEnhancedInputLocalPlayerSubsystem>
        (
            PC->GetLocalPlayer()
        );

    if (Subsystem)
    {
        Subsystem->ClearAllMappings();

        Subsystem->AddMappingContext(
            VehicleMappingContext,
            0
        );
    }

    IsHaveCharacter = true;
}

void AVehicleCar::ExitVehicle()
{
    if (!CurrentDriver) return;

    APlayerController* PC =
        Cast<APlayerController>(
            GetController());

    if (!PC) return;

    FVector ExitLoc =
        GetActorLocation()
        + GetActorRightVector() * 250;

    CurrentDriver->SetActorLocation(
        ExitLoc
    );

    CurrentDriver
        ->SetActorHiddenInGame(false);

    CurrentDriver
        ->SetActorEnableCollision(true);

    CurrentDriver
        ->GetCharacterMovement()
        ->SetMovementMode(
            MOVE_Walking
        );

    PC->Possess(CurrentDriver);

    UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem
        <UEnhancedInputLocalPlayerSubsystem>
        (
            PC->GetLocalPlayer()
        );

    if (Subsystem)
    {
        Subsystem->ClearAllMappings();

        Subsystem->AddMappingContext(
            CharacterMappingContext,
            0
        );
    }

    CurrentDriver = nullptr;
    IsHaveCharacter = false;
}

void AVehicleCar::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    AMyCharacter* Character =
        Cast<AMyCharacter>(OtherActor);

    if (Character)
    {
        //Character->CurrentVehicle = this;
        //CharacterId = Character->GetMyId();
        //Character->ShowVehicleUI();
    }
}

void AVehicleCar::OnOverlapEnd(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    AMyCharacter* Character =
        Cast<AMyCharacter>(OtherActor);

    if (Character)
    {
        Character->CurrentVehicle = nullptr;
        CharacterId = -1;
        //Character->HideVehicleUI();
    }
}

void AVehicleCar::MoveForward(
    const FInputActionValue& Value)
{

    if (!IsLocallyControlled())
        return;

    FVector2D Axis =
        Value.Get<FVector2D>();

    float DeltaTime =
        GetWorld()->GetDeltaSeconds();

    // W / S 전진 후진
    AddActorWorldOffset(
        GetActorForwardVector()
        * Axis.Y
        * MaxSpeed
        * DeltaTime,
        true
    );

    // A / D 차 회전
    AddActorWorldRotation(
        FRotator(
            0.f,
            Axis.X
            * TurnSpeed
            * DeltaTime,
            0.f
        )
    );
}

void AVehicleCar::MoveLook(
    const FInputActionValue& Value)
{

    if (!IsLocallyControlled())
        return;

    FVector2D Axis =
        Value.Get<FVector2D>();

    APlayerController* PC =
        Cast<APlayerController>(
            GetController());

    if (!PC)
        return;

    // 마우스 좌우
    PC->AddYawInput(
        Axis.X
    );

    // 마우스 상하
    PC->AddPitchInput(
        Axis.Y
    );
}



void AVehicleCar::ExitPressed()
{
    ExitVehicle();
}