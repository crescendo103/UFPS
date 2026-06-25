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
#include "PawnVehicle.h"
#include "killAccountWidget.h"
#include "EndGameUIWidget.h"

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
    bIsFirstPerson = true;//1인칭 3인칭 구분용

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

    globalSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("GlobalmapSprite"));
    globalSprite->SetupAttachment(RootComponent);    

    ConstructorHelpers::FObjectFinder<UPaperSprite> FOBJ_PaperSprite(TEXT("/Script/Paper2D.PaperSprite'/Game/Mycontent/MiniMap/row_Sprite.row_Sprite'"));
    if (FOBJ_PaperSprite.Succeeded())
    {
        minimapSprite->SetSprite(FOBJ_PaperSprite.Object);
    }
    
    HitPostProcessComp = CreateDefaultSubobject<UCP_BloodEffect>(TEXT("HitPostProcessComponent"));

    MarkMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MarkMesh"));
    MarkMesh->SetupAttachment(GetMesh());

    CurrentWeapon = EWeaponType::Melee;
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
    bIsHanging = true;
    CurrentHp = 100;

    MyServer = GetGameInstance()->GetSubsystem<UMyServer>();
    if (MyServer) {        
        MyServer->SetRocalPlayer(this);
    }    

    bulletId = 1;//주먹데이터 채우는 용도
    IsInventoryActive = false;
    bIsHealKit = false;
    bIsDeath = false;
    isSubWeaponActive = false;
    MyOwner = -1;
    CurrentHitMeCharacterID = -2;

    //아이템에 가까이 가면 인벤토리에 보이게 하는 효과
    OnActorBeginOverlap.AddDynamic(this, &AMyCharacter::OnOverlapWithItem);    
    OnActorEndOverlap.AddDynamic(this, &AMyCharacter::OnOverlapEndWithItem);       
    
    if (IsLocallyControlled())
    {
        PC = Cast<APlayerController>(GetController());
        if (PC)
        {
            InitScreenCenter();// 조준점 화면 가운데로
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("BeginPlay: PC is null"));
        }
    }    

    if(IsLocallyControlled()) // ← 이 안으로 이동
    {
        PC = Cast<APlayerController>(GetController());
        if (PC)
        {
            InitScreenCenter();
        }

        // 타이머를 로컬 플레이어에서만 등록
        GetWorldTimerManager().SetTimer(
            ExplosionTimerHandle,
            this,
            &AMyCharacter::CameraLineTrace,
            0.3f,
            true
        );

        UE_LOG(LogTemp, Warning, TEXT("Timer registered! IsValid: %d"), ExplosionTimerHandle.IsValid());
    }

    InventoryActive();//인벤토리 위젯 생성        

    if (IsLocallyControlled())
    {
        MinimapRT = NewObject<UTextureRenderTarget2D>(this);
        MinimapRT->InitAutoFormat(1024, 1024);
        minimapCapture->TextureTarget = MinimapRT;
        ACustomPlayerController* PCCustom = Cast<ACustomPlayerController>(GetController());
        if (PCCustom)
        {
            PCCustom->InitMinimap(MinimapRT);
        }
    }
    else
    {
        minimapCapture->Deactivate();
    }
}
/*
void AMyCharacter::OnRep_Controller()
{
    Super::OnRep_Controller();

    if (!IsLocallyControlled())
        return;

    PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    InitScreenCenter();
}*/



void AMyCharacter::InitScreenCenter()
{
    int32 ViewportX;
    int32 ViewportY;
    PC->GetViewportSize(ViewportX, ViewportY);
    ScreenCenter.X = ViewportX * 0.5f;  
    ScreenCenter.Y = ViewportY * 0.5f;
}

void AMyCharacter::CameraLineTrace()
{
    UE_LOG(LogTemp, Warning, TEXT("[RayCast]"));

    FVector Start = FollowCamera->GetComponentLocation();
    FVector End = Start + (FollowCamera->GetForwardVector() * 5000.f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_Visibility,//ItemHighlight 채널
        Params
    );
    if (bHit)
    {
        //UE_LOG(LogTemp, Warning, TEXT("Hit Location: %s"), *Hit.ImpactPoint.ToString());
        UE_LOG(LogTemp, Warning, TEXT("Hit: %s / Implements: %d"),
        *Hit.GetActor()->GetName(),
        Hit.GetActor()->Implements<UHighlightInterface>());
        AActor* HitActor = Hit.GetActor();
        
        if (HitActor && HitActor->Implements<UHighlightInterface>())
        {
            IHighlightInterface::Execute_ItemOutlineOn(HitActor);
            Items.Add(HitActor);
        }
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("No Hit"));

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
    int32 a = 0;
}

