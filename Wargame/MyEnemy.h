// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "MyEnemy.generated.h"


class UMyServer;

UCLASS()
class FPS_API AMyEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void SetSPD(float speed, FVector position, FVector direction, bool isjump, bool isfire);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void SetIgnoreCharacterId(int id);
	int GetIgnoreCharacterId();

	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	float CurrentSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	FVector Position;
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	FVector Direction;
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	FVector CharacterId;
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	bool IsJump;
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	bool IsFire;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Myserver")
	UMyServer* MyServer;
	
	int IgnoreCharacterID;
};
