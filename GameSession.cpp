// GameSession.cpp
#include "GameSession.h"
#include <chrono>
#include <cstdio>
#include "SessionManager.h" 

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
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            printf("send error: %d\n", err);
            return false;
        }
        if (sent == 0) return false;
        totalSent += sent;
    }
    return true;
}

void GameSession::Broadcast(const char* data, int size, int excludeClientId)
{
    printf("[Broadcast] clientSocketsIndex=%d excludeClientId=%d\n",
        clientSocketsIndex, excludeClientId);

    for (int i = 0; i < clientSocketsIndex; i++)
    {
        int clientId = SocketToClientId[clientSockets[i]];
        printf("[Broadcast] i=%d clientId=%d socket=%llu -> %s\n",
            i, clientId, (unsigned long long)clientSockets[i],
            clientId == excludeClientId ? "SKIP" : "SEND");

        if (SocketToClientId[clientSockets[i]] == excludeClientId) continue;
        std::lock_guard<std::mutex> lock(SendMutex);
        SendAll(clientSockets[i], data, size);
    }
}

void GameSession::StartSession()
{
    // 연결 순서 패킷 전송
    /*
    for (int i = 0; i < clientSocketsIndex; i++)
    {
        FConnectionPacket data{};
        data.Header.Size = sizeof(FConnectionPacket);
        data.Header.Type = static_cast<int>(EPacketType::Client);
        data.order = i;
        strncpy_s(data.Name, "temp", 7);
        data.Name[7] = '\0';
        send(clientSockets[i], (char*)&data, sizeof(data), 0);
    }*/

    // AI 2개 스폰
    for (int i = 0; i < 2; i++)
    {
        FSpawnAIPacket ai{};
        ai.Header.Type = static_cast<int>(EPacketType::AiSpawn);
        ai.Header.Size = sizeof(FSpawnAIPacket);
        ai.X = -1600.75f;
        ai.Y = -1059.25f;
        ai.Z = 432.43f;
        ai.AiID = nextAiId--;
        Broadcast((char*)&ai, sizeof(ai));
    }

    // 송신 루프 스레드 시작
    std::thread([this]() { SendLoop(); }).detach();

    printf("[Session %d] 게임 시작!\n", SessionId);
}

