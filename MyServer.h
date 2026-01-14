// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ClientTrd.h"
#include "TimerManager.h"   // 중요
#include "Bullet.h"
#include "MyEnemy.h"

#include "TypeDefine.h"
#include "MyServer.generated.h"
/**
 * 
 */
UCLASS()
class WARGAME_API UMyServer : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	

	void MoveClient(const FServerBullet& bullet);
	void Shotoccurred(const FServerBullet& bullet);
	void SetThreadSocketHandle();//쓰레드 생성
	void SpawnActor();
	void SetBulletClass(TSubclassOf<ABullet> blueprint);
	void SetEnermyClass(TSubclassOf<AMyEnemy> blueprint);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ABullet> BulletClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AMyEnemy> EnermyClass;
	UPROPERTY()
	TMap<int32, ABullet*> SpawnItems;
	UPROPERTY()
	TMap<int32, AMyEnemy*> SpawnEnemys;
	UFUNCTION()
	void OnBulletHit(int32 BulletId);
	UPROPERTY()
	TMap<int32, bool> Usedbullet;
	UPROPERTY()
	TMap<int32, bool> Usedbullet2;

private:
	void* SocketHandle;//socket으로 캐스팅해서 쓰기
	//AMyCharacter* MyChar;
	ClientTrd* ClientThread;
	FTimerHandle ServerTimer;
	//bool IsSpawned;
	
};
