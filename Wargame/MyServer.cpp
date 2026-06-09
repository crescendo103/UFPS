// Fill out your copyright notice in the Description page of Project Settings.


#include "MyServer.h"
#include "WorldCollision.h"      // FOverlapResult 실제 정의
#include "Windows/AllowWindowsPlatformTypes.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Engine/World.h"
#include "Engine/EngineTypes.h" // 여기서 FOverlapResult 정의됨
#include "CollisionQueryParams.h"
#include "AIEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyCharacter.h"
#include "StartMenuPawn.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "StartMenuWidget.h"
#include "GameFramework/PlayerController.h"
#include "CustomPlayerController.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include "CustomPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "killAccountWidget.h"
#include "TimerManager.h"
#include "Bomb.h"
#include "BlueHole.h"
#include "LoadingEnemy2.h"
#include "HAL/CriticalSection.h"
#include "Misc/ScopeLock.h"
#include "ItemManagerSubsystem.h"
#include "GameConfigData.h"
#include "PawnVehicle.h"

//FCriticalSection SendMutex;


void UMyServer::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	

	UE_LOG(LogTemp, Warning, TEXT("UMyServer Initialize"));
	UE_LOG(LogTemp, Warning, TEXT("sizeof FServerBullet = %d"), sizeof(FServerBullet));

	WSADATA wsaData;
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);	// Winsock 초기화
	if (nRet != 0) {
		UE_LOG(LogTemp, Error, TEXT("nRet = 0"));
	}
	SOCKET Socket = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, 0);
	SocketHandle = reinterpret_cast<void*>(Socket);
	// 소켓 생성
	if (Socket == INVALID_SOCKET) {
		UE_LOG(LogTemp, Error, TEXT("Socket == INVALID_SOCKET"));
	}
	
	// IP, Port 정보 입력
	SOCKADDR_IN stServerAddr;
	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_port = htons(6000);
	stServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//inet_addr("192.168.219.109");//내 pc ip 61.254.29.99

	// 접속
	nRet = connect(Socket, (sockaddr*)&stServerAddr, sizeof(sockaddr));
	if (nRet == SOCKET_ERROR) {
		int err = WSAGetLastError();
		UE_LOG(LogTemp, Error, TEXT("connect failed: %d"), err);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("connect success"));
		// 소켓 non-blocking 모드
		u_long mode = 1; // 1 = non-blocking
		ioctlsocket(Socket, FIONBIO, &mode);
		UE_LOG(LogTemp, Warning, TEXT("Socket set to non-blocking mode"));
	}
		
	MyOwner = -1;
	IsSetMyOwner = false;
	bInitialized = false;
	
	SetThreadSocketHandle();
	
	
	if (!GetWorld()->GetTimerManager().IsTimerActive(ServerTimer))
	{

		GetWorld()->GetTimerManager().SetTimer(
			ServerTimer,
			this,
			&UMyServer::ProcessPacket,
			0.016f,
			true
		);
		UE_LOG(LogTemp, Warning,
			TEXT("SetTimer Complete"));
	}		
}

void UMyServer::Deinitialize()
{
	Super::Deinitialize();	
	UE_LOG(LogTemp, Error, TEXT("🔥 Deinitialize called"));
	GetWorld()->GetTimerManager().ClearTimer(ServerTimer);

	if (ClientThread)
	{
		ClientThread->Stop();
		delete ClientThread;
		ClientThread = nullptr;
	}

	

}

void UMyServer::Init(UGameConfigData* Config)
{
	if (bInitialized || !Config) return;

	SetBombClass(Config->BombClass);
	SetBlueHoleClass(Config->BlueHoleClass);
	SetBulletClass(Config->BulletClass);
	SetEnermyClass(Config->EnermyClass);
	SetAiClass(Config->AIClass);
	SetGrenadeClass(Config->GrenadeClass);
	RedzoneClass = Config->RedzoneClass;
	ItemTable = Config->ItemTable;
	bInitialized = true;
	Items = Config;
}

void UMyServer::Tick(float DeltaTime)
{
	
}

TStatId UMyServer::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMyServer, STATGROUP_Tickables);
}



void UMyServer::MoveConnect(FConnectionPacket packet)
{
	FScopeLock Lock(&SendMutex);

	packet.order = MyOwner;

	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);	

	SendAll((char*)&packet, sizeof(FConnectionPacket));
}

