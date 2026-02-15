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
#include "Windows/HideWindowsPlatformTypes.h"
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
	stServerAddr.sin_addr.s_addr = inet_addr("192.168.219.109");//내 pc ip

	// 접속
	nRet = connect(Socket, (sockaddr*)&stServerAddr, sizeof(sockaddr));
	if (nRet == SOCKET_ERROR) {
		UE_LOG(LogTemp, Error, TEXT("SOCKET_ERROR"));
	}
	else {
		GEngine->AddOnScreenDebugMessage(
			-1,            // Key (-1 = 새 메시지)
			0.5f,          // 화면에 표시되는 시간 (초)
			FColor::Red,   // 색상
			TEXT("connect!")
		);
	}

	//MyChar = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(GWorld, 0));
	//한발 미리 배치
	FVector StartPos = FVector(10,10,10);
	FVector Dir = FVector(0, 0, 0);
	FServerBullet Bullet;
	Bullet.Header.Type = (int32)EPacketType::Bullet;
	Bullet.Header.Size = sizeof(FServerBullet);
	Bullet.BulletId = -2; 
	Bullet.CharacterId = -2;
	Bullet.X = StartPos.X;//100.f;
	Bullet.Y = StartPos.Y;//200.f;
	Bullet.Z = StartPos.Z;//300.f;
	Bullet.DirX = Dir.X;//1.f;
	Bullet.DirY = Dir.Y;//0.f;
	Bullet.DirZ = Dir.Z;//0.f;
	Bullet.Speed = 1200.f;
	Bullet.Sendtime = FPlatformTime::Seconds();
	Bullet.flag = false;
	Shotoccurred(Bullet);	
	MyOwner = -1;
	IsSetMyOwner = false;

	
	
}

void UMyServer::Deinitialize()
{
	Super::Deinitialize();	

	GetWorld()->GetTimerManager().ClearTimer(ServerTimer);

	if (ClientThread)
	{
		ClientThread->Stop();
		delete ClientThread;
		ClientThread = nullptr;
	}
}

void UMyServer::Tick(float DeltaTime)
{
	if (Usedbullet2.Num() == 0)
		return;

	for (auto It = Usedbullet2.CreateIterator(); It; ++It)
	{
		int32 Id = It.Key();

		if (ABullet* Bullet = SpawnItems.FindRef(Id)) //  FindRef
		{
			Bullet->Destroy();      // GameThread OK
			SpawnItems.Remove(Id);  // Remove OK (순회 중 아님)
		}
	}

	Usedbullet2.Empty(); // 반드시

	//ProcessBulletPacket();
	//PorcessCharacterPacket();
}

TStatId UMyServer::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMyServer, STATGROUP_Tickables);
}



void UMyServer::MoveClient(FCharacterPacket bullet)
{
	
	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);
	
	bullet.CharacterId = MyOwner;//처음에 -1로 초기화
	
	
	int nSendLen = send(Sock, (char*)&bullet, sizeof(FCharacterPacket), 0);

	//UE_LOG(LogTemp, Warning,
		//TEXT("%d"), MyOwner);

	//SetThreadSocketHandle();
	//UE_LOG(LogTemp, Warning,
		//TEXT("MoveClient Complete"));
}

void UMyServer::MoveAI(FCharacterPacket packet)
{
	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);

	int nSendLen = send(Sock, (char*)&packet, sizeof(FCharacterPacket), 0);

	UE_LOG(LogTemp, Warning,
		TEXT("MoveAI Complete"));
}

void UMyServer::MoveDmg(FDamagePacket packet)
{
	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);

	packet.CharacterId = MyOwner;//처음에 -1로 초기화


	int nSendLen = send(Sock, (char*)&packet, sizeof(FDamagePacket), 0);

	
}

