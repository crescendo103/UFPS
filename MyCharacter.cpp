// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "EnhancedInputSubsystems.h" // 핵심 서브시스템 포함
#include "EnhancedInputComponent.h"   // EnhancedInputComponent를 위해 포함
#include "GameFramework/SpringArmComponent.h" // 카메라 컴포넌트 추가 시 필요
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"        // UPROPERTY로 선언한 컴포넌트의 헤더를 여기서 포함


// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    GunSocket = TEXT("RightHandSocket");
    WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));

    GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향으로 회전
    bUseControllerRotationYaw = false; // 컨트롤러 회전 사용 안 함 (캐릭터가 직접 회전하도록)


    //카메라 관련 설정 
    // 캡슐 컴포넌트 (ACharacter의 RootComponent)가 YAW 회전을 따르도록 설정
    // 이렇게 해야 카메라 회전과 캐릭터 회전이 일치합니다.
    GetCharacterMovement()->bUseControllerDesiredRotation = true;
    GetCharacterMovement()->bOrientRotationToMovement = false;
    // 1. CameraBoom (USpringArmComponent) 생성 및 부착
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent); // 캐릭터의 루트 컴포넌트 (CapsuleComponent)에 부착
    CameraBoom->TargetArmLength = 300.0f; // 카메라와 캐릭터 사이의 거리
    CameraBoom->bUsePawnControlRotation = true; // 컨트롤러의 회전(Yaw)을 따라가도록 설정
    CameraBoom->bInheritPitch = true; // 컨트롤러의 피치(Pitch) 회전을 상속받도록 설정
    CameraBoom->bInheritYaw = true;   // 컨트롤러의 야(Yaw) 회전을 상속받도록 설정
    CameraBoom->bInheritRoll = true;  // 컨트롤러의 롤(Roll) 회전을 상속받도록 설정
    CameraBoom->bDoCollisionTest = true; // 카메라와 장애물 충돌 시 카메라가 당겨지도록 설정
    // 2. FollowCamera (UCameraComponent) 생성 및 부착
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // CameraBoom에 부착
    FollowCamera->bUsePawnControlRotation = false; // 카메라 자체는 컨트롤러 회전을 따르지 않음 (스프링암이 따르므로)

    /*
    //충돌 캡슐 설정
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
    SphereComponent->SetSphereRadius(70.f);
    // 캐릭터 Root (Capsule)에 붙인다
    SphereComponent->SetupAttachment(GetCapsuleComponent());
    */
    // 디버그용이면
    GetCapsuleComponent()->SetHiddenInGame(false);
    GetCapsuleComponent()->SetVisibility(true);

    
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
    // PlayerController를 가져와서 Enhanced Input Subsystem에 접근
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            // DefaultMappingContext가 유효하다면 추가
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0); // 0은 우선순위 (높을수록 우선)
            }
        }
    }

    bIsStanding = true;
    bIsInAir = false;
    bIsFirstPerson = false;

    CurrentHp = 100;

    if (WeaponClass) // BP_K2C1이 할당되어 있으면
    {
        FTransform SocketTransform = GetMesh()->GetSocketTransform(GunSocket, RTS_World);

        FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정
        SpawnParams.Owner = this;//소유자를 캐릭터로
        SpawnParams.Instigator = GetInstigator();//이 무기를 스폰한 주체가 누군지
        Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SocketTransform, SpawnParams);
        Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GunSocket);    
    
    }

    

    /*
    // 메쉬/캡슐 트랜스폼이 확정된 이후
    SphereComponent->SetRelativeLocation(FVector(0.f, 0.f, 30.f));
    */
    if (!AimWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("AimWidgetClass is NULL"));
    }
    

    if (AimWidgetClass)
    {
        AimWidget = CreateWidget<UAimWidget>(GetWorld()->GetFirstPlayerController(), AimWidgetClass);

        if (AimWidget)
        {
            AimWidget->AddToViewport();
            UE_LOG(LogTemp, Warning, TEXT("AimWidget Added To Viewport"));
            UE_LOG(LogTemp, Warning, TEXT("InViewport: %d"), AimWidget->IsInViewport());

        }
    }

}

