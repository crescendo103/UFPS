// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SplineMeshComponent.h"

#include "Components/DecalComponent.h"

#include "GrenadeThrowCalculateCompo.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_API UGrenadeThrowCalculateCompo : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrenadeThrowCalculateCompo();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//SplineMesh에 붙이고 끝부분에 붙일 데칼을 생성해줌//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
	UStaticMesh* DefalutMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
	class UMaterialInterface* DefaultMaterial;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Decal")
	class UDecalComponent* CircleDecal;
	// Decal Material
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal")
	UMaterialInterface* CircleDecalMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spline")
	FVector StartPos;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ViewPoint")
	APlayerController* PC;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ViewPoint")
	bool bShow;
	UPROPERTY()
	float MouseVal;
	//스플라인 컴포넌트//
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player", meta = (AllowPrivateAccess = "true"))
	class USplineComponent* Spline_Path;
	//스플라인메쉬 컴포넌트//
	TArray<USplineMeshComponent*> Spline_Meshs;
	TArray<AActor*> IgnoreActors;
	//UPROPERTY()
	//FRotator ThrowRot;//누적 회전값

	//틱함수에 호출하여 스플라인컴포넌트의 위치를 계속 추적함//
	void UpdateSplinePath();
	void SetStartPos(FVector position);
	void SetbShow(bool state);
	void SetMouseVal(float val);
};