void UMyServer::MoveClient(FCharacterPacket bullet)
{
	FScopeLock Lock(&SendMutex);

	
	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);
	
	bullet.CharacterId = MyOwner;//처음에 -1로 초기화
	
	SendAll((char*)&bullet, sizeof(FCharacterPacket));

	UE_LOG(LogTemp, Warning,
		TEXT("Client sizeof(FCharacterPacket) = %d"),
		sizeof(FCharacterPacket));
	/*
	int nSendLen = send(Sock, (char*)&bullet, sizeof(FCharacterPacket), 0);
	

	if (nSendLen == SOCKET_ERROR)
	{
		UE_LOG(LogTemp, Error, TEXT("Send failed: %d"), WSAGetLastError());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Sent bytes = %d"), nSendLen);
	}*/
	
	
}

void UMyServer::MoveAI(FCharacterPacket packet)
{
	FScopeLock Lock(&SendMutex);
	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);
	SendAll((char*)&packet, sizeof(FCharacterPacket));
	/*
	int nSendLen = send(Sock, (char*)&packet, sizeof(FCharacterPacket), 0);
	*/
	UE_LOG(LogTemp, Warning,
		TEXT("MoveAI Complete"));
}

void UMyServer::MoveDmg(FDamagePacket packet)
{
	FScopeLock Lock(&SendMutex);
	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);

	packet.CharacterId = MyOwner;//처음에 -1로 초기화???

	SendAll((char*)&packet, sizeof(FDamagePacket));
	/*
	int nSendLen = send(Sock, (char*)&packet, sizeof(FDamagePacket), 0);
	*/
	
}

void UMyServer::MoveTime(int32 time)
{
	FScopeLock Lock(&SendMutex);

	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);

	FInformationTextPacket packet;
	packet.Header.Size = sizeof(FInformationTextPacket);
	packet.Header.Type = static_cast<int>(EPacketType::InformationText);
	packet.time = time;

	UE_LOG(LogTemp, Warning,
		TEXT("MoveTime send before"));
	SendAll((char*)&packet, sizeof(FInformationTextPacket));
	
	UE_LOG(LogTemp, Warning,
		TEXT("MoveTime send after"));
}

void UMyServer::MoveDeath(FDeathPacket packet)
{
	FScopeLock Lock(&SendMutex);

	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);
	packet.characterid = MyOwner;

	SendAll((char*)&packet, sizeof(FDeathPacket));
	
}

void UMyServer::MoveItem(FItemPacket packet)
{
	FScopeLock Lock(&SendMutex);

	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);
	packet.OwnerId = MyOwner;

	SendAll((char*)&packet, sizeof(FItemPacket));
}

void UMyServer::MoveBroadCast(FBrodcastMessage packet)
{
	FScopeLock Lock(&SendMutex);
	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);	
	SendAll((char*)&packet, sizeof(FBrodcastMessage));
}

void UMyServer::MoveVehicle(FVehiclePacket packet)
{
	FScopeLock Lock(&SendMutex);
	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);
	packet.ownerId = MyOwner;
	SendAll((char*)&packet, sizeof(FVehiclePacket));
}

void UMyServer::Shotoccurred(FServerBullet bullet)
{
	FScopeLock Lock(&SendMutex);

	UE_LOG(LogTemp, Warning,
		TEXT("Shotoccurred inside"));
	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);
	
	UE_LOG(LogTemp, Warning,
		TEXT("bullet Id = %d"),
		bullet.BulletId
	);

	bullet.CharacterId = MyOwner;
	
	
	UE_LOG(LogTemp, Warning, TEXT("Bullet CharacterID = %d"), MyOwner);

	double Start = FPlatformTime::Seconds();

	SendAll((char*)&bullet, sizeof(FServerBullet));
}

void UMyServer::SetThreadSocketHandle()
{
	UE_LOG(LogTemp, Warning,
		TEXT("SetThreadSocketHandle inside"));

	if (ClientThread) {
		UE_LOG(LogTemp, Warning,
			TEXT("already ClientThread have"));
		return; //중복 생성 방지
	}
	

	ClientThread = new ClientTrd(SocketHandle, this);
	UE_LOG(LogTemp, Warning,
		TEXT("SetThreadSocketHandle inside2"));
}