void GameSession::DataProcess(FPacketHeader* Header, SOCKET soc)
{
    printf("DataProcess Type = %d\n", Header->Type);

    switch ((EPacketType)Header->Type)
    {
    case EPacketType::Bullet:
    {
        ServerBullet* Bullet = reinterpret_cast<ServerBullet*>(Header);

        ServerBullet data;
        data.Header = Bullet->Header;
        data.BulletId = Bullet->BulletId;
        data.CharacterId = Bullet->CharacterId;
        data.X = Bullet->X; data.Y = Bullet->Y; data.Z = Bullet->Z;
        data.DirX = Bullet->DirX * Bullet->Speed;
        data.DirY = Bullet->DirY * Bullet->Speed;
        data.DirZ = Bullet->DirZ * Bullet->Speed;
        data.Speed = Bullet->Speed;
        data.Sendtime = Bullet->Sendtime;
        data.flag = true;

        std::lock_guard<std::mutex> lock(BulletMutex);
        BulletQueue.push(data);
        break;
    }

    case EPacketType::Character:
    {
        FCharacterPacket* Char = reinterpret_cast<FCharacterPacket*>(Header);

        if (Char->CharacterId > -100)
        {
            int clientId = SocketToClientId[soc];
            if (People.find(clientId) != People.end())
            {
                printf("[Character] Id=%d Pos=(%.2f %.2f %.2f)\n",
                    Char->CharacterId, Char->X, Char->Y, Char->Z);
            }
            People[clientId] = *Char;
        }
        else
        {
            // AI 처리
            if (AIPacket.find(Char->CharacterId) != AIPacket.end())
            {
                if (Char->IsJump)
                {
                    PosMgr.RandomPositionWithScale();
                    Vector3 newPos = PosMgr.GetRandPos();
                    AIPacket[Char->CharacterId].DirX = newPos.x;
                    AIPacket[Char->CharacterId].DirY = newPos.y;
                    AIPacket[Char->CharacterId].DirZ = newPos.z;
                    AIPacket[Char->CharacterId].IsJump = false;
                    AIPacket[Char->CharacterId].IsDeath = false;
                    AIPacket[Char->CharacterId].IsHeal = false;
                    AIPacket[Char->CharacterId].WeaponType = WeaponType::Melee;
                    AIPacket[Char->CharacterId].AimActive = false;
                }
                if (Char->IsFire)
                {
                    AIPacket[Char->CharacterId].DirX = Char->DirX;
                    AIPacket[Char->CharacterId].DirY = Char->DirY;
                    AIPacket[Char->CharacterId].DirZ = Char->DirZ;
                    AIPacket[Char->CharacterId].IsJump = false;
                    AIPacket[Char->CharacterId].IsFire = false;
                    AIPacket[Char->CharacterId].IsHeal = false;
                    AIPacket[Char->CharacterId].WeaponType = WeaponType::Melee;
                    AIPacket[Char->CharacterId].AimActive = false;
                }
            }
            else
            {
                // AI 처음 도착
                FCharacterPacket data2 = *Char;
                AIPacket[Char->CharacterId] = data2;

                if (Char->IsJump)
                {
                    PosMgr.RandomPositionWithScale();
                    Vector3 newPos = PosMgr.GetRandPos();
                    AIPacket[Char->CharacterId].DirX = newPos.x;
                    AIPacket[Char->CharacterId].DirY = newPos.y;
                    AIPacket[Char->CharacterId].DirZ = newPos.z;
                    AIPacket[Char->CharacterId].IsJump = false;
                    AIPacket[Char->CharacterId].IsHeal = false;
                    AIPacket[Char->CharacterId].WeaponType = WeaponType::Melee;
                    AIPacket[Char->CharacterId].AimActive = false;
                }
            }

            std::lock_guard<std::mutex> lock(AIQMutex);
            AIQueue.push(AIPacket[Char->CharacterId]);
        }
        break;
    }

    case EPacketType::Grenade:
    {
        FGrenadePacket* packet = reinterpret_cast<FGrenadePacket*>(Header);
        int clientId = SocketToClientId[soc];
        packet->CharacterId = clientId;

        std::lock_guard<std::mutex> lock(GrenadeMutex);
        GrenadeQueue.push(*packet);
        break;
    }

    case EPacketType::InformationText:
    {
        FInforamtionTextPacket* timepacket =
            reinterpret_cast<FInforamtionTextPacket*>(Header);
        printf("timepacket = %d\n", timepacket->connect);

        {
            std::lock_guard<std::mutex> lock(TimQMutex);
            TimeQueue.push(*timepacket);
        }

        // 5초 뒤 타이머 재전송
        std::thread([this]()
            {
                std::this_thread::sleep_for(std::chrono::seconds(5));
                FInforamtionTextPacket packet;
                packet.Header.Size = sizeof(FInforamtionTextPacket);
                packet.Header.Type = static_cast<int>(EPacketType::InformationText);
                packet.connect = 10;
                std::lock_guard<std::mutex> lock(TimQMutex);
                TimeQueue.push(packet);
            }).detach();

        break;
    }

    case EPacketType::Death:
    {
        FDeathPacket* rip = reinterpret_cast<FDeathPacket*>(Header);

        FDeathPacket packet{};
        packet.Header.Size = sizeof(FDeathPacket);
        packet.Header.Type = static_cast<int>(EPacketType::Death);
        packet.aiid = rip->aiid;
        packet.characterid = rip->characterid;
        packet.causedCharacterId = rip->causedCharacterId;

        // 죽은 사람 이름
        auto sockIt = ClientIdToSocket.find(rip->characterid);
        if (sockIt != ClientIdToSocket.end())
        {
            std::string name = SessionManager::Get().GetNameBySocket(sockIt->second);
            strncpy_s(packet.Name, sizeof(packet.Name), name.c_str(), _TRUNCATE);
        }
        else
        {
            strncpy_s(packet.Name, sizeof(packet.Name), "Unknown", _TRUNCATE);
        }

        // 죽인 사람 이름
        auto sockIt2 = ClientIdToSocket.find(rip->causedCharacterId);
        if (sockIt2 != ClientIdToSocket.end())
        {
            std::string name2 = SessionManager::Get().GetNameBySocket(sockIt2->second);
            strncpy_s(packet.CausedName, sizeof(packet.CausedName), name2.c_str(), _TRUNCATE);
        }
        else
        {
            strncpy_s(packet.CausedName, sizeof(packet.CausedName), "Unknown", _TRUNCATE);
        }

        std::lock_guard<std::mutex> lock(DeathMutex);
        DeathQueue.push(packet);
        break;
    }

    case EPacketType::Melee:
    {
        ServerBullet* Melee = reinterpret_cast<ServerBullet*>(Header);

        ServerBullet Punch;
        Punch.Header = Melee->Header;
        Punch.BulletId = Melee->BulletId;
        Punch.CharacterId = Melee->CharacterId;
        Punch.X = Melee->X; Punch.Y = Melee->Y; Punch.Z = Melee->Z;
        Punch.DirX = Melee->DirX * Melee->Speed;
        Punch.DirY = Melee->DirY * Melee->Speed;
        Punch.DirZ = Melee->DirZ * Melee->Speed;
        Punch.Speed = Melee->Speed;
        Punch.Sendtime = Melee->Sendtime;
        Punch.flag = true;

        std::lock_guard<std::mutex> lock(MeleeMutex);
        MeleeQueue.push(Punch);
        break;
    }

    case EPacketType::Client:
    {
        FConnectionPacket* connection =
            reinterpret_cast<FConnectionPacket*>(Header);

        printf("Connection Recv CharacterId: %d Name: %.8s\n",
            connection->order, connection->Name);

        PlayerInfo PN{};
        memset(&PN, 0, sizeof(PlayerInfo));
        memcpy(PN.Name, connection->Name, 8);
        Players.insert({ connection->order, PN });
        LoginCount++;

        // 현재 접속자 수 모든 클라에게 전송
        for (int i = 0; i < clientSocketsIndex; i++)
        {
            FConnectionPacket data{};
            data.Header.Size = sizeof(FConnectionPacket);
            data.Header.Type = static_cast<int>(EPacketType::Client);
            data.order = LoginCount;
            strncpy_s(data.Name, "temp", 7);
            data.Name[7] = '\0';
            send(clientSockets[i], (char*)&data, sizeof(data), 0);
        }
        break;
    }

    case EPacketType::Item:
    {
        FItemPacket* ItemPacket = reinterpret_cast<FItemPacket*>(Header);
        printf("OwnerId=%d ItemRow=%d SpawnID=%d\n",
            ItemPacket->OwnerId, ItemPacket->ItemRow, ItemPacket->ItemSpawnID);

        auto it = Players.find(ItemPacket->OwnerId);
        if (it == Players.end()) { printf("OwnerId not mapped\n"); break; }

        if (ItemPacket->ShouldRemove)
        {
            for (auto& item : it->second.HaveItems)
            {
                if (item.itemRow == ItemPacket->ItemRow)
                {
                    item.count = 0;
                    std::lock_guard<std::mutex> lock(ItemMutex);
                    ItemQueue.push(*ItemPacket);
                    break;
                }
            }
        }
        else
        {
            bool bFound = false;
            for (auto& item : it->second.HaveItems)
            {
                if (item.itemRow == ItemPacket->ItemRow)
                {
                    item.count++;
                    it->second.itemCount++;
                    bFound = true;
                    break;
                }
            }
            if (!bFound && it->second.itemCount < 20)
            {
                Items newItem{ ItemPacket->ItemRow, 1 };
                it->second.HaveItems[it->second.itemCount++] = newItem;
            }
            ItemPacket->ShouldRemove = false;
            std::lock_guard<std::mutex> lock(ItemMutex);
            ItemQueue.push(*ItemPacket);
        }
        break;
    }

    case EPacketType::BoradCast:
    {
        FBrodcastMessage* packet = reinterpret_cast<FBrodcastMessage*>(Header);
        Broadcast((char*)packet, sizeof(FBrodcastMessage));
        break;
    }

    case EPacketType::Vehicle:
    {
        FVehiclePacket* packet = reinterpret_cast<FVehiclePacket*>(Header);
        packet->ownerId = SocketToClientId[soc];   // 여기서 강제로 일치시킴
        printf("[Vehicle] carId=%d ownerId=%d\n", packet->carId, packet->ownerId);

        std::lock_guard<std::mutex> lock(VehicleMutex);
        Vehicles[packet->carId] = *packet;
        break;
    }

    default:
        printf("Unknown packet type=%d\n", Header->Type);
        break;
    }
}