void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // 매 프레임 애니메이션 관련 변수 업데이트
    UpdateAnimationVariables();
    UE_LOG(LogTemp, Warning, TEXT("Pawn: %s"), *GetName());

    if (BulletRayResult.GetActor() != nullptr) {
        /*
        GEngine->AddOnScreenDebugMessage(
            -1,            // Key (-1 = 새 메시지)
            0.5f,          // 화면에 표시되는 시간 (초)
            FColor::Red,   // 색상
            TEXT("Hit!")
        );*/

        // 맞은 지점에 구 표시
        DrawDebugSphere(
            GetWorld(),
            BulletRayResult.ImpactPoint,
            8.f,
            12,
            FColor::Red,
            false,
            0.2f
        );
    }
    /*
    DrawDebugCapsule(
        GetWorld(),
        GetActorLocation(),
        GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
        GetCapsuleComponent()->GetScaledCapsuleRadius(),
        GetActorQuat(),
        FColor::Green,
        false,
        1.0f,
        0,
        2.f
    );
    */
    if (CurrentSpeed > 1) {
        AimWidget->SetAim(true);
    }
    else {
        AimWidget->SetAim(false);
    }
}

AWeaponBase* AMyCharacter::GetWeaponBase()
{
    return Weapon;
}

void AMyCharacter::UpdateAnimationVariables()
{
    // 현재 이동 속도 계산 (XY 평면 속도)
    CurrentSpeed = GetCharacterMovement()->Velocity.Size2D();
    // 공중에 떠 있는지 여부
    bIsInAir = GetCharacterMovement()->IsFalling();
    // 공격 상태는 추후 공격 시스템 구현 시 설정 (예시)
    // bIsAttacking = ...; 
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    // EnhancedInputComponent로 캐스팅
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Move Action 바인딩
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
        }
        // Look Action 바인딩
        if (LookAction)
        {
            EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
        }
        // Jump Action 바인딩
        if (JumpAction)
        {
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyCharacter::Jump);
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyCharacter::StopJumping);
        }
        if (CrouchMoveAction)
        {
            EnhancedInputComponent->BindAction(CrouchMoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::CrouchMove);
        }
        if (CrouchAction)
        {
            EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AMyCharacter::Crouching);
            EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AMyCharacter::CrouchingEnd);
        }
        if (FireAction)
        {
            EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AMyCharacter::StartFire);
            //EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AMyCharacter::StartFire);
            EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AMyCharacter::EndFire);
        }
        if (ChangeLookAction)
        {
            EnhancedInputComponent->BindAction(ChangeLookAction, ETriggerEvent::Started, this, &AMyCharacter::ChangeLook);            
        }
        if (SecondWeaponAction)
        {
            EnhancedInputComponent->BindAction(SecondWeaponAction, ETriggerEvent::Started, this, &AMyCharacter::ChangeSecondWeapon);

        }
        if (MouseWheelAction)
        {
            EnhancedInputComponent->BindAction(MouseWheelAction, ETriggerEvent::Triggered, this, &AMyCharacter::MouseWheel);
        }
    }

}


// 이동 함수 구현
void AMyCharacter::Move(const FInputActionValue& Value)
{
    // 입력 값 (Vector2D)을 가져옵니다.
    FVector2D MovementVector = Value.Get<FVector2D>();
    if (Controller != nullptr)
    {
        // 컨트롤러의 YAW 회전을 기준으로 앞/뒤 방향을 가져옵니다.
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        // 앞 방향 (Forward Vector)
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(ForwardDirection, MovementVector.Y);
        // 오른쪽 방향 (Right Vector)
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}
// 시점 변경 함수 구현
void AMyCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    if (Controller != nullptr)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}
// 점프 함수 구현
void AMyCharacter::Jump()
{
    Super::Jump(); // ACharacter의 기본 점프 함수 호출
    bIsJumping = true;
}
void AMyCharacter::StopJumping()
{
    Super::StopJumping(); // ACharacter의 기본 점프 정지 함수 호출
    bIsJumping = false;
}

