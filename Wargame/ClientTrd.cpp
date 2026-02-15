// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientTrd.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "MyServer.h"
#include "Windows/HideWindowsPlatformTypes.h"

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
	Thread = FRunnableThread::Create(this, TEXT("Network Thread"));
	//RecvQueue = new TQueue<FServerBulletPos, EQueueMode::Mpsc>();
	
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
	UE_LOG(LogNet, Warning, TEXT("Thread has been initialized"));
	
	return true;
	
}

uint32 ClientTrd::Run()
{
	SOCKET Sock = reinterpret_cast<SOCKET>(Socket);

	while (bRun)
	{
		/* ===============================
		   1. 헤더 먼저 받기
		   =============================== */
		FPacketHeader Header;
		int Received = 0;

		while (Received < sizeof(FPacketHeader))
		{
			int Len = recv(Sock,
				reinterpret_cast<char*>(&Header) + Received,
				sizeof(FPacketHeader) - Received,
				0);

			if (Len <= 0)
			{
				UE_LOG(LogTemp, Error, TEXT("Header recv failed"));
				return 0;
			}
			Received += Len;
		}

		/* ===============================
		   2. 패킷 타입 분기
		   =============================== */
		switch ((EPacketType)Header.Type)
		{
		case EPacketType::Bullet:
		{
			FServerBullet Bullet;
			Bullet.Header = Header;

			int BodySize = sizeof(FServerBullet) - sizeof(FPacketHeader);
			int BodyReceived = 0;

			while (BodyReceived < BodySize)
			{
				int Len = recv(Sock,
					reinterpret_cast<char*>(&Bullet) + sizeof(FPacketHeader) + BodyReceived,
					BodySize - BodyReceived,
					0);

				if (Len <= 0)
				{
					UE_LOG(LogTemp, Error, TEXT("Bullet recv failed"));
					return 0;
				}
				BodyReceived += Len;
			}

			BulletQueue->Enqueue(Bullet);
			Server->SetBulletPacket(Bullet);
			
			break;
		}

		case EPacketType::Character:
		{
			FCharacterPacket Character;
			Character.Header = Header;

			int BodySize = sizeof(FCharacterPacket) - sizeof(FPacketHeader);
			int BodyReceived = 0;

			while (BodyReceived < BodySize)
			{
				int Len = recv(Sock,
					reinterpret_cast<char*>(&Character) + sizeof(FPacketHeader) + BodyReceived,
					BodySize - BodyReceived,
					0);

				if (Len <= 0)
				{
					UE_LOG(LogTemp, Error, TEXT("Character recv failed"));
					return 0;
				}
				BodyReceived += Len;
			}

			CharacterQueue->Enqueue(Character);
			Server->SetCharacterPacket(Character);
			//
			successConnect = true;
			break;
		}
		case EPacketType::InformationText:
		{
			FInformationTextPacket TimePacket;
			TimePacket.Header = Header;

			int BodySize = sizeof(FInformationTextPacket) - sizeof(FPacketHeader);
			int BodyReceived = 0;

			while (BodyReceived < BodySize)
			{
				int Len = recv(Sock,
					reinterpret_cast<char*>(&TimePacket) + sizeof(FPacketHeader) + BodyReceived,
					BodySize - BodyReceived,
					0);

				if (Len <= 0)
				{
					UE_LOG(LogTemp, Error, TEXT("timetext recv failed"));
					return 0;
				}
				BodyReceived += Len;
			}

			TimeInformationQueue->Enqueue(TimePacket);
			//Server->StartCountdownByPacket();
			AsyncTask(ENamedThreads::GameThread, [this]()
				{
					if (Server)
					{
						Server->StartCountdownByPacket();
					}
				});
			break;
		}
		case EPacketType::Grenade:
		{
			FGrenadePacket Packet;
			Packet.Header = Header;

			int BodySize = sizeof(FGrenadePacket) - sizeof(FPacketHeader);
			int BodyReceived = 0;

			while (BodyReceived < BodySize)
			{
				int Len = recv(Sock,
					reinterpret_cast<char*>(&Packet) + sizeof(FPacketHeader) + BodyReceived,
					BodySize - BodyReceived,
					0);

				if (Len <= 0)
				{
					UE_LOG(LogTemp, Error, TEXT("grenade recv failed"));
					return 0;
				}
				BodyReceived += Len;
			}

			GrenadeQueue->Enqueue(Packet);
			
			AsyncTask(ENamedThreads::GameThread, [this]()
				{
					if (Server)
					{
						Server->SpawnGrenade();
					}
				});
			break;
		}
		case EPacketType::AiSpawn: {
			FSpawnAIPacket AIPacket;
			AIPacket.Header = Header;

			int BodySize = sizeof(FSpawnAIPacket) - sizeof(FPacketHeader);
			int BodyReceived = 0;

			while (BodyReceived < BodySize)
			{
				int Len = recv(Sock,
					reinterpret_cast<char*>(&AIPacket) + sizeof(FPacketHeader) + BodyReceived,
					BodySize - BodyReceived,
					0);

				if (Len <= 0)
				{
					UE_LOG(LogTemp, Error, TEXT("AI Spawn recv failed"));
					return 0;
				}
				BodyReceived += Len;
			}

			AISpawnQueue->Enqueue(AIPacket);
			//Server->SetCharacterPacket(AIPacket);
			//
			//successConnect = true;
			break;
		}
		case EPacketType::Damage: {
			FDamagePacket DmgPacket;
			DmgPacket.Header = Header;

			int BodySize = sizeof(FDamagePacket) - sizeof(FPacketHeader);
			int BodyReceived = 0;

			while (BodyReceived < BodySize)
			{
				int Len = recv(Sock,
					reinterpret_cast<char*>(&DmgPacket) + sizeof(FPacketHeader) + BodyReceived,
					BodySize - BodyReceived,
					0);

				if (Len <= 0)
				{
					UE_LOG(LogTemp, Error, TEXT("Damage Spawn recv failed"));
					return 0;
				}
				BodyReceived += Len;
			}

			DmgQueue->Enqueue(DmgPacket);
			
			break;
		}

		default:
			UE_LOG(LogTemp, Warning, TEXT("Unknown packet type: %d"), Header.Type);
			break;
		}
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
}

void ClientTrd::Stop()
{
	bRun = false;
}
