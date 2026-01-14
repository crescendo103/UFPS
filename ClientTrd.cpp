// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientTrd.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Windows/HideWindowsPlatformTypes.h"

ClientTrd::ClientTrd(void* InSocket)
	: Socket(InSocket)
{
	bRun = true;
	RecvQueue = new TQueue<FServerBulletPos, EQueueMode::Mpsc>();
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
	while (bRun)
	{
		char RecvBuff[sizeof(FServerBulletPos)]; // 패킷을 저장할 변수
		int TotalReceived = 0;
		const int PacketSize = sizeof(FServerBulletPos);
		SOCKET Sock = reinterpret_cast<SOCKET>(Socket);

		while (TotalReceived < PacketSize)
		{
			// Recv		
			int RecvLen = recv(Sock, RecvBuff + TotalReceived,
				PacketSize - TotalReceived, 0);

			if (RecvLen <= 0)
			{
				printf("recv failed or connection closed\n");
				return 0;
			}
			TotalReceived += RecvLen;
			/*
			UE_LOG(LogTemp, Warning,
				TEXT("Recv %d bytes, expected %d"),
				TotalReceived, sizeof(FServerBulletPos));
				*/
		}

		FServerBulletPos BulletPos;	
		memcpy(&BulletPos, RecvBuff, sizeof(FServerBulletPos));		
		RecvQueue->Enqueue(BulletPos);

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
