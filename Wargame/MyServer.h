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
class ALoadingEnemy2;
class AAIEnemy;
class AMyCharacter;
class AStartMenuPawn;
class UStartMenuWidget;
class ABomb;
class ABlueHole;
class UGameConfigData;
UCLASS()
class FPS_API UMyServer : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	void Init(UGameConfigData* Config);

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	
	void MoveConnect(FConnectionPacket packet);
	void MoveClient(FCharacterPacket bullet);
	void MoveAI(FCharacterPacket packet);
	void MoveDmg(FDamagePacket packet);
	void MoveTime(int32 time);
	void MoveDeath(FDeathPacket packet);
	void MoveItem(FItemPacket packet);
	void Shotoccurred(FServerBullet bullet);
	void SetThreadSocketHandle();//쓰레드 생성
	void SpawnActor();
	void SetBulletClass(TSubclassOf<ABullet> blueprint);
	void SetEnermyClass(TSubclassOf<AMyEnemy> blueprint);
	void SetAiClass(TSubclassOf<AAIEnemy> blueprint);
	
	
	void SetWatingRoomPointer(UWatingRoom* room);
	
	void SetRocalPlayer(AActor* actor);

	void ProcessBulletPacket();
	void PorcessCharacterPacket();

	void SetBulletPacket(FServerBullet resentPacket);
	void SetCharacterPacket(FCharacterPacket resentPacket);

	void CreateGrenade(FGrenadePacket packet);
	void SetGrenadeClass(TSubclassOf<AMyGrenade> blueprint);
	void SpawnGrenade();
	void SpawnBullet(FServerBullet bullet);
	void SpawnMelee();
	FTimerHandle CountdownTimerr;
	int32 CountdownTime = 300;

	void StartCountdownByPacket(int countdownamount); // 패킷 true 들어왔을 때 호출
	


	void MoveStartEnemyPawn(int order);
	void SetStartMenuPawn(APawn* pawn);

	bool SendAll(const char* data, int size);

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
	UPROPERTY(BlueprintReadOnly)
	TMap<int32, AMyEnemy*> SpawnEnemys;
	UPROPERTY()
	TMap<int32, AMyGrenade*> SpawnGrenades;
	UPROPERTY(BlueprintReadOnly)
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

	
private:
	FTimerHandle CountdownTimer;

	void UpdateCountdownUI();

public:
	void* SocketHandle;//socket으로 캐스팅해서 쓰기
	//AMyCharacter* MyChar;
	ClientTrd* ClientThread;
	FTimerHandle ServerTimer;
	int32 MyOwner;
	bool IsSetMyOwner;
	//bool IsSpawned;
public:
	UPROPERTY()
	TArray<ALoadingEnemy2*> StartPlayers;
	//SOCKET Socket;
	FCriticalSection SendMutex;

	//====redzone===관련
	void SetBombClass(TSubclassOf<ABomb> blueprint);
	void StartRedzone(FVector Center);
	void EndRedzone();
	void SpawnBomb();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "redZone")
	TSubclassOf<ABomb> BombClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "redZone")
	ABomb* Bomb;
	FTimerHandle timerHandle;
	
	FVector RedzoneCenter;
	float RedzoneRadius = 3000.0f;   // 반지름 (원하는 값으로 변경)
	int32 SpawnCount = 0;
	int32 MaxSpawnCount = 10;   // 10번 생성하면 끝
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BlueHole")
	TSubclassOf<ABlueHole> BlueHoleClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BlueHole")
	ABlueHole* BlueHole;

	void SetBlueHoleClass(TSubclassOf<ABlueHole> blueprint);
	void StartBlueHole(FVector Center);	



	// 아이템 정적 데이터
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	UDataTable* ItemTable;

	void SpawnItemByServer(int32 itemRow);
	bool bInitialized;
};