void UMyServer::ProcessPacket()
{		
	PorcessCharacterPacket();
	ProcessSpawnAIPacket();
	ProcessDamagePacket();
	ProcessDeathPacket();
	ProcessConnectionPacket();
	ProcessItemPacket();
	ProcessVehiclePacket();
}

void UMyServer::SetBulletClass(TSubclassOf<ABullet> blueprint)
{
	BulletClass = blueprint;
}

void UMyServer::SetEnermyClass(TSubclassOf<AMyEnemy> blueprint)
{
	EnermyClass = blueprint;
}

void UMyServer::SetAiClass(TSubclassOf<AAIEnemy> blueprint)
{
	AiClass = blueprint;
}




void UMyServer::SetWatingRoomPointer(UWatingRoom* room)
{
	WatingRoomWidget = room;
}
/*
void UMyServer::SetStartingMenuPointer(UStartMenuWidget* widget)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	UWatingRoom* WatingRoomWidget = CreateWidget<UWatingRoom>(PC, WatingRoomClass);
	WatingRoomWidget->AddToViewport();
}*/

void UMyServer::SetRocalPlayer(AActor* actor)
{
	LocalPlayer = actor;
}



void UMyServer::SetBulletPacket(FServerBullet resentPacket)
{
	bulletPacket = resentPacket;
}

void UMyServer::SetCharacterPacket(FCharacterPacket resentPacket)
{
	characterPacket = resentPacket;
}

void UMyServer::CreateGrenade(FGrenadePacket packet)
{
	FScopeLock Lock(&SendMutex);
	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);
	packet.CharacterId = MyOwner;
	SendAll((char*)&packet, sizeof(FGrenadePacket));
	//int nSendLen = send(Sock, (char*)&packet, sizeof(FGrenadePacket), 0);
}

void UMyServer::SetGrenadeClass(TSubclassOf<AMyGrenade> blueprint)
{
	GrenadeClass = blueprint;
}

TSubclassOf<AMyGrenade> UMyServer::GetGrenadeClass()
{
	return GrenadeClass;
}

TSubclassOf<AActor> UMyServer::GetRealActiveClassByID(int32 TargetID)
{
	if (!ItemTable)
		return nullptr;

	static const FString Context(TEXT("Find Item"));

	TArray<FItemStaticData*> Rows;
	ItemTable->GetAllRows(Context, Rows);

	for (FItemStaticData* Row : Rows)
	{
		if (Row && Row->ItemID == TargetID)
		{
			return Row->RealActiveActorClass;
		}
	}

	return nullptr;
}

void UMyServer::SpawnGrenade()
{
	UE_LOG(LogTemp, Warning, TEXT("수류탄 생성단계"));
	FGrenadePacket data;
	while (ClientThread->GrenadeQueue->Dequeue(data))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("큐 데이터 수신: ID=%d Pos=(%.1f %.1f %.1f) Dir=(%.1f %.1f %.1f) Power=%.1f"),
			data.GrenadeId,
			data.X, data.Y, data.Z,
			data.DirX, data.DirY, data.DirZ,
			data.Power
		);

		if (!ItemTable)
		{
			UE_LOG(LogTemp, Error, TEXT("ItemTable 없음!"));
			return;
		}

		if (data.GrenadeId == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("GrenadeId = 0"));
			continue;
		}
		if (ItemTable && data.GrenadeId!=0)
		{			
			TSubclassOf <AActor> RealGrenadeClass = GetRealActiveClassByID(data.GrenadeId);
			FVector Pos2 = FVector(data.X, data.Y, data.Z);
			FVector Dir2 = FVector(data.DirX, data.DirY, data.DirZ);
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(Pos2);			

			FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정					
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AMyGrenade* SpawnGrenade = GetWorld()->SpawnActor<AMyGrenade>(RealGrenadeClass, SpawnTransform, SpawnParams);
			SpawnGrenade->SetOwnerID(data.CharacterId);
			//SpawnEnemy->SetActorEnableCollision(false);
			
			//SpawnGrenades.Add(data.GrenadeId, SpawnGrenade);5-23  GrenadeClass도 정리 요망

			SpawnGrenade->Throw(Dir2, data.Power);
			UE_LOG(LogTemp, Warning, TEXT("수류탄 생성완료"));
		}
	}
}