void GameSession::SendLoop()
{
    char SendBuff[sizeof(ServerBullet)];
    char SendBuff2[sizeof(FCharacterPacket)];
    char SendBuff3[sizeof(FGrenadePacket)];
    char SendBuff4[sizeof(FCharacterPacket)];
    char SendBuff5[sizeof(FInforamtionTextPacket)];
    char SendBuff6[sizeof(FDeathPacket)];
    char SendBuff7[sizeof(ServerBullet)];
    char SendBuff8[sizeof(FItemPacket)];
    char SendBuffVehicle[sizeof(FVehiclePacket)];

    while (true)
    {
        // 1. 캐릭터 수집
        std::vector<FCharacterPacket> SendList2;
        {
            std::lock_guard<std::mutex> lock(CharacterMutex);
            for (auto& pair : People)
            {
                int id = pair.first;
                FCharacterPacket& bd = pair.second;
                FCharacterPacket pos2;
                pos2.Header.Size = sizeof(FCharacterPacket);
                pos2.Header.Type = static_cast<int>(EPacketType::Character);
                pos2.CharacterId = id;
                pos2.X = bd.X; pos2.Y = bd.Y; pos2.Z = bd.Z;
                pos2.DirX = bd.DirX; pos2.DirY = bd.DirY; pos2.DirZ = bd.DirZ;
                pos2.Speed = bd.Speed;
                pos2.Sendtime = bd.Sendtime;
                pos2.IsJump = bd.IsJump;
                pos2.IsFire = bd.IsFire;
                pos2.IsDeath = bd.IsDeath;
                pos2.IsHeal = bd.IsHeal;
                pos2.WeaponType = bd.WeaponType;
                pos2.AimActive = bd.AimActive;
                SendList2.push_back(pos2);
            }
        }

        // 2. 총알
        while (!BulletQueue.empty())
        {
            ServerBullet pos;
            {
                std::lock_guard<std::mutex> lock(BulletMutex);
                if (BulletQueue.empty()) break;
                pos = BulletQueue.front(); BulletQueue.pop();
            }
            memcpy(SendBuff, &pos, sizeof(ServerBullet));
            for (int i = 0; i < clientSocketsIndex; i++)
            {
                if (pos.CharacterId == SocketToClientId[clientSockets[i]]) continue;
                std::lock_guard<std::mutex> lock(SendMutex);
                SendAll(clientSockets[i], SendBuff, sizeof(ServerBullet));
            }
        }

        // 3. 캐릭터 위치
        for (const auto& pos : SendList2)
        {
            memcpy(SendBuff2, &pos, sizeof(FCharacterPacket));
            for (int i = 0; i < clientSocketsIndex; i++)
            {
                if (pos.CharacterId == SocketToClientId[clientSockets[i]]) continue;
                std::lock_guard<std::mutex> lock(SendMutex);
                SendAll(clientSockets[i], SendBuff2, sizeof(FCharacterPacket));
            }
        }

        // 4. 차량
        std::vector<FVehiclePacket> copiedVehicles;
        {
            std::lock_guard<std::mutex> lock(VehicleMutex);
            for (auto& pair : Vehicles) copiedVehicles.push_back(pair.second);
        }
        for (auto& v : copiedVehicles)
        {
            if (!v.IsHaveCharacter)
            {
                std::lock_guard<std::mutex> lock(VehicleMutex);
                Vehicles.erase(v.carId);
                continue;
            }
            memcpy(SendBuffVehicle, &v, sizeof(FVehiclePacket));
            for (int i = 0; i < clientSocketsIndex; i++)
            {
                if (v.ownerId == SocketToClientId[clientSockets[i]])
                {
                    continue;
                }
                std::lock_guard<std::mutex> lock(SendMutex);
                SendAll(clientSockets[i], SendBuffVehicle, sizeof(FVehiclePacket));
            }
        }

        // 5. 수류탄
        while (!GrenadeQueue.empty())
        {
            FGrenadePacket packet;
            {
                std::lock_guard<std::mutex> lock(GrenadeMutex);
                if (GrenadeQueue.empty()) break;
                packet = GrenadeQueue.front(); GrenadeQueue.pop();
            }
            memcpy(SendBuff3, &packet, sizeof(FGrenadePacket));
            for (int i = 0; i < clientSocketsIndex; i++)
            {
                if (packet.CharacterId == SocketToClientId[clientSockets[i]]) continue;
                std::lock_guard<std::mutex> lock(SendMutex);
                SendAll(clientSockets[i], SendBuff3, sizeof(FGrenadePacket));
            }
        }

        // 6. AI
        while (!AIQueue.empty())
        {
            FCharacterPacket packet;
            {
                std::lock_guard<std::mutex> lock(AIQMutex);
                if (AIQueue.empty()) break;
                packet = AIQueue.front(); AIQueue.pop();
            }
            memcpy(SendBuff4, &packet, sizeof(FCharacterPacket));
            for (int i = 0; i < clientSocketsIndex; i++)
            {
                std::lock_guard<std::mutex> lock(SendMutex);
                SendAll(clientSockets[i], SendBuff4, sizeof(FCharacterPacket));
            }
        }

        // 7. 타이머
        while (!TimeQueue.empty())
        {
            FInforamtionTextPacket packet;
            {
                std::lock_guard<std::mutex> lock(TimQMutex);
                if (TimeQueue.empty()) break;
                packet = TimeQueue.front(); TimeQueue.pop();
            }
            memcpy(SendBuff5, &packet, sizeof(FInforamtionTextPacket));
            for (int i = 0; i < clientSocketsIndex; i++)
            {
                std::lock_guard<std::mutex> lock(SendMutex);
                SendAll(clientSockets[i], SendBuff5, sizeof(FInforamtionTextPacket));
            }
        }

        // 8. 사망
        while (!DeathQueue.empty())
        {
            FDeathPacket packet;
            {
                std::lock_guard<std::mutex> lock(DeathMutex);
                if (DeathQueue.empty()) break;
                packet = DeathQueue.front(); DeathQueue.pop();
            }
            memcpy(SendBuff6, &packet, sizeof(FDeathPacket));
            for (int i = 0; i < clientSocketsIndex; i++)
            {
                std::lock_guard<std::mutex> lock(SendMutex);
                SendAll(clientSockets[i], SendBuff6, sizeof(FDeathPacket));
            }
        }

        // 9. 근접
        while (!MeleeQueue.empty())
        {
            ServerBullet packet;
            {
                std::lock_guard<std::mutex> lock(MeleeMutex);
                if (MeleeQueue.empty()) break;
                packet = MeleeQueue.front(); MeleeQueue.pop();
            }
            memcpy(SendBuff7, &packet, sizeof(ServerBullet));
            for (int i = 0; i < clientSocketsIndex; i++)
            {
                if (packet.CharacterId == SocketToClientId[clientSockets[i]]) continue;
                std::lock_guard<std::mutex> lock(SendMutex);
                SendAll(clientSockets[i], SendBuff7, sizeof(ServerBullet));
            }
        }

        // 10. 아이템
        while (!ItemQueue.empty())
        {
            FItemPacket packet;
            {
                std::lock_guard<std::mutex> lock(ItemMutex);
                if (ItemQueue.empty()) break;
                packet = ItemQueue.front(); ItemQueue.pop();
            }
            memcpy(SendBuff8, &packet, sizeof(FItemPacket));
            for (int i = 0; i < clientSocketsIndex; i++)
            {
                if (packet.OwnerId == SocketToClientId[clientSockets[i]]) continue;
                std::lock_guard<std::mutex> lock(SendMutex);
                SendAll(clientSockets[i], SendBuff8, sizeof(FItemPacket));
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(8));
    }
}