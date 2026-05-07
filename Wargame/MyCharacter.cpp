// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "EnhancedInputSubsystems.h" // 핵심 서브시스템 포함
#include "EnhancedInputComponent.h"   // EnhancedInputComponent를 위해 포함
#include "GameFramework/SpringArmComponent.h" // 카메라 컴포넌트 추가 시 필요
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"        // UPROPERTY로 선언한 컴포넌트의 헤더를 여기서 포함
#include "WeaponActor.h"
#include "EnemyBox.h"
#include "PawnManager.h"
#include "WeaponCompo.h"
#include "CustomPlayerController.h"
#include "FlashBang.h"



// Sets default values
AMyCharacter::AMyCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    GunSocket = TEXT("RightHandPinky4Socket");
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
    //CameraBoom->TargetArmLength = 300.0f; // 카메라와 캐릭터 사이의 거리
    CameraBoom->TargetArmLength = 0.0f;
    CameraBoom->SetRelativeLocation(FVector(42.f, 0.f, 62.f));
    bIsFirstPerson = true;
    CameraBoom->bUsePawnControlRotation = true; // 컨트롤러의 회전(Yaw)을 따라가도록 설정
    CameraBoom->bInheritPitch = true; // 컨트롤러의 피치(Pitch) 회전을 상속받도록 설정
    CameraBoom->bInheritYaw = true;   // 컨트롤러의 야(Yaw) 회전을 상속받도록 설정
    CameraBoom->bInheritRoll = true;  // 컨트롤러의 롤(Roll) 회전을 상속받도록 설정
    CameraBoom->bDoCollisionTest = true; // 카메라와 장애물 충돌 시 카메라가 당겨지도록 설정
    // 2. FollowCamera (UCameraComponent) 생성 및 부착
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // CameraBoom에 부착
    FollowCamera->bUsePawnControlRotation = false; // 카메라 자체는 컨트롤러 회전을 따르지 않음 (스프링암이 따르므로)

    CameraBoom->TargetArmLength = 0.0f;
    CameraBoom->SetRelativeLocation(FVector(42.f, 0.f, 62.f));
    bIsFirstPerson = true;

    // 디버그용이면
    GetCapsuleComponent()->SetHiddenInGame(false);
    GetCapsuleComponent()->SetVisibility(true);
    


    GrenadeCalComponent = CreateDefaultSubobject<UGrenadeThrowCalculateCompo>(TEXT("Calculate"));
    ///FVector HandLocation = GetMesh()->GetSocketLocation(GunSocket);    
    ///GrenadeCalComponent->SetStartPos(HandLocation);

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
    minimapCapture->bCaptureEveryFrame = true;
    minimapCapture->bCaptureOnMovement = true;
    
    
    
    minimapSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("MinimapSprite"));
    minimapSprite->SetupAttachment(RootComponent);
    //Actor의 Foward방향과 일치화
    minimapSprite->SetWorldRotation(FRotator::MakeFromEuler(FVector(-90.f, 0.f, 0.f)));
    minimapSprite->SetWorldScale3D(FVector(0.5f));
    minimapSprite->SetWorldLocation(FVector(0.f, 0.f, 300.f));
    //인게임에서 보이지 않게 하는 옵션(캡처에서만 보이게 하는)
    //minimapSprite->bVisibleInSceneCaptureOnly = true;

    ConstructorHelpers::FObjectFinder<UPaperSprite> FOBJ_PaperSprite(TEXT("/Script/Paper2D.PaperSprite'/Game/Mycontent/MiniMap/row_Sprite.row_Sprite'"));
    if (FOBJ_PaperSprite.Succeeded())
    {
        minimapSprite->SetSprite(FOBJ_PaperSprite.Object);
    }
    
    HitPostProcessComp =
        CreateDefaultSubobject<UCP_BloodEffect>(
            TEXT("HitPostProcessComponent")
        );
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("Pawn: %s Owner: %s"),
        *GetName(),
        GetOwner() ? *GetOwner()->GetName() : TEXT("None"));

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
    //bIsFirstPerson = false;
    CurrentWeapon = EWeaponType::Melee;
    CurrentHp = 100;
    MyServer = GetGameInstance()->GetSubsystem<UMyServer>();
    if (MyServer) {
        //MyServer->SetBulletClass(BulletClass);
        //MyServer->SetEnermyClass(EnermyClass);
       // MyServer->SetAiClass(AIClass);
        //MyServer->SetGrenadeClass(GrenadeClass);
        MyServer->SetRocalPlayer(this);
    }
    

    bulletId = 1;
    IsInventoryActive = false;
    bIsHealKit = false;
    bIsDeath = false;
    bIsHaveGun = false;
    

    // 오버랩 시작
    OnActorBeginOverlap.AddDynamic(this, &AMyCharacter::OnOverlapWithItem);

    // 오버랩 종료
    OnActorEndOverlap.AddDynamic(this, &AMyCharacter::OnOverlapEndWithItem);
    //1-30
    
    if (WeaponClass) // BP_K2C1이 할당되어 있으면
    {
        FTransform SocketTransform = GetMesh()->GetSocketTransform(GunSocket, RTS_World);

        FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정
        SpawnParams.Owner = this;//소유자를 캐릭터로
        SpawnParams.Instigator = GetInstigator();//이 무기를 스폰한 주체가 누군지
        Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SocketTransform, SpawnParams);
        Weapon->SetActorEnableCollision(false);//임시
        Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GunSocket); 

        Weapon->SetActorHiddenInGame(true);//test
    
    }
        
    
   
    
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

    GetWorldTimerManager().SetTimer(
        ExplosionTimerHandle,
        this,
        &AMyCharacter::CameraLineTrace, // ✅ 함수 포인터
        1.0f,
        true // 반복 호출
    );

    InventoryActive();

   

    
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();

    FTransform SpawnTM = GetMesh()->GetSocketTransform(
        TEXT("RightHandPinky4Socket"),
        RTS_World
    );

    VisualGrenade = GetWorld()->SpawnActor<AVisualGrenade>(
        VisualGrenadeClass,
        SpawnTM,
        SpawnParams
    );

    if (VisualGrenade)
    {
        VisualGrenade->AttachToComponent(
            GetMesh(),
            FAttachmentTransformRules::SnapToTargetNotIncludingScale,
            TEXT("RightHandPinky4Socket")
        );

        VisualGrenade->SetActorHiddenInGame(true);

        
    }

    if (IsLocallyControlled())
    {
        MinimapRT = NewObject<UTextureRenderTarget2D>(this);
        MinimapRT->InitAutoFormat(1024, 1024);

        minimapCapture->TextureTarget = MinimapRT;

        ACustomPlayerController* PsC = Cast<ACustomPlayerController>(GetController());
        if (PsC)
        {
            PsC->InitMinimap(MinimapRT); // 🔥 여기서 넘김
        }
    }
    else
    {
        minimapCapture->Deactivate();
    }


    //MyServer->SetBombClass(BombClass);
    //MyServer->SetBlueHoleClass(BlueHoleClass);
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

