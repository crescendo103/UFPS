#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "PawnVehicle.generated.h"

class UBoxComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AMyCharacter;
class UMyServer;
class UItemEffectComponent;
UCLASS()
class FPS_API APawnVehicle : public APawn
{
	GENERATED_BODY()

public:
	APawnVehicle();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(
		UInputComponent* PlayerInputComponent) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 carId = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsHaveCharacter = false;

	UPROPERTY(EditAnywhere)
	float MoveSpeed = 1000.f;

	UPROPERTY(EditAnywhere)
	float TurnSpeed = 100.f;

public:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CarMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* EnterTrigger;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

public:

	UPROPERTY(EditAnywhere)
	UInputMappingContext* VehicleMappingContext;

	UPROPERTY(EditAnywhere)
	UInputMappingContext* CharacterMappingContext;

	UPROPERTY(EditAnywhere)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere)
	UInputAction* ExitAction;

public:

	UPROPERTY()
	AMyCharacter* CurrentDriver;

	UPROPERTY()
	UMyServer* MyServer;

public:

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

public:

	void EnterVehicle(AMyCharacter* Character);
	void ExitVehicle();

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void ExitPressed();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	UItemEffectComponent* EffectCompo;
};