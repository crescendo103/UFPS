// Fill out your copyright notice in the Description page of Project Settings.


#include "RifleUseComponent.h"
#include "Bullet.h"
#include "MyServer.h"

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

void URifleUseComponent::SendBulletPacket(FVector startPos, FVector dir)
{
    UMyServer* MyServer =
        GetWorld()->GetGameInstance()->GetSubsystem<UMyServer>();
	if (MyServer) {		
        FVector StartPos = startPos;
        FVector Dir = dir;
        FServerBullet Bullet;
        Bullet.Header.Type = (int32)EPacketType::Bullet;
        Bullet.Header.Size = sizeof(FServerBullet);
        Bullet.BulletId = -1;//쓰레기값
        Bullet.CharacterId = -1;//쓰레기값
        Bullet.X = StartPos.X;//100.f;
        Bullet.Y = StartPos.Y;//200.f;
        Bullet.Z = StartPos.Z;//300.f;
        Bullet.DirX = Dir.X;//1.f;
        Bullet.DirY = Dir.Y;//0.f;
        Bullet.DirZ = Dir.Z;//0.f;
        Bullet.Speed = 1200.f;
        Bullet.Sendtime = FPlatformTime::Seconds();
        Bullet.flag = false;
        MyServer->Shotoccurred(Bullet);
	}
}
