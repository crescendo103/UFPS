#include <iostream>
#include <vector>
#include <algorithm>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <mutex>
#include <queue>
#include "AIEnemyPositionManager.h"
#include <conio.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

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
    Melee =9,
    Redzone = 10,
    BlueHole = 11,
    Item=12
};

struct Items
{
    int itemRow;
    int count;
};

enum class EPlayerEffect
{
    Red = 0,
    Blue = 1,
    White = 2,
};

struct FPacketHeader
{        
    int Size; // 전체 패킷 크기    
    int Type; // EPacketType
};

struct ServerBullet
{   
    FPacketHeader Header;
    //1총알, 2캐릭터 위치
    int BulletId;//삭제요청만
    int CharacterId;
    float X;
    float Y;
    float Z;
    float DirX;
    float DirY;
    float DirZ;
    float Speed;
    double Sendtime;
    bool flag;
};

struct FCharacterPacket
{       
    FPacketHeader Header;
    //1총알, 2캐릭터 위치    
    int CharacterId;    
    float X;    
    float Y;    
    float Z; 
    float DirX;//ai 경우 타겟 x    
    float DirY;//y    
    float DirZ;//z   
    float Speed;    
    double Sendtime;
    bool IsJump;//ai 경우 도착여부
    bool IsFire;//ai 경우 타겟 여부
    bool IsDeath;//죽음 여부
    bool IsHeal;
};

struct FInforamtionTextPacket
{
    FPacketHeader Header;
    //1총알, 2캐릭터 위치    
    int connect;    
};

struct FConnectionPacket
{
    FPacketHeader Header;
    //1총알, 2캐릭터 위치    
    int order;
    char Name[8];
};

struct FGrenadePacket
{
    FPacketHeader Header;
    //1총알, 2캐릭터 위치    
    int GrenadeId;
    int CharacterId;
    float X;
    float Y;
    float Z;
    float DirX;
    float DirY;
    float DirZ;

    float Power;

};

struct FSpawnAIPacket
{
    FPacketHeader Header;

    float X;
    
    float Y;
    
    float Z;

    int AiID; // 어느 플레이어 시점인지
};

struct FItemPacket
{
    FPacketHeader Header;

    float X;

    float Y;

    float Z;    

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
    char  Name[8]; // 최대 7글자 + null
    char  CausedName[8];
    int causedCharacterId;    
    Items HaveItems[20];
};

struct BulletData {
    int Header;
    Vector3 Position;
    //Vector3 Forward;
    Vector3 Velocity;
};

int clientSocketsindex = 0;
SOCKET clientSockets[2];
int LoginCount = 0;

//unordered_map <int, ServerBullet> Tree;
unordered_map <int, FCharacterPacket> People;
unordered_map <int, FCharacterPacket> AIPacket;
unordered_map <int, FGrenadePacket> Grenades;
std::queue<FGrenadePacket> GrenadeQueue;
std::unordered_map<SOCKET, int> SocketToClientId;

int nextAiId = -110;
AIEnemyPositionManager PosMgr;
std::queue<FCharacterPacket> AIQueue;

std::queue<FInforamtionTextPacket> TimeQueue;
std::queue<FDeathPacket> DeathQueue;
std::queue<ServerBullet> MeleeQueue;
std::queue<ServerBullet> BulletQueue;
std::queue<FItemPacket> ItemQueue;

std::mutex BulletMutex;
std::mutex CharacterMutex;
std::mutex AIQMutex;

std::mutex TimQMutex;
std::mutex DeathMutex;
std::mutex MeleeMutex;
std::mutex SendMutex;
std::mutex GrenadeMutex;


struct PlayerInfo
{
    char Name[8];
    //vector<Items> HaveItems;
    Items HaveItems[20];
    int itemCount; // 현재 서로 다른 아이템 개수
};
unordered_map <int, PlayerInfo> Players;

/*
strncpy_s(Players[id].Name, "Monster", 7);
Players[id].Name[7] = '\0';
*/

