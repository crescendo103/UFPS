// GameSession.h
#pragma once
#include <winsock2.h>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <vector>
#include <thread>
#include <memory>
#include "Common.h"
#include "AIEnemyPositionManager.h"

bool SendAll(SOCKET sock, const char* data, int size);

class SessionManager;

class GameSession
{
public:
    explicit GameSession(int id) : SessionId(id) {}

    int SessionId;
    static const int MAX_PLAYERS = 2;

    // ─── 소켓 ───────────────────────────
    SOCKET clientSockets[MAX_PLAYERS] = {};
    int clientSocketsIndex = 0;
    int LoginCount = 0;
    std::unordered_map<SOCKET, int> SocketToClientId;
    std::unordered_map<int, SOCKET> ClientIdToSocket;

    // GameSession.h - 기존 멤버에 추가
    SOCKET HostSocket = INVALID_SOCKET;  // 방장 소켓
    char   RoomName[16] = {};            // 방 이름
    bool   IsStarted = false;            // 게임 시작 여부

    // ─── 게임 상태 ──────────────────────
    std::unordered_map<int, FCharacterPacket>  People;
    std::unordered_map<int, FCharacterPacket>  AIPacket;
    std::unordered_map<int, FGrenadePacket>    Grenades;
    std::unordered_map<int, FVehiclePacket>    Vehicles;
    std::unordered_map<int, PlayerInfo>        Players;

    // ─── 큐 ─────────────────────────────
    std::queue<ServerBullet>            BulletQueue;
    std::queue<ServerBullet>            MeleeQueue;
    std::queue<FGrenadePacket>          GrenadeQueue;
    std::queue<FCharacterPacket>        AIQueue;
    std::queue<FInforamtionTextPacket>  TimeQueue;
    std::queue<FDeathPacket>            DeathQueue;
    std::queue<FItemPacket>             ItemQueue;

    // ─── 뮤텍스 ─────────────────────────
    std::mutex BulletMutex;
    std::mutex CharacterMutex;
    std::mutex AIQMutex;
    std::mutex TimQMutex;
    std::mutex DeathMutex;
    std::mutex MeleeMutex;
    std::mutex SendMutex;
    std::mutex GrenadeMutex;
    std::mutex ItemMutex;
    std::mutex VehicleMutex;

    // ─── AI ─────────────────────────────
    int nextAiId = -110;
    AIEnemyPositionManager PosMgr;


    bool IsFull()  const { return clientSocketsIndex >= MAX_PLAYERS; }
    bool IsEmpty() const { return clientSocketsIndex == 0; }

    void StartSession();
    void SendLoop();
    void DataProcess(FPacketHeader* Header, SOCKET soc);
    void Broadcast(const char* data, int size, int excludeClientId = -1);
    
};