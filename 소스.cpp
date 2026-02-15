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

#pragma comment(lib, "ws2_32.lib")

using namespace std;

enum class EPacketType : int
{
    Bullet = 1,
    Character = 2,
    InformationText = 3,
    Grenade = 4,
    AiSpawn = 5,
    Damage = 6
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
};

struct FInforamtionTextPacket
{
    FPacketHeader Header;
    //1총알, 2캐릭터 위치    
    bool connect;    
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

struct FDamagePacket
{
    FPacketHeader Header;
    int CharacterId;
    int Damage;
};


struct BulletData {
    int Header;
    Vector3 Position;
    //Vector3 Forward;
    Vector3 Velocity;
};

//vector<ServerBullet>bullets;
unordered_map <int, ServerBullet> Tree;
unordered_map <int, FCharacterPacket> People;
unordered_map <int, FCharacterPacket> AIPacket;
unordered_map <int, FGrenadePacket> Grenades;
std::queue<FGrenadePacket> GrenadeQueue;
std::unordered_map<SOCKET, int> SocketToClientId;
int nextClientId = 0;
int nextAiId = -110;
AIEnemyPositionManager PosMgr;
std::queue<FCharacterPacket> AIQueue;
std::queue<FDamagePacket> DamageQueue;

std::mutex BulletMutex;
std::mutex CharacterMutex;
std::mutex AIQMutex;
std::mutex DmgQMutex;

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
            //bullets.push_back(Bullet);
            if (Bullet->flag) {//플래그가 true 일때만 그때는 이미 클라한테 총알ID가 할당되어있으므로 그걸가지고 Tree접근 지우라는 명령
                if (Tree.find(Bullet->BulletId) != Tree.end()) {
                    Tree.erase(Bullet->BulletId);
                }
            }
            else {//플래그가 false이면 서버에서 총알 ID를 직접 할당후 클라로 보냄
                Tree[bulletId++] = data;
            }
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
                /*
                printf("[Character] Id=%d Pos=(%.2f %.2f %.2f)Dir=(%.2f %.2f %.2f)\n",
                    Char->CharacterId,
                    Char->X,
                    Char->Y,
                    Char->Z,
                    Char->DirX,
                    Char->DirY,
                    Char->DirZ

                );*/
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
                AIPacket[Char->CharacterId] = data2;

                if (Char->IsJump) {
                    PosMgr.RandomPositionWithScale();//번호생성
                    Vector3 newPos = PosMgr.GetRandPos();//번호얻기

                    AIPacket[Char->CharacterId].DirX = newPos.x;
                    AIPacket[Char->CharacterId].DirY = newPos.y;
                    AIPacket[Char->CharacterId].DirZ = newPos.z;
                    AIPacket[Char->CharacterId].IsJump = false;
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
    case EPacketType::Damage:
    {
        FDamagePacket* dmg =
            reinterpret_cast<FDamagePacket*>(Header);

        printf("[Damage] Character Id = %d Damage = %d\n",
            dmg->CharacterId,
            dmg->Damage
        );

        //int clientId = SocketToClientId[soc];
        //dmg->CharacterId = clientId;
        {
            std::lock_guard<std::mutex> lock(DmgQMutex);
            DamageQueue.push(*dmg);
        }
        break;
    }
    default:
        printf("Unknown packet\n");
        break;
    }
}


void recivePacket(SOCKET ClientSocket) {
    
    
    while (true)
    {
        FRecvBuffer RecvBuf;//...

        int RecvLen = recv(
            ClientSocket,
            RecvBuf.GetWritePtr(),
            FRecvBuffer::BUFFER_SIZE - RecvBuf.WritePos,
            0
        );

        if (RecvLen <= 0)
        {
            // 연결 종료 처리
            printf("패킷이안와");
            return;
        }

        RecvBuf.OnWrite(RecvLen);

        // 헤더도 못 읽으면 대기
        if (RecvBuf.GetDataSize() < sizeof(FPacketHeader))
            return;

        FPacketHeader* Header =
            reinterpret_cast<FPacketHeader*>(RecvBuf.GetReadPtr());

        // 패킷 전체가 아직 안 왔으면 대기
        if (RecvBuf.GetDataSize() < Header->Size)
            return;

        // 패킷 하나 완성 → 처리
        dataProcess(Header, ClientSocket);

        // 읽은 만큼 제거
        RecvBuf.OnRead(Header->Size);
    }

}



int index = 0;
SOCKET clientSockets[2];


