// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"




// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	/*
	MyChar = Cast<AMyCharacter>(GetOwner());
	if (MyChar)
	{
		MyWeapon = MyChar->GetWeaponBase();
	}
	*/
}


// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FHitResult UWeaponComponent::RayBullet(const FVector Start, const FVector Forward, float Distance)
{
	FHitResult Hit;
	//FVector Start = TargetActor->GetActorLocation();
	FVector End = Start + Forward * Distance; //TargetActor->GetActorForwardVector() * Distance;

	ECollisionChannel Channel = ECollisionChannel::ECC_Visibility;	

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());   // 무기 소유자 무시
	//QueryParams.bTraceComplex = true;          // (선택) 메시 기준 충

	bool isHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, Channel);// , QueryParams);
	// 3초 동안 선이 유지됨
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 3.0f, 0, 5.0f);

	if (isHit) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			BulletImpactEffect,
			Hit.ImpactPoint,                    //  맞은 지점
			Hit.ImpactNormal.Rotation()          //  표면 방향에 맞게 회전
		);
	}

	if (Hit.GetActor() != nullptr) {
		UGameplayStatics::ApplyDamage(Hit.GetActor(), 50, GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
		//UGameplayStatics::ApplyDamage(가해자, 데미지, 가해자컨트롤러, 총알 등, 데미지 타입);
		//임시로 캐릭터를 총알로 넣음
	}

	return Hit;
}



