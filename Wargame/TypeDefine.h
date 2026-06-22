#pragma once

#include "CoreMinimal.h"

#include "TypeDefine.generated.h"

class AActor;
class UMyItem;

enum class EPacketType : int
{
    Bullet = 1,
    Character = 2,
    InformationText = 3,
    Grenade = 4,
    AiSpawn =5,
    Damage=6,
    Client=7,
    Death=8,
    Melee=9,
    Redzone=10,
    BlueHole=11,
    Item=12,
    BoradCast=13,
    Vehicle = 14,
    Room = 15
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
    Weapon = 0,
    Character = 1,
    Hit = 2,
    Grenade = 3
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Rifle = 0,
    HealKit = 1,
    Melee = 2,
    FlashGrenade = 3,
    Grenade = 4
};

UENUM(BlueprintType)
enum class EPlayerEffect : uint8
{
    Red = 0,
    Blue = 1,
    White = 2,
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
    //1총알, 2캐릭터 위치, 9펀치

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

    UPROPERTY()
    bool IsDeath;

    UPROPERTY()
    bool IsHeal;
    UPROPERTY()
    EWeaponType WeaponType;
    UPROPERTY()
    bool AimActive;
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

    // ✅ 스폰할 블루프린트 클래스
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<AActor> RealActiveActorClass;

};


USTRUCT(BlueprintType)
struct FInformationTextPacket
{
    GENERATED_BODY()

    UPROPERTY()
    FPacketHeader Header;
    //1총알, 2캐릭터 위치

    UPROPERTY()
    int32 time;    

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
struct FItemPacket
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
    int32 OwnerId;

    UPROPERTY()
    int32 ItemRow;
    UPROPERTY()
    int32 ItemSpawnID;

    UPROPERTY()
    bool ShouldRemove;
};

USTRUCT(BlueprintType)
struct FItems
{
    GENERATED_BODY()

    UPROPERTY()
    int itemRow;
    UPROPERTY()
    int count;
};

USTRUCT(BlueprintType)
struct FDamagePacket
{
    GENERATED_BODY()

    UPROPERTY()
    FPacketHeader Header;

    UPROPERTY()
    EPlayerEffect Effect;
    UPROPERTY()
    int CharacterId;
    UPROPERTY()
    int Damage;    
    UPROPERTY()
    FItems HaveItems[20];
};


USTRUCT(BlueprintType)
struct FConnectionPacket
{
    GENERATED_BODY()

    UPROPERTY()
    FPacketHeader Header;
    UPROPERTY()
    int order;


    char Name[8];
};

USTRUCT(BlueprintType)
struct FDeathPacket
{
    GENERATED_BODY()

    UPROPERTY()
    FPacketHeader Header;

    UPROPERTY()
    int aiid;
    UPROPERTY()
    int characterid;

    char  Name[8];

    char   CausedName[8];
    UPROPERTY()
    int causedCharacterId;
};

USTRUCT(BlueprintType)
struct FInventorySlot
{
    GENERATED_BODY()

    UPROPERTY()
    int32 ItemID = -1;

    UPROPERTY()
    AActor* Actor = nullptr;

    UPROPERTY()
    UMyItem* Widget = nullptr;
    UPROPERTY()
    int32 ItemSpanwID = 0;
};

USTRUCT(BlueprintType)
struct FEnemyState
{
    GENERATED_BODY()

    UPROPERTY()
    float Speed;
    UPROPERTY()
    FVector Position;
    UPROPERTY()
    FVector Direction;
    UPROPERTY()
    bool  isjump;
    UPROPERTY()
    bool   isfire;
    UPROPERTY()
    bool isdeath;
    UPROPERTY()
    bool isHeal;
    UPROPERTY()
    EWeaponType WeaponType;
    UPROPERTY()
    bool AimActive;
};

USTRUCT(BlueprintType)
struct FBrodcastMessage
{
    GENERATED_BODY()

    UPROPERTY()
    FPacketHeader Header;   

    char   Information[128];
    
};

USTRUCT(BlueprintType)
struct FSubItemData
{
    GENERATED_BODY()


    // 수류탄 클래스 저장
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> SubItemClass;

    // 수류탄 id
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Id = 0;
};

USTRUCT(BlueprintType)
struct FVehiclePacket
{
    GENERATED_BODY()

    UPROPERTY()
    FPacketHeader Header;
    //1총알, 2캐릭터 위치

    UPROPERTY()
    int32 carId;

    UPROPERTY()
    int32 ownerId;

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
    bool IsHaveCharacter;
};
UENUM(BlueprintType)
enum class ERoomPacketType : uint8
{
    None = 0,
    CreateRoom = 1,  // 방 생성 요청
    JoinRoom = 2,  // 방 참가 요청
    RoomList = 3,  // 방 목록 조회
    StartGame = 4,  // 시작 버튼 (방장만)
    RoomInfo = 5,  // 방 정보 응답 (서버→클라)
    RemoveRoom = 6,
};


USTRUCT(BlueprintType)
struct FRoomPacket
{
    GENERATED_BODY()

    UPROPERTY()
    FPacketHeader Header;

    UPROPERTY()
    ERoomPacketType RoomType;

    UPROPERTY()
    int32 RoomId;

    
    char RoomName[16];

    UPROPERTY()
    int32 PlayerCount;

    UPROPERTY()
    int32 HostId;
};