void UMyServer::Shotoccurred(FServerBullet bullet)
{
	UE_LOG(LogTemp, Warning,
		TEXT("Shotoccurred inside"));
	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);
	
	UE_LOG(LogTemp, Warning,
		TEXT("bullet Id = %d"),
		bullet.BulletId
	);

	bullet.CharacterId = MyOwner;

	AMyCharacter* mc = Cast<AMyCharacter>(LocalPlayer);
	if (mc) {
		mc->SetPlayerID(MyOwner);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Bullet CharacterID = %d"), MyOwner);

	double Start = FPlatformTime::Seconds();

	// send 함수 사용 예시
	int nSendLen = send(Sock, (char*)&bullet, sizeof(FServerBullet), 0);

	double End = FPlatformTime::Seconds();
	

	
	UE_LOG(LogTemp, Warning,
		TEXT("Shotoccurred inside2"));
	
	/*
	if (bullet.flag == true) {
		if (ABullet* Bullet = SpawnItems.FindRef(bullet.BulletId))
		{
			//SpawnItems.Remove(bullet.BulletId);
			//Bullet->SetLifeSpan(0.01f);
			Usedbullet.Add(bullet.BulletId, true);
			Usedbullet2.Add(bullet.BulletId, true);
		}
	}
	//테스트
	*/

	SetThreadSocketHandle();
	
	if (!GetWorld()->GetTimerManager().IsTimerActive(ServerTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(
			ServerTimer,
			this,
			&UMyServer::SpawnActor,
			0.016f,
			true
		);
	}
	/*
	//제일먼저 얘까 실행됨
	if (!GetWorld()->GetTimerManager().IsTimerActive(ServerTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(
			ServerTimer,
			this,
			&UMyServer::ProcessBulletPacket,
			0.016f,
			true
		);
	}

	//제일먼저 얘까 실행됨
	if (!GetWorld()->GetTimerManager().IsTimerActive(ServerTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(
			ServerTimer,
			this,
			&UMyServer::PorcessCharacterPacket,
			0.016f,
			true
		);
	}
	*/
	//ProcessBulletPacket();
	//PorcessCharacterPacket();
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

void UMyServer::SpawnActor()
{
	FServerBullet Position;
	while(ClientThread->BulletQueue->Dequeue(Position))
	{		
			if (Usedbullet.Find(Position.BulletId)) {//좀비 총알제거
				/*
				UE_LOG(LogTemp, Warning,
					TEXT("conintue")
				);*/
				continue;
			}
			
			FVector Pos = FVector(Position.X, Position.Y, Position.Z);
			
			if (!SpawnItems.Contains(Position.BulletId) && BulletClass)
			{
				

				FTransform SpawnTransform;
				SpawnTransform.SetLocation(Pos);				

				FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정
				//SpawnParams.Owner = this;//소유자를 캐릭터로
				//SpawnParams.Instigator = GetInstigator();//이 무기를 스폰한 주체가 누군지		
				SpawnParams.SpawnCollisionHandlingOverride =
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				ABullet* SpawnItem = GetWorld()->SpawnActor<ABullet>(BulletClass, SpawnTransform, SpawnParams);
				SpawnItems.Add(Position.BulletId, SpawnItem);

				SpawnItem->SetBulletId(Position.BulletId);
				SpawnItem->SetBulletOwner(Position.CharacterId);
				SpawnItem->OnBulletHit.BindUObject(
					this,
					&UMyServer::OnBulletHit
				);

			}
			else {
				//이동로직
				// 충돌 무시하고 즉시 이동
				/*
				UE_LOG(LogTemp, Warning,
					TEXT("moveSpawnItem")
				);*/
				SpawnItems[Position.BulletId]->SetActorLocation(
					Pos,
					true,   // Sweep 
					nullptr,
					ETeleportType::TeleportPhysics
				);
			
		    }
		

		
	}
	FCharacterPacket data;
	while (ClientThread->CharacterQueue->Dequeue(data))
	{
		FVector Pos2 = FVector(data.X, data.Y, data.Z);
		FVector Dir2 = FVector(data.DirX, data.DirY, data.DirZ);
		/*
		UE_LOG(LogTemp, Warning,
			TEXT("EnemyPos => X: %.3f Y: %.3f Z: %.3f  DeltaTime: %.6f id = %d"),
			data.X,
			data.Y,
			data.Z,
			data.Sendtime,
			data.CharacterId
		);*/
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
			//SpawnTransform.SetRotation(Weapon->GetGunFoward().ToOrientationQuat());

			FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정
			//SpawnParams.Owner = this;//소유자를 캐릭터로
			//SpawnParams.Instigator = GetInstigator();//이 무기를 스폰한 주체가 누군지		
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AMyEnemy* SpawnEnemy = GetWorld()->SpawnActor<AMyEnemy>(EnermyClass, SpawnTransform, SpawnParams);
			//SpawnEnemy->SetActorEnableCollision(false);2-12
			SpawnEnemy->SetIgnoreCharacterId(MyOwner);
			SpawnEnemys.Add(data.CharacterId, SpawnEnemy);

			SpawnEnemy->SetSPD(data.Speed, Pos2, Dir2, data.IsJump, data.IsFire);
			MyOwner = data.CharacterId;


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
			Enemy->SetSPD(data.Speed, Pos2, Dir2, data.IsJump, data.IsFire);
		}
	}
	}	
	/*
	FGrenadePacket gredata;
	while (ClientThread->GrenadeQueue->Dequeue(gredata))
	{
		FVector Pos2 = FVector(gredata.X, gredata.Y, gredata.Z);
		FVector Dir2 = FVector(gredata.DirX, gredata.DirY, gredata.DirZ);
		
		
			if (!SpawnGrenades.Contains(gredata.GrenadeId) && GrenadeClass)
			{

				UE_LOG(LogTemp, Warning,
					TEXT("insidethe Grenade spawn id = %d"), gredata.GrenadeId
				);

				FTransform SpawnTransform;
				SpawnTransform.SetLocation(Pos2);
				//SpawnTransform.SetRotation(Weapon->GetGunFoward().ToOrientationQuat());

				FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정
				//SpawnParams.Owner = this;//소유자를 캐릭터로
				//SpawnParams.Instigator = GetInstigator();//이 무기를 스폰한 주체가 누군지		
				SpawnParams.SpawnCollisionHandlingOverride =
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				AMyGrenade* SpawnGrenade = GetWorld()->SpawnActor<AMyGrenade>(GrenadeClass, SpawnTransform, SpawnParams);
				
				SpawnGrenades.Add(gredata.GrenadeId, SpawnGrenade);

				


			}
			else {
				//이동로직
				// 충돌 무시하고 즉시 이동
				
				AMyGrenade* Grenade = SpawnGrenades[gredata.GrenadeId];
				Grenade->SetActorLocation(Pos2);
				FRotator LookRot = Dir2.Rotation();
				Grenade->SetActorRotation(LookRot);				
		}
	}*/
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

	FDamagePacket Dmgdata;
	while (ClientThread->DmgQueue->Dequeue(Dmgdata))
	{
		UGameplayStatics::ApplyDamage(LocalPlayer, Dmgdata.Damage, nullptr, nullptr, UDamageType::StaticClass());
	}
	
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

void UMyServer::CheckMyOwner(FVector pos, int32 id)
{
	float Radius = 20.f;
	TArray<FHitResult> Hits;

	FCollisionQueryParams Params;
	Params.bTraceComplex = false;

	bool bHit = GetWorld()->SweepMultiByChannel(
		Hits,
		pos,
		pos,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	if (!bHit)
	{
		return; // 아무 Pawn도 없음 → 판단 불가
	}

	for (const FHitResult& Hit : Hits)
	{
		APawn* Pawn = Cast<APawn>(Hit.GetActor());
		if (!Pawn) continue;

		// 🔥 핵심 판별 무용지물
		if (Pawn->IsPlayerControlled() && Pawn->IsLocallyControlled())
		{
			MyOwner = id;
			IsSetMyOwner = true;

			UE_LOG(LogTemp, Warning,
				TEXT("MyOwner determined! CharacterId = %d"),
				id
			);
			return;
		}
	}
}

void UMyServer::SetWatingRoomClass(TSubclassOf<UWatingRoom> blueprint)
{
	WatingRoomClass = blueprint;
}

void UMyServer::SetWatingRoomPointer(UWatingRoom* room)
{
	WatingRoomWidget = room;
}

void UMyServer::SetRocalPlayer(AActor* actor)
{
	LocalPlayer = actor;
}

void UMyServer::ProcessBulletPacket()
{
	FServerBullet Position;
	while (ClientThread->BulletQueue->Dequeue(Position)) {
		//Position = bulletPacket;

		if (Usedbullet.Find(Position.BulletId)) {//좀비 총알제거
			UE_LOG(LogTemp, Warning,
				TEXT("conintue")
			);
			return;
		}

		FVector Pos = FVector(Position.X, Position.Y, Position.Z);

		if (!SpawnItems.Contains(Position.BulletId) && BulletClass)
		{


			FTransform SpawnTransform;
			SpawnTransform.SetLocation(Pos);

			FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정
			//SpawnParams.Owner = this;//소유자를 캐릭터로
			//SpawnParams.Instigator = GetInstigator();//이 무기를 스폰한 주체가 누군지		
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ABullet* SpawnItem = GetWorld()->SpawnActor<ABullet>(BulletClass, SpawnTransform, SpawnParams);
			SpawnItems.Add(Position.BulletId, SpawnItem);

			SpawnItem->SetBulletId(Position.BulletId);

			SpawnItem->OnBulletHit.BindUObject(
				this,
				&UMyServer::OnBulletHit
			);

		}
		else {
			//이동로직
			// 충돌 무시하고 즉시 이동			
			SpawnItems[Position.BulletId]->SetActorLocation(
				Pos,
				true,   // Sweep 
				nullptr,
				ETeleportType::TeleportPhysics
			);

		}
	}
}

void UMyServer::PorcessCharacterPacket()
{
	FCharacterPacket data;
	while (ClientThread->CharacterQueue->Dequeue(data)) {
		//data = characterPacket;

		FVector Pos2 = FVector(data.X, data.Y, data.Z);
		FVector Dir2 = FVector(data.DirX, data.DirY, data.DirZ);

		UE_LOG(LogTemp, Warning,
			TEXT("EnemyPos => X: %.3f Y: %.3f Z: %.3f  Dirx : %f Diry : %f DirZ : %f DeltaTime: %.6f id = %d"),
			data.X,
			data.Y,
			data.Z,
			data.DirX,
			data.DirY,
			data.DirZ,
			data.Sendtime,
			data.CharacterId
		);

		


		if (!SpawnEnemys.Contains(data.CharacterId) && EnermyClass)
		{

			UE_LOG(LogTemp, Warning,
				TEXT("insidethe enemy spawn id = %d"), data.CharacterId
			);

			FTransform SpawnTransform;
			SpawnTransform.SetLocation(Pos2);
			//SpawnTransform.SetRotation(Weapon->GetGunFoward().ToOrientationQuat());

			FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정
			//SpawnParams.Owner = this;//소유자를 캐릭터로
			//SpawnParams.Instigator = GetInstigator();//이 무기를 스폰한 주체가 누군지		
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AMyEnemy* SpawnEnemy = GetWorld()->SpawnActor<AMyEnemy>(EnermyClass, SpawnTransform, SpawnParams);
			SpawnEnemy->SetActorEnableCollision(false);

			SpawnEnemys.Add(data.CharacterId, SpawnEnemy);

			SpawnEnemy->SetSPD(data.Speed, Pos2, Dir2, data.IsJump, data.IsFire);
			MyOwner = data.CharacterId;


		}
		else {
			//이동로직
			// 충돌 무시하고 즉시 이동
			UE_LOG(LogTemp, Warning,
				TEXT("moveSpawnenemy")
			);

			AMyEnemy* Enemy = SpawnEnemys[data.CharacterId];
			Enemy->SetActorLocation(Pos2);
			FRotator LookRot = Dir2.Rotation();
			//Enemy->SetActorRotation(LookRot);
			UE_LOG(LogTemp, Warning, TEXT("Dir2: %s"), *Dir2.ToString());
			Enemy->SetActorRotation(LookRot);
			Enemy->GetCharacterMovement()->Velocity = Dir2 * data.Speed;
			Enemy->SetSPD(data.Speed, Pos2, Dir2, data.IsJump, data.IsFire);
		}
	}
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
	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);
	packet.CharacterId = MyOwner;

	int nSendLen = send(Sock, (char*)&packet, sizeof(FGrenadePacket), 0);
}

void UMyServer::SetGrenadeClass(TSubclassOf<AMyGrenade> blueprint)
{
	GrenadeClass = blueprint;
}

void UMyServer::SpawnGrenade()
{
	UE_LOG(LogTemp, Warning, TEXT("수류탄 생성단계"));
	FGrenadePacket data;
	while (ClientThread->GrenadeQueue->Dequeue(data))
	{
		if (!SpawnGrenades.Contains(data.GrenadeId) && GrenadeClass)
		{

			
			FVector Pos2 = FVector(data.X, data.Y, data.Z);
			FVector Dir2 = FVector(data.DirX, data.DirY, data.DirZ);
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(Pos2);			

			FActorSpawnParameters SpawnParams;//스폰할 액터의 옵션 지정					
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AMyGrenade* SpawnGrenade = GetWorld()->SpawnActor<AMyGrenade>(GrenadeClass, SpawnTransform, SpawnParams);
			//SpawnEnemy->SetActorEnableCollision(false);

			SpawnGrenades.Add(data.GrenadeId, SpawnGrenade);

			SpawnGrenade->Throw(Dir2, data.Power);
			UE_LOG(LogTemp, Warning, TEXT("수류탄 생성완료"));
		}
	}
}



//이건 사용안함
void UMyServer::OnBulletHit(int32 BulletId, int32 CharacterId)
{
	UE_LOG(LogTemp, Warning, TEXT("이거쓰는데요?"));
	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);
	FServerBullet bullet;	
	bullet.Header.Type = (int)EPacketType::Bullet;
	bullet.Header.Size = sizeof(FServerBullet);
	if ((BulletId - 1) < 0) {
		return;
	}
	else {
		bullet.BulletId = BulletId-1;
	}
	bullet.CharacterId = CharacterId;
	bullet.X = 100.f;//의미없는 값
	bullet.Y = 200.f;
	bullet.Z = 300.f;
	bullet.DirX = 1.f;
	bullet.DirY = 0.f;
	bullet.DirZ = 0.f;
	bullet.Speed = 1200.f;
	bullet.Sendtime = FPlatformTime::Seconds();//의미없는값 여기까지
	bullet.flag = true;
	// send 함수 사용 예시
	int nSendLen = send(Sock, (char*)&bullet, sizeof(FServerBullet), 0);


	if (nSendLen == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		UE_LOG(LogTemp, Error,
			TEXT("[Pawn:%s] send() failed! OnBulletHit erase WSAGetLastError: %d"),
			*GetName(), err);
	}
	else
	{
		UE_LOG(LogTemp, Log,
			TEXT("[Pawn:%s] send() succeeded! erase Bytes sent: %d / Expected: %d"),
			*GetName(), nSendLen, sizeof(FServerBullet));
	}


	Usedbullet.Add(BulletId, true);
	Usedbullet2.Add(BulletId, true);
	/*게임쓰레드에서 지우자
	if (ABullet* Bullet = SpawnItems.FindRef(BulletId))
	{
		Bullet->Destroy();
		SpawnItems.Remove(BulletId);
	}*/

	
}