void AMyCharacter::CameraLineTrace()
{
    FVector Start = FollowCamera->GetComponentLocation();
    FVector End = Start + (FollowCamera->GetForwardVector() * 5000.f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_GameTraceChannel4,//ItemHighlight 채널
        Params
    );
    if (bHit)
    {
        //UE_LOG(LogTemp, Warning, TEXT("Hit Location: %s"), *Hit.ImpactPoint.ToString());
        AActor* HitActor = Hit.GetActor();
        
        if (HitActor && HitActor->Implements<UHighlightInterface>())
        {
            IHighlightInterface::Execute_ItemOutlineOn(HitActor);
            Items.Add(HitActor);
        }
    }
    else {
        for (AActor* Item : Items)
        {
            if (!Item) continue;

            IHighlightInterface::Execute_ItemOutlineOff(Item);
        }
        Items.Empty();
    }
    DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.f);
    if (bHit)
    {
        DrawDebugPoint(
            GetWorld(),
            Hit.ImpactPoint,
            10.f,
            FColor::Green,
            false,
            1.f
        );
    }
    
}

void AMyCharacter::InventoryActive()
{
    if (!GetWorld() || !GetWorld()->IsGameWorld())
        return;

    if (!IsLocallyControlled())
        return;

    if (!InventoryWidgetClass)
        return;

    if (InventoryWidget) {
        //todo..
        if (!IsInventoryActive)
        {
            // ===== 인벤토리 ON =====
            InventoryWidget->SetVisibility(ESlateVisibility::Visible);
            InventoryWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);//원리가 뭘까
            InventoryWidget->OnOffPZarts();
            FInputModeGameAndUI InputMode;
            InputMode.SetHideCursorDuringCapture(false);
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
        }
        else
        {
            // ===== 인벤토리 OFF (게임으로 복귀) =====
            InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
            InventoryWidget->OnOffPZarts();
            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = false;
        }
        IsInventoryActive = !IsInventoryActive;
        return;
    }

    APlayerController* pc = GetWorld()->GetFirstPlayerController();
    if (!pc)
        return;

    InventoryWidget = CreateWidget<UMyInventory>(pc, InventoryWidgetClass);
    if (InventoryWidget)
    {
        //InventoryWidget->AddToViewport();

        InventoryWidget->AddToViewport();
        InventoryWidget->SetVisibility(ESlateVisibility::Hidden);        
        InventoryWidget->SetMyOwner(this);
    }
}

