// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TypeDefine.h"

/**
 * 
 */
class WARGAME_API ClientTrd : public FRunnable
{
public:
	ClientTrd(void* InSocket);
	~ClientTrd();

	// 상속받은 세개의 함수
	bool Init() override;
	uint32 Run() override;
	void Exit() override;
	void Stop() override;

	// 소켓 변수
	void* Socket;
	bool bRun;
	TQueue<FServerBulletPos, EQueueMode::Mpsc>* RecvQueue;
private:
	// 쓰레드
	FRunnableThread* Thread;
	
	//UMyServer* InServer;
};
