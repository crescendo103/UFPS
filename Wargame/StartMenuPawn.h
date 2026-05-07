// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h" 
#include "MyServer.h"
#include "MyCharacter.h"

#include "StartMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "LoadingEnemy2.h"

#include "StartMenuPawn.generated.h"


UCLASS()
class FPS_API AStartMenuPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AStartMenuPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Myserver")
	UMyServer* MyServer;

	UPROPERTY(EditAnywhere, Category = "Enemy")
	TSubclassOf<ALoadingEnemy2> EnemyClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	ALoadingEnemy2* EnemyActor;

	

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UStartMenuWidget> StartMenuWidgetClass;//블루프린트 위젯 설계도
	UPROPERTY()
	UStartMenuWidget* StartMenuWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	FVector TargetLocation = FVector(-126763.797849, -3192.599134, 62.209195);
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	int32 MyOrder=0;
	//FRotator TargetRot;

	float MoveSpeed = 0.50f; // 클수록 빨라짐
	bool Iswalking;

	
	UPROPERTY()
	AMyCharacter* Player;
};
