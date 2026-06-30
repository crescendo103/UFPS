// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CaptureMapActor.generated.h"

UCLASS()
class FPS_API ACaptureMapActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACaptureMapActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneCaptureComponent2D* CaptureComp;

	UPROPERTY(EditAnywhere)
	ASceneCapture2D* CaptureActor;

	UPROPERTY(EditAnywhere)
	UTextureRenderTarget2D* RenderTarget;

	UFUNCTION(BlueprintCallable)
	void CaptureAndSave();
};
