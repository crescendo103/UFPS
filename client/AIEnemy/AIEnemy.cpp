// Fill out your copyright notice in the Description page of Project Settings.


#include "AIEnemy.h"
#include "MyCharacter.h"
#include "WeaponActor.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BrainComponent.h"
#include "WeaponActor.h"
#include "MyServer.h"

// Sets default values
AAIEnemy::AAIEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));    
    BoxCollision->SetupAttachment(GetCapsuleComponent());    
    BoxCollision->SetBoxExtent(FVector(500.f, 250.f, 100.f));
    // 캐릭터 앞쪽
    BoxCollision->SetRelativeLocation(FVector(500.f, 0.f, 0.f));
    BoxCollision->SetCollisionProfileName(TEXT("Trigger"));
    BoxCollision->SetGenerateOverlapEvents(true);

    // 오버랩 이벤트
    BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AAIEnemy::OnBoxOverlap);
    BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AAIEnemy::OnBoxEndOverlap);

    HaveGun = false;
    itemindex = 0;
    CapsuleCollision = GetCapsuleComponent();
    CapsuleCollision->SetCapsuleHalfHeight(88.f);
    CapsuleCollision->SetCapsuleRadius(65);

    CapsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &AAIEnemy::OnCapsuleOverlap);

    IsDead = false;
    HP = 100;

    AiId = -1;//임시값
    FirstInit = false;
    

    
}

// Called when the game starts or when spawned
void AAIEnemy::BeginPlay()
{
	Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("AAIEnemy BeginPlay"));
    /*
    GetWorldTimerManager().SetTimer(
        TestTimerHandle,
        this,
        &AAIEnemy::PlayDeathMontage,
        3.0f,
        false
    );
    */
    AAIController* AICon = Cast<AAIController>(GetController());
    if (!AICon) return;

    BB = AICon->GetBlackboardComponent();
    if (!BB) return;


    MyServer = GetGameInstance()->GetSubsystem<UMyServer>();

    
}

// Called every frame
void AAIEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (IsDead) return;

    UpdateAnimationVariables();
    
    if (!FirstInit && AiId!=-1) {

        UE_LOG(LogTemp, Warning, TEXT("inside First Init"));
        
        float Speed = GetCharacterMovement()->Velocity.Size();
        FVector MoveDir = GetVelocity().GetSafeNormal();//실제 이동 방향
        FVector ForwardDir = GetActorForwardVector();//바라보는방향
        FVector Location = GetCharacterMovement()->GetActorLocation();//위치

        FCharacterPacket man;
        man.Header.Type = (int32)EPacketType::Character;
        man.Header.Size = sizeof(FCharacterPacket);
        man.CharacterId = AiId;//AI식별
        man.X = Location.X;//100.f;
        man.Y = Location.Y;//200.f;
        man.Z = Location.Z;//300.f;
        man.DirX = ForwardDir.X;//1.f;
        man.DirY = ForwardDir.Y;//0.f;
        man.DirZ = ForwardDir.Z;//0.f;
        man.Speed = Speed;
        man.Sendtime = FPlatformTime::Seconds();
        man.IsJump = true;
        man.IsFire = fireing;
        man.IsDeath = false;
        man.IsHeal = false;
        man.WeaponType = EWeaponType::Melee;
        man.AimActive = false;

        UE_LOG(LogTemp, Warning,
            TEXT("ID:%d Pos(%.1f %.1f %.1f) Dir(%.1f %.1f %.1f) Speed:%.1f Jump:%d Fire:%d"),
            man.CharacterId,
            man.X, man.Y, man.Z,
            man.DirX, man.DirY, man.DirZ,
            man.Speed,
            man.IsJump,
            man.IsFire
        );

        if (MyServer) {
            MyServer->MoveAI(man);
        }
        
        FirstInit = true;
    }
    
    
}

// Called to bind functionality to input
void AAIEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAIEnemy::OnBoxOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{  
    AActor* Actor = OtherActor;

    if (Actor->IsA(AMyCharacter::StaticClass()))
    {
        if (HaveGun) {
            fireing = true;
            BB->SetValueAsBool(TEXT("IsFireing"), true);
        }
        else {
            punching = true;
            BB->SetValueAsBool(TEXT("IsPunching"), true);
        }
    }


    

}

