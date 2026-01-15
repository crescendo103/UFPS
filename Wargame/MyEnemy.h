// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyEnemy.generated.h"

UCLASS()
class WARGAME_API AMyEnemy : public ACharacter
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
	void SetSPD(float speed, FVector position, FVector direction);
	

	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	float CurrentSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	FVector Position;
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	FVector Direction;
	UPROPERTY(BlueprintReadOnly, Category = "Anim")
	FVector CharacterId;
	
	
};
