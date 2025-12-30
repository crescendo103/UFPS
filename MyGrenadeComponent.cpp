// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGrenadeComponent.h"
#include <Kismet/GameplayStatics.h>



// Sets default values for this component's properties
UMyGrenadeComponent::UMyGrenadeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	//Spline = CreateDefaultSubobject<USplineComponent>(TEXT("GrenadeSpline"));
	//Spline->SetMobility(EComponentMobility::Movable);
	
	
	// ...
}


// Called when the game starts
void UMyGrenadeComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!Spline)
	{
		Spline = NewObject<USplineComponent>(GetOwner());
		Spline->RegisterComponent(); // 이거 필수
		Spline->SetMobility(EComponentMobility::Movable);

		if (USceneComponent* Root = GetOwner()->GetRootComponent())
		{
			Spline->AttachToComponent(
				Root,
				FAttachmentTransformRules::KeepRelativeTransform
			);
		}
	}

	
	Spline->SetVisibility(true);
	Spline->SetHiddenInGame(false);
	// ...
	for (int32 i = 0; i < MaxSplineMeshes; i++)
	{
		USplineMeshComponent* Mesh =
			NewObject<USplineMeshComponent>(GetOwner());

		Mesh->SetStaticMesh(LineMesh);
		Mesh->SetForwardAxis(ESplineMeshAxis::X);//위치갱신해야해
		Mesh->SetMobility(EComponentMobility::Movable);
		Mesh->AttachToComponent(
			Spline,
			FAttachmentTransformRules::KeepRelativeTransform
		);

		Mesh->RegisterComponent();
		Mesh->SetVisibility(false);   // 숨겨둠

		SplineMeshPool.Add(Mesh);
	}

	bshow = false;
	
	CurrentPitch = 0.f;
}


// Called every frame
void UMyGrenadeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bshow) {
		RayGrenade(GetOwner()->GetActorLocation(), GetOwner()->GetActorForwardVector(), 100);
	}
	// ...
}

void UMyGrenadeComponent::RayGrenade(const FVector Start, const FVector Forward, float Distance)
{
	//Spline->SetVisibility(true);
	//Spline->SetHiddenInGame(false);

	FPredictProjectilePathParams Params;
	FPredictProjectilePathResult Result;

	//pitch회전
	FRotator PitchRot = FRotator(CurrentPitch, 0.f, 0.f);
	// Forward를 Pitch만큼 회전
	FVector NewForward = PitchRot.RotateVector(Forward);
	/*
	//debug
	FVector LaunchVel = NewForward * 700.f;
	GEngine->AddOnScreenDebugMessage(
		-1,
		2.0f,
		FColor::Red,
		LaunchVel.ToString()
	);
	*/
	Params.StartLocation = Start + Forward * 30.f;;
	Params.LaunchVelocity = NewForward * 700.f;
	Params.bTraceWithCollision = true;
	Params.ProjectileRadius = 5.f;
	Params.MaxSimTime = 2.5f;
	Params.SimFrequency = 15.f;
	Params.OverrideGravityZ = -980.f;
	Params.TraceChannel = ECC_Visibility;
	Params.ActorsToIgnore.Add(GetOwner());
	Params.ActorsToIgnore.Add(GetOwner()->GetOwner());

	bool bHit = UGameplayStatics::PredictProjectilePath(
		GetWorld(),
		Params,
		Result
	);
	
	//Grenade 기준 Transform
	const FTransform GrenadeTransform = GetOwner()->GetActorTransform();

	Spline->ClearSplinePoints();

	for (int i = 0; i < Result.PathData.Num(); i++)
	{
		const FVector WorldPos = Result.PathData[i].Location;
		// World -> Grenade(Local)
		const FVector LocalPos = GrenadeTransform.InverseTransformPosition(WorldPos);

		Spline->AddSplinePoint(
			LocalPos,
			ESplineCoordinateSpace::Local
		);

	}
	//디버그
	for (int32 i = 0; i < Result.PathData.Num() - 1; i++)
	{
		DrawDebugLine(
			GetWorld(),
			Result.PathData[i].Location,
			Result.PathData[i + 1].Location,
			FColor::Green,
			false,
			2.f,
			0,
			3.f
		);
	}

	Spline->UpdateSpline();
	/*
	GEngine->AddOnScreenDebugMessage(
		-1,            // -1 = 새 줄 (BP Print String 기본 동작)
		2.0f,          // 화면에 표시될 시간
		FColor::Yellow,
		TEXT("spline end")
	);
	*/
	const int32 SegmentCount = Spline->GetNumberOfSplinePoints() - 1;

	for (int32 i = 0; i < SplineMeshPool.Num(); i++)
	{
		if (i < SegmentCount)
		{
			FVector StartPos, StartTan, EndPos, EndTan;
			Spline->GetLocationAndTangentAtSplinePoint(i, StartPos, StartTan, ESplineCoordinateSpace::Local);
			Spline->GetLocationAndTangentAtSplinePoint(i + 1, EndPos, EndTan, ESplineCoordinateSpace::Local);

			

			SplineMeshPool[i]->SetStartAndEnd(StartPos, StartTan, EndPos, EndTan,true);
			SplineMeshPool[i]->SetVisibility(true);
		}
		else
		{
			SplineMeshPool[i]->SetVisibility(false);
		}
	}

	
	
}

void UMyGrenadeComponent::SetBshow(bool state)
{
	bshow = state;
}

void UMyGrenadeComponent::SetWheelVal(float val)
{
	CurrentPitch += val;
	CurrentPitch = FMath::Clamp(CurrentPitch, -60.f, 60.f);
}

