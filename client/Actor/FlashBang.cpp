// Fill out your copyright notice in the Description page of Project Settings.


#include "FlashBang.h"

/*
void AFlashBang::Explode()
{
	if (GrenadeImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			GrenadeImpactEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}
	ExplodeVoxel(GetActorLocation(), 200.f);
}


void AFlashBang::ExplodeVoxel(FVector ExplosionCenter, float ExplosionRadius)
{	
	// 범위 데미지
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		40.f,                 // 데미지
		ExplosionCenter,
		ExplosionRadius,
		UDamageType::StaticClass(),
		TArray<AActor*>(),     // ignore actors
		this,
		GetInstigatorController(),
		false
	);


	// 디버그용 범위 표시 (10초간, 두께 2)
	DrawDebugSphere(
		GetWorld(),
		ExplosionCenter,
		ExplosionRadius,
		32,                   // 세그먼트 수
		FColor::Red,
		false,                // 지속 시간
		10.f,                 // 살아있는 시간
		0,
		2.f                   // 선 두께
	);

	Destroy();
}*/