void AMyCharacter::InterActionRayCast()
{
    if (!Controller)
    {
        UE_LOG(LogTemp, Warning, TEXT("[RayCast] Controller is NULL"));
        return;
    }

    FVector Start;
    FRotator Rot;
    Controller->GetPlayerViewPoint(Start, Rot);

    FVector End = Start + (Rot.Vector() * 1000.f);

    // 🔴 디버그 라인
    DrawDebugLine(
        GetWorld(),
        Start,
        End,
        FColor::Red,
        false,
        1.0f,
        0,
        2.0f
    );

    UE_LOG(LogTemp, Warning,
        TEXT("[RayCast] Start: %s  End: %s"),
        *Start.ToString(),
        *End.ToString()
    );

    FHitResult Hit;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.bTraceComplex = true; // 중요

    FCollisionObjectQueryParams ObjectParams;
    ObjectParams.AddObjectTypesToQuery(ECC_GameTraceChannel5); // Item

    bool bHit = GetWorld()->LineTraceSingleByObjectType(
        Hit,
        Start,
        End,
        ObjectParams,
        Params
    );

    if (bHit)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[RayCast HIT] Actor: %s | Component: %s | Channel: %d"),
            *Hit.GetActor()->GetName(),
            *Hit.GetComponent()->GetName(),
            Hit.GetComponent()->GetCollisionObjectType()
        );

        if (AWeaponActor* HitItem = Cast<AWeaponActor>(Hit.GetActor()))
        {
            UE_LOG(LogTemp, Warning, TEXT("[RayCast] WeaponActor confirmed"));
            InventoryWidget->AddHaveItemBox(HitItem->GetItemName(), HitItem, HitItem->ItemSpawnID);
            //InventoryWidget->ItemActorRigister(HitItem);
            FVector TargetLocation(100.f, 200.f, 300.f); // ← 원하는 좌표

            HitItem->SetActorLocation(
                TargetLocation,
                false,   // sweep ❌ (충돌 무시)
                nullptr,
                ETeleportType::TeleportPhysics
            );
            HitItem->sendItemPacket();

        }
        else if (AEnemyBox* enemyBox = Cast<AEnemyBox>(Hit.GetActor())) {
            for (AActor* Item : enemyBox->GetDropBoxItems())
            {
                AWeaponActor* enemyItem = Cast<AWeaponActor>(Item);
                InventoryWidget->AddFloorItemBox(enemyItem->GetItemName(), Item, enemyItem->ItemSpawnID);
            }
            
            InventoryActive();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[RayCast] Hit actor is NOT WeaponActor"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[RayCast MISS] Nothing hit"));
    }
}

void AMyCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    APlayerController* PCC = Cast<APlayerController>(NewController);
    if (!PCC) return;

    ULocalPlayer* LP = PCC->GetLocalPlayer();
    if (!LP) return;

    PM = LP->GetSubsystem<UPawnManager>();
    if (!PM) return;

    PM->RegisterPlayer(this);
    PM->SetHeliClass(HeliClass);
    PM->SetParachuteClass(ParachuteClass);

    // 🔥 추가
    UEnhancedInputLocalPlayerSubsystem* Subsystem =
        LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

    if (Subsystem && DefaultMappingContext)
    {
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
}

void AMyCharacter::PullDownHeli()
{
    if (!PM)
    {
        APlayerController* PlayerController = Cast<APlayerController>(GetController());
        if (!PlayerController) return;

        ULocalPlayer* LP = PlayerController->GetLocalPlayer();
        if (!LP) return;

        PM = LP->GetSubsystem<UPawnManager>();
        if (!PM) return;
    }

    PM->PossesLocalPlayer();
}

void AMyCharacter::SetParachuteActor(AActor* actor)
{
    Parachute = actor;
    bIsHanging = true;
}

void AMyCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);

    bIsHanging = false;

    if (!Parachute) {
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("Character Landed"));

    DetachParachute();
}

void AMyCharacter::DetachParachute()
{

    UE_LOG(LogTemp, Warning, TEXT("DetachParachute 활성화"));
    // 1️⃣ 컴포넌트 기준으로 Detach (Actor 말고!)
    if (USceneComponent* RootComp = Parachute->GetRootComponent())
    {
        RootComp->DetachFromComponent(
            FDetachmentTransformRules::KeepWorldTransform
        );
    }

    // 2️⃣ 실제 Mesh에 물리 활성화
    if (UPrimitiveComponent* PMesh =
        Cast<UPrimitiveComponent>(
            Parachute->GetComponentByClass(UPrimitiveComponent::StaticClass())
        ))
    {
        PMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
        PMesh->SetSimulatePhysics(true);
        PMesh->SetEnableGravity(true);
        PMesh->WakeAllRigidBodies();
    }

    // 3️⃣ 일정 시간 후 제거
    Parachute->SetLifeSpan(3.0f);

    // 4️⃣ 참조 정리
    Parachute = nullptr;
}

void AMyCharacter::SetPlayerID(int32 id)
{
    PlayerID = id;
}

int32 AMyCharacter::GetPlayerID()
{
    return PlayerID;
}

void AMyCharacter::OnDamaged()
{
    if (HitPostProcessComp)
    {
        HitPostProcessComp->TriggerHitEffect(1.0f);
    }
}

void AMyCharacter::OutZone()
{
    if (HitPostProcessComp)
    {
        HitPostProcessComp->TriggerZoneEffect(1.0f);
        TakeDamage(5, FDamageEvent(), nullptr, nullptr);
    }
}

void AMyCharacter::InZone()
{
    if (HitPostProcessComp)
    {
        HitPostProcessComp->TriggerZoneEffect(0.f);
        
    }
}

void AMyCharacter::GunCompoActive()
{
    bIsZooming = true;
    //AimWidget->SetVisibility(ESlateVisibility::Hidden);


    if (!CurrentGunWeapon) {
        UE_LOG(LogTemp, Warning, TEXT("CurrentWeapon is NULL"));
        return;
    }
    
    if (CurrentGunWeapon)
    {
        FollowCamera->AttachToComponent(
            CurrentGunWeapon->Mesh,
            FAttachmentTransformRules::SnapToTargetIncludingScale,
            TEXT("GunCompo")
        );
    }
    /*
    CurrentGunWeapon->GetMyGunCompo()->EnableScope(true);
    */
}

