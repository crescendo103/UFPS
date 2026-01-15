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

    
    // 디버그용이면
    GetCapsuleComponent()->SetHiddenInGame(false);
    GetCapsuleComponent()->SetVisibility(true);
    


    GrenadeCalComponent = CreateDefaultSubobject<UGrenadeThrowCalculateCompo>(TEXT("Calculate"));
    FVector HandLocation = GetMesh()->GetSocketLocation(GunSocket);    
    GrenadeCalComponent->SetStartPos(HandLocation);

    //minimap
    minimapCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("MinimapSpringArmComp"));
    minimapCameraBoom->SetupAttachment(RootComponent);
    minimapCameraBoom->SetWorldRotation(FRotator(-90.0f, 45.0f, 0.0f));

    minimapCameraBoom->TargetArmLength = 300;
    minimapCameraBoom->bUsePawnControlRotation = false;
    minimapCameraBoom->bInheritPitch = false;
    minimapCameraBoom->bInheritRoll = false;
    minimapCameraBoom->bInheritYaw = false;

    minimapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureMinimap"));
    minimapCapture->SetupAttachment(minimapCameraBoom);
    minimapCapture->ProjectionType = ECameraProjectionMode::Orthographic;
    minimapCapture->OrthoWidth = 800.0f;
    // 라이팅 최소화
    minimapCapture->ShowFlags.SetLighting(false);
    minimapCapture->ShowFlags.SetDynamicShadows(false);
    minimapCapture->ShowFlags.SetAmbientOcclusion(false);

    // 필요 없는 오브젝트 제거
    minimapCapture->ShowFlags.SetDecals(false);
    minimapCapture->ShowFlags.SetSkeletalMeshes(false);
    minimapCapture->ShowFlags.SetParticles(false);
    minimapCapture->ShowFlags.SetFog(false);
    minimapCapture->ShowFlags.SetAtmosphere(false);
    minimapCapture->ShowFlags.SetCloud(false);
    //minimapCapture->ShowOnlyComponents.Add(minimapSprite);

    static ConstructorHelpers::FObjectFinder<UCanvasRenderTarget2D> renderObj(TEXT("/Game/Luco/MiniMap/CRT_Minimap.CRT_Minimap"));
    if (renderObj.Succeeded())
    {
        minimapCapture->TextureTarget = renderObj.Object;
    }
    minimapSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("MinimapSprite"));
    minimapSprite->SetupAttachment(GetMesh());

    minimapCapture->ShowOnlyComponents.Add(minimapSprite);
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
    CurrentWeapon = EWeaponType::Rifle;
    CurrentHp = 100;
    MyServer = GetGameInstance()->GetSubsystem<UMyServer>();
    MyServer->SetBulletClass(BulletClass);
    MyServer->SetEnermyClass(EnermyClass);
    bulletId = 1;

    if (WeaponClass) // BP_K2C1이 할당되어 있으면
    {
        FTransform SocketTransform = GetMesh()->GetSocketTransform(GunSocket, RTS_World);

        FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정
        SpawnParams.Owner = this;//소유자를 캐릭터로
        SpawnParams.Instigator = GetInstigator();//이 무기를 스폰한 주체가 누군지
        Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SocketTransform, SpawnParams);
        Weapon->SetActorEnableCollision(false);//임시
        Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GunSocket);    
    
    }

    
    
   
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

    /*
    PC = Cast<APlayerController>(GetController());
    // 화면 중앙 좌표 계산
    int32 ViewportX;
    int32 ViewportY;
    PC->GetViewportSize(ViewportX, ViewportY);
    ScreenCenter.X = ViewportX * 0.5f;  // X 값 초기화
    ScreenCenter.Y = ViewportY * 0.5f;
    할당전..
    */
    if (IsLocallyControlled())
    {
        PC = Cast<APlayerController>(GetController());
        if (PC)
        {
            InitScreenCenter();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("BeginPlay: PC is null"));
        }
    }
    test = 0;
}

void AMyCharacter::OnRep_Controller()
{
    Super::OnRep_Controller();

    if (!IsLocallyControlled())
        return;

    PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    InitScreenCenter();
}

void AMyCharacter::InitScreenCenter()
{
    int32 ViewportX;
    int32 ViewportY;
    PC->GetViewportSize(ViewportX, ViewportY);
    ScreenCenter.X = ViewportX * 0.5f;  // X 값 초기화
    ScreenCenter.Y = ViewportY * 0.5f;
}

