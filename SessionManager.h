// SessionManager.h
#pragma once
#include <unordered_map>
#include <memory>
#include <mutex>
#include "GameSession.h"
#include "Common.h"

class SessionManager {
public:
    static SessionManager& Get() {
        static SessionManager instance;
        return instance;
    }

    // 방 생성 - 방장 소켓 저장
    std::shared_ptr<GameSession> CreateRoom(SOCKET hostSocket, const char* roomName)
    {
        std::lock_guard<std::mutex> lock(Mutex);

        int newId = nextSessionId++;
        auto session = std::make_shared<GameSession>(newId);
        session->HostSocket = hostSocket;   // 방장 소켓 저장
        strncpy_s(session->RoomName, roomName, 15);

        Sessions[newId] = session;
        SocketToSession[hostSocket] = session;

        printf("[SessionManager] 방 생성: %d (%s)\n", newId, roomName);
        return session;
    }

    // 방 참가
    std::shared_ptr<GameSession> JoinRoom(int roomId, SOCKET clientSocket)
    {
        std::lock_guard<std::mutex> lock(Mutex);

        auto it = Sessions.find(roomId);
        if (it == Sessions.end())
        {
            printf("[SessionManager] 방 없음: %d\n", roomId);
            return nullptr;
        }

        auto& session = it->second;
        if (session->IsFull() || session->IsStarted)
        {
            printf("[SessionManager] 방 입장 불가 (꽉참or시작됨)\n");
            return nullptr;
        }

        SocketToSession[clientSocket] = session;
        printf("[SessionManager] 방 %d 참가\n", roomId);
        return session;
    }

    // 시작 요청 - 방장만 가능
    bool TryStartGame(SOCKET requesterSocket)
    {
        std::lock_guard<std::mutex> lock(Mutex);

        auto it = SocketToSession.find(requesterSocket);
        if (it == SocketToSession.end()) return false;

        auto& session = it->second;

        // 방장 확인
        if (session->HostSocket != requesterSocket)
        {
            printf("[SessionManager] 방장만 시작 가능\n");
            return false;
        }

        if (session->IsStarted)
        {
            printf("[SessionManager] 이미 시작된 방\n");
            return false;
        }

        session->IsStarted = true;
        session->StartSession();
        return true;
    }

    std::shared_ptr<GameSession> FindBySocket(SOCKET sock)
    {
        std::lock_guard<std::mutex> lock(Mutex);
        auto it = SocketToSession.find(sock);
        if (it != SocketToSession.end()) return it->second;
        return nullptr;
    }

    void RegisterSocket(SOCKET sock, std::shared_ptr<GameSession> session)
    {
        std::lock_guard<std::mutex> lock(Mutex);
        SocketToSession[sock] = session;
    }

    void RemoveSession(int sessionId)
    {
        std::lock_guard<std::mutex> lock(Mutex);
        Sessions.erase(sessionId);
    }

    // 방 목록 반환
    std::vector<std::shared_ptr<GameSession>> GetRoomList()
    {
        std::lock_guard<std::mutex> lock(Mutex);
        std::vector<std::shared_ptr<GameSession>> list;
        for (auto& pair : Sessions)
        {
            if (!pair.second->IsStarted)
                list.push_back(pair.second);
        }
        return list;
    }

    public:
        void AddLobbyClient(SOCKET sock)
        {
            std::lock_guard<std::mutex> lock(LobbyMutex);
            LobbyClients.push_back(sock);
        }

        void RemoveLobbyClient(SOCKET sock)
        {
            std::lock_guard<std::mutex> lock(LobbyMutex);
            LobbyClients.erase(
                std::remove(LobbyClients.begin(), LobbyClients.end(), sock),
                LobbyClients.end());
        }

        void BroadcastLobby(const char* data, int size)
        {
            std::lock_guard<std::mutex> lock(LobbyMutex);
            for (SOCKET sock : LobbyClients)
            {
                send(sock, data, size, 0);
            }
        }

        void SetClientMyOwnerId(SOCKET ClientSocket, const char* name)
        {
            // 연결 순서 패킷 전송
            int LobbyClientsCount = LobbyClients.size();
            for (int i = 0; i < LobbyClientsCount; i++)
            {
                if (ClientSocket == LobbyClients[i]) {
                    FConnectionPacket data{};
                    data.Header.Size = sizeof(FConnectionPacket);
                    data.Header.Type = static_cast<int>(EPacketType::Client);
                    data.order = nextClientId++;
                    strncpy_s(data.Name, name, 7);
                    data.Name[7] = '\0';
                    send(LobbyClients[i], (char*)&data, sizeof(data), 0);

                    SetClientName(ClientSocket, name);   //추가: 소켓에 이름 영구 저장


                    return;
                }               
            }
        }

        void SetClientName(SOCKET sock, const char* name)
        {
            std::lock_guard<std::mutex> lock(NameMutex);
            SocketToName[sock] = name;
        }

        std::string GetNameBySocket(SOCKET sock)
        {
            std::lock_guard<std::mutex> lock(NameMutex);
            auto it = SocketToName.find(sock);
            if (it != SocketToName.end()) return it->second;
            return "Unknown";
        }       
        

private:
    std::unordered_map<int, std::shared_ptr<GameSession>> Sessions;
    std::unordered_map<SOCKET, std::shared_ptr<GameSession>> SocketToSession;
    std::mutex Mutex;
    int nextSessionId = 0;
    int nextClientId = 0;

    private:
        std::vector<SOCKET> LobbyClients; // 로비에 있는 소켓들
        std::mutex LobbyMutex;

    private:
        std::unordered_map<SOCKET, std::string> SocketToName;   // 소켓 -> 이름
        std::mutex NameMutex;
};