void AAIEnemy::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AActor* Actor = OtherActor;

    if (Actor->IsA(AMyCharacter::StaticClass()))
    {
        fireing = false;
        punching = false;
        BB->SetValueAsBool(TEXT("IsFireing"), false);
        BB->SetValueAsBool(TEXT("IsPunching"), false);
        UE_LOG(LogTemp, Warning, TEXT("all false"));
    }
}

void AAIEnemy::OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{    
    AActor* Actor = OtherActor;
    UE_LOG(LogTemp, Warning, TEXT("Overlap Actor AIEnemy: %s"), *OtherActor->GetName());
    AWeaponActor* weaponactor = Cast<AWeaponActor>(Actor);

    if (!weaponactor || weaponactor->bPickedUp)
        return;
    UE_LOG(LogTemp, Warning, TEXT("HaveItemOccured"));
    weaponactor->bPickedUp = true;
    HaveItems.Add(itemindex++, Actor);
    searchItem(weaponactor->GetItemName(), weaponactor);
    FVector PoolWatingRoom = FVector(100.f, 200.f, 300.f);
    Actor->SetActorLocation(PoolWatingRoom);       
}

float AAIEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    HP -= ActualDamage;

    if (HP <= 0.f)
    {
        IsDead = true;
        
        if (AAIController* AICon = Cast<AAIController>(GetController()))
        {
            AICon->StopMovement();
            if (AICon->BrainComponent)
            {
                AICon->BrainComponent->StopLogic(TEXT("Dead"));
            }
        }

        if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
        {
            if (DeathMontage)
            {
                FOnMontageEnded EndDelegate;
                EndDelegate.BindUObject(this, &AAIEnemy::OnDeathMontageEnded);

                AnimInstance->Montage_SetEndDelegate(EndDelegate, DeathMontage);
                AnimInstance->Montage_Play(DeathMontage);
            }
        }
    }
    return ActualDamage;
}





void AAIEnemy::AttackMelee()
{
    FVector Start = GetActorLocation();
    FVector Forward = GetActorForwardVector();
    float MeleeDamage = 10;
    float MeleeRange = 10;
    float MeleeRadius = 100;

    // 공격 위치 (캐릭터 앞)
    FVector AttackCenter = Start + Forward * MeleeRange;

    // 충돌 설정
    TArray<FHitResult> HitResults;
    FCollisionShape Sphere = FCollisionShape::MakeSphere(MeleeRadius);

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this); // 자기 자신 제외

    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        AttackCenter,
        AttackCenter,          // Start == End → 고정 Sphere
        FQuat::Identity,
        ECC_GameTraceChannel2,
        //ECC_Pawn,              // 캐릭터만 맞추기
        Sphere,
        Params
    );

    // 디버그 구체
    DrawDebugSphere(
        GetWorld(),
        AttackCenter,
        MeleeRadius,
        24,                     // 세그먼트 수 (클수록 부드러움)
        bHit ? FColor::Red : FColor::Blue,
        false,
        1.5f                    // 몇 초 동안 보일지
    );

    if (!bHit)
        return;

    // 맞은 액터들 처리
    for (const FHitResult& Hit : HitResults)
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor)
            continue;

        UE_LOG(LogTemp, Warning, TEXT("맞음 applydamage로"));
        // 데미지 적용
        UGameplayStatics::ApplyDamage(
            HitActor,
            MeleeDamage,
            GetController(),
            this,
            UDamageType::StaticClass()
        );
    }
}

void AAIEnemy::AttackRanged()
{
    if (!BulletClass)
        return;
    /*
    AAIController* AICon = Cast<AAIController>(GetController());
    if (!AICon)
        return;

    UBlackboardComponent* BB = AICon->GetBlackboardComponent();
    if (!BB)
        return;

    */
    FVector TargetLocation =
        BB->GetValueAsVector(TEXT("TargetLocation"));

    FVector MuzzleLocation =
        GetActorLocation() + GetActorForwardVector() * 100.f + FVector(0, 0, 50.f);

    FRotator FireRotation =
        (TargetLocation - MuzzleLocation).Rotation();

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = this;

    UE_LOG(LogTemp, Warning, TEXT("총알 스폰"));
    AActor* Bullet = GetWorld()->SpawnActor<AActor>(
        BulletClass,
        MuzzleLocation,
        FireRotation,
        Params
    );

    
}



void AAIEnemy::UpdateAnimationVariables()
{
    // 현재 이동 속도 계산 (XY 평면 속도)
    CurrentSpeed = GetCharacterMovement()->Velocity.Size2D();
    // 공중에 떠 있는지 여부
    //bIsInAir = GetCharacterMovement()->IsFalling();
    // 공격 상태는 추후 공격 시스템 구현 시 설정 (예시)
    // bIsAttacking = ...; 
}

