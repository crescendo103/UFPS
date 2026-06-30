// main.cpp
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <queue>
#include <mutex>
#include <memory>
#include <conio.h>
#include "Common.h"
#include "GameSession.h"
#include "SessionManager.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// ─── 수신 큐 (전역 하나로 공유) ─────────────────────────
struct RecvPacket
{
    std::vector<char> data;
    SOCKET sock;
    std::shared_ptr<GameSession> session;
};

std::queue<RecvPacket> IncomingQueue;
std::mutex IncomingMutex;
bool ProcessThreadExisted = false;

struct FRecvBuffer
{
    static constexpr int BUFFER_SIZE = 8192;
    char Buffer[BUFFER_SIZE];
    int ReadPos = 0;
    int WritePos = 0;

    int   GetDataSize() { return WritePos - ReadPos; }
    char* GetReadPtr() { return Buffer + ReadPos; }
    char* GetWritePtr() { return Buffer + WritePos; }
    void  OnWrite(int Len) { WritePos += Len; }
    void  OnRead(int Len)
    {
        ReadPos += Len;
        if (ReadPos == WritePos) ReadPos = WritePos = 0;
    }
};

const char* RoomTypeName(int type)
{
    switch ((ERoomPacketType)type)
    {
    case ERoomPacketType::CreateRoom:  return "CreateRoom";
    case ERoomPacketType::JoinRoom:    return "JoinRoom";
    case ERoomPacketType::RoomList:    return "RoomList";
    case ERoomPacketType::StartGame:   return "StartGame";
    case ERoomPacketType::RoomInfo:    return "RoomInfo";
    case ERoomPacketType::RemoveRoom:  return "RemoveRoom";
    default:                           return "Unknown";
    }
}