void AMyCharacter::StartFire()
{
    bIsFireing = true;
    
    if (CurrentWeapon == EWeaponType::Grenade) {
        AMyGrenade* Grenade = Cast<AMyGrenade>(SubItem);
        //Grenade->SetbShow(true);
        Grenade->ThrowGrenade();
        return;
    }
    
    if (WeaponComponent)
    {
        // 예: 1000 유닛 거리로 레이 발사
        BulletRayResult = WeaponComponent->RayBullet(Weapon->GetGunStartLocation(),Weapon->GetGunFoward(), 1000.f);
    }

}

void AMyCharacter::EndFire()
{
    bIsFireing = false;
}

void AMyCharacter::Crouching()
{
    bIsStanding = false;
}

void AMyCharacter::CrouchingEnd()
{
    bIsStanding = true;
}

void AMyCharacter::CrouchMove(const FInputActionValue& Value)
{

}

void AMyCharacter::ChangeLook()
{
    
    //1->3인칭
    if (bIsFirstPerson) {
        bIsFirstPerson = false;
        CameraBoom->TargetArmLength = 500.0f;
        CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
        GEngine->AddOnScreenDebugMessage(
            -1,            // -1 = 새 줄 (BP Print String 기본 동작)
            2.0f,          // 화면에 표시될 시간
            FColor::Yellow,
            TEXT("3333")
        );

    }
    else {//3->1인칭
        bIsFirstPerson = true;
        CameraBoom->TargetArmLength = 0.0f;
        CameraBoom->SetRelativeLocation(FVector(42.f, 0.f, 50.f));
        GEngine->AddOnScreenDebugMessage(
            -1,            // -1 = 새 줄 (BP Print String 기본 동작)
            2.0f,          // 화면에 표시될 시간
            FColor::Yellow,
            TEXT("1111")
        );

    }
    

}

void AMyCharacter::ChangeSecondWeapon()
{
    CurrentWeapon = EWeaponType::Grenade;
    
    if (GrenadeClass) // BP_K2C1이 할당되어 있으면
    {
        FTransform SocketTransform = GetMesh()->GetSocketTransform(GunSocket, RTS_World);

        FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정
        SpawnParams.Owner = this;//소유자를 캐릭터로
        SpawnParams.Instigator = GetInstigator();//이 무기를 스폰한 주체가 누군지
        SubItem = GetWorld()->SpawnActor<AMyGrenade>(GrenadeClass, SocketTransform, SpawnParams);
        SubItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GunSocket);
        
        GEngine->AddOnScreenDebugMessage(
            -1,            // -1 = 새 줄 (BP Print String 기본 동작)
            2.0f,          // 화면에 표시될 시간
            FColor::Red,
            TEXT("AttachGrenage")
        );
        AMyGrenade* Grenade = Cast<AMyGrenade>(SubItem);
        Grenade->AttachToComponent(
            GetMesh(),
            FAttachmentTransformRules::SnapToTargetNotIncludingScale,
            GunSocket // 손 소켓
        );

    }
}

void AMyCharacter::MouseWheel(const FInputActionValue& Value)
{
    const float WheelValue = Value.Get<float>()*10.f;


    AMyGrenade* Grenade = Cast<AMyGrenade>(SubItem);
    Grenade->ThrowWheelVal(WheelValue);
}

float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    CurrentHp -= DamageAmount;

    if (CurrentHp < 0) {
        GEngine->AddOnScreenDebugMessage(
            -1,            // -1 = 새 줄 (BP Print String 기본 동작)
            2.0f,          // 화면에 표시될 시간
            FColor::Red,
            TEXT("Dead")
        );
    }

    return DamageAmount;
}