void AAIEnemy::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage != DeathMontage) return;

    if (bInterrupted) return; // 강제 중단 시 무시 (선택)

    // 박스 생성
    if (DropBoxClass)
    {
        DropBox = GetWorld()->SpawnActor<AEnemyBox>(
            DropBoxClass,
            GetActorLocation(),
            FRotator::ZeroRotator
        );

        
    }
    UE_LOG(LogTemp, Warning, TEXT("HaveItems Count: %d"), HaveItems.Num());
    for (const TPair<int32, AActor*>& Elem : HaveItems)
    {
        AActor* Item = Elem.Value;
        if (Item)
        {
            //Item->SetActorLocation(GetActorLocation());//?나중에 폐기하자
            DropBox->fillBox(Item);
        }
    }
    if (IsValid(weaponac))
    {
        weaponac->Destroy();
        weaponac = nullptr;
    }
    Destroy();
}

void AAIEnemy::PlayDeathMontage()
{
    if (!DeathMontage) return;

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance) return;

    AAIController* AICon = Cast<AAIController>(GetController());
    if (AICon)
    {
        AICon->StopMovement();
        //AICon->BrainComponent->StopLogic(TEXT("Dead"));
    }

    GetCharacterMovement()->DisableMovement();

    // 몽타주 실행
    AnimInstance->Montage_Play(DeathMontage);

    
    // 특정 몽타주 전용 종료 델리게이트
    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &AAIEnemy::OnDeathMontageEnded);

    AnimInstance->Montage_SetEndDelegate(EndDelegate, DeathMontage);
    
}

void AAIEnemy::searchItem(int32 rowname, AWeaponActor* actor)
{   
    const FItemStaticData* ItemData = ItemDataTable->FindRow<FItemStaticData>(
        FName(*FString::FromInt(rowname)),
        TEXT("Item Lookup")
    );

    if (ItemData)
    {
        if (ItemData->Name.ToString() == TEXT("K2C1")) {
            HaveGun = true;
            BB->SetValueAsBool(TEXT("HaveGun"), true);
            if (!actor) return;
            if (!GetMesh()) return;
            
            FVector PoolWatingRoom = FVector(100.f, 200.f, 300.f);
            actor->SetActorLocation(PoolWatingRoom);

            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = GetInstigator();

            FTransform SpawnTM = GetMesh()->GetSocketTransform(
                TEXT("RightHandPinky4Socket"),
                RTS_World
            );

            weaponac = GetWorld()->SpawnActor<AActor>(
                ItemData->ItemActorClass,
                SpawnTM,
                SpawnParams
            );            

            

            // 3️⃣ 소켓에 부착
            weaponac->AttachToComponent(
                GetMesh(),
                FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                TEXT("RightHandIndex4Socket")
            );                       
        }        
    }
}

void AAIEnemy::SetMyAIID(int32 id)
{
    AiId = id;
}

void AAIEnemy::RequestNewPos(float x, float y, float z, bool Jump, bool Fire)
{
    UE_LOG(LogTemp, Warning, TEXT("AIEnemy BB >> RequestNewPos"));
    float Speed = GetCharacterMovement()->Velocity.Size();
    FVector MoveDir = GetVelocity().GetSafeNormal();//실제 이동 방향
    FVector ForwardDir = GetActorForwardVector();//바라보는방향
    FVector Location = GetCharacterMovement()->GetActorLocation();//위치

    FCharacterPacket man;
    man.Header.Type = (int32)EPacketType::Character;
    man.Header.Size = sizeof(FCharacterPacket);
    man.CharacterId = AiId;//AI식별
    man.X = Location.X;//100.f;
    man.Y = Location.Y;//200.f;
    man.Z = Location.Z;//300.f;
    man.DirX = x;
    man.DirY = y;
    man.DirZ = z;
    man.Speed = Speed;
    man.Sendtime = FPlatformTime::Seconds();
    man.IsJump = Jump;//아마 새위치 보내라는 flag
    man.IsFire = Fire;//이거는 죽음 여부를 나타내는 듯
    man.IsDeath = false;
    man.IsHeal = false;
    man.WeaponType = EWeaponType::Melee;
    man.AimActive = false;

    if (MyServer) {
        MyServer->MoveAI(man);
    }    
    BB->SetValueAsBool("TargetReached", false);
}

