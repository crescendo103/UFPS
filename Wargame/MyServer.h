// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ClientTrd.h"
#include "TimerManager.h"   // 중요
#include "Bullet.h"
#include "MyEnemy.h"
#include "WatingRoom.h"
#include "MyGrenade.h"
#include "TypeDefine.h"


#include "MyServer.generated.h"
/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FOnStartCountdown);

class AAIEnemy;
class AMyCharacter;
class AStartMenuPawn;
class UStartMenuWidget;

UCLASS()
class FPS_API UMyServer : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	

	void MoveClient(FCharacterPacket bullet);
	void MoveAI(FCharacterPacket packet);
	void MoveDmg(FDamagePacket packet);
	void MoveTime(int32 time);
	void MoveDeath(FDeathPacket packet);
	void Shotoccurred(FServerBullet bullet);
	void SetThreadSocketHandle();//쓰레드 생성
	void SpawnActor();
	void SetBulletClass(TSubclassOf<ABullet> blueprint);
	void SetEnermyClass(TSubclassOf<AMyEnemy> blueprint);
	void SetAiClass(TSubclassOf<AAIEnemy> blueprint);
	void CheckMyOwner(FVector pos, int32 id);
	
	void SetWatingRoomPointer(UWatingRoom* room);
	
	void SetRocalPlayer(AActor* actor);

	void ProcessBulletPacket();
	void PorcessCharacterPacket();

	void SetBulletPacket(FServerBullet resentPacket);
	void SetCharacterPacket(FCharacterPacket resentPacket);

	void CreateGrenade(FGrenadePacket packet);
	void SetGrenadeClass(TSubclassOf<AMyGrenade> blueprint);
	void SpawnGrenade();
	void SpawnBullet();
	FTimerHandle CountdownTimerr;
	int32 CountdownTime = 300;

	void StartCountdownByPacket(int countdownamount); // 패킷 true 들어왔을 때 호출
	void SetMyCharacter(AMyCharacter* character);


	void MoveStartEnemyPawn(int order);
	void SetStartMenuPawn(APawn* pawn);

	UPROPERTY()
	APawn* StartMenuPawn;



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ABullet> BulletClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AMyEnemy> EnermyClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AMyGrenade> GrenadeClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AAIEnemy> AiClass;
	UPROPERTY()
	AActor* LocalPlayer;
	UPROPERTY()
	TMap<int32, ABullet*> SpawnItems;
	UPROPERTY()
	TMap<int32, AMyEnemy*> SpawnEnemys;
	UPROPERTY()
	TMap<int32, AMyGrenade*> SpawnGrenades;
	UPROPERTY()
	TMap<int32, AAIEnemy*> SpawnAis;
	UFUNCTION()
	void OnBulletHit(int32 BulletId, int32 CharacterId);
	UPROPERTY()
	TMap<int32, bool> Usedbullet;
	UPROPERTY()
	TMap<int32, bool> Usedbullet2;

	
	UPROPERTY()
	UWatingRoom* WatingRoomWidget;
	UPROPERTY()
	UStartMenuWidget* StartingMenuWidget;
	UPROPERTY()
	bool bCountdownStarted = false;

	FServerBullet bulletPacket;
	FCharacterPacket characterPacket;

	UPROPERTY()
	AMyCharacter* MyCharacter;
private:
	FTimerHandle CountdownTimer;

	void UpdateCountdownUI();

private:
	void* SocketHandle;//socket으로 캐스팅해서 쓰기
	//AMyCharacter* MyChar;
	ClientTrd* ClientThread;
	FTimerHandle ServerTimer;
	int32 MyOwner;
	bool IsSetMyOwner;
	//bool IsSpawned;
public:
	UPROPERTY()
	TArray<AStartMenuPawn*> StartPlayers;
};