// recivePacket 앞단에 로비 처리 추가
void HandleLobbyPacket(FPacketHeader* Header, SOCKET ClientSocket)
{
    FRoomPacket* room = reinterpret_cast<FRoomPacket*>(Header);

    printf("[RECV][Lobby] sock=%llu RoomType=%s (raw=%d)\n",
        (unsigned long long)ClientSocket,
        RoomTypeName((int)room->RoomType),
        (int)room->RoomType);

    switch (room->RoomType)
    {
    case ERoomPacketType::CreateRoom:
    {
        auto session = SessionManager::Get().CreateRoom(ClientSocket, room->RoomName);

        session->clientSockets[session->clientSocketsIndex] = ClientSocket;
        session->SocketToClientId[ClientSocket] = session->clientSocketsIndex;
        session->ClientIdToSocket[session->clientSocketsIndex] = ClientSocket;   // 추가
        session->clientSocketsIndex++;

        // 방 생성자는 로비에서 제거
        SessionManager::Get().RemoveLobbyClient(ClientSocket);

        FRoomPacket resp{};
        resp.Header.Size = sizeof(FRoomPacket);
        resp.Header.Type = static_cast<int>(EPacketType::Room);
        resp.RoomType = ERoomPacketType::RoomInfo;
        resp.RoomId = session->SessionId;
        resp.PlayerCount = session->clientSocketsIndex;
        resp.HostId = session->SocketToClientId[ClientSocket];
        strncpy_s(resp.RoomName, session->RoomName, 15);


        // 로비 전체에 새 방 생성 알림
        SessionManager::Get().BroadcastLobby((char*)&resp, sizeof(resp));
        // 방 만든 사람한테도 전송
        send(ClientSocket, (char*)&resp, sizeof(resp), 0);

        printf("[Lobby] 방 생성 완료 RoomId=%d\n", session->SessionId);
        break;
    }

    case ERoomPacketType::JoinRoom:
    {
        printf("[JoinRoom] 시도 RoomId=%d ClientSocket=%llu\n",
            room->RoomId, (unsigned long long)ClientSocket);

        auto session = SessionManager::Get().JoinRoom(room->RoomId, ClientSocket);
        if (!session)
        {
            printf("[JoinRoom] 실패! RoomId=%d\n", room->RoomId);
            
                return;
        }
        printf("[JoinRoom] 성공! 인원=%d\n", session->clientSocketsIndex);

        //auto session = SessionManager::Get().JoinRoom(room->RoomId, ClientSocket);
        /*6.25
        if (!session)
        {
            FRoomPacket resp{};
            resp.Header.Size = sizeof(FRoomPacket);
            resp.Header.Type = static_cast<int>(EPacketType::Room);
            resp.RoomType = ERoomPacketType::RoomInfo;
            resp.RoomId = -1;
            send(ClientSocket, (char*)&resp, sizeof(resp), 0);
            //return;
        }*/

        session->clientSockets[session->clientSocketsIndex] = ClientSocket;
        session->SocketToClientId[ClientSocket] = session->clientSocketsIndex;
        session->ClientIdToSocket[session->clientSocketsIndex] = ClientSocket;   //  추가
        session->clientSocketsIndex++;

        // 참가자는 로비에서 제거
        SessionManager::Get().RemoveLobbyClient(ClientSocket);

        // 방 안 모든 사람에게 새 플레이어 order 브로드캐스트
        FConnectionPacket connData{};
        connData.Header.Size = sizeof(FConnectionPacket);
        connData.Header.Type = static_cast<int>(EPacketType::Client);
        connData.order = session->clientSocketsIndex - 1; // 새로 들어온 사람 order?
        strncpy_s(connData.Name, "temp", 7);
        connData.Name[7] = '\0';
        session->Broadcast((char*)&connData, sizeof(connData));
        
        // ─── 참가한 사람한테 RemoveRoom 전송 (방 목록 UI 제거)
        FRoomPacket removeResp{};
        removeResp.Header.Size = sizeof(FRoomPacket);
        removeResp.Header.Type = static_cast<int>(EPacketType::Room);
        removeResp.RoomType = ERoomPacketType::RemoveRoom;
        removeResp.RoomId = session->SessionId;
        send(ClientSocket, (char*)&removeResp, sizeof(removeResp), 0);

        // ─── 방 안 사람들 + 로비 사람들에게 인원 변경 RoomInfo 전송
        FRoomPacket resp{};
        resp.Header.Size = sizeof(FRoomPacket);
        resp.Header.Type = static_cast<int>(EPacketType::Room);
        resp.RoomType = ERoomPacketType::RoomInfo;
        resp.RoomId = session->SessionId;
        resp.PlayerCount = session->clientSocketsIndex;
        resp.HostId = session->SocketToClientId[session->HostSocket];
        strncpy_s(resp.RoomName, session->RoomName, 15);

        session->Broadcast((char*)&resp, sizeof(resp));
        SessionManager::Get().BroadcastLobby((char*)&resp, sizeof(resp));

        printf("[Lobby] 방 %d 참가 완료 현재인원=%d\n",
            session->SessionId, session->clientSocketsIndex);
        break;
    }

    case ERoomPacketType::StartGame:
    {
        bool ok = SessionManager::Get().TryStartGame(ClientSocket);
        if (ok)
        {
            // 게임 시작된 방은 로비 목록에서 제거 알림
            auto session = SessionManager::Get().FindBySocket(ClientSocket);
            if (session)
            {
                FRoomPacket resp{};
                resp.Header.Size = sizeof(FRoomPacket);
                resp.Header.Type = static_cast<int>(EPacketType::Room);
                resp.RoomType = ERoomPacketType::RemoveRoom;
                resp.RoomId = session->SessionId;
                SessionManager::Get().BroadcastLobby((char*)&resp, sizeof(resp));
            }
        }
        printf("[Lobby] 시작 요청 결과: %s\n", ok ? "성공" : "실패");
        break;
    }

    case ERoomPacketType::RemoveRoom:
        // TODO..
        break;
    }
}

