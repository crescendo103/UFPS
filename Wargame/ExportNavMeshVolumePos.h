// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavMesh/RecastNavMesh.h"
#include "ExportNavMeshVolumePos.generated.h"

UCLASS()
class FPS_API AExportNavMeshVolumePos : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExportNavMeshVolumePos();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(CallInEditor, Category = "NavMesh")
	void ExportNavMeshPos();

private:
	// 현재 월드의 Recast NavMesh 얻기
	ARecastNavMesh* GetNavMeshData() const;
};
