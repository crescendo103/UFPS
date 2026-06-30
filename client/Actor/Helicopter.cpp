// Fill out your copyright notice in the Description page of Project Settings.


#include "Helicopter.h"
#include <EnhancedInputComponent.h>
#include "PawnManager.h"


// Sets default values
AHelicopter::AHelicopter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	

	rt = CreateDefaultSubobject<USphereComponent>(TEXT("rt"));
	RootComponent = rt;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 2000.0f;
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
	CameraBoom->SetRelativeRotation(FRotator(-5.f, 0.f, 0.f));
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bDoCollisionTest = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->bAutoActivate = false;
	// 이동 컴포넌트 추가
	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
	
	bFindCameraComponentWhenViewTarget = true; 	
}

// Called when the game starts or when spawned
void AHelicopter::BeginPlay()
{
	Super::BeginPlay();
	TargetLocation = FVector(28950.0, -56690.0, 4090.0);
	//TargetLocation = FVector(0, 0, 0);
}

// Called every frame
void AHelicopter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();	

	float InterpSpeed = 0.3f; // 클수록 빠름
	
	//지정된 위치로 갑니다.
	FVector NewLocation = FMath::VInterpTo(
		CurrentLocation,
		TargetLocation,
		DeltaTime,
		InterpSpeed
	);

	
	SetActorLocation(NewLocation);
}

// Called to bind functionality to input
void AHelicopter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		if (DownAction)
		{
			EnhancedInputComponent->BindAction(DownAction, ETriggerEvent::Started, this, &AHelicopter::JumpFromHelicopter);
		}
	}
}

void AHelicopter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Helicopter CalcCamera Called"));
	if (FollowCamera)
	{
		FollowCamera->GetCameraView(DeltaTime, OutResult);
	}
}

void AHelicopter::JumpFromHelicopter()
{
	UE_LOG(LogTemp, Warning, TEXT("JumpFromHelicopter Called"));


	if (Manager)
	{
		Manager->PossesLocalPlayer();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("JumpFromHelicopter Called wrong"));
	}
}

void AHelicopter::RegisterManager(UPawnManager* manager)
{
	Manager = manager;
}

void AHelicopter::ActivateCamera()
{
	if (FollowCamera)
	{
		FollowCamera->Activate(true);

		UE_LOG(LogTemp, Warning,
			TEXT("Heli Camera Active=%d"),
			FollowCamera->IsActive());
	}
}

void AHelicopter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	APlayerController* PC = Cast<APlayerController>(NewController);
	if (!PC) return;

	// Input MappingContext 추가
	if (ULocalPlayer* LP = PC->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(InputMappingContext, 1);
		}
	}

	// 헬기 카메라 활성
	ActivateCamera();

	FViewTargetTransitionParams Params;
	Params.BlendTime = 0.5f;
	Params.BlendFunction = VTBlend_Cubic;

	PC->SetViewTarget(this, Params);
}