void recivePacket(SOCKET ClientSocket, std::shared_ptr<GameSession> session)
{
    FRecvBuffer RecvBuf;
    int lastPacketSize = 0, lastPacketType = -1;

    while (true)
    {
        int RecvLen = recv(ClientSocket,
            RecvBuf.GetWritePtr(),
            FRecvBuffer::BUFFER_SIZE - RecvBuf.WritePos, 0);

        if (RecvLen == 0)
        {
            printf("disconnected\n");
            return;
        }
        else if (RecvLen < 0)
        {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            printf("recv error: %d\n", err);
            return;
        }

        RecvBuf.OnWrite(RecvLen);

        while (true)
        {
            if (RecvBuf.GetDataSize() < sizeof(FPacketHeader)) break;

            FPacketHeader* Header =
                reinterpret_cast<FPacketHeader*>(RecvBuf.GetReadPtr());

            lastPacketSize = Header->Size;
            lastPacketType = Header->Type;

            if (RecvBuf.GetDataSize() < Header->Size) break;

            // 캐릭터(플레이어)는 즉시 세션에 저장
            if (Header->Type == (int)EPacketType::Character)
            {
                FCharacterPacket* Char =
                    reinterpret_cast<FCharacterPacket*>(Header);
                if (Char->CharacterId > -100) // -100이하는 ai의 위치를 나타내는 패킷으로..
                {
                    if (session != nullptr) // 방생성 or joinRoom해서 게임세션이 있는 상태만 현재 캐릭터 위치를 저장하도록
                    {
                        int clientId = session->SocketToClientId[ClientSocket];
                        std::lock_guard<std::mutex> lock(session->CharacterMutex);
                        session->People[clientId] = *Char;
                    }
                    RecvBuf.OnRead(Header->Size);
                    continue;
                }
            }

            // 서버와 클라가 연결됬을 경우 이름이나 클라 자기 고유 ID(클라에서는 MYOwner)를 설정하기 위해 주고 받는 패킷
            if (Header->Type == (int)EPacketType::Client)
            {
                FConnectionPacket* Conn =
                    reinterpret_cast<FConnectionPacket*>(Header);

                if (Conn->order == -1) //처음 이런 종류의 패킷을 받음 고유 ID와 소켓과 대응되는 이름을 설정해야함
                {
                    SessionManager::Get().SetClientMyOwnerId(ClientSocket, Conn->Name);
                }                
            }

            if (Header->Type == (int)EPacketType::Room)
            {
                HandleLobbyPacket(Header, ClientSocket);//방(로비)생성과 참가에 대한 패킷 처리하는 함수
                // 방 생성/참가 후 session 업데이트
                if (session == nullptr)
                {
                    session = SessionManager::Get().FindBySocket(ClientSocket);
                }

                RecvBuf.OnRead(Header->Size);
                continue;
            }
            
            // 게임 패킷인데 아직 세션 없으면 무시
            if (session == nullptr)
            {
                RecvBuf.OnRead(Header->Size);
                continue;
            }

            // 나머지는 ProcessThread로
            RecvPacket pkt;
            pkt.data.resize(Header->Size);
            memcpy(pkt.data.data(), RecvBuf.GetReadPtr(), Header->Size);
            pkt.sock = ClientSocket;
            pkt.session = session;

            {
                std::lock_guard<std::mutex> lock(IncomingMutex);
                if (IncomingQueue.size() > 1000) IncomingQueue.pop();
                IncomingQueue.push(std::move(pkt));
            }

            RecvBuf.OnRead(Header->Size);
        }
    }
}

void ProcessThread()
{
    while (true)
    {
        RecvPacket pkt;
        {
            std::lock_guard<std::mutex> lock(IncomingMutex);
            if (IncomingQueue.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            pkt = std::move(IncomingQueue.front());
            IncomingQueue.pop();
        }

        FPacketHeader* header =
            reinterpret_cast<FPacketHeader*>(pkt.data.data());

        pkt.session->DataProcess(header, pkt.sock);
    }
}

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed\n");
        return 1;
    }

    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ListenSocket == INVALID_SOCKET) { WSACleanup(); return 1; }

    SOCKADDR_IN ServerAddr{};
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(6000);
    ServerAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(ListenSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
    {
        closesocket(ListenSocket); WSACleanup(); return 1;
    }
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        closesocket(ListenSocket); WSACleanup(); return 1;
    }

    // ProcessThread 하나만 실행
    std::thread(ProcessThread).detach();

    printf("Server listening on port 6000...\n");

    while (true)
    {
        SOCKADDR_IN ClientAddr{};
        int AddrLen = sizeof(ClientAddr);
        SOCKET ClientSocket = accept(ListenSocket, (sockaddr*)&ClientAddr, &AddrLen);

        if (ClientSocket == INVALID_SOCKET)
        {
            printf("accept() failed\n");
            continue;
        }

        // non-blocking
        u_long mode = 1;
        ioctlsocket(ClientSocket, FIONBIO, &mode);

        printf("Client connected!\n");       

        // 클라 접속하면 로비에 추가
        SessionManager::Get().AddLobbyClient(ClientSocket);

        // 세션 배정 없음 - 수신 스레드만 띄움
        // 클라이언트가 방 생성/참가 패킷을 보내면 그때 세션에 등록됨
        std::thread([ClientSocket]()
            {
                recivePacket(ClientSocket, nullptr); // session은 처음엔 null
            }).detach();
    }

    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}