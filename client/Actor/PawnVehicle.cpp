#include "PawnVehicle.h"

#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "MyCharacter.h"
#include "MyServer.h"
#include "ItemEffectComponent.h"



APawnVehicle::APawnVehicle()
{
	PrimaryActorTick.bCanEverTick = true;

	CarMesh =
		CreateDefaultSubobject<UStaticMeshComponent>(
			TEXT("CarMesh"));

	RootComponent = CarMesh;

	CarMesh->SetSimulatePhysics(false);

	EnterTrigger =
		CreateDefaultSubobject<UBoxComponent>(
			TEXT("EnterTrigger"));

	EnterTrigger->SetupAttachment(RootComponent);

	EnterTrigger->SetBoxExtent(
		FVector(250.f, 250.f, 100.f));

	SpringArm =
		CreateDefaultSubobject<USpringArmComponent>(
			TEXT("SpringArm"));

	SpringArm->SetupAttachment(RootComponent);

	SpringArm->TargetArmLength = 500.f;

	SpringArm->SetRelativeLocation(
		FVector(0.f, 0.f, 100.f));

	SpringArm->bUsePawnControlRotation = true;

	Camera =
		CreateDefaultSubobject<UCameraComponent>(
			TEXT("Camera"));

	Camera->SetupAttachment(SpringArm);

	Camera->bUsePawnControlRotation = false;


	EffectCompo = CreateDefaultSubobject<UItemEffectComponent>(TEXT("EffectComp"));
	EffectCompo->SetupAttachment(RootComponent);
}

void APawnVehicle::BeginPlay()
{
	Super::BeginPlay();

	EnterTrigger->OnComponentBeginOverlap.AddDynamic(
		this,
		&APawnVehicle::OnOverlapBegin);

	EnterTrigger->OnComponentEndOverlap.AddDynamic(
		this,
		&APawnVehicle::OnOverlapEnd);

	MyServer =
		GetGameInstance()->GetSubsystem<UMyServer>();

	if (MyServer)
	{
		if (carId < 0)
		{
			carId =
				MyServer->SpawnVehicles.Num();
		}

		if (!MyServer->SpawnVehicles.Contains(carId))
		{
			MyServer->SpawnVehicles.Add(
				carId,
				this);

			UE_LOG(LogTemp, Warning,
				TEXT("Vehicle Register Success carId=%d"),
				carId);
		}
		else
		{
			UE_LOG(LogTemp, Error,
				TEXT("Duplicate carId=%d"),
				carId);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error,
			TEXT("MyServer nullptr"));
	}
}

void APawnVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (!CurrentDriver)
	//	return;

	// 추가
	//if (!IsLocallyControlled())
	//	return;
	
		
		FVehiclePacket packet;
		packet.Header.Type = (int32)EPacketType::Vehicle;
		packet.Header.Size = sizeof(FVehiclePacket);
		packet.carId = carId;
		packet.ownerId = -1;
		// 위치
		FVector Location = GetActorLocation();
		packet.X = Location.X;
		packet.Y = Location.Y;
		packet.Z = Location.Z;
		// 방향
		FVector Forward = GetActorForwardVector();
		packet.DirX = Forward.X;
		packet.DirY = Forward.Y;
		packet.DirZ = Forward.Z;

		packet.IsHaveCharacter = IsHaveCharacter;		
	
		if (MyServer && IsHaveCharacter) {
			MyServer->MoveVehicle(packet);
		}

}

void APawnVehicle::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(
		PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput =
		Cast<UEnhancedInputComponent>(
			PlayerInputComponent);

	if (!EnhancedInput)
		return;

	if (MoveAction)
	{
		EnhancedInput->BindAction(
			MoveAction,
			ETriggerEvent::Triggered,
			this,
			&APawnVehicle::Move);
	}

	if (LookAction)
	{
		EnhancedInput->BindAction(
			LookAction,
			ETriggerEvent::Triggered,
			this,
			&APawnVehicle::Look);
	}

	if (ExitAction)
	{
		EnhancedInput->BindAction(
			ExitAction,
			ETriggerEvent::Started,
			this,
			&APawnVehicle::ExitPressed);
	}
}

void APawnVehicle::Move(
	const FInputActionValue& Value)
{
	if (!IsLocallyControlled())
		return;

	FVector2D Axis =
		Value.Get<FVector2D>();

	float Delta =
		GetWorld()->GetDeltaSeconds();

	AddActorWorldOffset(
		GetActorForwardVector()
		* Axis.Y
		* MoveSpeed
		* Delta,
		true);

	AddActorWorldRotation(
		FRotator(
			0.f,
			Axis.X * TurnSpeed * Delta,
			0.f));
}

void APawnVehicle::Look(
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

	PC->AddYawInput(Axis.X);
	PC->AddPitchInput(Axis.Y);
}

void APawnVehicle::EnterVehicle(
	AMyCharacter* Character)
{
	if (!Character)
		return;

	APlayerController* PC =
		Cast<APlayerController>(
			Character->GetController());

	if (!PC)
		return;

	CurrentDriver = Character;
	FVector tempLoc = FVector(100.f, 100.f, 100.f);
	Character->SetActorLocation(tempLoc);

	Character->SetActorHiddenInGame(true);

	Character->SetActorEnableCollision(false);

	Character->GetCharacterMovement()
		->DisableMovement();

	PC->Possess(this);

	IsHaveCharacter = true;

	EffectCompo->PlayStartSound();
	EffectCompo->StartLoopSound();
}

void APawnVehicle::ExitVehicle()
{
	if (!CurrentDriver)
		return;

	APlayerController* PC =
		Cast<APlayerController>(
			GetController());

	if (!PC)
		return;

	FVector ExitPos =
		GetActorLocation()
		+ GetActorRightVector() * 250.f;

	CurrentDriver->SetActorLocation(
		ExitPos);

	CurrentDriver->SetActorHiddenInGame(false);

	CurrentDriver->SetActorEnableCollision(true);

	CurrentDriver->GetCharacterMovement()
		->SetMovementMode(MOVE_Walking);

	PC->Possess(CurrentDriver);

	CurrentDriver = nullptr;

	IsHaveCharacter = false;

	//마지막으로 보냄 그만 패킷 보내라고 서버한테 요청
	FVehiclePacket packet;
	packet.Header.Type = (int32)EPacketType::Vehicle;
	packet.Header.Size = sizeof(FVehiclePacket);
	packet.carId = carId;
	packet.ownerId = -1;
	// 위치
	FVector Location = GetActorLocation();
	packet.X = Location.X;
	packet.Y = Location.Y;
	packet.Z = Location.Z;
	// 방향
	FVector Forward = GetActorForwardVector();
	packet.DirX = Forward.X;
	packet.DirY = Forward.Y;
	packet.DirZ = Forward.Z;
	packet.IsHaveCharacter = IsHaveCharacter;

	if (MyServer) {
		MyServer->MoveVehicle(packet);
	}

	EffectCompo->PlayDestroySound();
	EffectCompo->StopLoopSound();

}

void APawnVehicle::ExitPressed()
{
	ExitVehicle();
}

void APawnVehicle::OnOverlapBegin(
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
		Character->CurrentVehicle = this;
	}
}

void APawnVehicle::OnOverlapEnd(
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
	}
}