void AMyCharacter::GUnCompoDeActive()
{
    bIsZooming = false;
    //AimWidget->SetVisibility(ESlateVisibility::Visible);
    if (!CurrentGunWeapon) {
        UE_LOG(LogTemp, Warning, TEXT("CurrentWeapon is NULL"));
        return;
    }
    // 🔥 카메라를 SpringArm 기본 상태로 되돌림
    FollowCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    FollowCamera->AttachToComponent(CameraBoom, FAttachmentTransformRules::SnapToTargetIncludingScale);
    /*
    CurrentGunWeapon->GetMyGunCompo()->EnableScope(false);
    */
}

void AMyCharacter::Die(int32 CauserCharacterId)
{
    // 입력 막기
    APlayerController* PsdC = Cast<APlayerController>(GetController());
    if (PsdC)
    {
        DisableInput(PsdC);
    }

    // 이동 멈추기
    GetCharacterMovement()->DisableMovement();

    // 죽음 애니메이션 재생
    //if (DeathMontage)
    //{
    //    PlayAnimMontage(DeathMontage);
    //}

    

    FDeathPacket packet;
    packet.Header.Type = (int32)EPacketType::Death;
    packet.Header.Size = sizeof(FDeathPacket);
    packet.aiid = -1;
    packet.characterid = -1;
    {
        // 🔥 이름 안전하게 복사 (7글자 제한)
        FString InName = "client1";
        FString SafeName = InName.Left(7);
        FTCHARToUTF8 Convert(*SafeName);  // TCHAR → UTF8(char)
        strncpy(packet.Name, Convert.Get(), sizeof(packet.Name) - 1);
        packet.Name[sizeof(packet.Name) - 1] = '\0';
    }
    {
        FString InName = "client2";
        FString SafeName = InName.Left(7);
        FTCHARToUTF8 Convert(*SafeName);  // TCHAR → UTF8(char)
        strncpy(packet.CausedName, Convert.Get(), sizeof(packet.CausedName) - 1);
        packet.CausedName[sizeof(packet.CausedName) - 1] = '\0';
    }

    packet.causedCharacterId = CauserCharacterId;
    MyServer->MoveDeath(packet);
}

void AMyCharacter::PlayPunch()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    if (AnimInstance && PunchMontage)
    {
        if (!AnimInstance->Montage_IsPlaying(PunchMontage))
        {
            AnimInstance->Montage_Play(PunchMontage);
        }
    }
}

void AMyCharacter::SpawnPunchCollision()
{
    if (bPunchCollisionActive)
        return;

    bPunchCollisionActive = true;

    FVector Start = GetActorLocation();
    FVector End = Start + GetActorForwardVector() * 120.f;

    /*
    float LifeTime = 1.f;   // 스피어가 화면에 남아있는 시간

    DrawDebugSphere(
        GetWorld(),
        End,        // 스피어 위치
        30.f,       // 반지름
        16,         // 세그먼트 (원 해상도)
        FColor::Red,
        false,      // persistent
        LifeTime,   // 유지 시간
        0,
        2.f         // 선 두께
    );
    */
    //서버 패킷 작성

    UMyServer* MyServerr = GetGameInstance()->GetSubsystem<UMyServer>();
    
    FServerBullet Melee;
    Melee.Header.Type = (int32)EPacketType::Melee;
    Melee.Header.Size = sizeof(FServerBullet);
    Melee.BulletId = -2;
    Melee.CharacterId = -2;
    Melee.X = End.X;//100.f;
    Melee.Y = End.Y;//200.f;
    Melee.Z = End.Z;//300.f;
    Melee.DirX = End.X;//쓸모 없는값
    Melee.DirY = End.Y;//쓸모 없는값
    Melee.DirZ = End.Z;//쓸모 없는값
    Melee.Speed = 1200.f;
    Melee.Sendtime = FPlatformTime::Seconds();
    Melee.flag = false;
    MyServerr->Shotoccurred(Melee);
}

void AMyCharacter::EndPunchCollision()
{
    bPunchCollisionActive = false;
}

void AMyCharacter::AddViewPortGlobalMap()
{
    
    ACustomPlayerController* PsC = Cast<ACustomPlayerController>(GetController());
    if (PsC)
    {
        PsC->UpdateGlobalMap(bGlobalMap); // 🔥 여기서 넘김
    }

    bGlobalMap = !bGlobalMap;
}