void PrintBullet(const ServerBullet& B)
{
    printf("===== ServerBullet =====\n");
    printf("BulletId : %d\n", B.BulletId);
    printf("Pos      : (%.2f, %.2f, %.2f)\n", B.X, B.Y, B.Z);
    printf("Dir      : (%.2f, %.2f, %.2f)\n", B.DirX, B.DirY, B.DirZ);
    printf("Speed    : %.2f\n", B.Speed);
    printf("Sendtime    : %.4f\n", B.Sendtime);  
    printf("flag : %s\n", B.flag ? "true" : "false");
    printf("========================\n");
}
bool SendAll(SOCKET sock, const char* data, int size)
{
    int totalSent = 0;

    while (totalSent < size)
    {
        int sent = send(sock, data + totalSent, size - totalSent, 0);

        if (sent == SOCKET_ERROR)
        {
            int err = WSAGetLastError();

            if (err == WSAEWOULDBLOCK)
            {
                // ✅ 서버도 동일하게
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            printf("❌ send error: %d\n", err);
            return false;
        }

        if (sent == 0)
        {
            printf("❌ send returned 0 (connection closed)\n");
            return false;
        }

        totalSent += sent;
    }

    return true;
}
struct FRecvBuffer
{
    static constexpr int BUFFER_SIZE = 8192;

    char Buffer[BUFFER_SIZE];
    int ReadPos = 0;
    int WritePos = 0;

    int GetDataSize() { return WritePos - ReadPos; }
    char* GetReadPtr() { return Buffer + ReadPos; }
    char* GetWritePtr() { return Buffer + WritePos; }

    void OnWrite(int Len)
    {
        WritePos += Len;
    }

    void OnRead(int Len)
    {
        ReadPos += Len;
        if (ReadPos == WritePos)
        {
            ReadPos = WritePos = 0;
        }
    }

};



int bulletId = 0;
int GrenadeId = 0;
int manId = 0;

void dataProcess(FPacketHeader* Header, SOCKET soc) {
    printf("dataProcess 들어옴 Type = %d\n", Header->Type);

    switch ((EPacketType)Header->Type)
    {
    case EPacketType::Bullet:
    {
        ServerBullet* Bullet =
            reinterpret_cast<ServerBullet*>(Header);

        printf("[Bullet] Id=%d Pos=(%.2f %.2f %.2f) CharacterId=%d\n",
            Bullet->BulletId,
            Bullet->X,
            Bullet->Y,
            Bullet->Z,
            Bullet->CharacterId
        );

        ServerBullet data;
        data.Header.Type = Bullet->Header.Type;
        data.Header.Size = Bullet->Header.Size;
        data.BulletId = Bullet->BulletId;
        data.CharacterId = Bullet->CharacterId;
        data.X = Bullet->X;
        data.Y = Bullet->Y;
        data.Z = Bullet->Z;
        // ⭐⭐⭐ 이 줄들이 핵심 ⭐⭐⭐
        data.DirX = Bullet->DirX * Bullet->Speed;
        data.DirY = Bullet->DirY * Bullet->Speed;
        data.DirZ = Bullet->DirZ * Bullet->Speed;
        data.Speed = Bullet->Speed;
        data.Sendtime = Bullet->Sendtime;
        data.flag = true;//의미없는 값

        {
            std::lock_guard<std::mutex> lock(BulletMutex);
            BulletQueue.push(data);
        } // 여기서 lock 객체 소멸 → mutex 자동 unlock              

        break;
    }

    case EPacketType::Character:
    {
        FCharacterPacket* Char =
            reinterpret_cast<FCharacterPacket*>(Header);
       // {
        //    std::lock_guard<std::mutex> lock(CharacterMutex);

        if(Char->CharacterId>-100){
        int clientId = SocketToClientId[soc];
            if (Char->CharacterId != -1 && People.find(clientId) != People.end()) {//이미존재한다면        
                
                
                    printf("[Character] Id=%d Pos=(%.2f %.2f %.2f)Dir=(%.2f %.2f %.2f) heal bool %d\n",
                        Char->CharacterId,
                        Char->X,
                        Char->Y,
                        Char->Z,
                        Char->DirX,
                        Char->DirY,
                        Char->DirZ,
                        Char->IsHeal
                    );
                
                People[clientId] = *Char;
            }
            else {
                
                FCharacterPacket data2;
                data2.Header = Char->Header;
                data2.CharacterId = clientId;
                data2.X = Char->X;
                data2.Y = Char->Y;
                data2.Z = Char->Z;
                data2.DirX = Char->DirX;
                data2.DirY = Char->DirY;
                data2.DirZ = Char->DirZ;
                data2.Speed = Char->Speed;
                data2.Sendtime = Char->Sendtime;
                data2.IsJump = Char->IsJump;
                data2.IsFire = Char->IsFire;
                data2.IsDeath = Char->IsDeath;
                data2.IsHeal=Char->IsHeal;
                People[clientId] = data2;
            }
        }//ai 내용
        else {
            printf("Ai 분기점에 들어는 왔음\n");
            if (AIPacket.find(Char->CharacterId) != AIPacket.end()) {//이미존재한다면        
                
                printf("[AI] 들어온 Id=%d Pos=(%.2f %.2f %.2f)Dir=(%.2f %.2f %.2f) is jumping bool = %d  fire bool %d \n",
                    Char->CharacterId,
                    Char->X,
                    Char->Y,
                    Char->Z,
                    Char->DirX,
                    Char->DirY,
                    Char->DirZ,
                    Char->IsJump,
                    Char->IsFire
                );
                

                //도착함
                //클라가 두개라 두번올텐데 어떻게하지
                if (Char->IsJump) {
                    PosMgr.RandomPositionWithScale();//번호생성
                    Vector3 newPos = PosMgr.GetRandPos();//번호얻기
                    
                    AIPacket[Char->CharacterId].DirX = newPos.x;
                    AIPacket[Char->CharacterId].DirY = newPos.y;
                    AIPacket[Char->CharacterId].DirZ = newPos.z;
                    AIPacket[Char->CharacterId].IsJump = false;
                    AIPacket[Char->CharacterId].IsDeath = false;
                    AIPacket[Char->CharacterId].IsHeal = false;
                    printf("[AI] 나갈 Id = % d Pos = (% .2f % .2f % .2f) jumping bool = % d\n", 
                        Char->CharacterId,
                        AIPacket[Char->CharacterId].DirX,
                        AIPacket[Char->CharacterId].DirY,
                        AIPacket[Char->CharacterId].DirZ,
                        AIPacket[Char->CharacterId].IsJump
                    );
                }

                if (Char->IsFire) {
                    

                    AIPacket[Char->CharacterId].DirX = Char->DirX;
                    AIPacket[Char->CharacterId].DirY = Char->DirY;
                    AIPacket[Char->CharacterId].DirZ = Char->DirZ;
                    AIPacket[Char->CharacterId].IsJump = false;
                    AIPacket[Char->CharacterId].IsFire = false;
                    AIPacket[Char->CharacterId].IsHeal = false;
                    printf("[AI] 나갈 Id = % d Pos = (% .2f % .2f % .2f) jumping bool = % d fire bool = %d\n",
                        Char->CharacterId,
                        AIPacket[Char->CharacterId].DirX,
                        AIPacket[Char->CharacterId].DirY,
                        AIPacket[Char->CharacterId].DirZ,
                        AIPacket[Char->CharacterId].IsJump,
                        AIPacket[Char->CharacterId].IsFire
                    );
                }
                
                //걍 무시하면 될듯
                //People[Char->CharacterId] = *Char;

                
            }
            else {//처음 도착

                

                FCharacterPacket data2;
                data2.Header = Char->Header;
                data2.CharacterId = Char->CharacterId;
                data2.X = Char->X;
                data2.Y = Char->Y;
                data2.Z = Char->Z;
                data2.DirX = Char->DirX;
                data2.DirY = Char->DirY;
                data2.DirZ = Char->DirZ;
                data2.Speed = Char->Speed;
                data2.Sendtime = Char->Sendtime;
                data2.IsJump = Char->IsJump;
                data2.IsFire = Char->IsFire;
                data2.IsDeath = Char->IsDeath;
                data2.IsHeal = Char->IsHeal;
                AIPacket[Char->CharacterId] = data2;

                if (Char->IsJump) {
                    PosMgr.RandomPositionWithScale();//번호생성
                    Vector3 newPos = PosMgr.GetRandPos();//번호얻기

                    AIPacket[Char->CharacterId].DirX = newPos.x;
                    AIPacket[Char->CharacterId].DirY = newPos.y;
                    AIPacket[Char->CharacterId].DirZ = newPos.z;
                    AIPacket[Char->CharacterId].IsJump = false;
                    AIPacket[Char->CharacterId].IsHeal = false;
                }

                printf("[AI]가 처음 도착했고 나갈 Id = % d Pos = (% .2f % .2f % .2f) jumping bool = % d\n",
                    Char->CharacterId,
                    AIPacket[Char->CharacterId].DirX,
                    AIPacket[Char->CharacterId].DirY,
                    AIPacket[Char->CharacterId].DirZ,
                    AIPacket[Char->CharacterId].IsJump
                );
            }
            

            {
                std::lock_guard<std::mutex> lock(AIQMutex);
                AIQueue.push(AIPacket[Char->CharacterId]);
            }
        }
        break;
    }
    case EPacketType::Grenade:
    {
        int clientId = SocketToClientId[soc];
        FGrenadePacket* packet =
            reinterpret_cast<FGrenadePacket*>(Header);

        printf("[Grenade] Grenade Id %d CharacterId %d power=%f pos =(%.2f %.2f %.2f) dir=(%.2f %.2f %.2f)\n",
            packet->GrenadeId,
            packet->CharacterId,
            packet->Power,
            packet->X,
            packet->Y,
            packet->Z,
            packet->DirX,
            packet->DirY,
            packet->DirZ
        );
        //Grenades[clientId] = *packet;//?안씀
        packet->CharacterId = clientId;
        packet->GrenadeId = GrenadeId;
        Grenades[GrenadeId] = *packet;
        GrenadeId++;
        //받음과 동시에 송신
        GrenadeQueue.push(*packet);

        break;
    }   
    case EPacketType::InformationText:
    {
        FInforamtionTextPacket* timepacket = reinterpret_cast<FInforamtionTextPacket*>(Header);
        printf("timepacket = %d\n",
            timepacket->connect           
        );
        TimeQueue.push(*timepacket);

        // 👉 여기서 5초 뒤에 10을 다시 넣는다
        std::thread([soc]()
            {
                std::this_thread::sleep_for(std::chrono::seconds(5));

                FInforamtionTextPacket packet;
                packet.Header.Size = sizeof(FInforamtionTextPacket);
                packet.Header.Type = static_cast<int>(EPacketType::InformationText);
                packet.connect = 10;

                {
                    std::lock_guard<std::mutex> lock(TimQMutex);
                    TimeQueue.push(packet);   // ✅ 여기로 넣기
                }

                printf("[Thread] send timepacket = 10\n");

            }).detach();


        break;
    }
    case EPacketType::Death:
    {
        FDeathPacket* rip =
            reinterpret_cast<FDeathPacket*>(Header);

        
        printf("[Death] AI:%d Char:%d Name:%s -> Killer:%s (%d)\n",
            rip->aiid,
            rip->characterid,
            rip->Name,
            rip->CausedName,
            rip->causedCharacterId
        );

        FDeathPacket packet;
        packet.Header.Size = sizeof(FDeathPacket);
        packet.Header.Type = static_cast<int>(EPacketType::Death);
        packet.aiid = rip->aiid;
        packet.characterid = rip->characterid;
        packet.causedCharacterId = rip->causedCharacterId;

        printf("===== Players Map Dump =====\n");

        for (const auto& pair : Players)
        {
            int id = pair.first;
            const PlayerInfo& player = pair.second;

            printf("ID: %d, Name: %s\n", id, player.Name);
        }

        printf("================\n");
        {
            // Players에서 CharacterId로 찾기
            auto it = Players.find(rip->characterid);
            if (it != Players.end())
            {
                // 이름 복사 (안전하게)
                strncpy_s(packet.Name, sizeof(packet.Name), it->second.Name, _TRUNCATE);
                //strncpy(packet.Name, it->second.Name, sizeof(packet.Name) - 1);
                packet.Name[sizeof(packet.Name) - 1] = '\0'; // null 보장
                memcpy(packet.HaveItems, it->second.HaveItems, sizeof(packet.HaveItems));//아이템꾸러미 정보
            }
            else
            {
                  // 못 찾았을 경우 기본값
                strncpy_s(packet.Name, sizeof(packet.Name), "Unknown", _TRUNCATE);
                packet.Name[sizeof(packet.Name) - 1] = '\0';
            }
        }

        {
            // Players에서 causedCharacterId로 찾기
            auto it = Players.find(rip->causedCharacterId);
            if (it != Players.end())
            {
                // 이름 복사 (안전하게)
                strncpy_s(packet.CausedName, sizeof(packet.CausedName), it->second.Name, _TRUNCATE);
                packet.CausedName[sizeof(packet.CausedName) - 1] = '\0'; // null 보장
            }
            else
            {
                // 못 찾았을 경우 기본값
                //strncpy(packet.CausedName, "Unknown", sizeof(packet.CausedName) - 1);
                strncpy_s(packet.CausedName, sizeof(packet.CausedName), "Unknown", _TRUNCATE);
                packet.CausedName[sizeof(packet.CausedName) - 1] = '\0';
            }
        }

        {
            std::lock_guard<std::mutex> lock(DeathMutex);
            DeathQueue.push(packet);
        }
        break;
    }
    case EPacketType::Melee:
    {
        ServerBullet* Melee =
            reinterpret_cast<ServerBullet*>(Header);

        printf("[Melee] Id=%d Pos=(%.2f %.2f %.2f) CharacterId=%d\n",
            Melee->BulletId,
            Melee->X,
            Melee->Y,
            Melee->Z,
            Melee->CharacterId
        );

        ServerBullet Punch;
        Punch.Header.Type = Melee->Header.Type;
        Punch.Header.Size = Melee->Header.Size;
        Punch.BulletId = Melee->BulletId;
        Punch.CharacterId = Melee->CharacterId;
        Punch.X = Melee->X;
        Punch.Y = Melee->Y;
        Punch.Z = Melee->Z;
        
        Punch.DirX = Melee->DirX * Melee->Speed;//의미없는 값
        Punch.DirY = Melee->DirY * Melee->Speed;//의미없는 값
        Punch.DirZ = Melee->DirZ * Melee->Speed;//의미없는 값
        Punch.Speed = Melee->Speed;//의미없는 값
        Punch.Sendtime = Melee->Sendtime;//의미없는 값
        Punch.flag = true;//의미없는 값

        {
            std::lock_guard<std::mutex> lock(MeleeMutex);
            MeleeQueue.push(Punch);
        } // 여기서 lock 객체 소멸 → mutex 자동 unlock              

        break;
    }case EPacketType::Client:
    {
        FConnectionPacket* connection =
            reinterpret_cast<FConnectionPacket*>(Header);

        printf("Connection Recv and CharacterId: %d, Name: %.8s\n", connection->order, connection->Name);
        PlayerInfo PN{};
        memset(&PN, 0, sizeof(PlayerInfo));  // ⭐ 핵심
        memcpy(PN.Name, connection->Name, 8);

        Players.insert({ connection->order, PN });

        LoginCount++;

        //대기열 적 움직여라..
        for (int i = 0; i < LoginCount; i++)
        {
            FConnectionPacket data;
            data.Header.Size = sizeof(FConnectionPacket);
            data.Header.Type = static_cast<int>(EPacketType::Client);
            data.order = LoginCount;
            strncpy_s(data.Name, "temp", 7);
            data.Name[7] = '\0';

            char  SendBuffConnection[sizeof(FConnectionPacket)]; // 패킷을 저장할 변수
            memcpy(SendBuffConnection, &data, sizeof(FConnectionPacket));


            int nSendLen = send(clientSockets[i],
                SendBuffConnection,
                sizeof(FConnectionPacket),
                0);

            if (nSendLen == SOCKET_ERROR)
            {
                printf("send people failed: %d\n", WSAGetLastError());
            }
        }

        break;
    }
    case EPacketType::Item:
    {
        FItemPacket* ItemPacket =
            reinterpret_cast<FItemPacket*>(Header);
        printf("ItemPacket raw:\n");
        printf("OwnerId = %d\n", ItemPacket->OwnerId);
        printf("ItemRow = %d\n", ItemPacket->ItemRow);
        printf("SpawnID = %d\n", ItemPacket->ItemSpawnID);
        printf("OwnerId: %d, ItemRow: %d\n",
            ItemPacket->OwnerId, ItemPacket->ItemRow);

        auto it = Players.find(ItemPacket->OwnerId);
        // 플레이어 
        if (it == Players.end())
        {
            printf("OwnerId is not mapped");
            break;
        }

        //제거명령
        if (ItemPacket->ShouldRemove) {//이거 만족하는거 없음
            //bool bFound = false;
            printf("제거명령 실행\n");
            for (auto& item : it->second.HaveItems)
            {
                if (item.itemRow == ItemPacket->ItemRow)
                {
                    item.count=0;
                    ItemQueue.push(*ItemPacket);//다른클라에서 아이템위치하라는 명령
                    //bFound = true;
                    break;
                }
            }            
        }
        else {//저장명령
            printf("저장명령 실행\n");
            bool bFound = false;
            for (auto& item : it->second.HaveItems)
            {
                if (item.itemRow == ItemPacket->ItemRow)
                {
                    item.count++;
                    it->second.itemCount++;//
                    bFound = true;
                    break;
                }
            }

            if (!bFound)
            {
                Items newItem;
                newItem.itemRow = ItemPacket->ItemRow;
                newItem.count = 1;

                //it->second.HaveItems.push_back(newItem);

                auto& player = it->second;//

                if (player.itemCount < 20)
                {
                    player.HaveItems[player.itemCount] = newItem;
                    player.itemCount++;
                }//
            }
            ItemPacket->ShouldRemove = true;
            ItemQueue.push(*ItemPacket);//다른클라에서 지워달라는 요청
        }
        
        

        printf("아이템 패킷 받음\n");
        

            
        

        break;
    }
    default:
        printf("Unknown packet\n");
        break;
    }
}


void recivePacket(SOCKET ClientSocket) {
    
    FRecvBuffer RecvBuf;

    int lastPacketSize = 0;
    int lastPacketType = -1;

    printf("recivePacket before\n");
    while (true)
    {
        //printf("recv before\n");
        //FRecvBuffer RecvBuf;
        int RecvLen = recv(
            ClientSocket,
            RecvBuf.GetWritePtr(),
            FRecvBuffer::BUFFER_SIZE - RecvBuf.WritePos,
            0
        );
        //printf("recv after\n");
        /*
        if (RecvLen <= 0)
        {
            printf("❌ recv failed or disconnected\n");
            return;
        }
        */
        if (RecvLen == 0)
        {
            printf("❌ disconnected\n");
            printf("⚠️ last packet size=%d type=%d\n", lastPacketSize, lastPacketType);

            return;
        }
        else if (RecvLen < 0)
        {
            int err = WSAGetLastError();

            if (err == WSAEWOULDBLOCK)
            {
                // 👉 지금 받을 데이터 없음 → 그냥 다시 루프
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            else
            {
                printf("❌ recv error: %d\n", err);
                printf("⚠️ last packet size=%d type=%d\n", lastPacketSize, lastPacketType);

                return;
            }
        }

        RecvBuf.OnWrite(RecvLen);
        //printf("FPacketHeader before\n");
        // 헤더도 못 읽으면 대기
        if (RecvBuf.GetDataSize() < sizeof(FPacketHeader))
            continue;
        // 헤더는 읽힘
        if (RecvBuf.GetDataSize() >= sizeof(FPacketHeader))
        {
            FPacketHeader* Header =
                reinterpret_cast<FPacketHeader*>(RecvBuf.GetReadPtr());
            //printf("GetDataSize before\n");

            // ⭐ 진행 중 패킷 정보 기록
            lastPacketSize = Header->Size;
            lastPacketType = Header->Type;

            int currentSize = RecvBuf.GetDataSize();

            // 패킷 전체가 아직 안 왔으면 대기
            if (RecvBuf.GetDataSize() < Header->Size)
            {
                printf("⏳ partial packet: need=%d, current=%d\n",
                    Header->Size, currentSize);
                continue;

            }


            //printf("dataProcess before\n");
            // 패킷 하나 완성 → 처리

            dataProcess(Header, ClientSocket);
            //printf("dataProcess after\n");
            // 읽은 만큼 제거
            RecvBuf.OnRead(Header->Size);
        }
    }
    //printf("recivePacket out\n");
}





int main() {
    
    

    WSADATA wsaData;
    int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (nRet != 0)
    {
        std::cout << "WSAStartup failed\n";
        return 1;
    }

    printf("sizeof ServerBullet = %zu\n", sizeof(ServerBullet));

    // 1️⃣ 리슨 소켓 생성
    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ListenSocket == INVALID_SOCKET)
    {
        std::cout << "socket() failed\n";
        WSACleanup();
        return 1;
    }

    // 2️⃣ 주소 설정
    SOCKADDR_IN ServerAddr{};
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(6000);
    ServerAddr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0

    // 3️⃣ bind
    nRet = bind(ListenSocket,
        (sockaddr*)&ServerAddr,
        sizeof(ServerAddr));

    if (nRet == SOCKET_ERROR)
    {
        std::cout << "bind() failed\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // 4️⃣ listen
    nRet = listen(ListenSocket, SOMAXCONN);
    if (nRet == SOCKET_ERROR)
    {
        std::cout << "listen() failed\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    SOCKET ClientSocket;
    std::cout << "Server listening on port 6000...\n";
    while (true) {
        // 5️⃣ accept (여기서 블로킹)
        SOCKADDR_IN ClientAddr{};
        int AddrLen = sizeof(ClientAddr);

        ClientSocket = accept(
            ListenSocket,
            (sockaddr*)&ClientAddr,
            &AddrLen
        );

        //non block 설정
        if (ClientSocket != INVALID_SOCKET)
        {
            u_long mode = 1; // non-blocking
            ioctlsocket(ClientSocket, FIONBIO, &mode);
        }

        if (ClientSocket == INVALID_SOCKET)
        {
            std::cout << "accept() failed\n";
        }
        else
        {
            std::cout << "Client connected!\n";
            clientSockets[(clientSocketsindex)] = ClientSocket;
            SocketToClientId[ClientSocket] = clientSocketsindex;
            clientSocketsindex++;         
                      
                        

            std::thread t(recivePacket, ClientSocket);  // 스레드 생성
            //t.join();                  // 스레드 종료까지 대기    
            t.detach();


            if (clientSocketsindex >= 2) {
                break;
            }

        }
    }

    //연결 and myowner설정
    
    {        
        for (int i = 0; i < clientSocketsindex; i++)
        {
            FConnectionPacket data;
            data.Header.Size = sizeof(FConnectionPacket);
            data.Header.Type = static_cast<int>(EPacketType::Client);
            data.order = i;//clientSocketsindex - 1;
            strncpy_s(data.Name, "temp", 7);
            data.Name[7] = '\0';

            char  SendBuffConnection[sizeof(FConnectionPacket)]; // 패킷을 저장할 변수
            memcpy(SendBuffConnection, &data, sizeof(FConnectionPacket));


            int nSendLen = send(clientSockets[i],
                SendBuffConnection,
                sizeof(FConnectionPacket),
                0);

            if (nSendLen == SOCKET_ERROR)
            {
                printf("send people failed: %d\n", WSAGetLastError());
            }
        }
    }
    
    
    //ai 두개 생성
    for (int i = 0; i < 2; i++) {
        FSpawnAIPacket data;
        data.Header.Type = static_cast<int>(EPacketType::AiSpawn);
        data.Header.Size = sizeof(FSpawnAIPacket);
        data.X = -1600.750000;
        data.Y = -1059.250000;
        data.Z = 432.431034;
        data.AiID = nextAiId--;

        char  SendBuffAISpawn[sizeof(FSpawnAIPacket)]; // 패킷을 저장할 변수
        memcpy(SendBuffAISpawn, &data, sizeof(FSpawnAIPacket));
        for (auto& e : clientSockets)
        {
            int nSendLen = send(e,//클라이언트 
                SendBuffAISpawn,
                sizeof(FSpawnAIPacket),
                0);

            if (nSendLen == SOCKET_ERROR)
            {
                printf("send people failed: %d\n", WSAGetLastError());
                break;
            }
        }
    }
    

    printf("경기장 입장");
    /*
    char  SendTimeBuff[sizeof(FInforamtionTextPacket)];

    FPacketHeader header = { sizeof(FInforamtionTextPacket), (int)EPacketType::InformationText };
    FInforamtionTextPacket packet = { header, 10 };
    memcpy(SendTimeBuff, &packet, sizeof(FInforamtionTextPacket));
    for (int i = 0; i < 2; i++) {
        int nSendLen = send(clientSockets[i],//클라이언트 
            SendTimeBuff,
            sizeof(FInforamtionTextPacket),
            0);
    }*/

    
    
   
    char  SendBuff[sizeof(ServerBullet)]; // 패킷을 저장할 변수
    char  SendBuff2[sizeof(FCharacterPacket)]; // 패킷을 저장할 변수
    char  SendBuff3[sizeof(FGrenadePacket)]; // 패킷을 저장할 변수
    while (true)
    {

        if (_kbhit())
        {
            char key = _getch();

            if (key == '1')
            {
                printf("블루홀 생성\n");
                FSpawnAIPacket packet;
                packet.Header.Size = sizeof(FSpawnAIPacket);
                packet.Header.Type = static_cast<int>(EPacketType::BlueHole);
                PosMgr.RandomPositionWithScale();//번호생성
                Vector3 newPos = PosMgr.GetRandPos();//번호얻기
                packet.X = newPos.x;
                packet.Y = newPos.y;
                packet.Z = newPos.z;
                packet.AiID = -1;
                char  SendBuffAISpawn[sizeof(FSpawnAIPacket)]; // 패킷을 저장할 변수
                memcpy(SendBuffAISpawn, &packet, sizeof(FSpawnAIPacket));
                //ai 두개 생성
                for (int i = 0; i < 2; i++) {
                    for (auto& e : clientSockets)
                    {
                        int nSendLen = send(e,//클라이언트 
                            SendBuffAISpawn,
                            sizeof(FSpawnAIPacket),
                            0);

                        if (nSendLen == SOCKET_ERROR)
                        {
                            printf("send people failed: %d\n", WSAGetLastError());
                            break;
                        }
                    }
                }
            }

            if (key == '2')
            {
                printf("레드존 시작\n");
                FSpawnAIPacket packet;
                packet.Header.Size = sizeof(FSpawnAIPacket);
                packet.Header.Type = static_cast<int>(EPacketType::Redzone);
                PosMgr.RandomPositionWithScale();//번호생성
                Vector3 newPos = PosMgr.GetRandPos();//번호얻기
                packet.X = newPos.x;
                packet.Y = newPos.y;
                packet.Z = newPos.z;
                packet.AiID = -1;
                char  SendBuffAISpawn[sizeof(FSpawnAIPacket)]; // 패킷을 저장할 변수
                memcpy(SendBuffAISpawn, &packet, sizeof(FSpawnAIPacket));
                //ai 두개 생성
                for (int i = 0; i < 2; i++) {                 
                    for (auto& e : clientSockets)
                    {
                        int nSendLen = send(e,//클라이언트 
                            SendBuffAISpawn,
                            sizeof(FSpawnAIPacket),
                            0);

                        if (nSendLen == SOCKET_ERROR)
                        {
                            printf("send people failed: %d\n", WSAGetLastError());
                            break;
                        }
                    }
                }
            }

        }


        //std::vector<ServerBullet> SendList;
        std::vector<FCharacterPacket> SendList2;
        
        
        {
            std::lock_guard<std::mutex> lock(CharacterMutex);
            for (auto& pair : People) {
                int id = pair.first;
                FCharacterPacket& bd = pair.second;
                //사람 todo..
                FCharacterPacket pos2;
                pos2.Header.Size = sizeof(FCharacterPacket);;//bd.Header.Size;
                pos2.Header.Type = static_cast<int>(EPacketType::Character);//bd.Header.Type;
                pos2.CharacterId = id;
                pos2.X = bd.X;
                pos2.Y = bd.Y;
                pos2.Z = bd.Z;
                pos2.DirX = bd.DirX;
                pos2.DirY = bd.DirY;
                pos2.DirZ = bd.DirZ;
                pos2.Speed = bd.Speed;
                pos2.Sendtime = bd.Sendtime;
                pos2.IsJump = bd.IsJump;
                pos2.IsFire = bd.IsFire;
                pos2.IsDeath = bd.IsDeath;
                pos2.IsHeal = bd.IsHeal;
                //printf("x = %f Y= %f Z=%f\n", pos2.DirX, pos2.DirY, pos2.DirZ);
                SendList2.push_back(pos2);
            }
            

            
        }
        // 🔥 네트워크 전송은 락 밖에서
       while (!BulletQueue.empty()) {
           ServerBullet pos;
           {
               std::lock_guard<std::mutex> lock(BulletMutex);
               pos = BulletQueue.front();
               BulletQueue.pop();
           }
            memcpy(SendBuff, &pos, sizeof(ServerBullet));

            for (int i = 0; i < 2; i++) {
                if (pos.CharacterId == SocketToClientId[clientSockets[i]]) {
                    continue;
                }

                {
                    std::lock_guard<std::mutex> lock(SendMutex);

                    bool ok = SendAll(clientSockets[i],
                        SendBuff,
                        sizeof(ServerBullet));

                    if (!ok)
                    {
                        printf("send failed\n");
                        break;
                    }
                }
                /*
                int nSendLen = send(clientSockets[i],//클라이언트 두명에게 총알을 그린다.
                    SendBuff,
                    sizeof(ServerBullet),
                    0);

                if (nSendLen == SOCKET_ERROR)
                {
                    printf("send failed: %d\n", WSAGetLastError());
                    break;
                }*/
            }         
                       
        }

        

        for (const auto& pos : SendList2)
        {
            memcpy(SendBuff2, &pos, sizeof(FCharacterPacket));

            for (int i = 0; i < 2; i++) {
                if (pos.CharacterId == SocketToClientId[clientSockets[i]]) {
                    continue;
                }
                //printf("Header Size: %d\n", pos.Header.Size);
                {
                    std::lock_guard<std::mutex> lock(SendMutex);

                    bool ok = SendAll(clientSockets[i],
                        SendBuff2,
                        sizeof(FCharacterPacket));

                    if (!ok)
                    {
                        printf("send failed\n");
                        break;
                    }
                }/*
                int nSendLen = send(clientSockets[i],//클라이언트 
                    SendBuff2,
                    sizeof(FCharacterPacket),
                    0);

                if (nSendLen == SOCKET_ERROR)
                {
                    printf("send people failed: %d\n", WSAGetLastError());
                    break;
                }*/

            }           

            //PrintBulletPos(pos);

        }

        while (!GrenadeQueue.empty()) {
            

            
            FGrenadePacket packet = GrenadeQueue.front();
            GrenadeQueue.pop();

            memcpy(SendBuff3, &packet, sizeof(FGrenadePacket));

            for (int i = 0; i < 2; i++) {
                if (packet.CharacterId == SocketToClientId[clientSockets[i]]) {
                    continue;
                } 
                {
                    std::lock_guard<std::mutex> lock(SendMutex);


                    bool ok = SendAll(clientSockets[i],
                        SendBuff3,
                        sizeof(FGrenadePacket));

                    if (!ok)
                    {
                        printf("send failed\n");
                        break;
                    }
                }/*
                int nSendLen = send(clientSockets[i],//클라이언트 
                    SendBuff3,
                    sizeof(FGrenadePacket),
                    0);

                if (nSendLen == SOCKET_ERROR)
                {
                    printf("send people failed: %d\n", WSAGetLastError());
                    break;
                }*/

            }
        }

        char  SendBuff4[sizeof(FCharacterPacket)]; // 패킷을 저장할 변수
        
        while (!AIQueue.empty()) {

            FCharacterPacket packet;
            {
                std::lock_guard<std::mutex> lock(AIQMutex);
                packet = AIQueue.front();
                
                printf(
                    "[AIQueue] ID=%d | Pos(%.2f, %.2f, %.2f) | Dir(%.2f, %.2f, %.2f) | Speed=%.2f | Time=%.3f | Jump=%d Fire=%d Death=%d Heal=%d\n",
                    packet.CharacterId,
                    packet.X, packet.Y, packet.Z,
                    packet.DirX, packet.DirY, packet.DirZ,
                    packet.Speed,
                    packet.Sendtime,
                    packet.IsJump,
                    packet.IsFire,
                    packet.IsDeath,
                    packet.IsHeal
                );

                AIQueue.pop();
            }
            memcpy(SendBuff4, &packet, sizeof(FCharacterPacket));

            for (int i = 0; i < 2; i++) {       
                {
                    std::lock_guard<std::mutex> lock(SendMutex);

                    bool ok = SendAll(clientSockets[i],
                        SendBuff4,
                        sizeof(FCharacterPacket));

                    if (!ok)
                    {
                        printf("send failed\n");
                        break;
                    }
                }/*
                int nSendLen = send(clientSockets[i],//클라이언트 
                    SendBuff4,
                    sizeof(FCharacterPacket),
                    0);

                if (nSendLen == SOCKET_ERROR)
                {
                    printf("send AI failed: %d\n", WSAGetLastError());
                    break;
                }*/

            }
        }

        char  SendBuff5[sizeof(FInforamtionTextPacket)]; // 패킷을 저장할 변수        

        while (!TimeQueue.empty()) {

            FInforamtionTextPacket packet;
            {
                std::lock_guard<std::mutex> lock(TimQMutex);
                packet = TimeQueue.front();
                TimeQueue.pop();
            }
            memcpy(SendBuff5, &packet, sizeof(FInforamtionTextPacket));

            for (int i = 0; i < 2; i++) {

                {
                    std::lock_guard<std::mutex> lock(SendMutex);


                    bool ok = SendAll(clientSockets[i],
                        SendBuff5,
                        sizeof(FInforamtionTextPacket));

                    if (!ok)
                    {
                        printf("send failed\n");
                        break;
                    }
                }/*
                int nSendLen = send(clientSockets[i],//클라이언트 
                    SendBuff5,
                    sizeof(FInforamtionTextPacket),
                    0);

                if (nSendLen == SOCKET_ERROR)
                {
                    printf("send Time failed: %d\n", WSAGetLastError());
                    break;
                }*/

            }
        }

        char  SendBuff6[sizeof(FDeathPacket)]; // 패킷을 저장할 변수
        while (!DeathQueue.empty()) {

            FDeathPacket packet;
            {
                std::lock_guard<std::mutex> lock(DeathMutex);
                packet = DeathQueue.front();
                DeathQueue.pop();
            }
            memcpy(SendBuff6, &packet, sizeof(FDeathPacket));

            for (int i = 0; i < 2; i++) {
                

                printf("[Death send packet] aiid Id = %d characterid = %d \n",
                    packet.aiid,
                    packet.characterid
                );
                {
                    std::lock_guard<std::mutex> lock(SendMutex);

                    bool ok = SendAll(clientSockets[i],
                        SendBuff6,
                        sizeof(FDeathPacket));

                    if (!ok)
                    {
                        printf("send failed\n");
                        break;
                    }
                }/*
                int nSendLen = send(clientSockets[i],//클라이언트 
                    SendBuff6,
                    sizeof(FDeathPacket),
                    0);

                if (nSendLen == SOCKET_ERROR)
                {
                    printf("send Death failed: %d\n", WSAGetLastError());
                    break;
                }*/

            }
        }


        char  SendBuff7[sizeof(ServerBullet)]; // 패킷을 저장할 변수
        while (!MeleeQueue.empty()) {

            ServerBullet packet;
            {
                std::lock_guard<std::mutex> lock(MeleeMutex);
                packet = MeleeQueue.front();
                MeleeQueue.pop();
            }
            memcpy(SendBuff7, &packet, sizeof(ServerBullet));

            for (int i = 0; i < 2; i++) {
                if (packet.CharacterId == SocketToClientId[clientSockets[i]]) {
                    continue;
                }

                printf("[Melee send packet] characterid = %d \n",                    
                    packet.CharacterId
                );
                {
                    std::lock_guard<std::mutex> lock(SendMutex);

                    bool ok = SendAll(clientSockets[i],
                        SendBuff7,
                        sizeof(ServerBullet));

                    if (!ok)
                    {
                        printf("send failed\n");
                        break;
                    }
                }/*
                int nSendLen = send(clientSockets[i],//클라이언트 
                    SendBuff7,
                    sizeof(ServerBullet),
                    0);

                if (nSendLen == SOCKET_ERROR)
                {
                    printf("send Melee failed: %d\n", WSAGetLastError());
                    break;
                }
                */
            }
        }

        char  SendBuff8[sizeof(FItemPacket)]; // 패킷을 저장할 변수
        while (!ItemQueue.empty()) {

            FItemPacket packet;
            packet = ItemQueue.front();
            ItemQueue.pop();

            memcpy(SendBuff8, &packet, sizeof(FItemPacket));

            for (int i = 0; i < 2; i++) {
                if (packet.OwnerId == SocketToClientId[clientSockets[i]]) {
                    continue;
                }

                {
                    std::lock_guard<std::mutex> lock(SendMutex);

                    bool ok = SendAll(clientSockets[i],
                        SendBuff8,
                        sizeof(FItemPacket));

                    if (!ok)
                    {
                        printf("send failed\n");
                        break;
                    }
                }              
            }
        }
        
        SendList2.clear();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(8));

        //printf("still while\n");
    }

    closesocket(ClientSocket);
    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}
