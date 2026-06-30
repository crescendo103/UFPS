// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TypeDefine.h"

/**
 * 
 */
class UMyServer;
class FPS_API ClientTrd : public FRunnable
{
public:
	ClientTrd(void* InSocket, UMyServer* InServer);
	~ClientTrd();

	// 상속받은 세개의 함수
	bool Init() override;
	uint32 Run() override;
	void Exit() override;
	void Stop() override;

	// 소켓 변수
	void* Socket;
	bool bRun;
	bool successConnect;
	//-----Myserver에서 꺼네서 쓰는 서버로 부터 받은 패킷을 담은 큐들-----------
	TQueue<FServerBullet, EQueueMode::Mpsc>* BulletQueue;
	TQueue<FCharacterPacket, EQueueMode::Mpsc>* CharacterQueue;
	TQueue<FInformationTextPacket, EQueueMode::Mpsc>* TimeInformationQueue;
	TQueue<FGrenadePacket, EQueueMode::Mpsc>* GrenadeQueue;
	TQueue<FSpawnAIPacket, EQueueMode::Mpsc>* AISpawnQueue;
	TQueue<FDamagePacket, EQueueMode::Mpsc>* DmgQueue;
	TQueue<FConnectionPacket, EQueueMode::Mpsc>* ConnectQueue;
	TQueue<FDeathPacket, EQueueMode::Mpsc>* DeathQueue;
	TQueue<FServerBullet, EQueueMode::Mpsc>* MeleeQueue;
	TQueue<FItemPacket, EQueueMode::Mpsc>* ItemSpawnQueue;
	TQueue<FVehiclePacket, EQueueMode::Mpsc>* VehicleQueue;
	TQueue<FRoomPacket, EQueueMode::Mpsc>* RoomQueue;
	//----------------------------------------------------------------------
	UMyServer* Server;	
private:
	// 쓰레드
	FRunnableThread* Thread;	
};
