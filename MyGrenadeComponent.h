// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SplineComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"


#include "Components/SplineMeshComponent.h"
#include "MyGrenadeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WARGAME_API UMyGrenadeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyGrenadeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//UPROPERTY()
	//USplineComponent* Spline;

	//UPROPERTY()
	//TArray<USplineMeshComponent*> SplineMeshPool;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxSplineMeshes = 10;

	// 선으로 쓸 메시 (에디터에서 지정)
	//UPROPERTY(EditAnywhere, Category = "Grenade|Spline")
	//UStaticMesh* LineMesh;

	UPROPERTY(EditAnywhere, Category = "Grenade|Spline")
	UNiagaraComponent* TrajectoryNiagara;
	UPROPERTY(EditAnywhere, Category = "Grenade|Spline")
	UNiagaraSystem* TrajectoryNiagaraSystem;

	UPROPERTY()
	TArray<FVector> Points;
	//틱기반 렌더를 위한 bool
	UPROPERTY()
	bool bshow;

	
	UPROPERTY()
	float CurrentPitch;
	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RayGrenade(const FVector Start, const FVector Forward, float Distance);
	void SetBshow(bool state);
	void SetWheelVal(float val);
};
