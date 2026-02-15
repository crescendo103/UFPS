// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyBox.generated.h"

UCLASS()
class FPS_API AEnemyBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 구체 콜리전 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
	class USphereComponent* TriggerSphere;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	TArray<AActor*> dropBoxItems;

	void fillBox(AActor* actor);
	TArray<AActor*> GetDropBoxItems();
};
