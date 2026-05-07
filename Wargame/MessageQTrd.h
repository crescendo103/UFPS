// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FPS_API MessageQTrd : public FRunnable
{
public:
	MessageQTrd();
	~MessageQTrd();
	// 상속받은 세개의 함수
	bool Init() override;
	uint32 Run() override;
	void Exit() override;
	void Stop() override;
};