void AMyCharacter::ClearSubWeapon()
{
    isSubWeaponActive = false;
    SubItemData = FSubItemData(); // SubItemClass = nullptr, Id = 0

    UE_LOG(LogTemp, Warning, TEXT("ClearSubWeapon called"));
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
            InventoryWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
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
    else {
        APlayerController* pc = GetWorld()->GetFirstPlayerController();
        if (!pc)
            return;

        InventoryWidget = CreateWidget<UMyInventory>(pc, InventoryWidgetClass);
        if (InventoryWidget)
        {            
            InventoryWidget->AddToViewport();
            InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
            InventoryWidget->SetMyOwner(this);
        }
    }    
}

void AMyCharacter::InterActionRayCast()
{
    if (!Controller)
    {
        UE_LOG(LogTemp, Warning, TEXT("[RayCast] Controller is NULL"));
        return;
    }

    if (CurrentVehicle)
    {
        CurrentVehicle
            ->EnterVehicle(this);
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
    ObjectParams.AddObjectTypesToQuery(ECC_GameTraceChannel4); // Item

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
        else if (AEnemyBox* enemyBox = Cast<AEnemyBox>(Hit.GetActor())) {//미완성
            for (AActor* Item : enemyBox->GetDropBoxItems())
            {
                AWeaponActor* enemyItem = Cast<AWeaponActor>(Item);
                InventoryWidget->AddFloorItemBox(enemyItem->GetItemName(), Item, enemyItem->ItemSpawnID);
            }
            
            InventoryActive();
        }//미완성
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
    UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

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
/*
void AMyCharacter::SetPlayerID(int32 id)
{
    PlayerID = id;
}*/
/*
int32 AMyCharacter::GetPlayerID()
{
    return PlayerID;
}*/

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

void AMyCharacter::AimActive()
{
    bIsZooming = true;
    if (isSubWeaponActive && SubItemData.SubItemClass) {
        GrenadeCalComponent->SetbShow(true);        
        return;
    }            
    
    if (!CurrentGunWeapon) {
        UE_LOG(LogTemp, Warning, TEXT("CurrentWeapon is NULL"));
        return;
    }    

    if (CurrentWeapon != EWeaponType::Melee &&CurrentGunWeapon)
    {
        FollowCamera->AttachToComponent(
            CurrentGunWeapon->Mesh,
            FAttachmentTransformRules::SnapToTargetIncludingScale,
            TEXT("GunCompo")
        );
    }    
}

void AMyCharacter::AimDeActive()
{
    bIsZooming = false;

    // 보조무기 조준 중이었다면 확실하게 끄기
    if (isSubWeaponActive && SubItemData.SubItemClass) {
        if (GrenadeCalComponent)
        {
            GrenadeCalComponent->MouseVal = 0.f; // 휠 값도 안전하게 리셋
            GrenadeCalComponent->SetbShow(false);
        }
        return;
    }

    // 혹시나 위 조건문이 타이밍 때문에 패스되었더라도, 
    // 혹시 수류탄 스플라인이 켜져 있다면 여기서 무조건 꺼줍니다.
    if (GrenadeCalComponent && GrenadeCalComponent->bShow)
    {
        GrenadeCalComponent->MouseVal = 0.f;
        GrenadeCalComponent->SetbShow(false);
    }
    
    // 카메라를 SpringArm 기본 상태로 되돌림
    FollowCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    FollowCamera->AttachToComponent(CameraBoom, FAttachmentTransformRules::SnapToTargetIncludingScale);    
}

void AMyCharacter::Die(int32 CauserCharacterId)
{

    if (bIsDeadFinal)
        return;

    bIsDeadFinal = true;    

    UWorld* World = GetGameInstance()->GetWorld();
    if (!World) return;

    ACustomPlayerController* controller =
        Cast<ACustomPlayerController>(UGameplayStatics::GetPlayerController(World, 0));

    if (!controller) return;

    DisableInput(controller);
    
    // ===== 마우스 커서 보이게 + UI 입력 받게 설정 =====
    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    controller->SetInputMode(InputMode);
    controller->bShowMouseCursor = true;
    // ================================================

    controller->HiddenInformationText();
    UEndGameUIWidget* EndGameUI = controller->GetUEndGameUIWidget();
    UkillAccountWidget* Widget = controller->GetDeathWidget();

    if (EndGameUI && Widget)
    {
        GetWorld()->GetTimerManager().SetTimer(
            WaitKillerNameHandle,
            [this, EndGameUI, Widget, controller]()
            {
                if (!Widget)
                    return;

                if (!Widget->KillerName.IsEmpty())
                {
                    EndGameUI->UpdateEndGameUI(
                        TEXT("Lose"),
                        Widget->KillerName
                    );

                    controller->ShowEndGameUI();
                    EndGameUI->PlaySequence();

                    GetWorld()->GetTimerManager().ClearTimer(
                        WaitKillerNameHandle
                    );
                }
            },
            0.1f,
            true
        );
    }

    // 이동 완전 정지
    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->DisableMovement();
    GetCharacterMovement()->SetMovementMode(MOVE_None);    

    FDeathPacket packet;
    packet.Header.Type = (int32)EPacketType::Death;
    packet.Header.Size = sizeof(FDeathPacket);
    packet.aiid = -1;
    packet.characterid = -1;
    {
        // 이름 안전하게 복사 (7글자 제한)
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

void AMyCharacter::SetSubItemData(FSubItemData subData)
{
    SubItemData = subData;
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

    
    //서버 패킷 작성
    MyServer = GetGameInstance()->GetSubsystem<UMyServer>();
    
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
    MyServer->Shotoccurred(Melee);
}

void AMyCharacter::EndPunchCollision()
{
    bPunchCollisionActive = false;
}

void AMyCharacter::AddViewPortGlobalMap()
{
    
    ACustomPlayerController* CPC = Cast<ACustomPlayerController>(GetController());
    if (CPC)
    {
        CPC->UpdateGlobalMap(bGlobalMap); // 여기서 넘김
    }
    bGlobalMap = !bGlobalMap;
}

void AMyCharacter::WeaponAttach(AActor* weapon, FName sockname)
{
    if (weapon)
    {
        CurrentGunWeapon = Cast<AWeaponActor>(weapon);
        CurrentGunWeapon->SetMyOwner(this);

        USkeletalMeshComponent* SkelMesh = weapon->FindComponentByClass<USkeletalMeshComponent>();
        USphereComponent* sphere = weapon->FindComponentByClass<USphereComponent>();
        
        if (SkelMesh)
        {
            SkelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            SkelMesh->SetGenerateOverlapEvents(false);
            SkelMesh->SetSimulatePhysics(false); // 필요 시
        }
        if (sphere)
        {            
            sphere->SetGenerateOverlapEvents(false);            
        }       
        weapon->SetActorLocation(FVector::ZeroVector);
        weapon->SetActorRotation(FRotator::ZeroRotator);
        weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, sockname);
    }     
}

void AMyCharacter::setCurrentWeapon(EWeaponType type)
{
    CurrentWeapon = type;
    UE_LOG(LogTemp, Warning, TEXT("CurrentWeapon : %d"), (int32)CurrentWeapon);
}

void AMyCharacter::resetCurrentGunWeapon()
{
    CurrentGunWeapon = nullptr;
}

void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // 매 프레임 애니메이션 관련 변수 업데이트
    UpdateAnimationVariables();      
    
    //캐릭터 위치 방향을 서버로
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
    if (isSubWeaponActive && SubItemData.SubItemClass) {
        man.WeaponType = EWeaponType::Grenade;
    }
    else {
        man.WeaponType = CurrentWeapon;
    }    
    man.AimActive = bIsZooming;
    if (MyServer) {
        MyServer->MoveClient(man);
    }
    /*
    if (MarkMesh)
    {
        MarkMesh->MarkRenderStateDirty();
    }*/    
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
        if (AimAction)
        {
            EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AMyCharacter::AimActive);
            EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AMyCharacter::AimDeActive);
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
    
    if (isSubWeaponActive)
        return;
    
    
    if (bIsFireing) return; // 이미 발사 중이면 무시
    UE_LOG(LogTemp, Warning, TEXT("bIsFireing true"));
    bIsFireing = true;
    

    if (CurrentWeapon == EWeaponType::HealKit) {
        TakeDamage(-20, FDamageEvent(), nullptr, nullptr);
        bIsHealKit = true;
        return;
    }    
    else if (CurrentWeapon == EWeaponType::Grenade)
    {
        FVector HandLocation = GetMesh()->GetSocketLocation(GunSocket);
        return;
    }
    else if (CurrentWeapon == EWeaponType::Rifle) {     
        //if (CurrentGunWeapon) 아래 if문에서 패킷 보내는걸로
        /*
        FVector StartPos = CurrentGunWeapon->GetGunStartLocation();
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
        MyServer->Shotoccurred(Bullet); */   
    }
    else {
        PlayPunch();
    }
            
    if (CurrentGunWeapon)
    {
        APlayerController* CPC = Cast<APlayerController>(GetController());
        if (!CPC) return;

        // 카메라 위치와 방향 가져오기
        FVector CameraLocation;
        FRotator CameraRotation;
        CPC->GetPlayerViewPoint(CameraLocation, CameraRotation);

        int32 ViewportX, ViewportY;
        CPC->GetViewportSize(ViewportX, ViewportY);

        FVector WorldPos, WorldDir;
        CPC->DeprojectScreenPositionToWorld(
            ViewportX * 0.5f,
            ViewportY * 0.5f,
            WorldPos,
            WorldDir
        );

        // 시작지점 = 카메라 위치, 방향 = 화면 중앙
        CurrentGunWeapon->SpawnItem(CameraLocation, WorldDir);
    }

}

void AMyCharacter::EndFire()
{
    if (!IsLocallyControlled())
        return;

    bIsFireing = false;
    bIsHealKit = false;
    UE_LOG(LogTemp, Warning, TEXT("bIsFireing false"));

    if (isSubWeaponActive)
    {                
        if(SubItemData.SubItemClass && SubItemData.Id!=0)//수류탄은 여기서 발사
        {            
            FTransform SocketTransform = GetMesh()->GetSocketTransform("RightHandPinky4Socket", RTS_World);
            FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정
            SpawnParams.Owner = this;//소유자를 캐릭터로
            SpawnParams.Instigator = GetInstigator();//이 무기를 스폰한 주체가 누군지
            SubItem = GetWorld()->SpawnActor<AMyGrenade>(SubItemData.SubItemClass, SocketTransform, SpawnParams);
            //SubItem = GetWorld()->SpawnActor<AMyGrenade>(GrenadeClass, SocketTransform, SpawnParams);
           
            AMyGrenade* Grenade = Cast<AMyGrenade>(SubItem);
            FVector Direction = FVector::ForwardVector;
            if (GrenadeCalComponent)
            {
                Direction = GrenadeCalComponent->GetFinalThrowDirection();
            }
            //던지기
            if (Grenade)
            {
                Grenade->Throw(Direction, 1000.f);
            }            

            FGrenadePacket packet;
            packet.Header.Type = (int32)EPacketType::Grenade;
            packet.Header.Size = sizeof(FGrenadePacket);
            packet.GrenadeId = SubItemData.Id;//수류탄 ID 아마 안쓸듯
            packet.CharacterId = -2;//trash
            packet.X = SocketTransform.GetLocation().X;
            packet.Y = SocketTransform.GetLocation().Y;
            packet.Z = SocketTransform.GetLocation().Z;
            packet.DirX = Direction.X;
            packet.DirY = Direction.Y;
            packet.DirZ = Direction.Z;
            packet.Power = 1000;
            MyServer->CreateGrenade(packet);

            // 조준이 끝났으므로 휠 누적값 초기화 및 예측선 끄기
            if (GrenadeCalComponent)
            {
                GrenadeCalComponent->MouseVal = 0.f;
                GrenadeCalComponent->SetbShow(false);
            }
        }            
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
    }
    else {//3->1인칭
        bIsFirstPerson = true;
        CameraBoom->TargetArmLength = 0.0f;
        CameraBoom->SetRelativeLocation(FVector(42.f, 0.f, 62.f));        
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

    isSubWeaponActive = false;        
}

void AMyCharacter::ChangeSecondWeapon()
{
    if (!IsLocallyControlled())
        return;

    isSubWeaponActive = true;        
}

void AMyCharacter::MouseWheel(const FInputActionValue& Value)
{
    if (!IsLocallyControlled())
        return;
    const float WheelValue = Value.Get<float>()*2.0f;

    // 계산 컴포넌트에 마우스 변위값을 누적시킵니다.
    if (GrenadeCalComponent)
    {
        GrenadeCalComponent->SetMouseVal(WheelValue);
    }         
}

float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDeadFinal)
        return 0;

    Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);    

    int32 OwnerId = -2;
    if (ABullet* Bullet = Cast<ABullet>(DamageCauser))
    {
        OwnerId = Bullet->GetOwnerID();
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Hit by Bullet CharacterId = %d"),
            OwnerId
        );
    }
    else if (AMyGrenade* Grenade = Cast<AMyGrenade>(DamageCauser))
    {
        OwnerId = Grenade->GetOwnerID();
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Hit by Grenade CharacterId = %d"),
            OwnerId
        );
    }
    else
    {
        OwnerId = CurrentHitMeCharacterID;
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Hit by Melee CharacterId = %d"),
            OwnerId
        );
    }

    if (Cast<AFlashBang>(DamageCauser))
    {
        UE_LOG(LogTemp, Warning, TEXT("FlashGrenade Damage"));
        HitPostProcessComp->TriggerWhiteEffect(1.0f);
    }
    else {
        HitPostProcessComp->TriggerHitEffect(1.0f);
    }

    ACustomPlayerController* CPC = Cast<ACustomPlayerController>(GetController());
    if (CPC && CPC->HpBarWidget)
    {
        bIsDeath = CPC->HpBarWidget->SettHpBar(DamageAmount);
        if (bIsDeath) {            
            Die(OwnerId);
        }
    }
    return DamageAmount;
}

//인벤토리에 주변 아이템 보이게하기
void AMyCharacter::OnOverlapWithItem(AActor* OverlappedActor, AActor* OtherActor)
{    
    if (OtherActor && OtherActor->IsA(AWeaponActor::StaticClass()))
    {        
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