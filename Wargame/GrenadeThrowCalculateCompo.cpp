// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeThrowCalculateCompo.h"
#include "Components/SplineComponent.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/Character.h"
#include "MyCharacter.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UGrenadeThrowCalculateCompo::UGrenadeThrowCalculateCompo()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	//스플라인을 사용하기위해 생성해줌//
	Spline_Path = CreateDefaultSubobject<USplineComponent>(TEXT("Spline_Path"));
	bShow = false;
	// ...
}


// Called when the game starts
void UGrenadeThrowCalculateCompo::BeginPlay()
{
	Super::BeginPlay();
	IgnoreActors.Add(GetOwner());
	
	{
		
	}
	//UpdateSpline();
	// ...
	AActor* Owner = GetOwner();
	if (!Owner) return;

	// 🔹 Decal 생성
	CircleDecal = NewObject<UDecalComponent>(Owner, UDecalComponent::StaticClass(), TEXT("CircleDecal"));
	if (!CircleDecal) return;

	if (CircleDecal && CircleDecalMaterial)
	{
		CircleDecal->SetDecalMaterial(CircleDecalMaterial);
	}
	UE_LOG(LogTemp, Warning, TEXT("CircleDecalMaterial = %s"),
		CircleDecalMaterial ? *CircleDecalMaterial->GetName() : TEXT("NULL"));

	// 🔹 월드 등록 (이거 안 하면 안 보임)
	CircleDecal->RegisterComponent();

	// 🔹 Owner Root에 부착
	if (Owner->GetRootComponent())
	{
		CircleDecal->AttachToComponent(
			Owner->GetRootComponent(),
			FAttachmentTransformRules::KeepWorldTransform
		);
	}

	// 🔹 기본 설정
	CircleDecal->SetVisibility(false);
	CircleDecal->DecalSize = FVector(128.f, 256.f, 256.f);
	CircleDecal->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f)); // 바닥 투영용

	IgnoreActors.Add(Owner);

	MouseVal=0;
}


// Called every frame
void UGrenadeThrowCalculateCompo::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	UpdateSplinePath();		
}