void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // 매 프레임 애니메이션 관련 변수 업데이트
    UpdateAnimationVariables();
    //UE_LOG(LogTemp, Warning, TEXT("Pawn: %s"), *GetName());

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
    /*
    if (CurrentSpeed > 1) {
        AimWidget->SetAim(true);
    }
    else {
        AimWidget->SetAim(false);
    }*/
    if (IsLocallyControlled() && AimWidget)
    {
        AimWidget->SetAim(CurrentSpeed > 1);
    }

    float Speed = GetCharacterMovement()->Velocity.Size();
    FVector MoveDir = GetVelocity().GetSafeNormal();//실제 이동 방향
    FVector ForwardDir = GetActorForwardVector();//바라보는방향
    FVector Location = GetCharacterMovement()->GetActorLocation();//위치

    FCharacterPacket man;
    man.Header.Type = (int32)EPacketType::Character;
    man.Header.Size = sizeof(FCharacterPacket);
    man.CharacterId = -1;//쓰레기값
    man.X = Location.X;//100.f;
    man.Y = Location.Y;//200.f;
    man.Z = Location.Z;//300.f;
    man.DirX = ForwardDir.X;//1.f;
    man.DirY = ForwardDir.Y;//0.f;
    man.DirZ = ForwardDir.Z;//0.f;
    man.Speed = Speed;
    man.Sendtime = FPlatformTime::Seconds();
    //Bullet.flag = false;
    
    if (MyServer) {
        MyServer->MoveClient(man);
    }
   
}

AWeaponBase* AMyCharacter::GetWeaponBase()
{
    return Weapon;
}