void UMyServer::StartCountdownByPacket()
{
	// ✅ 이미 실행 중이면 무시
	if (bCountdownStarted)
		return;


	// 중복 방지
	if (GetWorld()->GetTimerManager().IsTimerActive(CountdownTimerr))
		return;

	UE_LOG(LogTemp, Warning, TEXT("Start Countdown Timer"));

	CountdownTime = 10;

	GetWorld()->GetTimerManager().SetTimer(
		CountdownTimerr,
		this,
		&UMyServer::UpdateCountdownUI,
		1.f,
		true
	);

}

void UMyServer::SetMyCharacter(AMyCharacter* character)
{
	MyCharacter = character;
}

void UMyServer::UpdateCountdownUI()
{
	//int32 Time = 300;
	

	UE_LOG(LogTemp, Warning, TEXT("Countdown: %d"), CountdownTime);

	// UI 업데이트용 델리게이트 따로 쏴도 되고
	// 그냥 내부 상태만 줄여도 됨
	WatingRoomWidget->SetInformationText(FText::AsNumber(CountdownTime));
	CountdownTime--;
	
	if (CountdownTime <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(CountdownTimerr);
		UE_LOG(LogTemp, Warning, TEXT("Countdown Finished"));
		WatingRoomWidget->RemoveFromParent();
	}

}
