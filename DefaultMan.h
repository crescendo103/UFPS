// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h" // 추가할 컴포넌트를 위해 미리 포함

#include "DefaultMan.generated.h"

UCLASS()
class WARGAME_API ADefaultMan : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADefaultMan();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Man")
	UStaticMeshComponent* m_StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Man")
	FString m_Message;

	UFUNCTION(BlueprintCallable, Category = "Man")
	void PrintMessage();


};
