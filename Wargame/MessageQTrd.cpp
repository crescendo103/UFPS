// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageQTrd.h"

MessageQTrd::MessageQTrd()
{
}

MessageQTrd::~MessageQTrd()
{
}

bool MessageQTrd::Init()
{
	UE_LOG(LogNet, Warning, TEXT("MessageQTrd has been initialized"));

	return true;
}

uint32 MessageQTrd::Run()
{
	return uint32();
}

void MessageQTrd::Exit()
{
	UE_LOG(LogTemp, Error, TEXT("MessageQTrd Exit"));
}

void MessageQTrd::Stop()
{
	//bRun = false;
}
