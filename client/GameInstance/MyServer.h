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
class APawnVehicle;
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
	
	//-------서버로 패킷 보내는 Move 함수들-------
	void MoveConnect(FConnectionPacket packet);
	void MoveClient(FCharacterPacket bullet);
	void MoveAI(FCharacterPacket packet);
	void MoveDmg(FDamagePacket packet);
	void MoveTime(int32 time);
	void MoveDeath(FDeathPacket packet);
	void MoveItem(FItemPacket packet);
	void MoveBroadCast(FBrodcastMessage packet);
	void MoveVehicle(FVehiclePacket packet);
	void MoveRoomPacket(FRoomPacket packet);
	
	void Shotoccurred(FServerBullet bullet);
	void CreateGrenade(FGrenadePacket packet);
	//------------------------------------------

	void SetThreadSocketHandle();//쓰레드 생성		

	void SetBulletClass(TSubclassOf<ABullet> blueprint);
	void SetEnermyClass(TSubclassOf<AMyEnemy> blueprint);
	void SetAiClass(TSubclassOf<AAIEnemy> blueprint);
	void SetGrenadeClass(TSubclassOf<AMyGrenade> blueprint);
	void SetBlueHoleClass(TSubclassOf<ABlueHole> blueprint);
	void SetBombClass(TSubclassOf<ABomb> blueprint);

	void SetRocalPlayer(AActor* actor);		
	
	TSubclassOf<AMyGrenade> GetGrenadeClass();
	//서버에서 받은 값을 토대로 총알, 적, 수류탄, 차량 등을 생성하는 함수들
	void SpawnGrenade();
	void SpawnBullet(FServerBullet bullet);
	void SpawnMelee();
	//------------------------------------------------------------------
		

	void StartCountdownByPacket(int countdownamount); // 패킷 true 들어왔을 때 호출
	void SetInformationText(FString message);

	void SetStartMenuPawn(APawn* pawn);

	bool SendAll(const char* data, int size);

	UPROPERTY()
	APawn* StartMenuPawn;
	UGameConfigData* Items;
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
	UPROPERTY(BlueprintReadOnly)
	TMap<int32, APawnVehicle*> SpawnVehicles;	
	UPROPERTY(BlueprintReadOnly)
	TMap<int32, AAIEnemy*> SpawnAis;
		
	UPROPERTY()
	UWatingRoom* WatingRoomWidget;
	UPROPERTY()
	UStartMenuWidget* StartingMenuWidget;
	UPROPERTY()
	bool bCountdownStarted = false;

	
private:
	FTimerHandle CountdownTimerr;
	int32 CountdownTime = 300;
	FTimerHandle CountdownTimer;
	FRoomPacket empty;
	void UpdateCountdownUI();

//public:
	void* SocketHandle;//socket으로 캐스팅해서 쓰기	
	ClientTrd* ClientThread;
	FTimerHandle ServerTimer;
	int32 MyOwner;	
	FCriticalSection SendMutex;
	FTimerHandle timerHandle;
public:
	UPROPERTY()
	TArray<ALoadingEnemy2*> StartPlayers;//시작화면 캐릭터들 연출용	

	//====자기장, redzone===관련	
	void StartRedzone(FVector Center);
	void EndRedzone();
	void SpawnBomb();
	void StartBlueHole(FVector Center);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "redZone")
	TSubclassOf<ABomb> BombClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "redZone")
	ABomb* Bomb;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "redZone")
	TSubclassOf<AActor> RedzoneClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "redZone")
	AActor* VisualRedZone;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BlueHole")
	TSubclassOf<ABlueHole> BlueHoleClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BlueHole")
	ABlueHole* BlueHole;

	FVector RedzoneCenter;
	float RedzoneRadius = 10000.0f;   // 반지름 (원하는 값으로 변경)
	int32 SpawnCount = 0;
	int32 MaxSpawnCount = 10;   // 10번 생성하면 끝		

	// 아이템 정적 데이터
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	UDataTable* ItemTable;

	TSubclassOf<AActor> GetRealActiveClassByID(int32 TargetID);
	
	bool bInitialized;
	const int MaxCharacterPacketsPerTick = 50;//최대 50개의 캐릭터 패킷을 한 틱에 처리
	const int MaxVehiclePacketsPerTick = 5;//최대 5개의 차량 패킷을 한 틱에 처리


	void ProcessPacket();//여기서 아래것들 실행

	//----------서버로부터 온 패킷 처리함수들-------
	void PorcessCharacterPacket();
	void ProcessSpawnAIPacket();
	void ProcessDamagePacket();
	void ProcessDeathPacket();
	void ProcessConnectionPacket();
	void ProcessItemPacket();
	void ProcessVehiclePacket();
	void ProcessRoomPacket(FRoomPacket packet);
	//------------------------------------------
};