void UMyServer::SpawnBullet(FServerBullet bullet)
{
	UE_LOG(LogTemp, Warning, TEXT("총알 생성단계"));
	
	FServerBullet data = bullet;
	
	// ❗ 한 개만 처리하도록 변경
	//if (ClientThread->BulletQueue->Dequeue(data))
	//{
		if (BulletClass)
		{
			FVector Pos2 = FVector(data.X, data.Y, data.Z);
			FVector Dir2 = FVector(data.DirX, data.DirY, data.DirZ);
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(Pos2);
			SpawnTransform.SetRotation(Dir2.Rotation().Quaternion()
			);

			FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정					
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
			
			//ABullet* SpawnBullet = GetWorld()->SpawnActor<ABullet>(BulletClass, SpawnTransform, SpawnParams);
			

			ABullet* SpawnBullet = GetWorld()->SpawnActor<ABullet>(BulletClass, Pos2, Dir2.Rotation(), SpawnParams);

			SpawnBullet->SetOwnerID(data.CharacterId);
			

			SpawnBullet->Throw(Dir2, 100);
			UE_LOG(LogTemp, Warning, TEXT("패킷받아서 총알 생성완료"));
		}
	//}

	
}

void UMyServer::SpawnMelee()
{
	UE_LOG(LogTemp, Warning, TEXT("주먹 생성단계"));
	FServerBullet data;
	while (ClientThread->MeleeQueue->Dequeue(data))
	{
		
		FVector End = { data.X, data.Y, data.Z };

		float LifeTime = 1.f;   // 스피어가 화면에 남아있는 시간

		DrawDebugSphere(
			GetWorld(),
			End,        // 스피어 위치
			30.f,       // 반지름
			16,         // 세그먼트 (원 해상도)
			FColor::Red,
			false,      // persistent
			LifeTime,   // 유지 시간
			0,
			2.f         // 선 두께
		);

		// 충돌 검사
		TArray<FOverlapResult> Overlaps;
		float Radius = 30.f;
		FVector Center = End;

		FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);

		bool bHit = GetWorld()->OverlapMultiByChannel(
			Overlaps,
			Center,
			FQuat::Identity,
			ECC_Pawn,   // 캐릭터만 검사
			Sphere
		);

		if (bHit)
		{
			for (auto& Result : Overlaps)
			{
				//AActor* HitActor = Result.GetActor();
				AMyCharacter* Victim = Cast<AMyCharacter>(Result.GetActor());
				if (Victim)
				{
					Victim->CurrentHitMeCharacterID = data.CharacterId;
					UGameplayStatics::ApplyDamage(
						Victim,
						10.f,      // 데미지
						nullptr,
						nullptr,
						nullptr
					);
				}
			}
		}

	}
}


void UMyServer::StartCountdownByPacket(int countdownamount)
{
	// ✅ 이미 실행 중이면 무시
	/*
	if (bCountdownStarted)
		return;
	*/

	// 중복 방지
	/*
	if (GetWorld()->GetTimerManager().IsTimerActive(CountdownTimerr))
		return;
	*/
	UE_LOG(LogTemp, Warning, TEXT("Start Countdown Timer"));

	CountdownTime = countdownamount;//10;

	GetWorld()->GetTimerManager().SetTimer(
		CountdownTimerr,
		this,
		&UMyServer::UpdateCountdownUI,
		1.f,
		true
	);

}

void UMyServer::SetInformationText(FString message)
{
	UWorld* World = GetGameInstance()->GetWorld();
	if (!World) return;
	ACustomPlayerController* PC =
		Cast<ACustomPlayerController>(UGameplayStatics::GetPlayerController(World, 0));
	if (!PC) return;

	UInformationWidget* Widget = PC->GetInformationWidget();
	if (Widget)
	{
		Widget->UpdateInformationUI(message);
	}
}


void UMyServer::MoveStartEnemyPawn(int order)
{
}

void UMyServer::SetStartMenuPawn(APawn* pawn)
{
	StartMenuPawn = pawn;
}

bool UMyServer::SendAll(const char* data, int size)
{
	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);

	int totalSent = 0;

	while (totalSent < size)
	{
		int sent = send(Sock, data + totalSent, size - totalSent, 0);

		if (sent == SOCKET_ERROR)
		{
			int err = WSAGetLastError();

			if (err == WSAEWOULDBLOCK)
			{
				FPlatformProcess::Sleep(0.001f); // 1ms
				continue;
			}

			printf("❌ send error: %d\n", err);
			return false;
		}

		totalSent += sent;
	}

	return true;
}