void UGrenadeThrowCalculateCompo::UpdateSplinePath()
{	
	if (!bShow)
	{
		Spline_Path->ClearSplinePoints(true);
		if (Spline_Meshs.Num() > 0)
		{
			for (int32 i = 0; i < Spline_Meshs.Num(); i++)
			{
				if (Spline_Meshs[i])
				{
					// 눈에서 즉시 가려버리고 컴포넌트 파괴
					Spline_Meshs[i]->SetVisibility(false);
					Spline_Meshs[i]->DestroyComponent();
				}
			}
			Spline_Meshs.Empty();
		}

		if (CircleDecal)
		{
			CircleDecal->SetVisibility(false);
		}

		return; // 청소가 끝났으니 여기서 함수를 안전하게 끝냅니다!
	}

	//전에 저장되있는데 스프라인 포인트를 다 지움//
	Spline_Path->ClearSplinePoints(true);
	if (Spline_Meshs.Num() > 0)
	{
		for (int32 i = 0; i < Spline_Meshs.Num(); i++)
		{
			if (Spline_Meshs[i])
			{
				//저장된 배열의 메시컴포넌트를 삭제하여 계속 지속시킴//
				//Spline_Meshs[i]->DetachFromParent();
				Spline_Meshs[i]->DestroyComponent();
			}
		}
		//배열을 삭제함//
		Spline_Meshs.Empty();
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	//필요값들//
	FHitResult OutHIt;
	TArray<FVector> OutPathPositions;
	FVector LastPosition;
	///////////

	//라인트레이스 시작점// 손소켓위치로하자
	
	//비효율적이다..
	//ACharacter* Character = Cast<ACharacter>(GetOwner());
	//if (!Character) return;
	ACharacter* Character =
		Cast<ACharacter>(GetOwner());
	if (!Character) return;

	AMyCharacter* MyCharacter = Cast<AMyCharacter>(Character);
	

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh) return;

	FVector HandLocation = Mesh->GetSocketLocation("RightHandPinky4Socket");
	
	// [수정] 카메라 앞방향 벡터를 가져온 뒤, 마우스 휠 회전값(MouseVal)을 추가합니다.
	FRotator CameraRot = MyCharacter->FollowCamera->GetComponentRotation();
	CameraRot.Pitch += MouseVal; // 마우스 휠로 쌓인 위/아래 각도 추가
	CameraRot.Pitch = FMath::Clamp(CameraRot.Pitch, -60.f, 60.f); // 각도 제한 (안전장치)
	
	FVector Direction = CameraRot.Vector().GetSafeNormal(); // 최종 조준 방향
	FVector LaunchVelocity = Direction * 1000.f; // 던지는 힘 (1000)

	//라인 트레이스를 쏴 그 길의 각 포인트지점을 따라 Spline에 저장해줌//
	bool isHit = UGameplayStatics::Blueprint_PredictProjectilePath_ByTraceChannel(GetWorld(), OutHIt, OutPathPositions, LastPosition, HandLocation, LaunchVelocity, true, 10.f, ECollisionChannel::ECC_WorldStatic, false, IgnoreActors, EDrawDebugTrace::None, 15.f);
	for (int i = 0; i < OutPathPositions.Num(); i++)
	{
		//포인트지점을 따라 저장해줌//
		Spline_Path->AddSplinePointAtIndex(OutPathPositions[i], i, ESplineCoordinateSpace::World);
	}
	
	//이거는 제가 만든거라 원하는 State를 이용하여 보이게하거나 안보이게 할것이다.//
	if (bShow)
	{
		

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////
		for (int SplineCount = 0; SplineCount < (Spline_Path->GetNumberOfSplinePoints()) - 1; SplineCount++)
		{
			USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
			//만약 실린터메쉬를 사용할떄 메쉬를 X이 정면이라 가정하면 Z축으로 뒤집어서 정면으로 바꿈(만약 박스만 이용할경우 사용 안해도됨.)// 
			SplineMeshComponent->SetForwardAxis(ESplineMeshAxis::Z);
			SplineMeshComponent->SetStaticMesh(DefalutMesh);
			//정적 움직임//
			SplineMeshComponent->SetMobility(EComponentMobility::Movable);
			SplineMeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			//월드에 등록해줌//
			SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
			//스플라인 컴포넌트에 컴포넌트에 따라 크기 위치를 변경함//
			SplineMeshComponent->AttachToComponent(Spline_Path, FAttachmentTransformRules::KeepRelativeTransform);
			SplineMeshComponent->SetStartScale(FVector2D(UKismetSystemLibrary::MakeLiteralFloat(0.1f), UKismetSystemLibrary::MakeLiteralFloat(0.1f)));
			SplineMeshComponent->SetEndScale(FVector2D(UKismetSystemLibrary::MakeLiteralFloat(0.1f), UKismetSystemLibrary::MakeLiteralFloat(0.1f)));

			//시작지점//
			const FVector StartPoint = Spline_Path->GetLocationAtSplinePoint(SplineCount, ESplineCoordinateSpace::Local);
			const FVector StartTangent = Spline_Path->GetTangentAtSplinePoint(SplineCount, ESplineCoordinateSpace::Local);
			const FVector EndPoint = Spline_Path->GetLocationAtSplinePoint(SplineCount + 1, ESplineCoordinateSpace::Local);
			const FVector EndTangent = Spline_Path->GetTangentAtSplinePoint(SplineCount + 1, ESplineCoordinateSpace::Local);
			SplineMeshComponent->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, true);

			//메쉬에 충돌할것인지 아닌지확인함(일단은 안함)//
			SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			//메쉬는 꼭아래에다 해줘야한다.//
			if (DefaultMaterial)
			{
				SplineMeshComponent->SetMaterial(0, DefaultMaterial);
			}
			////////////////////////////////

			//메쉬의 위치를 저장해줌//
			Spline_Meshs.Add(SplineMeshComponent);
		}
		CircleDecal->SetVisibility(true);
		CircleDecal->SetWorldLocation(LastPosition);
		
	}
	else
	{
		/*
		GEngine->AddOnScreenDebugMessage(
			-1,            // -1 = 새 줄 (BP Print String 기본 동작)
			2.0f,          // 화면에 표시될 시간
			FColor::Yellow,
			TEXT("novision")
		);
		*/
		//위에있는 State가 false일경우에는 계속 안보이게 설정해줌//
		Spline_Path->ClearSplinePoints(true);
		if (Spline_Meshs.Num() > 0)
		{
			for (int32 i = 0; i < Spline_Meshs.Num(); i++)
			{
				if (Spline_Meshs[i])
				{
					//Spline_Meshs[i]->DetachFromParent();
					Spline_Meshs[i]->DestroyComponent();
				}
			}
			Spline_Meshs.Empty();
		}
		CircleDecal->SetVisibility(false);
	}

}

void UGrenadeThrowCalculateCompo::SetStartPos(FVector position)
{
	StartPos = position;
}

void UGrenadeThrowCalculateCompo::SetbShow(bool state)
{
	bShow = state;
}

void UGrenadeThrowCalculateCompo::SetMouseVal(float val)
{
	MouseVal += val;
}

FVector UGrenadeThrowCalculateCompo::GetFinalThrowDirection() const
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character) return FVector::ForwardVector;

	AMyCharacter* MyCharacter = Cast<AMyCharacter>(Character);
	if (!MyCharacter || !MyCharacter->FollowCamera) return FVector::ForwardVector;

	// 예측선 틱에서 계산하는 방식과 완전히 동일하게 처리
	FRotator CameraRot = MyCharacter->FollowCamera->GetComponentRotation();
	CameraRot.Pitch += MouseVal;
	CameraRot.Pitch = FMath::Clamp(CameraRot.Pitch, -60.f, 60.f);

	return CameraRot.Vector().GetSafeNormal();
}


