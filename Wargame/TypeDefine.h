#pragma once

#include "CoreMinimal.h"

#include "TypeDefine.generated.h"

enum class EPacketType : int
{
    Bullet = 1,
    Character = 2,
    Hit = 3,
};

USTRUCT(BlueprintType)
struct FPacketHeader
{
    GENERATED_BODY()

    UPROPERTY()
    int Size; // 전체 패킷 크기
    UPROPERTY()
    int Type; // EPacketType
};

USTRUCT(BlueprintType)
struct FServerBullet
{
    GENERATED_BODY()

    UPROPERTY()
    FPacketHeader Header;
    //1총알, 2캐릭터 위치

    UPROPERTY()
    int32 BulletId;
    
    UPROPERTY()
    float X;
    UPROPERTY()
    float Y;
    UPROPERTY()
    float Z;
    UPROPERTY()
    float DirX;
    UPROPERTY()
    float DirY;
    UPROPERTY()
    float DirZ;
    UPROPERTY()
    float Speed;
    UPROPERTY()
    double Sendtime;    

    UPROPERTY()
    bool flag;
};

USTRUCT(BlueprintType)
struct FCharacterPacket
{
    GENERATED_BODY()

    UPROPERTY()
    FPacketHeader Header;
    //1총알, 2캐릭터 위치

    UPROPERTY()
    int32 CharacterId;

    UPROPERTY()
    float X;
    UPROPERTY()
    float Y;
    UPROPERTY()
    float Z;
    UPROPERTY()
    float DirX;
    UPROPERTY()
    float DirY;
    UPROPERTY()
    float DirZ;
    UPROPERTY()
    float Speed;
    UPROPERTY()
    double Sendtime;

};

/*
USTRUCT(BlueprintType)
struct FServerBulletPos
{
    GENERATED_BODY()

    UPROPERTY()
    int32 Header;
    //1총알, 2캐릭터 위치
    
    UPROPERTY()
    int32 BulletId;
    
    UPROPERTY()
    float X;

    UPROPERTY()
    float Y;

    UPROPERTY()
    float Z;

    UPROPERTY()
    float DX=0;

    UPROPERTY()
    float DY=0;

    UPROPERTY()
    float DZ=0;

    UPROPERTY()
    float Speed=0;
   
    UPROPERTY()
    double DeltaTime;
};*/