void UMyServer::UpdateCountdownUI()
{
	if (CountdownTime < 0) {
		return;
	}
	
	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* BasePC = World->GetFirstPlayerController();
	if (!BasePC)
	{
		UE_LOG(LogTemp, Warning, TEXT("BasePC is null"));
		return;
	}

	ACustomPlayerController* PC = Cast<ACustomPlayerController>(BasePC);
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast to CustomPlayerController failed. Actual: %s"),
			*BasePC->GetClass()->GetName());
		return;
	}

	PC->SetTextTime(CountdownTime);
	UE_LOG(LogTemp, Warning, TEXT("Countdown: %d"), CountdownTime);
	CountdownTime--;
	
}

void UMyServer::SetMyCharacterId()
{
	AMyCharacter* Player = Cast<AMyCharacter>(LocalPlayer);
	if (Player) {
		Player->SetMyId(MyOwner);
	}
}

void UMyServer::SetBombClass(TSubclassOf<ABomb> blueprint)
{
	BombClass = blueprint;
}

void UMyServer::StartRedzone(FVector Center)
{
	if (!GetWorld()) return;
	SpawnCount = 0;  // 시작할 때 초기화
	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Cyan,
		FString::Printf(TEXT("레드존")));

	RedzoneCenter = Center;	
	VisualRedZone = GetWorld()->SpawnActor<AActor>(RedzoneClass, RedzoneCenter, FRotator::ZeroRotator);	

	GetWorld()->GetTimerManager().SetTimer(
		timerHandle,
		this,
		&UMyServer::SpawnBomb,
		3.0f,
		true
	);
}

void UMyServer::EndRedzone()
{
	if (!GetWorld()) return;

	GetWorld()->GetTimerManager().ClearTimer(timerHandle);
	VisualRedZone->Destroy();
}

void UMyServer::SpawnBomb()
{
	SpawnCount++;

	FVector2D randomPoint = FMath::RandPointInCircle(RedzoneRadius);
	FVector targetPos = FVector(
		RedzoneCenter.X + randomPoint.X,
		RedzoneCenter.Y + randomPoint.Y,
		RedzoneCenter.Z + 5000.0f   // 위에서 떨어지게
	);

	GetWorld()->SpawnActor<ABomb>(BombClass, targetPos, FRotator::ZeroRotator);

	// 최대 횟수 도달하면 종료
	if (SpawnCount >= MaxSpawnCount)
	{
		EndRedzone();
	}
}

void UMyServer::SetBlueHoleClass(TSubclassOf<ABlueHole> blueprint)
{
	BlueHoleClass = blueprint;
}

void UMyServer::StartBlueHole(FVector Center)
{
	if (!GetWorld()) return;
	
	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Cyan,
		FString::Printf(TEXT("블루홀")));
	
	FVector targetPos = FVector(
		Center.X,
		Center.Y,
		Center.Z   // 위에서 떨어지게
	);

	BlueHole = GetWorld()->SpawnActor<ABlueHole>(BlueHoleClass, targetPos, FRotator::ZeroRotator);
	BlueHole->SetOverlapActive(true);
	
}



