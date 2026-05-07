// Fill out your copyright notice in the Description page of Project Settings.


#include "RifleUseComponent.h"
#include "Bullet.h"

void URifleUseComponent::UseWeapon(AMyCharacter* Owner, FVector StartPos, FVector Dir)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Owner;        // 스폰 주체
	SpawnParams.Instigator = Owner;   // 총알이 누구로부터 나온건지
	if (BulletClass) {
		ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(BulletClass, StartPos, Dir.Rotation(), SpawnParams);
		Bullet->Throw(Dir, 3000);
	}
	
}