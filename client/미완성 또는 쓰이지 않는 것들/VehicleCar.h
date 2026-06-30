// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "InputActionValue.h"

#include "VehicleCar.generated.h"

/**
 * 
 */
class UMyServer;
UCLASS()
class FPS_API AVehicleCar : public APawn
{
	GENERATED_BODY()
	
public:

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(
        UInputComponent* PlayerInputComponent
    ) override;

    AVehicleCar();

    UPROPERTY(EditAnywhere)
    class UStaticMeshComponent* CarMesh;

    UPROPERTY(EditAnywhere)
    class UBoxComponent* EnterTrigger;
    UPROPERTY(EditAnywhere)
    class USpringArmComponent* SpringArm;
    UPROPERTY(EditAnywhere)
    class UCameraComponent* Camera;
    class AMyCharacter* CurrentDriver;

    void EnterVehicle(class AMyCharacter* Character);
    void ExitVehicle();

    void MoveForward(const FInputActionValue& Value);
    void MoveLook(const FInputActionValue& Value);
    void ExitPressed();

    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void OnOverlapEnd(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex
    );

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* MoveForwardAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* LookAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* ExitAction;

    UPROPERTY(EditDefaultsOnly)
    class UInputMappingContext* VehicleMappingContext;

    UPROPERTY(EditDefaultsOnly)
    class UInputMappingContext* CharacterMappingContext;

    UPROPERTY(EditAnywhere)
    float MaxSpeed = 800.f;

    UPROPERTY(EditAnywhere)
    float TurnSpeed = 100.f;

    float CurrentSpeed = 0.f;
    float SteeringInput = 0.f;

    UPROPERTY(EditAnywhere)
    int32 carId = 0;

    bool IsHaveCharacter = false;
    int32 CharacterId = -1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Myserver")
    UMyServer* MyServer;
};