void UMyServer::PorcessCharacterPacket()
{
	FCharacterPacket data;
	int ProcessedCount = 0;
	while (ProcessedCount < MaxCharacterPacketsPerTick &&
		ClientThread->CharacterQueue->Dequeue(data))
	{

		ProcessedCount++;

		FVector Pos2 = FVector(data.X, data.Y, data.Z);
		FVector Dir2 = FVector(data.DirX, data.DirY, data.DirZ);

		//ai
		if (data.CharacterId < -90) {
			//이동로직
			// 충돌 무시하고 즉시 이동
			UE_LOG(LogTemp, Warning,
				TEXT("aiPos => X: %.3f Y: %.3f Z: %.3f  DeltaTime: %.6f id = %d"),
				data.DirX,
				data.DirY,
				data.DirZ,
				data.Sendtime,
				data.CharacterId
			);

			if (SpawnAis.Contains(data.CharacterId)) {
				AAIEnemy* ai = SpawnAis[data.CharacterId];
				ai->BB->SetValueAsVector(
					TEXT("TargetLocation"),
					Dir2
				);//?
			}
			else {
				continue;
			}
		}
		else {

			if (!SpawnEnemys.Contains(data.CharacterId) && EnermyClass)
			{

				UE_LOG(LogTemp, Warning,
					TEXT("insidethe enemy spawn id = %d"), data.CharacterId
				);

				FTransform SpawnTransform;
				SpawnTransform.SetLocation(Pos2);

				FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정
				//SpawnParams.Owner = this;//소유자를 캐릭터로
				//SpawnParams.Instigator = GetInstigator();//이 무기를 스폰한 주체가 누군지	
				SpawnParams.SpawnCollisionHandlingOverride =
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				AMyEnemy* SpawnEnemy = GetWorld()->SpawnActor<AMyEnemy>(EnermyClass, SpawnTransform, SpawnParams);
				//SpawnEnemy->SetActorEnableCollision(false);2-12
				SpawnEnemy->SetIgnoreCharacterId(MyOwner);
				SpawnEnemys.Add(data.CharacterId, SpawnEnemy);

				FEnemyState enemyData = { data.Speed, Pos2, Dir2, data.IsJump, data.IsFire, data.IsDeath, data.IsHeal , data.WeaponType, data.AimActive };
				SpawnEnemy->SetSPD(enemyData);
				//MyOwner = data.CharacterId;
				//3-21 적이 MyOwner이군요...

			}
			else {
				//이동로직
				// 충돌 무시하고 즉시 이동
				/*
				UE_LOG(LogTemp, Warning,
					TEXT("moveSpawnenemy")
				);
				*/
				AMyEnemy* Enemy = SpawnEnemys[data.CharacterId];
				Enemy->SetActorLocation(Pos2);
				FRotator LookRot = Dir2.Rotation();
				Enemy->SetActorRotation(LookRot);
				Enemy->GetCharacterMovement()->Velocity = Dir2 * data.Speed;
				FEnemyState enemyData = { data.Speed, Pos2, Dir2, data.IsJump, data.IsFire, data.IsDeath, data.IsHeal , data.WeaponType, data.AimActive };
				Enemy->SetSPD(enemyData);
			}
		}
	}
}

void UMyServer::ProcessSpawnAIPacket()
{
	//ai
	FSpawnAIPacket aidata;
	while (ClientThread->AISpawnQueue->Dequeue(aidata))
	{
		FVector Pos2 = FVector(aidata.X, aidata.Y, aidata.Z);

		UE_LOG(LogTemp, Warning,
			TEXT("Ai respon => X: %.3f Y: %.3f Z: %.3f id = %d"),
			aidata.X,
			aidata.Y,
			aidata.Z,
			aidata.AiID
		);

		if (!SpawnAis.Contains(aidata.AiID) && AiClass)
		{

			UE_LOG(LogTemp, Warning,
				TEXT("insidethe ai spawn id = %d"), aidata.AiID
			);

			FTransform SpawnTransform;
			SpawnTransform.SetLocation(Pos2);

			FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정					
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AAIEnemy* Spawnai = GetWorld()->SpawnActor<AAIEnemy>(AiClass, SpawnTransform, SpawnParams);
			//Spawnai->SetActorEnableCollision(false);

			SpawnAis.Add(aidata.AiID, Spawnai);
			Spawnai->SetMyAIID(aidata.AiID);

		}
	}
}

void UMyServer::ProcessDamagePacket()
{
	FDamagePacket Dmgdata;
	while (ClientThread->DmgQueue->Dequeue(Dmgdata))
	{
		//if (Dmgdata.CharacterId != MyOwner) // ← 여기서 체크
		//	continue;

		AMyCharacter* myCharacter = Cast<AMyCharacter>(LocalPlayer);

		UE_LOG(LogTemp, Warning,
			TEXT("서버로부터 데미지를 받음! CharacterId=%d"),
			Dmgdata.CharacterId  // int			
		);

		UGameplayStatics::ApplyDamage(LocalPlayer, Dmgdata.Damage, nullptr, nullptr, UDamageType::StaticClass());
		if (Dmgdata.Effect == EPlayerEffect::Red) {
			myCharacter->HitPostProcessComp->TriggerHitEffect(1.0f);
		}
		else if (Dmgdata.Effect == EPlayerEffect::White) {
			myCharacter->HitPostProcessComp->TriggerWhiteEffect(1.0f);
		}
		else {
			myCharacter->HitPostProcessComp->TriggerZoneEffect(1.0f);
		}
	}
}

