// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h" // FInputActionValue를 위해 포함
#include "WeaponBase.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/UserWidget.h"
#include "AimWidget.h"
#include "Kismet/GameplayStatics.h"
#include "MyGrenade.h"
#include "TypeWeapon.h"
#include "Bullet.h"
#include "GrenadeThrowCalculateCompo.h"
#include "MyServer.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "PaperSpriteComponent.h"

#include "WeaponComponent.h"
#include "MyCharacter.generated.h"





// 미리 선언 (Forward Declarations)
class UInputMappingContext;
class UInputAction;
class USpringArmComponent; // 스프링 암 컴포넌트 추가
class UCameraComponent;    // 카메라 컴포넌트 추가

UCLASS()
class WARGAME_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

    virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
    void spawnActor(FServerBulletPos pos);
    virtual void OnRep_Controller() override;
protected:
    // =============== Input System 관련 UPROPERTY 변수들 ===============
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MoveAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* LookAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* JumpAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* CrouchMoveAction;    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* CrouchAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* FireAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* ChangeLookAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* FirstWeaponAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* SecondWeaponAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MouseWheelAction;

    

    // 새로운 컴포넌트들을 UPROPERTY로 선언
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* CameraBoom; // 카메라를 팔처럼 지지하는 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* FollowCamera; // 실제 카메라 렌더링을 담당하는 컴포넌트

    // 애니메이션 블루프린트에 노출할 변수들
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    float CurrentSpeed; // 현재 이동 속도
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsInAir; // 공중에 떠 있는지 여부
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsAttacking; 
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsStanding;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsFireing;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsJumping;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsFirstPerson;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SocketName")
    FName GunSocket;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SocketName")
    AWeaponBase* Weapon;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    AActor* SubItem;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ViewPoint")
    FVector2D ScreenCenter;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ViewPoint")
    APlayerController* PC;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Myserver")
    UMyServer* MyServer;

    //총기 조준점
    UPROPERTY(EditDefaultsOnly, Category = "AIMUI")
    TSubclassOf<UUserWidget> AimWidgetClass;//블루프린트 위젯 설계도
    UPROPERTY()
    class UAimWidget* AimWidget;
    //UUserWidget* AimWidget;//실제로 만들어진 객체
    //
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<AWeaponBase> WeaponClass;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<AMyGrenade> GrenadeClass;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<ABullet> BulletClass;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<AMyEnemy> EnermyClass;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    UWeaponComponent* WeaponComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponRay")
    FHitResult BulletRayResult;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    UGrenadeThrowCalculateCompo* GrenadeCalComponent;

    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HP")
    float CurrentHp;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    EWeaponType CurrentWeapon;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    int32 bulletId;
    
    UPROPERTY(VisibleAnywhere, Category = "MinimapCamera")
    class USpringArmComponent* minimapCameraBoom;
    UPROPERTY(VisibleAnywhere, Category = "MinimapCamera")
    class USceneCaptureComponent2D* minimapCapture;
    UPROPERTY(VisibleAnywhere, Category = "MinimapCamera")
    class UPaperSpriteComponent* minimapSprite;

    int32 test;

    // 애니메이션 블루프린트에 필요한 데이터를 업데이트하는 함수 (Tick에서 호출)
    void UpdateAnimationVariables();
    

    // =============== Input Action 바인딩될 함수들 ===============
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void Jump();
    void StopJumping();
    void StartFire();    
    void EndFire();   
    void Crouching();
    void CrouchingEnd();
    void CrouchMove(const FInputActionValue& Value);
    void ChangeLook();
    void ChangeFirstWeapon();
    void ChangeSecondWeapon();
    void MouseWheel(const FInputActionValue& Value);
    AWeaponBase* GetWeaponBase();
    UAimWidget* GetPlayerUI();
    void InitScreenCenter();
    
};
