// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientTrd.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "MyServer.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "CustomPlayerController.h"

ClientTrd::ClientTrd(void* InSocket, UMyServer* InServer)
	: Socket(InSocket), Server(InServer)
{
	bRun = true;
	successConnect = false;
	BulletQueue = new TQueue<FServerBullet, EQueueMode::Mpsc>();
	CharacterQueue = new TQueue<FCharacterPacket, EQueueMode::Mpsc>();
	TimeInformationQueue = new TQueue<FInformationTextPacket, EQueueMode::Mpsc>();
	GrenadeQueue = new TQueue<FGrenadePacket, EQueueMode::Mpsc>();
	AISpawnQueue = new TQueue<FSpawnAIPacket, EQueueMode::Mpsc>();
	DmgQueue = new TQueue<FDamagePacket, EQueueMode::Mpsc>();
	ConnectQueue = new TQueue<FConnectionPacket, EQueueMode::Mpsc>();
	DeathQueue = new TQueue<FDeathPacket, EQueueMode::Mpsc>();
	MeleeQueue = new TQueue<FServerBullet, EQueueMode::Mpsc>();
	ItemSpawnQueue = new TQueue<FItemPacket, EQueueMode::Mpsc>();
	VehicleQueue = new TQueue<FVehiclePacket, EQueueMode::Mpsc>();
	Thread = FRunnableThread::Create(this, TEXT("Network Thread"));
	//RecvQueue = new TQueue<FServerBulletPos, EQueueMode::Mpsc>();
	RoomQueue = new TQueue<FRoomPacket, EQueueMode::Mpsc>();
}

ClientTrd::~ClientTrd()
{
	if (Thread)
	{
		// 스레드 종료
		Thread->WaitForCompletion();
		Thread->Kill();
		delete Thread;
	}
}