void UMyServer::ProcessDeathPacket()
{
	FDeathPacket DeathData;
	while (ClientThread->DeathQueue->Dequeue(DeathData))
	{

		AMyCharacter* myCharacter = Cast<AMyCharacter>(LocalPlayer);

		FString VictimName = FString(ANSI_TO_TCHAR(DeathData.Name));
		FString KillerName = FString(ANSI_TO_TCHAR(DeathData.CausedName));

		UWorld* World = GetGameInstance()->GetWorld();
		if (!World) return;

		ACustomPlayerController* PC =
			Cast<ACustomPlayerController>(UGameplayStatics::GetPlayerController(World, 0));

		if (!PC) return;

		UkillAccountWidget* Widget = PC->GetDeathWidget();
		//UEndGameUIWidget* EndGameUI

		if (Widget)
		{
			Widget->UpdateDeathUI(KillerName, VictimName);
			Widget->KillerName = KillerName;
			//PC->ShowEndGameUI();
		}
	}
}

void UMyServer::ProcessConnectionPacket()
{
	FConnectionPacket ConnectPacket;
	while (ClientThread->ConnectQueue->Dequeue(ConnectPacket))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ConnectPacket 안입니다."));

		if (MyOwner == -1) {
			MyOwner = ConnectPacket.order;
		}
		else {
			for (ALoadingEnemy2* Player : StartPlayers)//?
			{
				if (Player->getMyOrder() == ConnectPacket.order) {
					Player->MoveEnemy();
				}
			}
		}
	}
}

void UMyServer::ProcessItemPacket()
{
	

	FItemPacket ItemPacket;
	while (ClientThread->ItemSpawnQueue->Dequeue(ItemPacket))
	{
		
		UE_LOG(LogTemp, Warning,
			TEXT("ItemPacket 안입니다."));
		UItemManagerSubsystem* Manager =
			GetGameInstance()->GetSubsystem<UItemManagerSubsystem>();

		Manager->InitItems(GetWorld());


		if (ItemPacket.ShouldRemove) {//서버에서 제거 >> 클라에서는 스폰
			AWeaponActor* Item = Manager->GetItem(ItemPacket.ItemSpawnID);
			Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			FVector SpawnLocation = FVector(ItemPacket.X, ItemPacket.Y, ItemPacket.Z);
			
			Item->SetActorLocation(SpawnLocation);
		}
		else {
			
			AWeaponActor* Itemm = Manager->GetItem(ItemPacket.ItemSpawnID);
			//FVector TargetLocation(100.f, 200.f, 300.f);//526
			//Itemm->SetActorLocation(TargetLocation);526
			
			UE_LOG(LogTemp, Warning, TEXT("enemyattachitem"));

			SpawnEnemys[ItemPacket.OwnerId]->AttachWeaponActor(Itemm);
		}
	}
}

void UMyServer::ProcessVehiclePacket()
{
	FVehiclePacket data;
	int ProcessedCount = 0;
	while (ProcessedCount < MaxVehiclePacketsPerTick &&
		ClientThread->VehicleQueue->Dequeue(data))
	{
		ProcessedCount++;

		FVector Pos2 = FVector(data.X, data.Y, data.Z);
		FVector Dir2 = FVector(data.DirX, data.DirY, data.DirZ);

		UE_LOG(LogTemp, Warning,
			TEXT("[VehiclePacket] carId=%d ownerId=%d Pos=(%.2f, %.2f, %.2f) Dir=(%.2f, %.2f, %.2f) HasChar=%s"),
			data.carId,
			data.ownerId,
			data.X,
			data.Y,
			data.Z,
			data.DirX,
			data.DirY,
			data.DirZ,
			data.IsHaveCharacter ? TEXT("true") : TEXT("false")
		);

		if (APawnVehicle** CarPtr =	SpawnVehicles.Find(data.carId))
		{					
			(*CarPtr)->SetActorLocation(Pos2);			
			FRotator Rot = Dir2.Rotation();
			(*CarPtr)->SetActorRotation(Rot);
			//SpawnEnemys[data.ownerId]->ActorStateInWorldByVehicle(true);//델리게이트로 하면 좋을듯 숨김
		}
		else
		{
			UE_LOG(LogTemp, Error,
				TEXT("Vehicle not found carId=%d"),
				data.carId);
		}
	}
}
