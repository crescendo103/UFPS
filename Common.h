// Common.h
#pragma once
#include <winsock2.h>
#include <string>
#include <vector>

enum class EPacketType : int
{
    Bullet = 1,
    Character = 2,
    InformationText = 3,
    Grenade = 4,
    AiSpawn = 5,
    Damage = 6,
    Client = 7,
    Death = 8,
    Melee = 9,
    Redzone = 10,
    BlueHole = 11,
    Item = 12,
    BoradCast = 13,
    Vehicle = 14,
    Room = 15
};

struct Items
{
    int itemRow;
    int count;
};

struct FPacketHeader
{
    int Size;
    int Type;
};

enum class WeaponType : uint8_t
{
    Melee = 0,
    Rifle = 1,
    Grenade = 2,
    HealKit = 3
};

struct ServerBullet
{
    FPacketHeader Header;
    int BulletId;
    int CharacterId;
    float X, Y, Z;
    float DirX, DirY, DirZ;
    float Speed;
    double Sendtime;
    bool flag;
};

struct FCharacterPacket
{
    FPacketHeader Header;
    int CharacterId;
    float X, Y, Z;
    float DirX, DirY, DirZ;
    float Speed;
    double Sendtime;
    bool IsJump;
    bool IsFire;
    bool IsDeath;
    bool IsHeal;
    WeaponType WeaponType;
    bool AimActive;
};

struct FInforamtionTextPacket
{
    FPacketHeader Header;
    int connect;
};

struct FConnectionPacket
{
    FPacketHeader Header;
    int order;
    char Name[8];
};

struct FGrenadePacket
{
    FPacketHeader Header;
    int GrenadeId;
    int CharacterId;
    float X, Y, Z;
    float DirX, DirY, DirZ;
    float Power;
};

struct FSpawnAIPacket
{
    FPacketHeader Header;
    float X, Y, Z;
    int AiID;
};

struct FItemPacket
{
    FPacketHeader Header;
    float X, Y, Z;
    int OwnerId;
    int ItemRow;
    int ItemSpawnID;
    bool ShouldRemove;
};

struct FDeathPacket
{
    FPacketHeader Header;
    int aiid;
    int characterid;
    char Name[8];
    char CausedName[8];
    int causedCharacterId;
    Items HaveItems[20];
};

struct FBrodcastMessage
{
    FPacketHeader Header;
    char Information[128];
};

struct FVehiclePacket
{
    FPacketHeader Header;
    int carId;
    int ownerId;
    float X, Y, Z;
    float DirX, DirY, DirZ;
    bool IsHaveCharacter;
};

struct PlayerInfo
{
    char Name[8];
    Items HaveItems[20];
    int itemCount;
};

enum class ERoomPacketType : uint8_t
{
    None = 0,
    CreateRoom = 1,  // 방 생성 요청
    JoinRoom = 2,  // 방 참가 요청
    RoomList = 3,  // 방 목록 조회
    StartGame = 4,  // 시작 버튼 (방장만)
    RoomInfo = 5,  // 방 정보 응답 (서버→클라)
    RemoveRoom = 6,
};

struct FRoomPacket
{
    FPacketHeader Header;
    ERoomPacketType RoomType;
    int RoomId;       // 참가/시작 시 사용
    char RoomName[16];
    int PlayerCount;  // 현재 인원 (서버→클라 응답용)
    int HostId;       // 방장 클라이언트 ID
};