#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyBox.h"
#include "AIEnemy.generated.h"

class ABullet;
class AWeaponActor;
class UMyServer;
UCLASS()
class FPS_API AAIEnemy : public ACharacter
{
    GENERATED_BODY()

public:
    AAIEnemy();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
    UBoxComponent* BoxCollision;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
    UCapsuleComponent* CapsuleCollision;

    UFUNCTION()
    void OnBoxOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void OnBoxEndOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex
    );

    UFUNCTION()
    void OnCapsuleOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    virtual float TakeDamage(
        float DamageAmount,
        struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator,
        AActor* DamageCauser
    ) override;

    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    TSubclassOf<ABullet> BulletClass;

    void AttackMelee();
    void AttackRanged();
    void UpdateAnimationVariables();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool fireing;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    float CurrentSpeed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool HaveGun;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool punching;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    bool IsDead;

    int32 itemindex;
    float HP;

    UPROPERTY()
    TMap<int32, AActor*> HaveItems;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* DeathMontage;

    

    UPROPERTY(EditDefaultsOnly, Category = "Drop")
    TSubclassOf<AEnemyBox> DropBoxClass;
    UPROPERTY()
    AEnemyBox* DropBox;

    UFUNCTION()
    void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    void PlayDeathMontage();
    
    FTimerHandle TestTimerHandle;
    

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    UDataTable* ItemDataTable;

    void searchItem(int32 rowname,AWeaponActor* actor);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    UBlackboardComponent* BB;
    
    UPROPERTY()
    AActor* weaponac;

    UPROPERTY()
    UMyServer* MyServer;

    UPROPERTY()
    int AiId;

    void SetMyAIID(int32 id);
    void RequestNewPos(float x, float y, float z, bool Jump, bool Fire);
    bool FirstInit;
};
