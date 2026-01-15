// Fill out your copyright notice in the Description page of Project Settings.


#include "MyServer.h"
#include "WorldCollision.h"      // FOverlapResult 실제 정의
#include "Windows/AllowWindowsPlatformTypes.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Engine/World.h"
#include "Engine/EngineTypes.h" // 여기서 FOverlapResult 정의됨
#include "CollisionQueryParams.h"

#include "GameFramework/CharacterMovementComponent.h"

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
}

TStatId UMyServer::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMyServer, STATGROUP_Tickables);
}



void UMyServer::MoveClient(FCharacterPacket bullet)
{
	UE_LOG(LogTemp, Warning,
		TEXT("MoveClient inside"));
	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);
	
	bullet.CharacterId = MyOwner;//처음에 -1로 초기화
	
	
	int nSendLen = send(Sock, (char*)&bullet, sizeof(FCharacterPacket), 0);

	//SetThreadSocketHandle();
}

void UMyServer::Shotoccurred(const FServerBullet& bullet)
{
	UE_LOG(LogTemp, Warning,
		TEXT("Shotoccurred inside"));
	SOCKET Sock = reinterpret_cast<SOCKET>(SocketHandle);
	
	UE_LOG(LogTemp, Warning,
		TEXT("bullet Id = %d"),
		bullet.BulletId
	);

	
	double Start = FPlatformTime::Seconds();

	// send 함수 사용 예시
	int nSendLen = send(Sock, (char*)&bullet, sizeof(FServerBullet), 0);

	double End = FPlatformTime::Seconds();
	UE_LOG(LogTemp, Warning, TEXT("send time = %f"), End - Start);

	
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

	//제일먼저 얘까 실행됨
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
	

	ClientThread = new ClientTrd(SocketHandle);
	UE_LOG(LogTemp, Warning,
		TEXT("SetThreadSocketHandle inside2"));
}

void UMyServer::SpawnActor()
{
	UE_LOG(LogTemp, Warning,
		TEXT("SpawnActor inside"));
	FServerBullet Position;
	while(ClientThread->BulletQueue->Dequeue(Position))
	{		
			if (Usedbullet.Find(Position.BulletId)) {//좀비 총알제거
				UE_LOG(LogTemp, Warning,
					TEXT("conintue")
				);
				continue;
			}
			//FServerBulletPos Position = Child->BulletPos;
			FVector Pos = FVector(Position.X, Position.Y, Position.Z);
			/*
			UE_LOG(LogTemp, Warning,
				TEXT("BulletPos => X: %.3f Y: %.3f Z: %.3f  DeltaTime: %.6f"),
				Position.X,
				Position.Y,
				Position.Z,
				Position.Sendtime
			);
			*/
			if (!SpawnItems.Contains(Position.BulletId) && BulletClass)
			{
				/*
				UE_LOG(LogTemp, Warning,
					TEXT("insidethe spawn")
				);*/

				FTransform SpawnTransform;
				SpawnTransform.SetLocation(Pos);
				//SpawnTransform.SetRotation(Weapon->GetGunFoward().ToOrientationQuat());

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

		UE_LOG(LogTemp, Warning,
			TEXT("EnemyPos => X: %.3f Y: %.3f Z: %.3f  DeltaTime: %.6f id = %d"),
			data.X,
			data.Y,
			data.Z,
			data.Sendtime,
			data.CharacterId
		);
		/*
		if (!IsSetMyOwner) {
			UE_LOG(LogTemp, Warning,
				TEXT("checkmy Owner")
			);
			CheckMyOwner(Pos2, data.CharacterId);
		}

		if (data.CharacterId == MyOwner) {
			UE_LOG(LogTemp, Warning,
				TEXT("i dont draw myself")
			);
			continue;
		}
		*/
		
		if (!SpawnEnemys.Contains(data.CharacterId) && EnermyClass)
		{

			UE_LOG(LogTemp, Warning,
				TEXT("insidethe enemy spawn id = %d"),data.CharacterId
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

			SpawnEnemy->SetSPD(data.Speed, Pos2, Dir2);
			MyOwner = data.CharacterId;


		}
		else {
			//이동로직
			// 충돌 무시하고 즉시 이동
			UE_LOG(LogTemp, Warning,
				TEXT("moveSpawnenemy")
			);
			/*
			SpawnEnemys[Position.BulletId]->SetActorLocation(
				Pos,
				true,   // Sweep
				nullptr,
				ETeleportType::TeleportPhysics
			);*/

			AMyEnemy* Enemy = SpawnEnemys[data.CharacterId];
			Enemy->SetActorLocation(Pos2);
			Enemy->GetCharacterMovement()->Velocity = Dir2 * data.Speed;
			Enemy->SetSPD(data.Speed, Pos2, Dir2);
		}
	}

	UE_LOG(LogTemp, Warning,
		TEXT("OutofSpawnActor")
	);
}

void UMyServer::SetBulletClass(TSubclassOf<ABullet> blueprint)
{
	BulletClass = blueprint;
}

void UMyServer::SetEnermyClass(TSubclassOf<AMyEnemy> blueprint)
{
	EnermyClass = blueprint;
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

//이건 사용안함
void UMyServer::OnBulletHit(int32 BulletId)
{
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
	//bullet.BulletId = BulletId;
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
