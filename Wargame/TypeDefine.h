#pragma once

#include "CoreMinimal.h"

#include "TypeDefine.generated.h"

enum class EPacketType : int
{
    Bullet = 1,
    Character = 2,
    InformationText = 3,
    Grenade = 4,
    AiSpawn =5,
    Damage=6
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
    Weapon = 0,
    Character = 1,
    Hit = 2,
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

    UPROPERTY()
    bool IsJump;
    UPROPERTY()
    bool IsFire;

};
USTRUCT(BlueprintType)
struct FItemStaticData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* Icon;

    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EItemType ItemType;

    // ✅ 스폰할 블루프린트 클래스
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<AActor> ItemActorClass;

};


USTRUCT(BlueprintType)
struct FInformationTextPacket
{
    GENERATED_BODY()

    UPROPERTY()
    FPacketHeader Header;
    //1총알, 2캐릭터 위치

    UPROPERTY()
    bool time;    

};

USTRUCT(BlueprintType)
struct FGrenadePacket
{
    GENERATED_BODY()

    UPROPERTY()
    FPacketHeader Header;
    //1총알, 2캐릭터 위치
    UPROPERTY()
    int32 GrenadeId;

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
    float Power;

};

USTRUCT(BlueprintType)
struct FSpawnAIPacket
{
    GENERATED_BODY()
    
    UPROPERTY()
    FPacketHeader Header;

    UPROPERTY()
    float X;
    UPROPERTY()
    float Y;
    UPROPERTY()
    float Z;

    UPROPERTY()
    int32 AiID; 
};

USTRUCT(BlueprintType)
struct FDamagePacket
{
    GENERATED_BODY()

    UPROPERTY()
    FPacketHeader Header;
    UPROPERTY()
    int CharacterId;
    UPROPERTY()
    int Damage;    
};

