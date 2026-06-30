// Fill out your copyright notice in the Description page of Project Settings.


#include "FragGrenade.h"
/*
void AFragGrenade::Explode()
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
	ExplodeVoxel(GetActorLocation(), 40.f);
}

void AFragGrenade::ExplodeVoxel(FVector ExplosionCenter, float ExplosionRadius)
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

	Destroy();
}*/