AVisualGrenade* AMyCharacter::GetVisualGrenadePointer()
{
    return VisualGrenade;
}

void AMyCharacter::WeaponAttach(AActor* weapon, FName sockname)
{
    if (weapon)
    {
        CurrentGunWeapon = Cast<AWeaponActor>(weapon);

        CurrentGunWeapon->SetMyOwner(this);

        USkeletalMeshComponent* SkelMesh =
            weapon->FindComponentByClass<USkeletalMeshComponent>();
        USphereComponent* sphere = 
            weapon->FindComponentByClass<USphereComponent>();
        //sphere->SetGenerateOverlapEvents(false);
        if (SkelMesh)
        {
            SkelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            SkelMesh->SetGenerateOverlapEvents(false);
            SkelMesh->SetSimulatePhysics(false); // 필요 시
        }
        if (sphere)
        {
            //SkelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            sphere->SetGenerateOverlapEvents(false);
            //SkelMesh->SetSimulatePhysics(false); // 필요 시
        }
       
        weapon->SetActorLocation(FVector::ZeroVector);
        weapon->SetActorRotation(FRotator::ZeroRotator);

        weapon->AttachToComponent(
            GetMesh(),
            FAttachmentTransformRules::SnapToTargetNotIncludingScale,
            sockname
        );
    }

     
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
    if (IsLocallyControlled() && AimWidget)
    {
        AimWidget->SetAim(CurrentSpeed > 1);
    }*/
    
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
    man.IsJump = bIsJumping;
    man.IsFire = bIsFireing;
    man.IsDeath = bIsDeath;
    man.IsHeal = bIsHealKit;
    man.IsHaveGun = bIsHaveGun;

    if (MyServer) {
        MyServer->MoveClient(man);
    }
    /*
    SendTimer += DeltaTime;
    if (SendTimer >= 0.1f) // 20Hz
    {
        if (MyServer) {
            MyServer->MoveClient(man);
        }
        SendTimer = 0;
    }
    */
   
  
    
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

    UE_LOG(LogTemp, Warning, TEXT("🔥 SetupPlayerInputComponent 호출됨"));

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
        if (InventoryAction)
        {
            EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &AMyCharacter::InventoryActive);
        }
        if (InterAction)
        {
            EnhancedInputComponent->BindAction(InterAction, ETriggerEvent::Started, this, &AMyCharacter::InterActionRayCast);
        }
        if (TestAction)
        {
            EnhancedInputComponent->BindAction(TestAction, ETriggerEvent::Started, this, &AMyCharacter::PullDownHeli);
        }
        if (GunCompoAction)
        {
            EnhancedInputComponent->BindAction(GunCompoAction, ETriggerEvent::Started, this, &AMyCharacter::GunCompoActive);
            EnhancedInputComponent->BindAction(GunCompoAction, ETriggerEvent::Completed, this, &AMyCharacter::GUnCompoDeActive);
        }
        if (GlobalMapAction)
        {
            EnhancedInputComponent->BindAction(GlobalMapAction, ETriggerEvent::Started, this, &AMyCharacter::AddViewPortGlobalMap);
        }
    }

}


