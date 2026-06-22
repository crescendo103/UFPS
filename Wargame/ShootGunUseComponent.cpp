// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootGunUseComponent.h"
#include "Bullet.h"
#include "MyServer.h"

void UShootGunUseComponent::UseWeapon(AMyCharacter* Owner, FVector StartPos, FVector Dir)
{
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Owner;
    SpawnParams.Instigator = Owner;

    if (!BulletClass)
        return;

    FRotator BaseRot = Dir.Rotation();

    TArray<FRotator> Rotations =
    {
        BaseRot,                                // Áß¾Ó
        BaseRot + FRotator(3.f,  0.f, 0.f),   // À§
        BaseRot + FRotator(-3.f,  0.f, 0.f),   // ¾Æ·¡
        BaseRot + FRotator(0.f,  3.f, 0.f),   // ¿À¸¥ÂÊ
        BaseRot + FRotator(0.f, -3.f, 0.f)    // ¿ÞÂÊ
    };

    for (const FRotator& Rot : Rotations)
    {
        FVector NewDir = Rot.Vector();

        ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(
            BulletClass,
            StartPos,
            Rot,
            SpawnParams);

        if (Bullet)
        {
            Bullet->Throw(NewDir, 3000.f);
        }
    }
}

void UShootGunUseComponent::SendBulletPacket(FVector startPos, FVector dir)
{
    UMyServer* MyServer =
        GetWorld()->GetGameInstance()->GetSubsystem<UMyServer>();

    if (!MyServer)
        return;

    FRotator BaseRot = dir.Rotation();

    TArray<FRotator> Rotations =
    {
        BaseRot,                                // Áß¾Ó
        BaseRot + FRotator(3.f,  0.f, 0.f),   // À§
        BaseRot + FRotator(-3.f,  0.f, 0.f),   // ¾Æ·¡
        BaseRot + FRotator(0.f,  3.f, 0.f),   // ¿À¸¥ÂÊ
        BaseRot + FRotator(0.f, -3.f, 0.f)    // ¿ÞÂÊ
    };

    for (const FRotator& Rot : Rotations)
    {
        FVector NewDir = Rot.Vector();

        FServerBullet Bullet;

        Bullet.Header.Type = (int32)EPacketType::Bullet;
        Bullet.Header.Size = sizeof(FServerBullet);

        Bullet.BulletId = -1;
        Bullet.CharacterId = -1;

        Bullet.X = startPos.X;
        Bullet.Y = startPos.Y;
        Bullet.Z = startPos.Z;

        Bullet.DirX = NewDir.X;
        Bullet.DirY = NewDir.Y;
        Bullet.DirZ = NewDir.Z;

        Bullet.Speed = 1200.f;
        Bullet.Sendtime = FPlatformTime::Seconds();
        Bullet.flag = false;

        MyServer->Shotoccurred(Bullet);
    }
}