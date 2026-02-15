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
	
}

// Called every frame
void AHelicopter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = FVector(-1766.314006, 1151.793497, 1925.888404);

	float InterpSpeed = 0.1f; // 클수록 빠름

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
	Manager->PossesLocalPlayer();
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

	// ⭐ 자동 카메라 관리 끄기 (이거 안 하면 1프레임 뒤에 다시 캐릭터 카메라로 돌아감)
	//PC->bAutoManageActiveCameraTarget = false;

	// 헬기 카메라 활성
	ActivateCamera();

	FViewTargetTransitionParams Params;
	Params.BlendTime = 0.5f;
	Params.BlendFunction = VTBlend_Cubic;

	PC->SetViewTarget(this, Params);
}