// 이동 함수 구현
void AMyCharacter::Move(const FInputActionValue& Value)
{
    //UE_LOG(LogTemp, Warning, TEXT("MOVE INPUT 들어옴"));
    if (!IsLocallyControlled())
        return;
    //UE_LOG(LogTemp, Warning, TEXT("MOVE2 INPUT 들어옴"));
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
    //InventoryWidget->AddFloorItemBox(0);
   
    OnDamaged();
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
    
    
    if (bIsFireing) return; // 이미 발사 중이면 무시
    UE_LOG(LogTemp, Warning, TEXT("bIsFireing true"));
    bIsFireing = true;
    

    if (CurrentWeapon == EWeaponType::HealKit) {
        TakeDamage(-20, FDamageEvent(), nullptr, nullptr);
        bIsHealKit = true;
    }    
    else if (CurrentWeapon == EWeaponType::Grenade)
    {
        FVector HandLocation = GetMesh()->GetSocketLocation(GunSocket);
        //GrenadeCalComponent->SetStartPos(HandLocation);아마 안써
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
            TEXT("EWeaponType::Rifle"));
        FVector StartPos = Weapon->GetGunStartLocation();
        FVector Dir = PC->GetControlRotation().Vector();
        FServerBullet Bullet;
        Bullet.Header.Type = (int32)EPacketType::Bullet;
        Bullet.Header.Size = sizeof(FServerBullet);        
        Bullet.BulletId = -1;//쓰레기값
        Bullet.CharacterId = -1;//쓰레기값
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
    else {
        PlayPunch();
    }
    
    
    if (CurrentGunWeapon) {
        FVector StartPos = Weapon->GetGunStartLocation();
        FVector Dir = PC->GetControlRotation().Vector();

        //CurrentGunWeapon->UseItem();//이펙트 스폰아이템이 처리하는게 좋아보이네 3 29
        CurrentGunWeapon->SpawnItem(StartPos, Dir);//나는 내가그림
    }
    bIsFireing = false;
}

void AMyCharacter::EndFire()
{
    if (!IsLocallyControlled())
        return;

    bIsFireing = false;
    bIsHealKit = false;
    UE_LOG(LogTemp, Warning, TEXT("bIsFireing false"));

    if (CurrentWeapon == EWeaponType::Grenade) 
    {
        //if (GrenadeClass)
        if(SubItemClass)
        {
            
            FTransform SocketTransform = GetMesh()->GetSocketTransform("RightHandPinky4Socket", RTS_World);

            FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정
            SpawnParams.Owner = this;//소유자를 캐릭터로
            SpawnParams.Instigator = GetInstigator();//이 무기를 스폰한 주체가 누군지
            SubItem = GetWorld()->SpawnActor<AMyGrenade>(SubItemClass, SocketTransform, SpawnParams);
           
            AMyGrenade* Grenade = Cast<AMyGrenade>(SubItem);
            
            // 1. 액터의 회전값 가져오기
            FRotator ThrowRot = GetActorRotation();

            // 2. 위 / 아래 각도 추가 (Pitch)
            ThrowRot.Pitch += VisualGrenade->AccumulateWheelVal;

            // 3. 회전 → 방향 벡터
            FVector Direction = ThrowRot.Vector().GetSafeNormal();

            // 4. 던지기
            Grenade->Throw(Direction, 1000.f);
            VisualGrenade->AccumulateWheelVal = 0;

            FGrenadePacket packet;
            packet.Header.Type = (int32)EPacketType::Grenade;
            packet.Header.Size = sizeof(FGrenadePacket);
            packet.GrenadeId = -2;//쓰레기값
            packet.CharacterId = -2;//trash
            packet.X = SocketTransform.GetLocation().X;
            packet.Y = SocketTransform.GetLocation().Y;
            packet.Z = SocketTransform.GetLocation().Z;
            packet.DirX = Direction.X;
            packet.DirY = Direction.Y;
            packet.DirZ = Direction.Z;
            packet.Power = 1000;

            MyServer->CreateGrenade(packet);
        }


        
        GrenadeCalComponent->SetbShow(false);
    }
}

void AMyCharacter::Crouching()
{

    if (!IsLocallyControlled())
        return;
    bIsStanding = false;

    
    //PM->SpawnHeli();
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
        CameraBoom->SetRelativeLocation(FVector(42.f, 0.f, 62.f));
        GEngine->AddOnScreenDebugMessage(
            -1,            // -1 = 새 줄 (BP Print String 기본 동작)
            2.0f,          // 화면에 표시될 시간
            FColor::Yellow,
            TEXT("1111")
        );

    }
    /*
    if (Parachute) {
        DetachParachute();
    }*/

}

void AMyCharacter::ChangeFirstWeapon()
{
    if (!IsLocallyControlled())
        return;

    CurrentWeapon = EWeaponType::Rifle;

    
    VisualGrenade->SetActorHiddenInGame(true);

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
    
    VisualGrenade->SetActorHiddenInGame(false);

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


    //AMyGrenade* Grenade = Cast<AMyGrenade>(SubItem);
    VisualGrenade->AddThrowAngle(WheelValue);
    
}

