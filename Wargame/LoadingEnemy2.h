// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Components/SkeletalMeshComponent.h"
#include "LoadingEnemy2.generated.h"

UCLASS()
class FPS_API ALoadingEnemy2 : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALoadingEnemy2();

	// ½ºÄÌ·¹Å» ¸Þ½Ã
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* SkeletalMeshComp;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsWalking;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	FVector TargetLocation = FVector(-126763.797849, -3192.599134, 62.209195);
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	int32 MyOrder = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float MoveSpeed = 0.50f; // Å¬¼ö·Ï »¡¶óÁü

	void MoveEnemy();
	int32 getMyOrder();
};