UAimWidget* AMyCharacter::GetPlayerUI()
{
    return AimWidget;
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
        if (FirstWeaponAction)
        {
            EnhancedInputComponent->BindAction(FirstWeaponAction, ETriggerEvent::Started, this, &AMyCharacter::ChangeFirstWeapon);
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

    if (!IsLocallyControlled())
        return;

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

    if (!IsLocallyControlled())
        return;


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
    if (!IsLocallyControlled())
        return;
    
    Super::Jump(); // ACharacter의 기본 점프 함수 호출
    bIsJumping = true;
    /*
    FVector StartPos = Weapon->GetGunStartLocation();
    FVector Dir = PC->GetControlRotation().Vector();
    FServerBullet Bullet;
    Bullet.BulletId = test;//쓰레기값
    Bullet.X = StartPos.X;//100.f;
    Bullet.Y = StartPos.Y;//200.f;
    Bullet.Z = StartPos.Z;//300.f;
    Bullet.DirX = Dir.X;//1.f;
    Bullet.DirY = Dir.Y;//0.f;
    Bullet.DirZ = Dir.Z;//0.f;
    Bullet.Speed = 1200.f;
    Bullet.Sendtime = FPlatformTime::Seconds();
    Bullet.flag = true;
    MyServer->Shotoccurred(Bullet);
    test++;
    */
}
void AMyCharacter::StopJumping()
{
    if (!IsLocallyControlled())
        return;

    Super::StopJumping(); // ACharacter의 기본 점프 정지 함수 호출
    bIsJumping = false;
}

void AMyCharacter::StartFire()
{
    if (!IsLocallyControlled())
        return;

    bIsFireing = true;
    
    if (CurrentWeapon == EWeaponType::Grenade)
    {
        FVector HandLocation = GetMesh()->GetSocketLocation(GunSocket);
        GrenadeCalComponent->SetStartPos(HandLocation);
        GrenadeCalComponent->SetbShow(true);
        return;
    }
    else if (CurrentWeapon == EWeaponType::Rifle) {
        if (!GetGameInstance()) {
            UE_LOG(LogTemp, Warning,
                TEXT("[Pawn:%s] GetGameInstance() is nullptr"),
                *GetName());
            return;
        }
        if (!MyServer) {
            UE_LOG(LogTemp, Warning,
                TEXT("[Pawn:%s] UMyServer Subsystem is nullptr"),
                *GetName());
            return;
        }
        UE_LOG(LogTemp, Warning,
            TEXT("SPACE OCCUERRED"));
        FVector StartPos = Weapon->GetGunStartLocation();
        FVector Dir = PC->GetControlRotation().Vector();
        FServerBullet Bullet;
        Bullet.Header.Type = (int32)EPacketType::Bullet;
        Bullet.Header.Size = sizeof(FServerBullet);        
        Bullet.BulletId = -1;//쓰레기값
        Bullet.X = StartPos.X;//100.f;
        Bullet.Y = StartPos.Y;//200.f;
        Bullet.Z = StartPos.Z;//300.f;
        Bullet.DirX = Dir.X;//1.f;
        Bullet.DirY = Dir.Y;//0.f;
        Bullet.DirZ = Dir.Z;//0.f;
        Bullet.Speed = 1200.f;
        Bullet.Sendtime = FPlatformTime::Seconds();
        Bullet.flag = false;
        MyServer->Shotoccurred(Bullet);
        //bulletId += 1;
    }
    
    /*
    if (WeaponComponent)
    {
        // 예: 1000 유닛 거리로 레이 발사
        BulletRayResult = WeaponComponent->RayBullet(Weapon->GetGunStartLocation(),Weapon->GetGunFoward(), 1000.f);
    }
    */
    
    /*싱글일때나 써먹을듯
    if (BulletClass)
    {
        FTransform SpawnTransform;
        SpawnTransform.SetLocation(Weapon->GetGunStartLocation());
        SpawnTransform.SetRotation(Weapon->GetGunFoward().ToOrientationQuat());

        FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정
        SpawnParams.Owner = this;//소유자를 캐릭터로
        SpawnParams.Instigator = GetInstigator();//이 무기를 스폰한 주체가 누군지
        SubItem = GetWorld()->SpawnActor<ABullet>(BulletClass, SpawnTransform, SpawnParams);
        ABullet* Bullet = Cast<ABullet>(SubItem);
        
        FVector start = Weapon->GetGunStartLocation();
        FVector end = (PC->GetControlRotation().Vector()) + start;
        FVector Direction = end - start;
        Bullet->ShootBullet(Direction);                        
    }
    */
}

void AMyCharacter::EndFire()
{
    if (!IsLocallyControlled())
        return;

    bIsFireing = false;

    if (CurrentWeapon == EWeaponType::Grenade) 
    {
        if (GrenadeClass)
        {
            FTransform SocketTransform = GetMesh()->GetSocketTransform(GunSocket, RTS_World);

            FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정
            SpawnParams.Owner = this;//소유자를 캐릭터로
            SpawnParams.Instigator = GetInstigator();//이 무기를 스폰한 주체가 누군지
            SubItem = GetWorld()->SpawnActor<AMyGrenade>(GrenadeClass, SocketTransform, SpawnParams);
            //SubItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GunSocket);
            AMyGrenade* Grenade = Cast<AMyGrenade>(SubItem);
            Grenade->Throw(PC->GetControlRotation().Vector().GetSafeNormal(), 1000);

        }


        GrenadeCalComponent->SetbShow(false);
    }
}

void AMyCharacter::Crouching()
{
    if (!IsLocallyControlled())
        return;
    bIsStanding = false;
}

void AMyCharacter::CrouchingEnd()
{
    if (!IsLocallyControlled())
        return;
    bIsStanding = true;
}

void AMyCharacter::CrouchMove(const FInputActionValue& Value)
{
    if (!IsLocallyControlled())
        return;
}

void AMyCharacter::ChangeLook()
{
    if (!IsLocallyControlled())
        return;
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

void AMyCharacter::ChangeFirstWeapon()
{
    if (!IsLocallyControlled())
        return;

    CurrentWeapon = EWeaponType::Rifle;

    


    //기존총 활성화
    Weapon->SetActorHiddenInGame(false);
    //Weapon->SetActorEnableCollision(true);
    Weapon->SetActorTickEnabled(true);
}

void AMyCharacter::ChangeSecondWeapon()
{
    if (!IsLocallyControlled())
        return;

    CurrentWeapon = EWeaponType::Grenade;
    
    

    //기존총 숨김처리
    Weapon->SetActorHiddenInGame(true);      // 화면에서 숨김
    //Weapon->SetActorEnableCollision(false); // 충돌 제거
    Weapon->SetActorTickEnabled(false);     // Tick 끄기 (선택)

    
}

void AMyCharacter::MouseWheel(const FInputActionValue& Value)
{
    if (!IsLocallyControlled())
        return;
    const float WheelValue = Value.Get<float>()*10.f;


    AMyGrenade* Grenade = Cast<AMyGrenade>(SubItem);
    Grenade->ThrowWheelVal(WheelValue);
}

float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    
    AimWidget->SetCurrentHp(DamageAmount);

    return DamageAmount;
}
/*
void AMyCharacter::spawnActor(FServerBulletPos pos)
{
    if (BulletClass)
    {
        FTransform SpawnTransform;
        FVector setloaction = FVector(pos.X, pos.Y, pos.Z);
        SpawnTransform.SetLocation(setloaction);
        //SpawnTransform.SetRotation(Weapon->GetGunFoward().ToOrientationQuat());


        FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정
        SpawnParams.Owner = this;//소유자를 캐릭터로
        SpawnParams.Instigator = GetInstigator();//이 무기를 스폰한 주체가 누군지
        SubItem = GetWorld()->SpawnActor<ABullet>(BulletClass, SpawnTransform, SpawnParams);

        
    }
}
*/