bool ClientTrd::Init()
{
	UE_LOG(LogNet, Warning, TEXT("ClientTrd has been initialized"));
	
	return true;
	
}
uint32 ClientTrd::Run()
{
	SOCKET Sock = reinterpret_cast<SOCKET>(Socket);

	const int BUFFER_SIZE = 8192;
	char Buffer[BUFFER_SIZE];

	int ReadPos = 0;
	int WritePos = 0;

	while (bRun)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Thread Socket ptr = %p"), Socket);
		// 🔥 1. recv (데이터 쌓기)
		int len = recv(Sock, Buffer + WritePos, BUFFER_SIZE - WritePos, 0);

		if (len == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				// 데이터 없음 → 잠깐 쉬고 루프 계속
				FPlatformProcess::Sleep(0.001f); // 1ms
				continue;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("recv error: %d"), err);
				break;
			}
		}

		else if (len == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Server disconnected"));
			break;
		}

		

		WritePos += len;

		// 🔥 2. 패킷 파싱 루프
		while (true)
		{
			int DataSize = WritePos - ReadPos;

			// 2-1. 헤더 부족하면 대기
			if (DataSize < sizeof(FPacketHeader))
				break;

			FPacketHeader* Header =
				reinterpret_cast<FPacketHeader*>(Buffer + ReadPos);

			//UE_LOG(LogTemp, Warning, TEXT("Recv Header Size: %d"), Header->Size);
			// 🔥 안전 체크 (이거 중요)
			if (Header->Size <= 0 || Header->Size > 10000)
			{
				UE_LOG(LogTemp, Error, TEXT("Invalid packet size: %d"), Header->Size);
				bRun = false;
				break;
			}

			// 2-2. 패킷 전체 도착 안했으면 대기
			if (DataSize < Header->Size)
				break;

			// 🔥 3. 패킷 처리
			char* PacketPtr = Buffer + ReadPos;

			switch ((EPacketType)Header->Type)
			{
			case EPacketType::Bullet:
			{
				FServerBullet* Bullet = reinterpret_cast<FServerBullet*>(PacketPtr);
				//BulletQueue->Enqueue(*Bullet);

				AsyncTask(ENamedThreads::GameThread, [this, Bullet]()
					{
						if (Server)
							Server->SpawnBullet(*Bullet);
					});
				break;
			}

			case EPacketType::Character:
			{
				FCharacterPacket* Character = reinterpret_cast<FCharacterPacket*>(PacketPtr);
				CharacterQueue->Enqueue(*Character);
				successConnect = true;
				break;
			}
			case EPacketType::Vehicle:
			{
				FVehiclePacket* vehicle = reinterpret_cast<FVehiclePacket*>(PacketPtr);
				VehicleQueue->Enqueue(*vehicle);				
				break;
			}

			case EPacketType::InformationText:
			{
				FInformationTextPacket* TimePacket =
					reinterpret_cast<FInformationTextPacket*>(PacketPtr);

				int32 ReceivedTime = TimePacket->time;

				UE_LOG(LogTemp, Warning,
					TEXT("[CLIENT RECV] InformationText | Type=%d | Time=%d"),
					TimePacket->Header.Type,
					ReceivedTime);

				AsyncTask(ENamedThreads::GameThread, [this, ReceivedTime]()
					{
						if (Server)
							Server->StartCountdownByPacket(ReceivedTime);
					});
				break;
			}
			case EPacketType::BoradCast:
			{
				FBrodcastMessage* TextPacket =
					reinterpret_cast<FBrodcastMessage*>(PacketPtr);

				FString Message = FString(UTF8_TO_TCHAR(TextPacket->Information));


				AsyncTask(ENamedThreads::GameThread, [this, Message]()
					{
						if (Server)
							Server->SetInformationText(Message);
					});
				break;
			}
			case EPacketType::Redzone:
			{
				FSpawnAIPacket* RedzonePacket =
					reinterpret_cast<FSpawnAIPacket*>(PacketPtr);

				int32 CenterX = RedzonePacket->X;
				int32 CenterY = RedzonePacket->Y;
				int32 CenterZ = RedzonePacket->Z;
				FVector Center = FVector( CenterX ,CenterY ,CenterZ );
				AsyncTask(ENamedThreads::GameThread, [this, Center]()
					{
						if (Server)
							Server->StartRedzone(Center);
					});
				break;
			}
			case EPacketType::BlueHole:
			{
				FSpawnAIPacket* BlueHolePacket =
					reinterpret_cast<FSpawnAIPacket*>(PacketPtr);

				int32 CenterX = BlueHolePacket->X;
				int32 CenterY = BlueHolePacket->Y;
				int32 CenterZ = BlueHolePacket->Z;
				FVector Center = FVector(CenterX, CenterY, CenterZ);
				AsyncTask(ENamedThreads::GameThread, [this, Center]()
					{
						if (Server)
							Server->StartBlueHole(Center);
					});
				break;
			}
			case EPacketType::Grenade:
			{
				FGrenadePacket* Packet = reinterpret_cast<FGrenadePacket*>(PacketPtr);
				GrenadeQueue->Enqueue(*Packet);

				AsyncTask(ENamedThreads::GameThread, [this]()
					{
						if (Server)
							Server->SpawnGrenade();
					});
				break;
			}

			case EPacketType::AiSpawn:
			{
				FSpawnAIPacket* AIPacket = reinterpret_cast<FSpawnAIPacket*>(PacketPtr);
				AISpawnQueue->Enqueue(*AIPacket);
				break;
			}

			case EPacketType::Client:
			{
				FConnectionPacket* Conn = reinterpret_cast<FConnectionPacket*>(PacketPtr);
				ConnectQueue->Enqueue(*Conn);
				break;
			}

			case EPacketType::Death:
			{
				FDeathPacket* Packet = reinterpret_cast<FDeathPacket*>(PacketPtr);
				DeathQueue->Enqueue(*Packet);
				break;
			}

			case EPacketType::Item:
			{
				FItemPacket* Packet = reinterpret_cast<FItemPacket*>(PacketPtr);
				ItemSpawnQueue->Enqueue(*Packet);
				break;
			}

			case EPacketType::Melee:
			{
				FServerBullet* Melee = reinterpret_cast<FServerBullet*>(PacketPtr);
				MeleeQueue->Enqueue(*Melee);

				AsyncTask(ENamedThreads::GameThread, [this]()
					{
						if (Server)
							Server->SpawnMelee();
					});
				break;
			}
			case EPacketType::Room:
			{
				FRoomPacket* RoomPacket =
					reinterpret_cast<FRoomPacket*>(PacketPtr);				
				RoomQueue->Enqueue(*RoomPacket);
				//AsyncTask(ENamedThreads::GameThread, [this, RoomPacket]()
				//	{
				//		if (Server)
				//			Server->ProcessRoomPacket(*RoomPacket);
				//	});
				break;
			}
			default:
				UE_LOG(LogTemp, Warning, TEXT("Unknown packet type: %d"), Header->Type);
				break;
			}

			// 🔥 4. 읽은 만큼 이동
			ReadPos += Header->Size;
		}

		// 🔥 5. 버퍼 정리
		if (ReadPos == WritePos)
		{
			ReadPos = WritePos = 0;
		}
		else if (ReadPos > 0)
		{
			memmove(Buffer, Buffer + ReadPos, WritePos - ReadPos);
			WritePos -= ReadPos;
			ReadPos = 0;
		}

		//Server->MoveClient(5);
	}

	return 0;
}


void ClientTrd::Exit()
{
	/*
	SOCKET Sock = reinterpret_cast<SOCKET>(Socket);
	if (Sock)
	{
		// Socket 연결을 끊고 Winsock 사용을 종료
		closesocket(Sock);
		//WSACleanup();
	}*/

	UE_LOG(LogTemp, Error, TEXT("ClientTrd Exit"));
}

void ClientTrd::Stop()
{
	bRun = false;
}