int main() {
    /*
    PosMgr.PrintAllPoints();
    PosMgr.RandomPositionWithScale();
    Vector3 test = PosMgr.GetRandPos();
    printf("%f %f %f\n", test.x, test.y, test.z);

    PosMgr.RandomPositionWithScale();
    test = PosMgr.GetRandPos();
    printf("%f %f %f\n", test.x, test.y, test.z);
    PosMgr.RandomPositionWithScale();
    test = PosMgr.GetRandPos();
    printf("%f %f %f\n", test.x, test.y, test.z);

    printf("%zu\n", sizeof(FSpawnAIPacket));
    */
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

        if (ClientSocket == INVALID_SOCKET)
        {
            std::cout << "accept() failed\n";
        }
        else
        {
            std::cout << "Client connected!\n";
            clientSockets[(index)] = ClientSocket;
            index++;
            SocketToClientId[ClientSocket] = nextClientId++;
            std::thread t(recivePacket, ClientSocket);  // 스레드 생성
            //t.join();                  // 스레드 종료까지 대기    
            t.detach();
            
            if (index >= 2) {
                break;
            }
            
        }
    }
   //두명 모일때까지 대기
            
    //ai 두개 생성
    for (int i = 0; i < 1; i++) {
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
    char  SendTimeBuff[sizeof(FInforamtionTextPacket)];

    FPacketHeader header = { sizeof(FInforamtionTextPacket), (int)EPacketType::InformationText };
    FInforamtionTextPacket packet = { header, true };
    memcpy(SendTimeBuff, &packet, sizeof(FInforamtionTextPacket));
    for (int i = 0; i < 2; i++) {
        int nSendLen = send(clientSockets[i],//클라이언트 
            SendTimeBuff,
            sizeof(FInforamtionTextPacket),
            0);
    }

    
    

    //물리계산    
    Vector3 Gravity = Vector3(0, 0, -980.f); // cm/s²  
    //double DeltaTime = ArriedTime - Bullet.Sendtime;//대략적인 통신 지연시간 클라->서버
    double DeltaTime = 0.016; // or 서버 Tick 시간
    printf("DeltaTime    : %.6f\n", DeltaTime);
   
    char  SendBuff[sizeof(ServerBullet)]; // 패킷을 저장할 변수
    char  SendBuff2[sizeof(FCharacterPacket)]; // 패킷을 저장할 변수
    char  SendBuff3[sizeof(FGrenadePacket)]; // 패킷을 저장할 변수
    while (true)
    {
        std::vector<ServerBullet> SendList;
        std::vector<FCharacterPacket> SendList2;
        
        {
            std::lock_guard<std::mutex> lock(BulletMutex);

            for (auto& pair : Tree)
            {                
                int id = pair.first;
                ServerBullet& bd = pair.second;
                FPacketHeader header = bd.Header;///
                switch (header.Type) {
                case ((int)EPacketType::Bullet):
                {
                    // 물리 계산
                    /*
                    bd.Velocity += Gravity * DeltaTime;
                    bd.Position += bd.Velocity * DeltaTime;
                    */
                    ServerBullet pos;
                    pos.Header = bd.Header;
                    pos.BulletId = id;
                    /*
                    bd.X += bd.X * Gravity.x * DeltaTime * DeltaTime;
                    bd.Y += bd.Y * Gravity.y * DeltaTime * DeltaTime;
                    bd.Z += bd.Z * Gravity.z * DeltaTime * DeltaTime;
                    */
                    // 중력 적용 (Z만)
                    bd.DirZ += -980.f * DeltaTime;

                    // 이동
                    bd.X += bd.DirX * DeltaTime;
                    bd.Y += bd.DirY * DeltaTime;
                    bd.Z += bd.DirZ * DeltaTime;


                    pos.X = bd.X;
                    pos.Y = bd.Y;
                    pos.Z = bd.Z;
                    pos.DirX = bd.DirX;
                    pos.DirY = bd.DirY;
                    pos.DirZ = bd.DirZ;
                    pos.Speed = bd.Speed;
                    pos.Sendtime = DeltaTime;

                    SendList.push_back(pos);
                    break;
                }
                case 2:
                {
                    /*
                    //사람 todo..
                    ServerBullet pos2;
                    pos2.Header = 2;
                    pos2.BulletId = id;
                    pos2.X = bd.X;
                    pos2.Y = bd.Y;
                    pos2.Z = bd.Z;
                    pos2.DX = bd.DX;
                    pos2.DY = bd.DY;
                    pos2.DZ = bd.DZ;
                    pos2.Speed = bd.Speed;
                    pos2.DeltaTime = DeltaTime;
                    SendList.push_back(pos2);
                    */
                    break;
                }
                default:
                    break;
                }               
            }      



        } // 🔓 mutex 해제

        //{
        //    std::lock_guard<std::mutex> lock(CharacterMutex);
            for (auto& pair : People) {
                int id = pair.first;
                FCharacterPacket& bd = pair.second;
                //사람 todo..
                FCharacterPacket pos2;
                pos2.Header.Size = bd.Header.Size;
                pos2.Header.Type = bd.Header.Type;
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
                //printf("x = %f Y= %f Z=%f\n", pos2.DirX, pos2.DirY, pos2.DirZ);
                SendList2.push_back(pos2);
            }
            

            /*

            for (auto& pair : Grenades) {
                int id = pair.first;
                FGrenadePacket& bd = pair.second;

                FGrenadePacket pos2;

                bd.DirZ += -980.f * DeltaTime;
                // 이동
                bd.X += bd.DirX * DeltaTime;
                bd.Y += bd.DirY * DeltaTime;
                bd.Z += bd.DirZ * DeltaTime;
                pos2.X = bd.X;
                pos2.Y = bd.Y;
                pos2.Z = bd.Z;
                pos2.DirX = bd.DirX;
                pos2.DirY = bd.DirY;
                pos2.DirZ = bd.DirZ;
                //사람 todo..
                
                pos2.Header.Size = bd.Header.Size;
                pos2.Header.Type = bd.Header.Type;
                pos2.GrenadeId = id;
                pos2.CharacterId = bd.CharacterId;
                
                pos2.Power = 1000;
                GrenadeQueue.push(pos2);
            }
            */
       // }
        // 🔥 네트워크 전송은 락 밖에서
        for (const auto& pos : SendList)
        {
            memcpy(SendBuff, &pos, sizeof(ServerBullet));

            for (int i = 0; i < 2; i++) {
                int nSendLen = send(clientSockets[i],//클라이언트 두명에게 총알을 그린다.
                    SendBuff,
                    sizeof(ServerBullet),
                    0);

                if (nSendLen == SOCKET_ERROR)
                {
                    printf("send failed: %d\n", WSAGetLastError());
                    break;
                }

            }
            
            //PrintBulletPos(pos);
            
        }

        

        for (const auto& pos : SendList2)
        {
            memcpy(SendBuff2, &pos, sizeof(FCharacterPacket));

            for (int i = 0; i < 2; i++) {
                if (pos.CharacterId == SocketToClientId[clientSockets[i]]) {
                    continue;
                }

                int nSendLen = send(clientSockets[i],//클라이언트 
                    SendBuff2,
                    sizeof(FCharacterPacket),
                    0);

                if (nSendLen == SOCKET_ERROR)
                {
                    printf("send people failed: %d\n", WSAGetLastError());
                    break;
                }

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

                int nSendLen = send(clientSockets[i],//클라이언트 
                    SendBuff3,
                    sizeof(FGrenadePacket),
                    0);

                if (nSendLen == SOCKET_ERROR)
                {
                    printf("send people failed: %d\n", WSAGetLastError());
                    break;
                }

            }
        }

        char  SendBuff4[sizeof(FCharacterPacket)]; // 패킷을 저장할 변수

        while (!AIQueue.empty()) {

            FCharacterPacket packet;
            {
                std::lock_guard<std::mutex> lock(AIQMutex);
                packet = AIQueue.front();
                AIQueue.pop();
            }
            memcpy(SendBuff4, &packet, sizeof(FCharacterPacket));

            for (int i = 0; i < 2; i++) {                

                int nSendLen = send(clientSockets[i],//클라이언트 
                    SendBuff4,
                    sizeof(FCharacterPacket),
                    0);

                if (nSendLen == SOCKET_ERROR)
                {
                    printf("send AI failed: %d\n", WSAGetLastError());
                    break;
                }

            }
        }

        char  SendBuff5[sizeof(FDamagePacket)]; // 패킷을 저장할 변수
        while (!DamageQueue.empty()) {

            FDamagePacket packet;
            {
                std::lock_guard<std::mutex> lock(DmgQMutex);
                packet = DamageQueue.front();
                DamageQueue.pop();
            }
            memcpy(SendBuff5, &packet, sizeof(FDamagePacket));

            for (int i = 0; i < 2; i++) {
                if (packet.CharacterId == SocketToClientId[clientSockets[i]]) {
                    continue;
                }            
                int nSendLen = send(clientSockets[i],//클라이언트 
                    SendBuff5,
                    sizeof(FDamagePacket),
                    0);

                if (nSendLen == SOCKET_ERROR)
                {
                    printf("send AI failed: %d\n", WSAGetLastError());
                    break;
                }

            }
        }

        SendList.clear();
        SendList2.clear();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(8));

        //printf("still while\n");
    }

    closesocket(ClientSocket);
    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}
