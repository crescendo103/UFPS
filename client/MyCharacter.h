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
#include "WatingRoom.h"
#include "MyInventory.h"
#include "VisualGrenade.h"
#include "WD_HPBar.h"
#include "Engine/DamageEvents.h"
#include "AIEnemy.h"
#include "CP_BloodEffect.h"

#include "WeaponComponent.h"
#include "MyCharacter.generated.h"





// 미리 선언 (Forward Declarations)
class UInputMappingContext;
class UInputAction;
class USpringArmComponent; // 스프링 암 컴포넌트 추가
class UCameraComponent;    // 카메라 컴포넌트 추가
class AHelicopter;
class UPawnManager;
class APawnVehicle;
UCLASS()
class FPS_API AMyCharacter : public ACharacter
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
    //virtual void OnRep_Controller() override;6-18

    UFUNCTION()
    void OnOverlapWithItem(AActor* OverlappedActor, AActor* OtherActor);
    UFUNCTION()
    void OnOverlapEndWithItem(AActor* OverlappedActor, AActor* OtherActor);
    
    void WeaponAttach(AActor* weapon, FName sockname);
    void setCurrentWeapon(EWeaponType type);
    void resetCurrentGunWeapon();
public:
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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* InventoryAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* InterAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* TestAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* AimAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* GlobalMapAction;

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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsHealKit;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsHanging;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsZooming;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    float Pitch;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsDeath;    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool isSubWeaponActive;


    


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SocketName")
    FName GunSocket;    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    AActor* SubItem;    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ViewPoint")
    FVector2D ScreenCenter;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ViewPoint")
    APlayerController* PC;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Myserver")
    UMyServer* MyServer;
    int32 MyOwner;
    
    
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UMyInventory> InventoryWidgetClass;//블루프린트 위젯 설계도
    UPROPERTY()
    UMyInventory* InventoryWidget;

    
    
   
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<AVisualGrenade> VisualGrenadeClass;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    UWeaponComponent* WeaponComponent;    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    UGrenadeThrowCalculateCompo* GrenadeCalComponent;

    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HP")
    float CurrentHp;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    EWeaponType CurrentWeapon;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    int32 bulletId;
    
    UPROPERTY(VisibleAnywhere, Category = "MinimapCamera")
    USpringArmComponent* minimapCameraBoom;
    UPROPERTY(VisibleAnywhere, Category = "MinimapCamera")
    USceneCaptureComponent2D* minimapCapture;
    UPROPERTY(VisibleAnywhere, Category = "MinimapCamera")
    UPaperSpriteComponent* minimapSprite;
    UPROPERTY(VisibleAnywhere, Category = "MinimapCamera")
    UPaperSpriteComponent* globalSprite;
    UPROPERTY()
    UTextureRenderTarget2D* MinimapRT;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* MarkMesh;
        
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 PlayerID;
    
    UPROPERTY()
    TArray<AActor*> Items;

    //타이머 핸들 선언
    FTimerHandle ExplosionTimerHandle;
    bool IsInventoryActive;

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
   
    void InitScreenCenter();
    void CameraLineTrace();

    void ClearSubWeapon();
    void InventoryActive();
    void InterActionRayCast();
   
    void PossessedBy(AController* NewController)override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heli")
    TSubclassOf<AHelicopter> HeliClass;

    UPROPERTY()
    UPawnManager* PM;

    void PullDownHeli();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    AActor* Parachute;

    void SetParachuteActor(AActor* actor);

    virtual void Landed(const FHitResult& Hit) override;
    void DetachParachute();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heli")
    TSubclassOf<AActor> ParachuteClass;

    //void SetPlayerID(int32 id);
    //int32 GetPlayerID();


    void OnDamaged();
    void OutZone();
    void InZone();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PostProcess")
    UCP_BloodEffect* HitPostProcessComp;


    void AimActive();
    void AimDeActive();
    void Die(int32 CauserCharacterId);

    UPROPERTY()
    AWeaponActor* CurrentGunWeapon;
    
    /*5-23
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<AActor> SubItemClass;*/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    FSubItemData SubItemData;
    

    void SetSubItemData(FSubItemData subData);
    //추가
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    UAnimMontage* PunchMontage;
    
    bool bPunchCollisionActive = false;

    void PlayPunch();
    void SpawnPunchCollision();
    void EndPunchCollision();

    bool bGlobalMap = false;
    void AddViewPortGlobalMap();

    
    public:
        UTextureRenderTarget2D* GetMinimapRT() const { return MinimapRT; }        

        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "redZone")
        TSubclassOf<ABomb> BombClass;
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BlueHole")
        TSubclassOf<ABlueHole> BlueHoleClass;


        UPROPERTY()
        APawnVehicle* CurrentVehicle;

        bool bIsDeadFinal = false;
        FTimerHandle WaitKillerNameHandle;
        int32 CurrentHitMeCharacterID;
};