float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // 🔹 Instigator 이름
    FString InstigatorName = EventInstigator ? EventInstigator->GetName() : TEXT("None");

    // 🔹 Damage를 유발한 액터 이름
    FString CauserName = DamageCauser ? DamageCauser->GetName() : TEXT("None");

    UE_LOG(LogTemp, Warning,
        TEXT("TakeDamage called! Damage=%f, Instigator=%s, Causer=%s"),
        DamageAmount,
        *InstigatorName,
        *CauserName
    );

    // int32 전달값 꺼내기
    ABullet* Bullet = Cast<ABullet>(DamageCauser);
    int32 OwnerId = -2;
    if (Bullet)
    {
        OwnerId = Bullet->GetBulletOwner();
        UE_LOG(LogTemp, Warning, TEXT("Hit by characterid = %d"), OwnerId);
    }

    if (Cast<AFlashBang>(DamageCauser))
    {
        UE_LOG(LogTemp, Warning, TEXT("FlashGrenade Damage"));
        HitPostProcessComp->TriggerWhiteEffect(1.0f);
    }
    else {
        HitPostProcessComp->TriggerHitEffect(1.0f);
    }

    ACustomPlayerController* PppC = Cast<ACustomPlayerController>(GetController());

    if (PppC && PppC->HpBarWidget)
    {
        bIsDeath = PppC->HpBarWidget->SettHpBar(DamageAmount);
        if (bIsDeath) {            
            Die(OwnerId);
        }
    }

    return DamageAmount;
}


void AMyCharacter::OnOverlapWithItem(AActor* OverlappedActor, AActor* OtherActor)
{
    UE_LOG(LogTemp, Warning, TEXT("===== OnOverlapWithItem 호출 ====="));

    UE_LOG(LogTemp, Warning, TEXT("OverlappedActor: %s"),
        OverlappedActor ? *OverlappedActor->GetName() : TEXT("NULL"));

    UE_LOG(LogTemp, Warning, TEXT("OtherActor: %s"),
        OtherActor ? *OtherActor->GetName() : TEXT("NULL"));


    if (OtherActor && OtherActor->IsA(AWeaponActor::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("플레이어가 아이템 구체 안으로 들어왔습니다!"));

        AWeaponActor* item = Cast<AWeaponActor>(OtherActor);
        if (item)
        {
            InventoryWidget->AddFloorItemBox(item->GetItemName(), item, item->ItemSpawnID);
        }
    }
}

void AMyCharacter::OnOverlapEndWithItem(AActor* OverlappedActor, AActor* OtherActor)
{
    /*
    if (OtherActor && OtherActor->IsA(AWeaponActor::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("플레이어가 아이템 구체에서 나갔습니다!"));
        if (InventoryWidget->GetItemIndexFromActor(OtherActor) != INDEX_NONE) {
            UE_LOG(LogTemp, Warning, TEXT("아이템을 가지고있다"));
            return;
        }
        else {
            AWeaponActor* item = Cast<AWeaponActor>(OtherActor);
            if (item)
            {
                UE_LOG(LogTemp, Warning, TEXT("아이템을 획득하지는 않았다"));
                
                int32 SlotIndex = InventoryWidget->GetItemIndexFromActor(item);

                if (SlotIndex != INDEX_NONE)
                {
                    InventoryWidget->RemoveItemWidgetNoItemSpawn(SlotIndex);
                }
            }
        }        
    }
    */
    UE_LOG(LogTemp, Warning, TEXT("OnOverlapEndWithItem 호출"));

    if (!OtherActor || !OtherActor->IsA(AWeaponActor::StaticClass()))
        return;

    // 인벤토리에 있는 아이템이면 무시
    if (InventoryWidget->GetItemIndexFromActor(OtherActor) != INDEX_NONE)
        return;

    // FloorBox에서 제거
    InventoryWidget->RemoveFloorItem(OtherActor);
}